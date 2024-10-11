#include <internal_energy/energy.hpp>
#include <internal_energy/topology.hpp>

namespace internal_energy
{
std::vector<HwmonDevice> get_hwmon_devices()
{
    return { HwmonDevice() };
}

std::vector<location_t> get_all_devices()
{
    std::vector<location_t> res;
    for (auto cpu : perf_cpp::get_cpus())
    {
        res.emplace_back(Cpu(cpu.as_int()));
    }

    for (auto hwmon_device : get_hwmon_devices())
    {
        res.emplace_back(HwmonDevice());
    }
#ifdef HAVE_NVML
    for (auto cuda_device : get_cuda_devices())
    {
        res.emplace_back(cuda_device);
    }
#endif
#ifdef HAVE_NEC
    for (auto nec_device : get_nec_devices())
    {
        res.emplace_back(nec_device);
    }
#endif
    return res;
}

#ifdef HAVE_NEC
std::vector<NecDevice> get_nec_devices()
{
    std::vector<NecDevice> devices;
    vedaInit(0);

    int count;

    vedaDeviceGetCount(&count);

    for (int id = 0; id < count; id++)
    {
        devices.emplace_back(id);
    }
    vedaExit();

    return devices;
}
#endif

#ifdef HAVE_NVML
std::vector<CudaDevice> get_cuda_devices()
{
    nvmlInit();
    std::vector<CudaDevice> res;
    nvmlDevice_t handle;
    for (int i = 0; nvmlDeviceGetHandleByIndex_v2(i, &handle) == 0; i++)
    {
        res.emplace_back(CudaDevice(handle));
    }
    return res;
}
#endif

#ifdef HAVE_ROCM_SMI
std::vector<RocmDevice> get_rocm_devices()
{
    std::vector<RocmDevice> res;
    rsmi_init(0);
    uint32_t num_devices;

    rsmi_num_monitor_devices(&num_devices);
    for (int i = 0; i < num_devices; i++)
    {
        res.emplace_back(RocmDevice(i));
    }
    return res;
}
#endif
} // namespace internal_energy
