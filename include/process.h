#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;
  void ReadStats();
  void SetCpuUtilization();
  void ReadStatus();
  void SetUser();
  void SetCommand();

 private:
  int pid_{0};
  long int utime_{0}, stime_{0}, cutime_{0}, cstime_{0}, starttime_{0},
      uptime_{0};
  float cpuutil_ = 0.0;
  std::string command_{}, memutil_{}, user_{}, uid_{};
};

#endif