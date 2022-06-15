/**
 * @file system.cpp
 * @author Mina-Ryad (mina.hany.kameel.ryad@gmail.com)
 * @brief
 * @version 2.0
 * @date 06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

/**
 * @brief return System object cpu_.
 *
 * @return Processor&
 */
Processor& System::Cpu() { return cpu_; }

/**
 * @brief return vector of Sorted processes by Cpu Utiliaztion.
 *
 * @return std::vector<Process>&
 */
std::vector<Process>& System::Processes() {
  std::vector<int> pid_int;
  pid_int = LinuxParser::Pids();
  Process ps;
  if (init == 0) {
    for (uint32_t i = 0; i < pid_int.size(); i++) {
      ps.set_PID(pid_int[i]);
      ps.Command();
      ps.CpuUtilization();
      ps.Ram();
      ps.UpTime();
      ps.User();
      processes_.emplace_back(ps);
    }
  }
  init = 1;
  sort(processes_.begin(), processes_.end(),
       [](Process& a, Process& b) { return a < b; });
  return processes_;
}

/**
 * @brief Return the system's kernel identifier
 *
 * @return std::string
 */
std::string System::Kernel() { return LinuxParser::Kernel(); }

/**
 * @brief Return the system's memory utilization
 *
 * @return float
 */
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

/**
 * @brief Return the operating system name
 *
 * @return std::string
 */
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

/**
 * @brief Return the number of processes actively running on the system
 *
 * @return int
 */
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

/**
 * @brief Return the total number of processes on the system
 *
 * @return int
 */
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

/**
 * @brief Return the number of seconds since the system started running
 *
 * @return long int
 */
long int System::UpTime() { return LinuxParser::UpTime(); }
