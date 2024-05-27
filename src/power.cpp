#include <internal_power/intel/intel.hpp>
#include <internal_power/power.hpp>
namespace internal_power
{
std::vector<Device> detect_devices()
{
    return intel::detect();
}
} // namespace internal_power
