#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid), uptime_(LinuxParser::UpTime()) {
  ReadStats();
  SetCpuUtilization();
  ReadStatus();
  SetUser();
  SetCommand();
}

int Process::Pid() { return pid_; }

void Process::ReadStats() {
  std::ifstream statstream(LinuxParser::kProcDirectory + to_string(pid_) +
                           LinuxParser::kStatFilename);
  if (statstream.is_open()) {
    string line;
    std::getline(statstream, line);
    std::istringstream linestream(line);
    string value;
    for (int idx = 0; idx < 22; idx++) {
      linestream >> value;
      switch (idx) {
        case 1:
          command_ = value.substr(1, value.size() - 2);
          break;
        case 13:
          utime_ = std::stol(value);
          break;
        case 14:
          stime_ = std::stol(value);
          break;
        case 15:
          cutime_ = std::stol(value);
          break;
        case 16:
          cstime_ = std::stol(value);
          break;
        case 21:
          starttime_ = std::stol(value);
          break;
        default:
          break;
      }
    }
  }
}

void Process::SetCpuUtilization() {
  auto total = utime_ + stime_ + cutime_ + cstime_;
  auto seconds = uptime_ - long(starttime_ / sysconf(_SC_CLK_TCK));
  cpuutil_ = float(total / sysconf(_SC_CLK_TCK)) / seconds;
}

void Process::ReadStatus() {
  std::ifstream statustream(LinuxParser::kProcDirectory + to_string(pid_) +
                            LinuxParser::kStatusFilename);
  if (statustream.is_open()) {
    string line;
    while (std::getline(statustream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string key;
      linestream >> key;
      if (key == "Uid") {
        linestream >> uid_;
      } else if (key == "VmSize") {
        linestream >> memutil_;
        // convert kB to mB
        memutil_ = to_string(std::stol(memutil_) / 1024);
        break;
      }
    }
  }
}

void Process::SetUser() {
  std::ifstream passstream(LinuxParser::kPasswordPath);
  if (passstream.is_open()) {
    string line;
    while (std::getline(passstream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string user, uid, pass;
      linestream >> user >> pass >> uid;
      if (uid == uid_) {
        user_ = user;
        break;
      }
    }
  }
}

void Process::SetCommand() {
  std::ifstream cmdstream(LinuxParser::kProcDirectory + to_string(pid_) +
                          LinuxParser::kCmdlineFilename);
  if (cmdstream.is_open()) {
    std::getline(cmdstream, command_);
  }
}

// TODO: Return this process's updated CPU utilization instead of aggregate
float Process::CpuUtilization() { return cpuutil_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return memutil_; }

string Process::User() { return user_; }

long int Process::UpTime() {
  return uptime_ - long(starttime_ / sysconf(_SC_CLK_TCK));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return a.cpuutil_ < this->cpuutil_;
}