#include "processor.h"

#include <iostream>

#include "linux_parser.h"

Processor::Processor() {
  cpustats_ = LinuxParser::CpuUtilization();
  this->SetJiffies();
  this->SetActiveJiffies();
  this->SetIdleJiffies();
  total_processes_ = std::stoi(cpustats_[10]);
  running_processes_ = std::stoi(cpustats_[11]);
}

void Processor::SetJiffies() {
  for (int idx = LinuxParser::CPUStates::kUser_;
       idx <= LinuxParser::CPUStates::kGuestNice_; idx++) {
    jiffies_ += std::stoul(cpustats_[idx]);
  }
}

void Processor::SetActiveJiffies() {
  active_jiffies_ = std::stol(cpustats_[LinuxParser::CPUStates::kUser_]) +
                    std::stol(cpustats_[LinuxParser::CPUStates::kNice_]) +
                    std::stol(cpustats_[LinuxParser::CPUStates::kSystem_]) +
                    std::stol(cpustats_[LinuxParser::CPUStates::kIRQ_]) +
                    std::stol(cpustats_[LinuxParser::CPUStates::kSoftIRQ_]) +
                    std::stoul(cpustats_[LinuxParser::CPUStates::kSteal_]);
}

void Processor::SetIdleJiffies() {
  prev_idle_jiffies_ = idle_jiffies_;
  idle_jiffies_ = std::stoul(cpustats_[LinuxParser::CPUStates::kIdle_]) +
                  std::stoul(cpustats_[LinuxParser::CPUStates::kIOwait_]);
}

// TODO: Update the aggregate CPU utilization with approximate real-time
// utlization
float Processor::Utilization() {
  // PrevIdle = previdle + previowait
  // Idle = idle + iowait

  // PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq +
  // prevsteal NonIdle = user + nice + system + irq + softirq + steal

  // PrevTotal = PrevIdle + PrevNonIdle
  // Total = Idle + NonIdle

  // # differentiate: actual value minus the previous one
  // totald = Total - PrevTotal
  // idled = Idle - PrevIdle

  // CPU_Percentage = (totald - idled)/totald
  prev_total_ = total_;

  total_ = idle_jiffies_ + active_jiffies_;
  unsigned long delta_total = total_ - prev_total_;
  unsigned long delta_idle = idle_jiffies_ - prev_idle_jiffies_;
  return float(delta_total - delta_idle) / delta_total;
}
