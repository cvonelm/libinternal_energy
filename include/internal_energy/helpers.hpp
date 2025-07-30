#pragma once

#include <regex>

#include <internal_energy/topology.hpp>
#include <stdexcept>
#include <string>
namespace internal_energy
{
Subsystem parse_subsystem(std::string subsystem)
{
    if (subsystem == "perf" || subsystem == "PERF")
    {
        return Subsystem::PERF;
    }
    if (subsystem == "hwmon" || subsystem == "HWMON")
    {
        return Subsystem::HWMON;
    }
    if (subsystem == "cuda" || subsystem == "CUDA")
    {
        return Subsystem::CUDA;
    }

    if (subsystem == "nec" || subsystem == "NEC")
    {
        return Subsystem::NEC;
    }

    if (subsystem == "rocm" || subsystem == "ROCM")
    {
        return Subsystem::ROCM;
    }

    if (subsystem == "msr" || subsystem == "MSR")
    {
        return Subsystem::MSR;
    }

    if (subsystem == "*")
    {
        return Subsystem::ANY;
    }

    throw std::runtime_error(fmt::format("Could not parse {} as subsystem!", subsystem));
}

std::tuple<std::string, std::string, std::string> split_metric_str(std::string metric_str)
{
    std::regex split_regex("([^:]+)::([^:]+)::([^:]+)");

    std::smatch match;
    if (std::regex_match(metric_str, match, split_regex))
    {
        return { match[1], match[2], match[3] };
    }
    else
    {
        throw std::runtime_error(fmt::format("Not a Metric String: {}", metric_str));
    }
}

} // namespace internal_energy
