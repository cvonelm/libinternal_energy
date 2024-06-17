//  SPDX-FileCopyrightText: 2024 Technische Universität Dresden
//  SPDX-License-Identifier: MIT

#include <fmt/core.h>
#include <veda/api.h>

namespace internal_energy
{

enum class SensorType
{
    INVALID,
    POWER,
    CURRENT,
    CURRENT_EDGE,
    VOLTAGE,
    VOLTAGE_EDGE,
    TEMPERATURE
};

class NecSensor
{
public:
    NecSensor() : type_(SensorType::INVALID), id_(0)
    {
    }
    NecSensor(SensorType type, int id, int core = 0) : type_(type), id_(id), core_(core)
    {
        vedaDeviceGet(&dev_, id);
    }
    std::string name() const
    {
        if (type_ == SensorType::POWER)
        {
            return fmt::format("ve{}::power", id_);
        }
        else if (type_ == SensorType::CURRENT)
        {
            return fmt::format("ve{}::current", id_);
        }
        else if (type_ == SensorType::CURRENT_EDGE)
        {
            return fmt::format("ve{}::current_edge", id_);
        }
        else if (type_ == SensorType::VOLTAGE)
        {
            return fmt::format("ve{}::voltage", id_);
        }
        else if (type_ == SensorType::VOLTAGE_EDGE)
        {
            return fmt::format("ve{}::voltage_edge", id_);
        }
        else if (type_ == SensorType::TEMPERATURE)
        {
            return fmt::format("ve{}::temp::core{}", id_, core_);
        }
        else
        {
            throw std::runtime_error("Invalid sensor!");
        }
    }
    std::string unit() const
    {
        if (type_ == SensorType::POWER)
        {
            return "Watt";
        }
        else if (type_ == SensorType::CURRENT)
        {
            return "Ampere";
        }
        else if (type_ == SensorType::CURRENT_EDGE)
        {
            return "Ampere";
        }
        else if (type_ == SensorType::VOLTAGE)
        {
            return "Volt";
        }
        else if (type_ == SensorType::VOLTAGE_EDGE)
        {
            return "Volt";
        }
        else if (type_ == SensorType::TEMPERATURE)
        {
            return "Celsius";
        }
        else
        {
            throw std::runtime_error("Invalid sensor!");
        }
    }
    std::string description() const
    {
        if (type_ == SensorType::POWER)
        {
            return fmt::format("Vector Engine {} Power Consumption", id_);
        }
        else if (type_ == SensorType::CURRENT)
        {
            return fmt::format("Vector Engine {} Current Draw", id_);
        }
        else if (type_ == SensorType::CURRENT_EDGE)
        {
            return fmt::format("Vector Engine {} Current Edge", id_);
        }
        else if (type_ == SensorType::VOLTAGE)
        {
            return fmt::format("Vector Engine {} Voltage", id_);
        }
        else if (type_ == SensorType::VOLTAGE_EDGE)
        {
            return fmt::format("Vector Engine {} Voltage Edge", id_);
        }
        else if (type_ == SensorType::TEMPERATURE)
        {
            return fmt::format("Vector Engine {} Core {} Temperature", id_, core_);
        }
        else
        {
            throw std::runtime_error("Invalid sensor!");
        }
    }

    template <typename T>
    T read() const;

    template <>
    double read() const
    {
        float value;

        if (type_ == SensorType::POWER)
        {
            vedaDeviceGetPower(&value, dev_);
            return value;
        }
        else if (type_ == SensorType::CURRENT)
        {
            vedaDeviceGetCurrent(&value, dev_);
            return value;
        }
        else if (type_ == SensorType::CURRENT_EDGE)
        {
            vedaDeviceGetCurrentEdge(&value, dev_);
            return value;
        }
        else if (type_ == SensorType::VOLTAGE)
        {
            vedaDeviceGetVoltage(&value, dev_);
            return value;
        }
        else if (type_ == SensorType::VOLTAGE_EDGE)
        {
            vedaDeviceGetVoltageEdge(&value, dev_);
            return value;
        }
        else if (type_ == SensorType::TEMPERATURE)
        {
            vedaDeviceGetTemp(&value, core_, dev_);
            return value;
        }
        else
        {
            throw std::runtime_error("Invalid sensor!");
        }
    }

    bool invalid() const
    {
        return type_ == SensorType::INVALID;
    }

protected:
    SensorType type_;
    int id_;
    int core_;
    VEDAdevice dev_;
};
} // namespace internal_energy
