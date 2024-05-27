#pragma once
#include <vector>

#include <internal_power/power.hpp>

namespace internal_power
{
namespace intel
{

class Device : public internal_power::Device
{
	Device();
	double read_power() override;
};

std::vector<internal_power::Device> detect();
}
}
