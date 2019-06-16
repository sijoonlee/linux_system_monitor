#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "util.h"
#include "SysInfo.h"
#include "ProcessContainer.h"

// for test
//#include "ProcessParser.h"
//#include "Process.h"
//void test();

using namespace std;


char* getCString(std::string str){
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    return cstr;
}
void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win) {
    sys.setAttributes();

    mvwprintw(sys_win,2,2,getCString(( "OS: " + sys.getOSName())));
    mvwprintw(sys_win,3,2,getCString(( "Kernel version: " + sys.getKernelVersion())));
    mvwprintw(sys_win,4,2,getCString( "CPU: "));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getCpuPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,5,2,getCString(( "CPU Details:")));
    wattron(sys_win,COLOR_PAIR(1));
    std::vector<std::string> val = sys.getProcessorsStats();
    for (int i = 0; i < val.size(); i++) {
        mvwprintw(sys_win,(6+i),2,getCString(val[i]));
    }
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,10,2,getCString(( "Memory: ")));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getMemPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,11,2,getCString(( "Total Processes:" + sys.getTotalProc())));
    mvwprintw(sys_win,12,2,getCString(( "Running Processes:" + sys.getRunningProc())));
    mvwprintw(sys_win,13,2,getCString(( "Up Time: " + Util::convertToTime(sys.getUpTime()))));

}
void getProcessListToConsole(std::vector<std::string> process, WINDOW* win, int row){

    wattron(win,COLOR_PAIR(2));
    mvwprintw(win,1,2,"PID:"); // Change has been done in the following some lines.
    mvwprintw(win,1,9,"User:"); //We just changed some values so that we get nice UI
    mvwprintw(win,1,20,"CPU[%]:"); //For this go through the videos of ncurses library
    mvwprintw(win,1,28,"RAM[MB]:");
    mvwprintw(win,1,36,"Uptime:");
    mvwprintw(win,1,44,"CMD:");
    wattroff(win, COLOR_PAIR(2));

    int column, positionX;

    for(column =0; column < 6; column++){
        switch(column){
            case 0:
                positionX = 2;
                break;
            case 1:
                positionX = 9;
                break;
            case 2:
                positionX = 19;
                break;
            case 3:
                positionX = 28;
                break;
            case 4:
                positionX = 37;
                break;
            default:
                positionX = 45;
        }
        mvwprintw(win,2+row,positionX,getCString(process[column]));
    }
}

void printMain(SysInfo sys,ProcessContainer procs){
    initscr();          /* Start curses mode          */
    noecho(); // not printing input values
    cbreak(); // Terminating on classic ctrl + c
    start_color(); // Enabling color change of text
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax); // getting size of window measured in lines and columns(column one char length)
    WINDOW *sys_win = newwin(17,xMax-1,0,0);
    WINDOW *proc_win = newwin(15,xMax-1,18,0);


    init_pair(1,COLOR_BLUE,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    int counter = 0;
    const int MAX_PER_PAGE = 12;
    while(1){
        box(sys_win,0,0);
        box (proc_win,0,0);
        procs.refreshList();
        std::vector<std::vector<std::string>> processes = procs.getList();
        writeSysInfoToConsole(sys,sys_win);
        getProcessListToConsole(processes[counter], proc_win, counter%MAX_PER_PAGE);
        wrefresh(sys_win);
        wrefresh(proc_win);
        refresh();
        sleep(1);
        if(counter >= (processes.size() -1)){
            counter = 0;
        }
        else{
            counter++;
        }
    }
    endwin();
}
int main( int   argc, char *argv[] )
{
    //Object which contains list of current processes, Container for Process Class
    ProcessContainer procs;
// Object which containts relevant methods and attributes regarding system details
    SysInfo sys;
    //std::string s = writeToConsole(sys);
    printMain(sys,procs);
    return 0;

//    test();
}



// https://stackoverflow.com/questions/834748/gcc-makefile-error-no-rule-to-make-target
//That's usually because you don't have a file called vertex.cpp available to make. Check that:
//- that file exists.
//- you're in the right directory when you make.


void test (){

    std::cout << "ProcessParser::getNumberOfProcessors() called" << "\n";
    std::cout << ProcessParser::getNumberOfProcessors() << "\n";

    std::cout << "ProcessParser::getPidList() called" << "\n";
    int count = 1;
    for( auto elm : ProcessParser::getPidList()){
        std::string newLine = ( (count%20 == 0) ? "\n" : " ");
        std::cout << elm << newLine;
        count++;
    }
    cout << "\n";

    std::string pid = ProcessParser::getPidList()[ProcessParser::getPidList().size()-1];
    std::cout << "ProcessParser::getVmSize(some pid) called" << "\n";
    std::cout << ProcessParser::getVmSize(pid) << "\n";

    std::cout << "ProcessParser::getCpuPercent(some pid) called" << "\n";
    std::cout << ProcessParser::getCpuPercent(pid) << "\n";

    std::cout << "ProcessParser::getProcUser(some pid) called" << "\n";
    std::cout << ProcessParser::getProcUser(pid) << "\n";

    std::cout << "ProcessParser::getCmd(some pid) called" << "\n";
    std::cout << ProcessParser::getCmd(pid) << "\n";

    std::cout << "ProcessParser::getSysCpuPercent() called" << "\n";
    for(auto elm : ProcessParser::getSysCpuPercent())
        std::cout <<  elm << " ";
    std::cout << "\n";

    std::cout << "ProcessParser::getSysCpuPercent(\"0\") called" << "\n";
    for(auto elm : ProcessParser::getSysCpuPercent("0"))
        std::cout <<  elm << " ";
    std::cout << "\n";

    std::cout << "ProcessParser::printCpuStats(first, second) called" << "\n";
    vector<std::string> first = ProcessParser::getSysCpuPercent("0");
    sleep(1);
    vector<std::string> second = ProcessParser::getSysCpuPercent("0");
    ProcessParser::getSysCpuPercent("0");
    cout << ProcessParser::printCpuStats(first, second) << "\n";

    std::cout << "ProcessParser::getSysRamPercent() called" << "\n";
    std::cout << ProcessParser::getSysRamPercent() << "\n";

    std::cout << "ProcessParser::getSysKernelVersion() called" << "\n";
    std::cout << ProcessParser::getSysKernelVersion() << "\n";

    std::cout << "ProcessParser::getOSName() called" << "\n";
    std::cout << ProcessParser::getOSName() << "\n";

    std::cout << "ProcessParser::getTotalThreads() called" << "\n";
    std::cout << ProcessParser::getTotalThreads() << "\n";

    std::cout << "ProcessParser::getTotalNumberOfProcesses() called" << "\n";
    std::cout << ProcessParser::getTotalNumberOfProcesses() << "\n";

    std::cout << "ProcessParser::getNumberOfRunningProcesses() called" << "\n";
    std::cout << ProcessParser::getNumberOfRunningProcesses() << "\n";

    std::cout << "ProcessParser::isPidExisting(some pid) called" << "\n";
    std::cout << ProcessParser::isPidExisting(pid) << "\n";

    std::cout << "ProcessParser::isPidExisting(\"10000000\") called" << "\n";
    std::cout << ProcessParser::isPidExisting("10000000") << "\n";

    SysInfo mySystem = SysInfo();

    std::cout << "SysInfo.getOSName() called" << "\n";
    std::cout << mySystem.getOSName() << "\n";

    std::cout << "SysInfo.getCpuPercent() called" << "\n";
    std::cout << mySystem.getCpuPercent() << "\n";
    sleep(1);
    mySystem.setAttributes();
    std::cout << mySystem.getCpuPercent() << "\n";

    std::cout << "SysInfo.getProcessorsStats() called" << "\n";
    for ( auto elm : mySystem.getProcessorsStats())
        std::cout << elm << "\n";

    std::cout << "SysInfo.getKernelVersion() called" << "\n";
    std::cout << mySystem.getKernelVersion() << "\n";

    std::cout << "SysInfo.getMemPercent() called" << "\n";
    std::cout << mySystem.getMemPercent() << "\n";

    std::cout << "SysInfo.getRunningProc() called" << "\n";
    std::cout << mySystem.getRunningProc() << "\n";

    std::cout << "SysInfo.getThreads() called" << "\n";
    std::cout << mySystem.getThreads() << "\n";

    std::cout << "SysInfo.getTotalProc() called" << "\n";
    std::cout << mySystem.getTotalProc() << "\n";

    std::cout << "SysInfo.getUpTime() called" << "\n";
    std::cout << mySystem.getUpTime() << "\n";

    Process myProcess = Process(pid);
    std::cout << "myProcess.getUpTime() called" << "\n";
    std::cout << myProcess.getUpTime() << "\n";
    std::cout << "myProcess.getCmd() called" << "\n";
    std::cout << myProcess.getCmd() << "\n";
    std::cout << "myProcess.getCpu() called" << "\n";
    std::cout << myProcess.getCpu() << "\n";
    std::cout << "myProcess.getMem() called" << "\n";
    std::cout << myProcess.getMem() << "\n";
    std::cout << "myProcess.getPid() called" << "\n";
    std::cout << myProcess.getPid() << "\n";
    std::cout << "myProcess.getProcess() called" << "\n";
    std::cout << myProcess.getProcess().size() << "\n";
    std::cout << "myProcess.getUser() called" << "\n";
    std::cout << myProcess.getUser() << "\n";

    ProcessContainer myContainer = ProcessContainer();
    myContainer.refreshList();
    std::cout << "myContainer.getList() called" << "\n";

    for( auto elm : myContainer.getList())
        for (auto str : elm)
            std::cout << str << "\n";
    std::cout << "\n";
}