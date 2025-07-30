#pragma once

#include <cstdint>
#include <fstream>
#include <regex>
#include <stdexcept>

#include <fmt/core.h>
namespace internal_energy
{
namespace msr
{
enum class Vendor
{
    AMD,
    INTEL,
    UNKNOWN
};
class CpuInfo
{
public:
    static const CpuInfo& instance()
    {
        static CpuInfo c = CpuInfo();
        return c;
    }

    uint64_t family() const
    {
        return family_;
    }

    Vendor vendor() const
    {
        return vendor_;
    }

private:
    CpuInfo()
    {
        std::ifstream cpuinfo_file("/proc/cpuinfo");

        std::string line;
        while (std::getline(cpuinfo_file, line))
        {
            std::regex vendor_regex("vendor_id\\s+:\\s+(\\w+)");
            std::smatch vendor_match;
            if (std::regex_match(line, vendor_match, vendor_regex))
            {
                if (vendor_match[1] == "AuthenticAMD")
                {
                    vendor_ = Vendor::AMD;
                }
                else if (vendor_match[1] == "GenuineIntel")
                {
                    vendor_ = Vendor::INTEL;
                }
                else
                {
                    throw std::runtime_error(
                        fmt::format("Unknown CPU Vendor: {}!", vendor_match[1].str()));
                }
            }

            std::regex family_regex("cpu family\\s+:\\s+([0-9]+)");
            std::smatch family_match;

            if (std::regex_match(line, family_match, family_regex))
            {
                family_ = std::stoi(family_match[1], nullptr, 16);
            }

            if (vendor_ != Vendor::UNKNOWN && family_ != 0x0)
            {
                return;
            }
        }
    }

    Vendor vendor_ = Vendor::UNKNOWN;
    uint64_t family_ = 0x0;
};
} // namespace msr
} // namespace internal_energy
