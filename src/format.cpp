#include "format.h"

#include <string>

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  long hours, minutes;
  string time_format = "";
  hours = long(seconds / 3600);
  minutes = long((seconds % 3600) / 60);
  seconds = seconds % 60;
  time_format =
      to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
  return time_format;
}