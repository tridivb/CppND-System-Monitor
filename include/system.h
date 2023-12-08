#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

class System {
 public:
  void Cache();
  Processor& Cpu();
  std::vector<Process>& Processes();
  void SetProcesses();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses() const;
  int RunningProcesses() const;
  std::string Kernel();
  std::string OperatingSystem();

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string os_;
  std::string kernel_;
  long uptime_ = 0;
  float memutil_ = 0.0;
};

#endif