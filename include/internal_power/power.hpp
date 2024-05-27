#pragma once

#include <vector>
namespace internal_power
{
class Device
{
public:
	Device() {};
	// reads momentary power in Watt
	virtual double read_power() = 0;
};

std::vector<Device> detect_devices();
}
