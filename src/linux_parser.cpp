#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Check if there is a more accurate memory util calculation
float LinuxParser::MemoryUtilization() {
  string key, value, line;
  double memtotal, memfree;
  std::ifstream memfilestream(kProcDirectory + kMeminfoFilename);
  if (memfilestream.is_open()) {
    while (std::getline(memfilestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memtotal = std::stold(value);
        } else if (key == "MemFree") {
          memfree = std::stold(value);
          return float((memtotal - memfree) / memtotal);
        }
      }
    }
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  std::ifstream uptimestream(kProcDirectory + kUptimeFilename);
  if (uptimestream.is_open()) {
    string line;
    double uptime;
    std::getline(uptimestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return long(uptime);
  }

  return 0;
}

long LinuxParser::Jiffies() {
  auto cpustats = LinuxParser::CpuUtilization();
  unsigned long jiffies = 0;
  for (auto& stat : cpustats) {
    jiffies += std::stoul(stat);
  }
  return jiffies;
}

long LinuxParser::ActiveJiffies() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stol(cpustats[LinuxParser::CPUStates::kUser_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kNice_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kSystem_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kIRQ_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kSoftIRQ_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kSteal_]);
}

long LinuxParser::IdleJiffies() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stol(cpustats[LinuxParser::CPUStates::kIdle_]) +
         std::stol(cpustats[LinuxParser::CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  std::ifstream statstream(kProcDirectory + kStatFilename);
  vector<string> cpustats{};
  if (statstream.is_open()) {
    string line;
    while (std::getline(statstream, line)) {
      std::istringstream linestream(line);
      string key, value;
      linestream >> key;
      if (key == "cpu") {
        while (linestream >> value) {
          cpustats.push_back(value);
        }
      } else if (key == "processes" || key == "procs_running") {
        linestream >> value;
        cpustats.push_back(value);
      }
    }
  }
  return cpustats;
}

vector<string> LinuxParser::CpuUtilization(int pid) {
  std::vector<string> processstats{};
  std::ifstream statstream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  std::vector<int> indices{13, 14, 15, 16, 21};
  if (statstream.is_open()) {
    string line;
    std::getline(statstream, line);
    std::istringstream linestream(line);
    string value;
    for (int idx = 0; idx < 22; idx++) {
      linestream >> value;
      if (std::find(indices.begin(), indices.end(), idx) != indices.end()) {
        processstats.push_back(value);
      }
    }
  }
  return processstats;
}

long LinuxParser::ActiveJiffies(int pid) {
  auto procstats = LinuxParser::CpuUtilization(pid);
  return std::stol(procstats[LinuxParser::ProcCPUStates::kPUser_]) +
         std::stol(procstats[LinuxParser::ProcCPUStates::kPKernel_]) +
         std::stol(procstats[LinuxParser::ProcCPUStates::kCPKernel_]) +
         std::stol(procstats[LinuxParser::ProcCPUStates::kCPUser_]);
}

int LinuxParser::TotalProcesses() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stoi(cpustats[10]);
}

int LinuxParser::RunningProcesses() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stoi(cpustats[11]);
}

string LinuxParser::Command(int pid) {
  string command{};
  std::ifstream cmdstream(LinuxParser::kProcDirectory + to_string(pid) +
                          LinuxParser::kCmdlineFilename);
  if (cmdstream.is_open()) {
    std::getline(cmdstream, command);
  }
  return command;
}

vector<string> LinuxParser::ReadProcessStatus(int pid) {
  vector<string> pidstatus{};
  std::ifstream statustream(LinuxParser::kProcDirectory + to_string(pid) +
                            LinuxParser::kStatusFilename);
  if (statustream.is_open()) {
    string line;
    while (std::getline(statustream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string key, value;
      linestream >> key;
      if (key == "Uid") {
        linestream >> value;
        pidstatus.push_back(value);
      } else if (key == "VmSize") {
        linestream >> value;
        // convert kB to mB
        pidstatus.push_back(to_string(std::stol(value) / 1024));
        break;
      }
    }
  }
  return pidstatus;
}

string LinuxParser::Ram(int pid) {
  auto status = LinuxParser::ReadProcessStatus(pid);
  if (status.size() == 2) {
    return status[1];
  }
  return to_string(0);
}

string LinuxParser::Uid(int pid) {
  auto status = LinuxParser::ReadProcessStatus(pid);
  if (status.size()) {
    return status[0];
  }
  return string();
}

string LinuxParser::User(int pid) {
  auto uid = LinuxParser::Uid(pid);
  string user{};
  std::ifstream passstream(LinuxParser::kPasswordPath);
  if (passstream.is_open()) {
    string line;
    while (std::getline(passstream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string userid, pass;
      linestream >> user >> pass >> userid;
      if (userid == uid) {
        return user;
      }
    }
  };
  return user;
}

long LinuxParser::UpTime(int pid) {
  auto procstats = LinuxParser::CpuUtilization(pid);
  return LinuxParser::UpTime() -
         long(std::stol(procstats[LinuxParser::ProcCPUStates::kPStart_]) /
              sysconf(_SC_CLK_TCK));
}
