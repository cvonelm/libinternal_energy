#pragma oncea

class NecDevice
{
public:
    NecDevice(VEDAdevice dev) : dev_(dev)
    {
    }

    VEDAdevice as_device_t() const
    {
        return dev_;
    }
    static std::vector<NecDevice> get_available_devices()
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

private:
    VEDAdevice dev_;
};
