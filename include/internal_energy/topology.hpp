#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <fmt/core.h>

#ifdef HAVE_NEC
#include <nec/nec_sensors.hpp>
#endif

#ifdef HAVE_NVML
#include <internal_energy/nvml/nvidia_device.hpp>
#endif

#ifdef HAVE_ROCM_SMI
#include <internal_energy/rocm_smi/amd_device.hpp>
#endif

#include <perf-cpp/topology.hpp>

namespace internal_energy
{

enum class Subsystem
{
    INVALID,
    PERF,
    MSR,
    HWMON,
    CUDA,
    NEC,
    ROCM,
    ANY
};

class Cpu
{
public:
    Cpu(int cpuid) : cpuid_(cpuid)
    {
    }
    int as_int() const
    {
        return cpuid_;
    }

    perf_cpp::Cpu as_perf_cpu() const
    {
        return perf_cpp::Cpu(cpuid_);
    }

    std::string str() const
    {
        return fmt::format("CPU{}", cpuid_);
    }

    static std::vector<Cpu> get_available_devices()
    {
        auto cpus = perf_cpp::Topology::instance().cpus();

        std::vector<Cpu> res;

        std::transform(cpus.begin(), cpus.end(), res.begin(),
                       [](auto& elem) { return Cpu(elem.as_int()); });

        return res;
    }

    friend bool operator==(const Cpu& lhs, const Cpu& rhs)
    {
        return lhs.cpuid_ == rhs.cpuid_;
    }

    friend bool operator<(const Cpu& lhs, const Cpu& rhs)
    {
        return lhs.cpuid_ < rhs.cpuid_;
    }

private:
    int cpuid_;
};

class Location;
class Package
{
public:
    Package(int package_id) : package_id_(package_id)
    {
    }

    Cpu measuring_cpu() const
    {
        return Cpu(
            perf_cpp::Topology::instance().measuring_cpu_for_package(as_perf_package()).as_int());
    }
    perf_cpp::Package as_perf_package() const
    {
        return perf_cpp::Package(package_id_);
    }

    bool contains(Location l) const;

    std::string str() const
    {
        return fmt::format("PKG{}", package_id_);
    }

    friend bool operator==(const Package& lhs, const Package& rhs)
    {
        return lhs.package_id_ == rhs.package_id_;
    }

    friend bool operator<(const Package& lhs, const Package& rhs)
    {
        return lhs.package_id_ < rhs.package_id_;
    }

    perf_cpp::Package as_perf_package()
    {
        return perf_cpp::Package(package_id_);
    }

private:
    int package_id_;
};

class Core
{
public:
    Core(int core_id) : core_id_(core_id)
    {
    }

    Cpu measuring_cpu() const
    {
        return Cpu(perf_cpp::Topology::instance().measuring_cpu_for_core(as_perf_core()).as_int());
    }
    perf_cpp::Core as_perf_core() const
    {
        return perf_cpp::Core(core_id_);
    }

    bool contains(Location l) const;

    std::string str() const
    {
        return fmt::format("CORE{}", core_id_);
    }

    friend bool operator==(const Core& lhs, const Core& rhs)
    {
        return lhs.core_id_ == rhs.core_id_;
    }

    friend bool operator<(const Core& lhs, const Core& rhs)
    {
        return lhs.core_id_ < rhs.core_id_;
    }
    perf_cpp::Core as_perf_core()
    {
        return perf_cpp::Core(core_id_);
    }

private:
    int core_id_;
};

enum class DeviceType
{
#ifdef HAVE_NEC
    NEC_DEVICE,
#endif
#ifdef HAVE_NVML
    NVIDIA_DEVICE,
    NVIDIA_DEVICE_ANY,
#endif
#ifdef HAVE_ROCM_SMI
    AMD_DEVICE,
    AMD_DEVICE_ANY,
#endif
    PACKAGE,
    CPU,
    CORE,

    CPU_ANY,
    CORE_ANY,
    PACKAGE_ANY,
    LOCATION_ANY
};

class CpuAny
{
};

class PackageAny
{
};

class CoreAny
{
};

class LocationAny
{
};

class NVidiaDeviceAny
{
};
class AMDDeviceAny
{
};

class Location
{
public:
    template <typename T>
    T as() const;

    template <typename T>
    bool is() const;

    // Every Location contains itself
    // Outside of that:
    //  - Packages contain their cores
    bool contains(Location l) const;

    std::string str() const
    {
        switch (tag)
        {
        case DeviceType::CPU:
            return cpu_.str();
        case DeviceType::PACKAGE:
            return package_.str();
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE:
            return nvidia_device_.str();
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE:
            return amd_device_.str();
#endif
#ifdef HAVE_NEC
        case DeviceType::NEC_DEVICE:
            return nec_device_.str();
#endif
        case DeviceType::CORE:
            return core_.str();
        case DeviceType::CORE_ANY:
            return "CORE*";
        case DeviceType::CPU_ANY:
            return "CPU*";
        case DeviceType::PACKAGE_ANY:
            return "PKG*";
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE_ANY:
            return "NVIDIAGPU*";
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE_ANY:
            return "AMDGPU*";
#endif
        case DeviceType::LOCATION_ANY:
            return "*";
        }

        throw std::runtime_error(fmt::format("Unknown tag: {}", (int)tag));
    }

    static Location core_any()
    {
        Location l;
        l.tag = DeviceType::CORE_ANY;
        return l;
    }

    static Location pkg_any()
    {
        Location l;
        l.tag = DeviceType::PACKAGE_ANY;
        return l;
    }

    static Location cpu_any()
    {
        Location l;
        l.tag = DeviceType::CPU_ANY;
        return l;
    }
#ifdef HAVE_NVML
    static Location nvidia_device_any()
    {
        Location l;
        l.tag = DeviceType::NVIDIA_DEVICE_ANY;
        return l;
    }
#endif
#ifdef HAVE_ROCM_SMI
    static Location amd_device_any()
    {
        Location l;
        l.tag = DeviceType::AMD_DEVICE_ANY;
        return l;
    }
#endif
    static Location any()
    {
        Location l;
        l.tag = DeviceType::LOCATION_ANY;
        return l;
    }

    static Location from_str(std::string location);

    friend bool operator==(const Location& lhs, const Location& rhs);
    friend bool operator<(const Location& lhs, const Location& rhs);
    friend bool operator!=(const Location& lhs, const Location& rhs)
    {
        return !(lhs == rhs);
    }
    template <typename T>
    static Location from(T from);

    DeviceType tag;

private:
    Location()
    {
    }

    union
    {
        Cpu cpu_;
        Core core_;
        Package package_;
#ifdef HAVE_NEC
        NecDevice nec_device_;
#endif
#ifdef HAVE_NVML
        internal_energy::cuda::NVidiaDevice nvidia_device_;
#endif
#ifdef HAVE_ROCM_SMI
        internal_energy::rocm::AMDDevice amd_device_;
#endif
    };
};

} // namespace internal_energy
