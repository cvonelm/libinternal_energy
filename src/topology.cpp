#include <fmt/core.h>
#include <internal_energy/topology.hpp>
#include <perf-cpp/topology.hpp>

namespace internal_energy
{

template <>
Location Location::from<Cpu>(Cpu from)
{
    Location l;
    l.tag = DeviceType::CPU;
    l.cpu_ = from;
    return l;
}

template <>
Cpu Location::as<Cpu>() const
{
    if (tag == DeviceType::CPU)
    {
        return cpu_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a CPU: {}", str()));
    }
}

template <>
bool Location::is<Cpu>() const
{
    return tag == DeviceType::CPU;
}
template <>
bool Location::is<CpuAny>() const
{
    return tag == DeviceType::CPU_ANY;
}

template <>
Location Location::from<Core>(Core from)
{
    Location l;
    l.tag = DeviceType::CORE;
    l.core_ = from;
    return l;
}

template <>
Core Location::as<Core>() const
{
    if (tag == DeviceType::CORE)
    {
        return core_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a Core: {}", str()));
    }
}

template <>
bool Location::is<CoreAny>() const
{
    return tag == DeviceType::CORE_ANY;
}
template <>
bool Location::is<Core>() const
{
    return tag == DeviceType::CORE;
}

bool Core::contains(Location l) const
{
    if (l.is<Cpu>())
    {
        auto pkg = perf_cpp::Topology::instance().core_of(l.as<Cpu>().as_perf_cpu());
        return pkg.as_int() == core_id_;
    }

    return false;
}

template <>
Location Location::from<Package>(Package from)
{
    Location l;
    l.tag = DeviceType::PACKAGE;
    l.package_ = from;
    return l;
}

template <>
Package Location::as<Package>() const
{
    if (tag == DeviceType::PACKAGE)
    {
        return package_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a Package: {}", str()));
    }
}

template <>
bool Location::is<Package>() const
{
    return tag == DeviceType::PACKAGE;
}

template <>
bool Location::is<PackageAny>() const
{
    return tag == DeviceType::PACKAGE_ANY;
}

#ifdef HAVE_NVML
template <>
Location Location::from<cuda::NVidiaDevice>(cuda::NVidiaDevice from)
{
    Location l;
    l.tag = DeviceType::NVIDIA_DEVICE;
    l.nvidia_device_ = from;
    return l;
}

template <>
cuda::NVidiaDevice Location::as<cuda::NVidiaDevice>() const
{
    if (tag == DeviceType::NVIDIA_DEVICE)
    {
        return nvidia_device_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a NVidia GPU: {}", str()));
    }
}

template <>
bool Location::is<NVidiaDeviceAny>() const
{
    return tag == DeviceType::NVIDIA_DEVICE_ANY;
}

template <>
bool Location::is<cuda::NVidiaDevice>() const
{
    return tag == DeviceType::NVIDIA_DEVICE;
}
#endif

#ifdef HAVE_NEC
template <>
Location Location::from<NecDevice>(NecDevice from)
{
    Location l;
    l.tag = DeviceType::NEC_DEVICE;
    l.nec_device_ = from;
    return l;
}

template <>
NecDevice Location::as<NecDevice>() const
{
    if (tag == DeviceType::NEC_DEVICE)
    {
        return nec_device_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a NEC device: {}", str()));
    }
}
#endif

#ifdef HAVE_ROCM_SMI
template <>
Location Location::from<rocm::AMDDevice>(rocm::AMDDevice from)
{
    Location l;
    l.tag = DeviceType::AMD_DEVICE;
    l.amd_device_ = from;
    return l;
}
template <>

bool Location::is<rocm::AMDDevice>() const
{
    return tag == DeviceType::AMD_DEVICE;
}
template <>
rocm::AMDDevice Location::as<rocm::AMDDevice>() const
{
    if (tag == DeviceType::AMD_DEVICE)
    {
        return amd_device_;
    }
    else
    {
        throw std::runtime_error(fmt::format("Location is not a AMD GPU: {}", str()));
    }
}

template <>
bool Location::is<AMDDeviceAny>() const
{
    return tag == DeviceType::AMD_DEVICE_ANY;
}

#endif

template <>
bool Location::is<LocationAny>() const
{
    return tag == DeviceType::LOCATION_ANY;
}

bool Package::contains(Location l) const
{
    if (l.is<Cpu>())
    {
        auto pkg = perf_cpp::Topology::instance().package_of(l.as<Cpu>().as_perf_cpu());
        return pkg.as_int() == package_id_;
    }

    else if (l.is<Core>())
    {
        auto pkg = perf_cpp::Topology::instance().package_of(l.as<Core>().as_perf_core());
        return pkg.as_int() == package_id_;
    }
    return false;
}
bool operator<(const Location& lhs, const Location& rhs)
{
    if (lhs.tag != rhs.tag)
    {
        return lhs.tag < rhs.tag;
    }
    if (lhs.tag == rhs.tag)
    {
        switch (lhs.tag)
        {
        case DeviceType::CPU:
            return lhs.cpu_ < rhs.cpu_;
        case DeviceType::CORE:
            return lhs.core_ < rhs.core_;
        case DeviceType::PACKAGE:
            return lhs.package_ < rhs.package_;
#ifdef HAVE_NEC
        case DeviceType::NEC_DEVICE:
            return lhs.nec_device_ < rhs.nec_device_;
#endif
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE:
            return lhs.nvidia_device_ < rhs.nvidia_device_;
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE:
            return lhs.amd_device_ < rhs.amd_device_;
#endif
        case DeviceType::CPU_ANY:
        case DeviceType::CORE_ANY:
        case DeviceType::LOCATION_ANY:
        case DeviceType::PACKAGE_ANY:
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE_ANY:
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE_ANY:
#endif
            return false;
        }
    }
    return false;
}
bool operator==(const Location& lhs, const Location& rhs)
{
    if (lhs.is<LocationAny>() || rhs.is<LocationAny>())
    {
        return true;
    }

    if (lhs.is<Cpu>() && rhs.is<CpuAny>() || lhs.is<CpuAny>() && rhs.is<Cpu>())
    {
        return true;
    }
#ifdef HAVE_NVML

    if (lhs.is<cuda::NVidiaDevice>() && rhs.is<NVidiaDeviceAny>() ||
        lhs.is<NVidiaDeviceAny>() && rhs.is<cuda::NVidiaDevice>())
    {
        return true;
    }
#endif
#ifdef HAVE_ROCM_SMI

    if (lhs.is<rocm::AMDDevice>() && rhs.is<AMDDeviceAny>() ||
        lhs.is<AMDDeviceAny>() && rhs.is<rocm::AMDDevice>())
    {
        return true;
    }
#endif

    if (lhs.is<Package>() && rhs.is<PackageAny>() || lhs.is<PackageAny>() && rhs.is<Package>())
    {
        return true;
    }
    if (lhs.is<Core>() && rhs.is<CoreAny>() || lhs.is<CoreAny>() && rhs.is<Core>())
    {
        return true;
    }

    if (lhs.tag == rhs.tag)
    {
        switch (lhs.tag)
        {
        case DeviceType::CPU:
            return lhs.cpu_ == rhs.cpu_;
        case DeviceType::CORE:
            return lhs.core_ == rhs.core_;
        case DeviceType::PACKAGE:
            return lhs.package_ == rhs.package_;
#ifdef HAVE_NEC
        case DeviceType::NEC_DEVICE:
            return lhs.nec_device_ == rhs.nec_device_;
#endif
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE:
            return lhs.nvidia_device_ == rhs.nvidia_device_;
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE:
            return lhs.amd_device_ == rhs.amd_device_;
#endif
        case DeviceType::CPU_ANY:
        case DeviceType::CORE_ANY:
        case DeviceType::LOCATION_ANY:
        case DeviceType::PACKAGE_ANY:
#ifdef HAVE_NVML
        case DeviceType::NVIDIA_DEVICE_ANY:
#endif
#ifdef HAVE_ROCM_SMI
        case DeviceType::AMD_DEVICE_ANY:
#endif
            throw std::runtime_error("Should not happen!");
        }
    }
    return false;
}

Location Location::from_str(std::string location)
{
    if (location == "*")
    {
        return Location::any();
    }

    std::regex location_regex("([a-zA-Z]+)([0-9*]+)");
    std::smatch match;

    if (std::regex_match(location, match, location_regex))
    {
        if (match[1] == "cpu" || match[1] == "CPU")
        {
            if (match[2] == "*")
            {
                return Location::cpu_any();
            }
            return Location::from<Cpu>(Cpu(std::stoi(match[2])));
        }
        else if (match[1] == "pkg" || match[1] == "PKG")
        {
            if (match[2] == "*")
            {
                return Location::pkg_any();
            }
            return Location::from<Package>(Package(std::stoi(match[2])));
        }
        else if (match[1] == "core" || match[1] == "CORE")
        {
            if (match[2] == "*")
            {
                return Location::core_any();
            }
            return Location::from(Core(std::stoi(match[2])));
        }
#ifdef HAVE_NVML
        else if (match[1] == "nvidiagpu" || match[1] == "NVIDIAGPU")
        {
            if (match[2] == "*")
            {
                return Location::nvidia_device_any();
            }
            return Location::from(cuda::NVidiaDevice(std::stoi(match[2])));
        }
#endif
#ifdef HAVE_ROCM_SMI
        else if (match[1] == "amdgpu" || match[1] == "AMDGPU")
        {
            if (match[2] == "*")
            {
                return Location::amd_device_any();
            }
            return Location::from(rocm::AMDDevice(std::stoi(match[2])));
        }
#endif
    }
    throw std::runtime_error(fmt::format("Can not parse {} as Location!", location));
}

bool Location::contains(Location l) const
{
    if (l == *this)
    {
        return true;
    }

    if (tag == DeviceType::PACKAGE_ANY)
    {
        return l.is<Package>();
    }

    if (tag == DeviceType::CORE_ANY)
    {
        return l.is<Core>();
    }
    if (tag == DeviceType::PACKAGE)
    {
        return package_.contains(l);
    }

    if (tag == DeviceType::CORE)
    {
        return core_.contains(l);
    }
    return false;
}
} // namespace internal_energy
