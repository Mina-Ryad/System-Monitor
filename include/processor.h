#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  std::vector<std::string> rec_vs;
  float total = 0;
  float prevtotal = 0;
  float idle = 0;
  float previdle = 0;
  float CPU_util = 0;
};

#endif