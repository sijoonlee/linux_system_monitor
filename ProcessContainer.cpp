#include "ProcessContainer.h"

void ProcessContainer::refreshList(){
    std::vector<std::string> pidList = ProcessParser::getPidList();
    this->_list.clear();
    for(int i=0;i<pidList.size();i++){
        Process proc(pidList[i]);
        this->_list.push_back(proc);
    }
}

std::vector<std::vector<std::string>> ProcessContainer::getList(){

    std::vector<std::vector<std::string>> values;
//    for (int i = (this->_list.size()-10); i < this->_list.size(); i++){
//        values.push_back(this->_list[i].getProcess());
//    }
    for (Process elm : this->_list)
        values.push_back(elm.getProcess());
    return values;
}
