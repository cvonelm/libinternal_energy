#include <internal_power/power.hpp>
#include <internal_power/intel/intel.hpp>
namespace internal_power
{
std::vector<Device> detect_devices()
{
	return intel::detect();
}
}
