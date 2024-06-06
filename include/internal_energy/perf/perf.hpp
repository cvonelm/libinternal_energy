#pragma once
#include <cstdint>
#include <string>

#include <internal_energy/energy.hpp>

struct UserspaceReadFormat
{
    UserspaceReadFormat() : value(0), time_enabled(0), time_running(0)
    {
    }
    uint64_t value;
    uint64_t time_enabled;
    uint64_t time_running;
};

namespace internal_energy
{
namespace perf
{
class PerfCounter
{
public:
    PerfCounter(uint64_t type, uint64_t config, uint64_t config1, std::string name);
    ~PerfCounter();

protected:
    int fd_;
};

class RaplAmdPkgCounter : public PerfCounter
{
public:
    RaplAmdPkgCounter();
    double read_energy();
};

} // namespace perf
} // namespace internal_energy
