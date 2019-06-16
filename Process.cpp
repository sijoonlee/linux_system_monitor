#include "Process.h"

void Process::setPid(int pid) {
    this->pid = pid;
}
string Process::getPid() const {
    return this->pid;
}
string Process::getUser() const {
    return this->user;
}
string Process::getCmd() const {
    return this->cmd;
}
int Process::getCpu() const {
    return std::stoi(this->cpu);
}
int Process::getMem() const {
    return std::stoi(this->mem);
}
string Process::getUpTime() const {
    return this->upTime;
}

string Process::getProcess() {
    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);

    return (this->pid + "   "
            + this->user
            + "   "
            + this->mem.substr(0,5)
            + "     "
            + this->cpu.substr(0,5)
            + "     "
            + this->upTime.substr(0,5)
            + "    "
            + this->cmd.substr(0,30)
            + "...");
}