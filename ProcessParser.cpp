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
    // std::cout << Path::basePath() + pid + Path::statusPath()  << "\n";

    while(std::getline(stream, line)){
        // search line by line
        // std::cout << line << "\n";

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

std::string ProcessParser::getCpuPercent(string pid){
    std::string line;
    float result;

    // Opening stream for specific file
    std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statPath()));
    // >> cat /proc/165/stat
    // 165 (irq/35-pciehp) S 2 0 0 0 -1 2129984 0 0 0 0 0 0 0 0 -51 0 1 0 251 0 0 18446744073709551615 0 0 0 0 0 0 0 2147483647 0 0 0 0 17 1 50 1 0 0 0 0 0 0 0 0 0 0 0

    std::getline(stream, line);
    std::string str = line;
    std::istringstream buf(str);
    std::istream_iterator<string> beg(buf), end;
    std::vector<string> values(beg, end); // done!

    // acquiring relevant times for calculation of active occupation of CPU for selected process
    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return std::to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
    std::string line;
    std::string value;
    float result;
    std::ifstream stream = Util::getStream((Path::basePath() + pid + "/" +  Path::statPath()));
    std::getline(stream, line);
    std::string str = line;
    std::istringstream buf(str);
    std::istream_iterator<string> beg(buf), end;
    std::vector<string> values(beg, end); // done!
    // Using sysconf to get clock ticks of the host machine
    return std::to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime() {
    std::string line;
    std::ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
    std::getline(stream,line);
    std::istringstream buf(line);
    std::istream_iterator<string> beg(buf), end;
    std::vector<string> values(beg, end);
    return std::stoi(values[0]);
}


string ProcessParser::getProcUser(string pid){
    string line;
    string name = "Uid:"; // user unique identifier
    string result ="";
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));

    // Getting UID for user
    // cat /proc/165/status
    //  Pid:	165
    //  PPid:	2
    //  TracerPid:	0
    //  Uid:	0	0	0	0

    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
                //Syntax 2: Compares at most, len characters of string *this, starting with index idx with the string str.
                //int string::compare (size_type idx, size_type len, const string& str) const
                //Throws out_of_range if index > size().
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result =  values[1];
            break;
        }
    }
    stream = Util::getStream("/etc/passwd");
    name =("x:" + result);

    // Searching for name of the user with selected UID
    // cat /etc/passwd | grep -i 0
    //  root:x:0:0:root:/root:/bin/bash

    while (std::getline(stream, line)) {
        if (line.find(name) != std::string::npos) {
            result = line.substr(0, line.find(":"));
                // string.find return the position of the first character of the first match.
                // If no matches were found, the function returns string::npos.
            return result;
        }
    }
    return "";
}

