#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

void System::Cache() {
  os_ = LinuxParser::OperatingSystem();
  kernel_ = LinuxParser::Kernel();
  cpu_ = Processor();
  uptime_ = LinuxParser::UpTime();
  memutil_ = LinuxParser::MemoryUtilization();
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { return processes_; }

std::string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return memutil_; }

std::string System::OperatingSystem() { return os_; }

int System::RunningProcesses() const { return cpu_.running_processes_; }

int System::TotalProcesses() const { return cpu_.total_processes_; }

long int System::UpTime() { return uptime_; }

void System::SetProcesses() {
  // vector<int> pids = LinuxParser::Pids();
  // for (auto& pid : pids){
  //     processes_.push_back(Process(pid))
  // }
}