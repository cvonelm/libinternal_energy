#pragma once

#include "internal_energy/topology.hpp"
#include "perf-cpp/topology.hpp"
#include <internal_energy/metric.hpp>
#include <internal_energy/msr/cpuinfo.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
extern "C"
{
#include <fcntl.h>
}
namespace internal_energy
{
namespace msr
{

class MetricSource : public internal_energy::MetricSource
{
public:
    MetricSource(Location l) : internal_energy::MetricSource(l)
    {
    }
    Subsystem get_subsystem() const override
    {
        return Subsystem::MSR;
    }
};

class MetricInstance : public internal_energy::MetricInstance
{
public:
    MetricInstance(const msr::MetricSource* src) : internal_energy::MetricInstance(src)
    {
        std::string path = "";

        if (src->get_location().is<Core>())
        {
            path = fmt::format("/dev/cpu/{}/msr",
                               src->get_location().as<Core>().measuring_cpu().as_int());
        }
        else if (src->get_location().is<Package>())
        {
            path = fmt::format("/dev/cpu/{}/msr",
                               src->get_location().as<Package>().measuring_cpu().as_int());
        }
        else
        {
            throw std::runtime_error(
                fmt::format("Location for MSR Metric must be Package or CPU, not {}",
                            src->get_location().str()));
        }
        fd_ = open(path.c_str(), O_RDONLY);
        if (fd_ == -1)
        {
            throw std::runtime_error(fmt::format("Couldn't open MSR for CPU {}: {}",
                                                 src->get_location().str(), strerror(errno)));
        }
    }
    uint64_t rdmsr(uint64_t address, uint64_t upper, uint64_t lower, bool overflows = false)
    {
        uint64_t res;
        pread(fd_, &res, 8, address);
        res = res << (63 - upper);
        res = res >> (63 - (upper - lower));

        if (overflows)
        {
            /*
             * Sometimes, pretty smol counters are used for MSRs.
             * AMD, for example on older generations uses only
             * 32 bit for the energy counter.
             *
             * For a very modest energy consumption of 2 Joules a second)
             * (As can be seen on a laptop), the counter rolls over in ca. 76 days
             * Very long but far from an eternity, especially as the counter is
             * every only reset on boot.
             *
             * For counters that are marked `overflows`, cache the old values and
             * check new values against them.
             * If we notice an overflow increase the overflow counter and address
             * UINT(UPPER-LOWER)_MAX to int
             */
            if (num_overflows.count(address) == 0)
            {
                num_overflows[address] = 0;
            }

            if (last_read.count(address) == 0)
            {
                last_read[address] = 0;
            }

            if (last_read.at(address) > res)
            {
                num_overflows.at(address)++;
            }

            return res + num_overflows.at(address) * ((2 << (upper - lower)) - 1);
        }
        return res;
    }

protected:
    std::map<uint64_t, uint64_t> num_overflows;
    std::map<uint64_t, uint64_t> last_read;
    int fd_;
};

std::vector<std::unique_ptr<msr::MetricSource>> get_all_metrics();
} // namespace msr

} // namespace internal_energy
