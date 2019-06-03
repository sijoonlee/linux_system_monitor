//
// Created by sijoonlee on 6/2/19.
//

//class ProcessParser{
//private:
//    std::ifstream stream;
//public:
//    static string getCmd(string pid);
//    static vector<string> getPidList();
//    static std::string getVmSize(string pid);
//    static std::string getCpuPercent(string pid);
//    static long int getSysUpTime();
//    static std::string getProcUpTime(string pid);
//    static string getProcUser(string pid);
//    static vector<string> getSysCpuPercent(string coreNumber = "");
//    static float getSysRamPercent();
//    static string getSysKernelVersion();
//    static int getTotalThreads();
//    static int getTotalNumberOfProcesses();
//    static int getNumberOfRunningProcesses();
//    static string getOSName();
//    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
//    static bool isPidExisting(string pid);
//};

#include "ProcessParser.h"

std::string ProcessParser::getVmSize(std::string pid){
    std::string line;

    // Declaring search attribute for file
    std::string name = "VmData";
    float result;

    // Opening stream for specific file
    std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath())); // ex. /proc/4431/status
    std::cout << Path::basePath() + pid + Path::statusPath()  << "\n";

    while(std::getline(stream, line)){
        // search line by line
        std::cout << line << "\n";
        if(line.compare(0, name.size(), name) == 0){
            // int string::compare (const string& str) const
            // int string::compare (size_type idx, size_type len, const string& str) const
            //      Returns:
            //      0 : if both strings are equal.
            //      A value < 0 : if *this is shorter than str or,
            //                    first character that doesn't match is smaller than str.
            //      A value > 0 : if *this is longer than str or,
            //                    first character that doesn't match is greater

            // slicing string line on ws for values using sstream
            std::istringstream buf(line);
            std::istream_iterator<string> start(buf), end; //
            std::vector<string> values(start, end);
            // for(string x: values)
            //      std::cout << x << "\n";

            // https://stackoverflow.com/questions/17427943/creating-stl-vector-from-pair-of-iterator-c
            // The vector constructor uses an open interval so the end is not included

            // line = "VmData:	     420 kB"
            // values = ["VmData:","420","kB"]

            // conversion KB -> GB
            result = std::stof(values[1])/float(1024); //
            break;
        }
    }
    return to_string(result);
}