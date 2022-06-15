/**
 * @file processor.cpp
 * @author Mina-Ryad (mina.hany.kameel.ryad@gmail.com)
 * @brief
 * @version 2.0
 * @date 06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

/**
 * @brief Return the aggregate CPU utilization by calculating average in 1
 * second.
 *
 * @return float
 */
float Processor::Utilization() {
  std::vector<std::string> rec_vs;
  rec_vs = LinuxParser::CpuUtilization();

  previdle = std::stof(rec_vs[1]);
  prevtotal = std::stof(rec_vs[0]) + previdle;
  idle = std::stof(rec_vs[3]);
  total = std::stof(rec_vs[2]) + idle;

  CPU_util =
      ((((total - prevtotal) - (idle - previdle)) / (total - prevtotal)));

  return CPU_util;
}