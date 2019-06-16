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

std::vector<std::string> Process::getProcess() {
    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);

    vector<string> temp;

    temp.push_back((this->pid).substr(0,5));
    temp.push_back((this->user).substr(0,9));
    temp.push_back(cpu.substr(0,5));
    temp.push_back(mem.substr(0,5));
    temp.push_back(upTime.substr(0,5));
    temp.push_back(cmd.size() < 28 ? cmd + std::string(28-cmd.size(), ' ') : cmd.substr(0,28));

    return temp;
}