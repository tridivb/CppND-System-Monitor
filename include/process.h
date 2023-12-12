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

  void SetCpuUtilization();

 private:
  int pid_ = 0;
  float cpuutil_ = 0.0;
  long int uptime_{0}, total_{0}, prev_total{0};
  std::string command_{}, ram_{}, user_{};
};

#endif