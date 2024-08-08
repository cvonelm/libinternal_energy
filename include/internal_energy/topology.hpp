#pragma once

#include <variant>
#include <vector>

#include <perf-cpp/perf_event_instance.hpp>
#include <perf-cpp/sysfs_event.hpp>
#include <perf-cpp/topology.hpp>

#ifdef HAVE_NEC
#include <veda/api.h>
#endif

#ifdef HAVE_NVML
#include <nvml.h>
#endif

namespace internal_energy
{

enum class Subsystem
{
    INVALID,
    PERF,
    HWMON,
    CUDA,
    NEC
};

class Location
{
};

class Cpu : public Location
{
public:
    Cpu(int cpuid) : cpuid_(cpuid)
    {
    }
    int as_int()
    {
        return cpuid_;
    }

private:
    int cpuid_;
};

class Process : public Location
{
public:
    Process(pid_t pid) : pid_(pid)
    {
    }

    pid_t as_pid_t()
    {
        return pid_;
    }

private:
    pid_t pid_;
};

#ifdef HAVE_NEC
class NecDevice : public Location
{
public:
    NecDevice(VEDAdevice dev) : dev_(dev)
    {
    }

    VEDAdevice as_device_t() const
    {
        return dev_;
    }

private:
    VEDAdevice dev_;
};
std::vector<NecDevice> get_nec_devices();
#else
class NecDevice
{
};
#endif

#ifdef HAVE_NVML
class CudaDevice
{
public:
    CudaDevice(nvmlDevice_t dev) : dev_(dev)
    {
    }
    nvmlDevice_t as_device_t() const
    {
        return dev_;
    }

private:
    nvmlDevice_t dev_;
};
std::vector<CudaDevice> get_cuda_devices();
#else
class CudaDevice
{
};
#endif

class HwmonDevice
{
};

// TODO: replace with real thing, like PCI id parsing or sumthing
std::vector<HwmonDevice> get_hwmon_devices();

using location_t = std::variant<Process, Cpu, NecDevice, CudaDevice, HwmonDevice>;

std::vector<location_t> get_all_devices();

} // namespace internal_energy
