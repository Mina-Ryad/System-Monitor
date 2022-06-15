/**
 * @file linux_parser.cpp
 * @author Mina-Ryad (mina.hany.kameel.ryad@gmail.com)
 * @brief Parsing proc directory
 * @version 3.0
 * @date 06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Get OS name.
 *
 * @return std::string
 */
std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
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
  filestream.close();
  return value;
}

/**
 * @brief Get kernel name.
 *
 * @return std::string
 */
std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

/**
 * @brief Get Ppids from file names in directory proc.
 *
 * @return std::vector<int>
 */
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

/**
 * @brief Get Mmemory utilization for the system.
 *
 * @return float
 */
float LinuxParser::MemoryUtilization() {
  std::string line;
  std::string key;
  std::string value;
  uint64_t memTotal = 0;
  uint64_t memFree = 0;
  uint64_t totalUsed = 0;
  uint64_t Buffers = 0;
  uint64_t Cached = 0;
  uint64_t SReclaimable = 0;
  uint64_t Shmem = 0;
  uint64_t NonCached = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterMemTotalString) {
          memTotal = std::stoi(value);
        } else if (key == filterMemFreeString) {
          memFree = std::stoi(value);
        } else if (key == filterBuffersString) {
          Buffers = std::stoi(value);
        } else if (key == filterCachedString) {
          Cached = std::stoi(value);
        } else if (key == filterSReclaimableString) {
          SReclaimable = std::stoi(value);
        } else if (key == filterShmemString) {
          Shmem = std::stoi(value);
        }
      }
    }
  }
  totalUsed = memTotal - memFree;
  // Yellow in HTop
  Cached = Cached + SReclaimable - Shmem;
  // Green in HTop
  NonCached = totalUsed - (Buffers + Cached);
  // Blue --> Buffers
  filestream.close();
  return (float)((double)NonCached / ((double)memTotal));
}

/**
 * @brief Get up time for the machine.
 *
 * @return long
 */
long LinuxParser::UpTime() {
  std::string line;
  std::string value1;
  uint32_t UpTime_Total = 0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value1;
    UpTime_Total = std::stoi(value1);
  }
  filestream.close();
  return (long)UpTime_Total;
}

/**
 * @brief Calculate total jiffies for the machine.
 *
 * @return long
 */
long LinuxParser::Jiffies() {
  uint32_t TotalJiffies =
      LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();

  return (long)TotalJiffies;
}

/**
 * @brief Get active jiffies for certain process
 *
 * @param pid
 * @return long
 */
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::string key;
  uint32_t uptime{0};
  uint32_t utime{0};
  uint32_t stime{0};
  uint32_t cutime{0};
  uint32_t cstime{0};
  uint32_t starttime{0};
  const uint32_t Hertz = sysconf(_SC_CLK_TCK);
  float seconds{0};
  uint32_t cpu_usage{0};
  uint32_t total_time{0};

  uint8_t counter{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        counter++;
        if (counter == 14) {
          utime = std::stoi(key);
        } else if (counter == 15) {
          stime = std::stoi(key);
        } else if (counter == 16) {
          cutime = std::stoi(key);
        } else if (counter == 17) {
          cstime = std::stoi(key);
        } else if (counter == 22) {
          starttime = std::stoi(key);
        }
      }
    }
  }
  counter = 0;

  /* get uptime for the system */
  uptime = LinuxParser::UpTime();

  /* First we determine the total time spent for the process: */
  total_time = utime + stime;

  /* We also have to decide whether we want to include the
     time from children processes. If we do, then we add those values to
     total_time */
  total_time = total_time + cutime + cstime;

  /* Next we get the total elapsed time in seconds since the process
   * started:*/
  seconds = uptime - (starttime / Hertz);

  // // Finally we calculate the CPU usage percentage:
  cpu_usage = 100 * ((total_time / Hertz) / seconds);

  filestream.close();
  return (long)cpu_usage;
}

/**
 * @brief Get active jiffies for the machine.
 *
 * @return long
 */
long LinuxParser::ActiveJiffies() {
  std::string line;
  std::string key;
  std::string norm_user;
  std::string nice_user;
  std::string proc_kernel;
  long Active{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> norm_user >> nice_user >> proc_kernel;
  }
  Active = (stoi(norm_user) + std::stoi(nice_user) + std::stoi(proc_kernel));

  filestream.close();
  return Active;
}

/**
 * @brief Get idle jiffies for the machine.
 *
 * @return long
 */
long LinuxParser::IdleJiffies() {
  std::string line;
  std::string key;
  std::string norm_user;
  std::string nice_user;
  std::string proc_kernel;
  std::string idle;
  uint32_t Idle = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> norm_user >> nice_user >> proc_kernel >> idle;
    Idle = std::stoi(idle);
  }

  filestream.close();
  return (long)Idle;
}

/**
 * @brief Calculate avarege CPU utilization with time span 1 second.
 *
 * @return std::vector<std::string>
 */
std::vector<std::string> LinuxParser::CpuUtilization() {
  uint32_t Idle;
  uint32_t totalUtil = 0;
  std::vector<std::string> ret_vs;
  for (int i = 0; i < 2; i++) {
    std::ifstream filestream(kProcDirectory + kStatFilename);

    totalUtil = LinuxParser::Jiffies();
    ret_vs.push_back(std::to_string(totalUtil));

    Idle = LinuxParser::IdleJiffies();
    ret_vs.push_back(std::to_string(Idle));
    sleep(1);
    filestream.close();
  }
  return ret_vs;
}

/**
 * @brief Get total processes for the machine.
 *
 * @return int
 */
int LinuxParser::TotalProcesses() {
  std::string line;
  std::string key;
  std::string value;
  int processes = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterProcesses) {
          processes = std::stoi(value);
          break;
        }
      }
    }
  }
  filestream.close();
  return (int)processes;
}

/**
 * @brief Get running processes for the machine.
 *
 * @return int
 */
int LinuxParser::RunningProcesses() {
  std::string line;
  std::string key;
  std::string value;
  int procs_running = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterRunningProcesses) {
          procs_running = std::stoi(value);
          break;
        }
      }
    }
  }
  filestream.close();
  return (int)procs_running;
}

/**
 * @brief Get the command for certain process.
 *
 * @param pid
 * @return std::string
 */
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::string cmdline;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cmdline;
  }
  filestream.close();
  return cmdline;
}

/**
 * @brief Get RAM usage for certain process.
 *
 * @param pid
 * @return std::string
 */
std::string LinuxParser::Ram(int pid) {
  std::string line;
  std::string key;
  std::string value;
  std::string VmRSS;
  uint32_t VmRSS_Mb{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterProcMem) {
          VmRSS_Mb = std::stoi(value) / 1024;
          break;
        }
      }
    }
  }

  /* VmRSS gives the exact physical memory being used as a part of Physical RAM
   */
  VmRSS = std::to_string(VmRSS_Mb);
  return VmRSS;
}

/**
 * @brief Get user ID for ceratin process.
 *
 * @param pid
 * @return std::string
 */
std::string LinuxParser::Uid(int pid) {
  std::string line;
  std::string key;
  std::string value;
  std::string uid_str;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterUID) {
          uid_str = value;
          break;
        }
      }
    }
  }
  filestream.close();
  return uid_str;
}

/**
 * @brief Get username from user ID.
 *
 * @param pid
 * @return std::string
 */
std::string LinuxParser::User(int pid) {
  std::string uidStr;
  std::string line;
  std::string key;
  std::string value1;
  std::string value2;
  std::string user;
  uidStr = LinuxParser::Uid(pid);
  std::ifstream filestream("/etc/passwd");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value1 >> value2) {
        if (value2 == uidStr) {
          user = key;
          break;
        }
      }
    }
  }
  filestream.close();
  return user;
}

/**
 * @brief Get uptime for certain process.
 *
 * @param pid
 * @return long
 */
long LinuxParser::UpTime(int pid) {
  std::string line;
  std::string key;
  uint32_t uptime{0};
  uint8_t counter{0};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      counter++;
      if (counter == 22) {
        uptime = std::stoi(key);
        break;
      }
    }
  }
  filestream.close();

  /* Subtracting from the system uptime */
  uptime = LinuxParser::UpTime() - (uptime / sysconf(_SC_CLK_TCK));
  return (long)uptime;
}
