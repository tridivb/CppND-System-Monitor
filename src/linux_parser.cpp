#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

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

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  // auto cpustats = LinuxParser::CpuUtilization(pid);
  // return std::stol(cpustats[LinuxParser::CPUStates::kUser_]) +
  //        std::stol(cpustats[LinuxParser::CPUStates::kNice_]) +
  //        std::stol(cpustats[LinuxParser::CPUStates::kSystem_]) +
  //        std::stol(cpustats[LinuxParser::CPUStates::kIRQ_]) +
  //        std::stol(cpustats[LinuxParser::CPUStates::kSoftIRQ_]);
  return 0;
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

int LinuxParser::TotalProcesses() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stoi(cpustats[10]);
}

int LinuxParser::RunningProcesses() {
  auto cpustats = LinuxParser::CpuUtilization();
  return std::stoi(cpustats[11]);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
