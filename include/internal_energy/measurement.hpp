#include <internal_energy/event.hpp>
#include <numeric>
namespace internal_energy

{
class Measurement
{
public:
    Measurement(std::vector<location_t> locations, std::vector<std::unique_ptr<EventSource>> events)
    {
        for (auto location : locations)
        {
            for (auto& event : events)
            {
                auto res = event->open(location);

                if (res)
                {
                    instances_.emplace_back(std::move(res));
                }
            }
        }
    }

    void start()
    {
        for (auto& instance : instances_)
        {
            start_.emplace_back(instance->read());
        }
    }

    void stop()
    {
        int i = 0;
        for (auto& instance : instances_)
        {
            start_[i] = instance->read() - start_[i];
            i++;
        }
    }

    double get_total_energy()
    {
        return std::accumulate(start_.begin(), start_.end(), 0.0);
    }

private:
    std::vector<std::unique_ptr<EventInstance>> instances_;
    std::vector<double> start_;
};
} // namespace internal_energy
