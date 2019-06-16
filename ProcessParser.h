#pragma once

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"


using namespace std;

class ProcessParser{
    private:
        std::ifstream stream;
    public:
        static string getCmd(string pid);
        static vector<string> getPidList();
        static std::string getVmSize(string pid);
        static std::string getCpuPercent(string pid);
        static long int getSysUpTime();
        static std::string getProcUpTime(string pid);
        static string getProcUser(string pid);
        static int getNumberOfProcessors();
        static vector<string> getSysCpuPercent(string processorNumber = "");
        static std::string printCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
        static float getSysRamPercent();
        static string getSysKernelVersion();
        static string getOSName();
        static int getTotalThreads();
        static int getTotalNumberOfProcesses();
        static int getNumberOfRunningProcesses();
        static bool isPidExisting(string pid);
        static std::vector<std::string>splitByWhiteSpace(string stringToSplit);
};

