#include <internal_energy/hwmon/hwmon.hpp>

#include <iostream>

namespace internal_energy
{
namespace hwmon
{

std::vector<hwmon::EventSource> hwmon::EventSource::get_all_events()
{
    std::vector<hwmon::EventSource> res;
    std::regex sensor_regex("([a-zA-Z]+)([0-9]+)_(input|average)");
    for (auto const& entry : std::filesystem::directory_iterator{ "/sys/class/hwmon/" })
    {
        if (entry.is_directory())
        {
            for (auto const& hwmon_content : std::filesystem::directory_iterator(entry))
            {
                std::smatch sensor_match;
                if (hwmon_content.is_regular_file())
                {
                    std::string str = hwmon_content.path().filename().string();
                    if (std::regex_match(str, sensor_match, sensor_regex))
                    {
                        if (sensor_match[1] == "power")
                        {
                            if (sensor_match[2] == "input" || sensor_match[2] == "average")
                            {
                                res.emplace_back(hwmon_content);
                            }
                        }
                    }
                }
            }

            // Stupid stuff because Nvidia cant get their hwmon implementation right
            for (auto const& devices_entry :
                 std::filesystem::directory_iterator{ entry.path() / "device" })
            {
                std::smatch sensor_match;
                if (devices_entry.is_regular_file())
                {
                    std::string str = devices_entry.path().filename().string();
                    if (std::regex_match(str, sensor_match, sensor_regex))
                    {
                        if (sensor_match[1] == "power")
                        {
                            res.emplace_back(devices_entry);
                        }
                    }
                }
            }
        }
    }
    return res;
}
} // namespace hwmon
} // namespace internal_energy
