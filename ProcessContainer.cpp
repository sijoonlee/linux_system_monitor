#include "ProcessContainer.h"

void ProcessContainer::refreshList(){
    std::vector<std::string> pidList = ProcessParser::getPidList();
    this->_list.clear();
    for(int i=0;i<pidList.size();i++){
        Process proc(pidList[i]);
        this->_list.push_back(proc);
    }
}
std::string ProcessContainer::printList(){
    std::string result="";
//    for (auto i : _lists) {
//        result += i.getProcess();
//    }
    for(int i=0;i<this->_list.size();i++){
        result += this->_list[i].getProcess();
    }
    return result;
}
std::vector<std::string> ProcessContainer::getList(){

    vector<string> values;
    for (int i = (this->_list.size()-10); i < this->_list.size(); i++){
        values.push_back(this->_list[i].getProcess());
    }
    return values;
}
