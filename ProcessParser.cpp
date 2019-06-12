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
    std::string line;
    std::string name = "Uid:"; // user unique identifier
    std::string result ="";
    std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));

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
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            std::vector<string> values(beg, end);
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


// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
// http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
// https://www.dreamincode.net/forums/topic/59943-accessing-directories-in-cc-part-i/

// about dirent.h
// http://www.cplusplus.com/forum/unices/16005/
// readdir() only returns one directory entry at a time.
// You have to keep calling readdir() until it returns 0, which indicates that there are no more entries.

// https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
// struct dirent is declared in dirent.h
// char d_name[] ; the null-terminated file name component.
// unsigned char d_type
//      DT_REG - regular file
//      DT_DIR - directory
//      DT_UNKNOWN - unknown type

vector<string> ProcessParser::getPidList(){
    DIR* dir;
    // Basically, we are scanning /proc dir for all directories with numbers as their names
    // If we get valid check we store dir names in vector as list of machine pids
    std::vector<string> container;
    if(!(dir = opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));

    while (dirent* dirp = readdir(dir)) { // while there is still something in the directory to list

        // is this a directory?
        if(dirp->d_type != DT_DIR)
            continue;

        // Is every character of the name a digit?
        if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){ return std::isdigit(c); })) {
                // https://www.geeksforgeeks.org/stdall_of-in-cpp/
                // all_of() is defined in <algorithm>
                // template <class InputIterator, class UnaryPredicate>
                // bool all_of (InputIterator first, InputIterator last, UnaryPredicate pred);
                // first : Input iterators to the initial positions in a sequence.
                // last : Input iterators to the final positions in a sequence.
                // pred : An unary predicate function that accepts an element and returns a bool.
            container.push_back(dirp->d_name);
        }
    }
    //Validating process of directory closing
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}
