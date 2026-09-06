#ifndef SENSORCURRENTTIMELOGIC_H
#define SENSORCURRENTTIMELOGIC_H

#include <string>

// Pure, hardware-free logic extracted from SensorCurrentTime so it can be
// unit-tested on host without the CurrentDateTimeController singleton.
namespace SensorCurrentTimeLogic {

// Formats hour/minute/second as "HH:MM:SS", zero-padded to 2 digits each.
std::string formatHms(int hour, int minute, int second);

} // namespace SensorCurrentTimeLogic

#endif
