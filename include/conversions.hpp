#pragma once
#include <string>
constexpr const char* units[] = {
    "bps",
    "Kbps",
    "Mbps",
    "Gbps",
    "Tbps"
};

//number of units in the units array
constexpr int total_units = sizeof(units) / sizeof(units[0]);

/**
 * @brief convert the bit rate into the correct unit based on size
 * @param bits total bits received or sent
 * @param speed_unit the appropriate unit based on speed
 */

void formatSpeed(double &bits,std::string &speed_unit);