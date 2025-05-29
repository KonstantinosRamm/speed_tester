#include "conversions.hpp"



void formatSpeed(double &bits, std::string &speed_unit) {
    int speed_unit_index = 0;

    while (bits >= 1024 && speed_unit_index < total_units - 1) {
        bits /= 1024;
        speed_unit_index++;
    }

    speed_unit = units[speed_unit_index];
}
