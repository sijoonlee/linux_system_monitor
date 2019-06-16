//
// Created by sijoonlee on 6/2/19.
//

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
            std::vector<string> values = splitByWhiteSpace(line);

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
    std::vector<string> values = splitByWhiteSpace(line);

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
    std::vector<string> values = splitByWhiteSpace(line);
    // Using sysconf to get clock ticks of the host machine
    return std::to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime() {
    std::string line;
    std::ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
    std::getline(stream,line);
    std::vector<string> values = splitByWhiteSpace(line);
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
            std::vector<string> values = splitByWhiteSpace(line);
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


string ProcessParser::getCmd(string pid){
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
    std::getline(stream, line); // just one line
    return line ;
}

int ProcessParser::getNumberOfProcessors(){
    // Get the number of host cpu processors
    // Alternative solution that works in Linux:
    //    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    //    std::cout << numCPU ;
    string line;
    string name = "processor";
//    string name = "cpu cores";
    int numberOfProcessors = 0;
    ifstream stream = Util::getStream((Path::basePath() + Path::cpuInfoPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            numberOfProcessors++;
//            istringstream buf(line);
//            istream_iterator<string> beg(buf), end;
//            vector<string> values(beg, end);
//            numberOfProcessors += stoi(values[3]);
        }
    }
    return numberOfProcessors;
}

vector<string> ProcessParser::getSysCpuPercent(string processorNumber){
    // It is possible to use this method for selection of data for overall cpu or every core.
    // when nothing is passed "cpu" line is read
    // when, for example "0" is passed  -> "cpu0" -> data for first core is read
    //  /proc/stat
    //      cpu  112810 297 24916 2906426 935 0 1329 0 0 0
    //      cpu0 28016 146 6449 726597 195 0 381 0 0 0
    //      cpu1 29715 6 6042 725665 241 0 346 0 0 0
    //      cpu2 28170 7 5966 727135 168 0 252 0 0 0
    //      cpu3 26908 137 6458 727028 329 0 350 0 0 0
    string line;
    string name = "cpu" + processorNumber;
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            // set of cpu data active and idle times;
            return values;
        }
    }
    return (vector<string>());
}


// https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated
//cat /proc/stat
//        user nice system idle iowait  irq  softirq steal guest guest_nice
//        cpu  4705 356  584    3699   23    23     0       0     0          0

//user: normal processes executing in user mode
//nice: niced processes executing in user mode
//system: processes executing in kernel mode
//        idle: twiddling thumbs
//iowait: waiting for I/O to complete
//        irq: servicing interrupts
//softirq: servicing softirqs
//steal: involuntary wait
//guest: running a normal guest
//guest_nice: running a niced guest

//Total CPU time since boot = user+nice+system+idle+iowait+irq+softirq+steal
//Total CPU Idle time since boot = idle + iowait
//Total CPU usage time since boot = Total CPU time since boot - Total CPU Idle time since boot
//Total CPU percentage = Total CPU usage time since boot/Total CPU time since boot X 100


float getSysActiveCpuTime(vector<string> values) {
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string>values) {
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

string ProcessParser::printCpuStats(std::vector<string> values1, std::vector<string> values2) {
/*
Because CPU stats can be calculated only if you take measures in two different time,
this function has two parameters: two vectors of relevant values.
We use a formula to calculate overall activity of processor.
*/
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}


float ProcessParser::getSysRamPercent()
{
    string line;
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    string value;
    ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;
    while (std::getline(stream, line)) {
        if (total_mem != 0 && free_mem != 0)
            break;
        if (line.compare(0, name1.size(), name1) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            total_mem = stof(values[1]);
        }
        if (line.compare(0, name2.size(), name2) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            free_mem = stof(values[1]);
        }
        if (line.compare(0, name3.size(), name3) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            buffers = stof(values[1]);
        }
    }
    //calculating usage:
    return float(100.0*(1-(free_mem/(total_mem-buffers))));
}

string ProcessParser::getSysKernelVersion()
{
    string line;
    string name = "Linux version ";
    ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            return values[2];
        }
    }
    return "";
}

string ProcessParser::getOSName()
{
    string line;
    string name = "PRETTY_NAME=";

    ifstream stream = Util::getStream(("/etc/os-release"));

    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            std::size_t found = line.find("=");
            found++;
            string result = line.substr(found);
            result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
            return result;
        }
    }
    return "";
}

int ProcessParser::getTotalThreads() {
    string line;
    int result = 0;
    string name = "Threads:";
    vector<string> _list = ProcessParser::getPidList();
    for (int i = 0; i < _list.size(); i++) {
        string pid = _list[i];
        //getting every process and reading their number of their threads
        ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
        while (std::getline(stream, line)) {
            if (line.compare(0, name.size(), name) == 0) {
                std::vector<string> values = splitByWhiteSpace(line);
                result += stoi(values[1]);
                break;
            }
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses() {
    string line;
    int result = 0;
    string name = "processes";
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}

int ProcessParser::getNumberOfRunningProcesses() {
    string line;
    int result = 0;
    string name = "procs_running";
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            std::vector<string> values = splitByWhiteSpace(line);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}

bool ProcessParser::isPidExisting(string pid){
    vector<string> pidList = ProcessParser::getPidList();
    return ( std::find(pidList.begin(), pidList.end(), pid) != pidList.end() );
    // std::find ; <algorithm>
}

// https://stackoverflow.com/questions/17427943/creating-stl-vector-from-pair-of-iterator-c
// The vector constructor uses an open interval so the end is not included

// line = "VmData:	     420 kB"
// values = ["VmData:","420","kB"]
std::vector<std::string> ProcessParser::splitByWhiteSpace(string stringToSplit){
    istringstream buf(stringToSplit);
    istream_iterator<string> beg(buf),end;
    return vector<string>(beg,end);
}