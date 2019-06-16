#pragma once
#include "Process.h"
#include <vector>
class ProcessContainer{

private:
    std::vector<Process>_list;
public:
    ProcessContainer(){
        this->refreshList();
    }
    void refreshList();
    std::string printList();
    std::vector<std::vector<std::string>> getList();
};
