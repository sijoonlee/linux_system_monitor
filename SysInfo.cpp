#include "SysInfo.h"

void SysInfo::getOtherCores(int _size){
//when number of cores is detected, vectors are modified to fit incoming data
    this->coresStats = std::vector<std::string>();
    this->coresStats.resize(_size);
        // https://www.geeksforgeeks.org/vector-resize-c-stl/
        // vectorname.resize(int n, int val)
        // n – it is new container size, expressed in number of elements.
        // val – if this parameter is specified then new elements are initialized with this value.

    this->lastCpuCoresStats = std::vector<std::vector<std::string>>();
    this->lastCpuCoresStats.resize(_size);
    this->currentCpuCoresStats = std::vector<std::vector<std::string>>();
    this->currentCpuCoresStats.resize(_size);
    for(int i=0;i<_size;i++){
        this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));
    }
}
void SysInfo::setLastCpuMeasures(){
    this->lastCpuStats = ProcessParser::getSysCpuPercent();
}
void SysInfo::setCpuCoresStats(){
// Getting data from files (previous data is required)
    for(int i=0;i<this->currentCpuCoresStats.size();i++){
        this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));
    }
    for(int i=0;i<this->currentCpuCoresStats.size();i++){
        // after acquirement of data we are calculating every core percentage of usage
        this->coresStats[i] = ProcessParser::printCpuStats(this->lastCpuCoresStats[i],this->currentCpuCoresStats[i]);
    }
    this->lastCpuCoresStats = this->currentCpuCoresStats;
}
void SysInfo::setAttributes(){
// getting parsed data
    this->memPercent = ProcessParser::getSysRamPercent();
    this->upTime = ProcessParser::getSysUpTime();
    this->totalProc = ProcessParser::getTotalNumberOfProcesses();
    this->runningProc = ProcessParser::getNumberOfRunningProcesses();
    this->threads = ProcessParser::getTotalThreads();
    this->currentCpuStats = ProcessParser::getSysCpuPercent();
    this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats,this->currentCpuStats);
    this->lastCpuStats = this->currentCpuStats;
    this->setCpuCoresStats();

}
// Constructing string for every core data display
std::vector<std::string> SysInfo::getCoresStats()const{
    std::vector<std::string> result= std::vector<std::string>();
    for(int i=0;i<this->coresStats.size();i++){
        std::string temp =("cpu" + to_string(i) +": ");
        float check;
        if(!this->coresStats[i].empty())
            check = stof(this->coresStats[i]);
        if(!check || this->coresStats[i] == "nan"){
            temp += Util::getProgressBar("0.0");
        } else {
            temp += Util::getProgressBar(this->coresStats[i]);
        }
        result.push_back(temp);
    }
    return result;
}
std::string SysInfo::getCpuPercent()const {
    return this->cpuPercent;
}
std::string SysInfo::getMemPercent()const {
    return to_string(this->memPercent);
}
long SysInfo::getUpTime()const {
    return this->upTime;
}
std::string SysInfo::getKernelVersion()const {
    return this->kernelVer;
}
std::string SysInfo::getTotalProc()const {
    return to_string(this->totalProc);
}
std::string SysInfo::getRunningProc()const {
    return to_string(this->runningProc);
}
std::string SysInfo::getThreads()const {
    return to_string(this->threads);
}
std::string SysInfo::getOSName()const {
    return this->OSname;
}
