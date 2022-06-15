/**
 * @file process.cpp
 * @author Mina-Ryad (mina.hany.kameel.ryad@gmail.com)
 * @brief
 * @version 2.0
 * @date 06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

/**
 * @brief set private pid_
 *
 * @param pid
 */
void Process::set_PID(int pid) { pid_ = pid; }

/**
 * @brief get private pid_
 *
 * @return int
 */
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  cpu_util_ = ((float)LinuxParser::ActiveJiffies(pid_) / 100.0);

  return cpu_util_;
}

/**
 * @brief Return the command that generated this process from private command_
 *
 * @return std::string
 */
std::string Process::Command() {
  command_ = LinuxParser::Command(pid_);
  command_.resize(40);
  command_.append("...");
  return command_;
}

/**
 * @brief Return this process's memory utilization from private ram_
 *
 * @return std::string
 */
std::string Process::Ram() {
  ram_ = LinuxParser::Ram(pid_);
  return ram_;
}

/**
 * @brief Return the user (name) that generated this process from private user_
 *
 * @return std::string
 */
std::string Process::User() {
  user_ = LinuxParser::User(pid_);
  return user_;
}

/**
 * @brief Return the age of this process (in seconds) from private uptime_
 *
 * @return long int
 */
long int Process::UpTime() {
  upTime_ = LinuxParser::UpTime(pid_);
  return upTime_;
}

/**
 * @brief Overload the "less than" comparison operator for Process objects
 *
 * @param a
 * @return true
 * @return false
 */
bool Process::operator<(Process const& a) const {
  return cpu_util_ > a.cpu_util_;
}