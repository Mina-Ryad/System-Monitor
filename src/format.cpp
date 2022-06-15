#include "format.h"

#include <iostream>
#include <string>

using std::stoi;
using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string Time;
  uint32_t hours = 0;
  uint32_t minutes = 0;
  uint32_t seconds2 = 0;
  hours = ((seconds / 3600) % 24);
  minutes = (seconds / 60) % 60;
  seconds2 = seconds % 60;

  Time.push_back((hours / 10) + 48);
  Time.push_back((hours % 10) + 48);

  Time.push_back(':');

  Time.push_back((minutes / 10) + 48);
  Time.push_back((minutes % 10) + 48);

  Time.push_back(':');

  Time.push_back((seconds2 / 10) + 48);
  Time.push_back((seconds2 % 10) + 48);

  return Time;
}