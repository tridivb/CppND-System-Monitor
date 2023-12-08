#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp
  void SetJiffies();
  void SetActiveJiffies();
  void SetIdleJiffies();

  unsigned int total_processes_ = 0;
  unsigned int running_processes_ = 0;

  // TODO: Declare any necessary private members
 private:
  std::vector<std::string> cpustats_{};
  unsigned long jiffies_{0};
  unsigned long active_jiffies_{0};
  unsigned long idle_jiffies_{0}, prev_idle_jiffies_{0};
  unsigned long total_{0}, prev_total_{0};
};

#endif