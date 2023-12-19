#include "process.h"

#include <unistd.h>

#include <cctype>
#include <cmath>
// #include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  // initialize values only the process is still running else default
  // filesystem doesn't work on the udacity workspace
  // if (std::filesystem::exists(LinuxParser::kProcDirectory + to_string(pid)))
  // {
  pid_ = pid;
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  user_ = LinuxParser::User(pid_);
  SetCpuUtilization();
  // }
}

int Process::Pid() { return pid_; }

void Process::SetCpuUtilization() {
  prev_total = total_;
  uptime_ = LinuxParser::UpTime(pid_);
  auto total = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  if (uptime_ > 0) {
    cpuutil_ = float(total - prev_total) / uptime_;
  }
}

float Process::CpuUtilization() { return cpuutil_; }

string Process::Command() {
  if (command_.size() <= 50) {
    return command_;
  } else {
    return command_.substr(0, 50) + "...";
  }
}

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const {
  return a.cpuutil_ < this->cpuutil_;
}