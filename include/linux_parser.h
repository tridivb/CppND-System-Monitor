#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
const std::string kOSName("PRETTY_NAME");
const std::string kMemTotal("MemTotal");
const std::string kMemFree("MemFree");
const std::string kCPU("cpu");
const std::string kTotalProcess("processes");
const std::string kRunningProcess("procs_running");
const std::string kUid("Uid");
// const std::string kRam("VmSize");
// VmRSS provides RAM allocation
const std::string kRam("VmRSS");

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
enum ProcCPUStates { kPUser_ = 0, kPKernel_, kCPUser_, kCPKernel_, kPStart_ };
std::vector<std::string> CpuUtilization();
std::vector<std::string> CpuUtilization(int pid);
long Jiffies();
long ActiveJiffies(int pid);
long ActiveJiffies();
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);

};  // namespace LinuxParser

#endif