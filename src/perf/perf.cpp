#include <cerrno>
#include <cmath>
#include <cstring>
#include <internal_energy/perf/perf.hpp>
#include <system_error>

extern "C"
{
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <unistd.h>
}

static int perf_event_open(struct perf_event_attr* attr, pid_t pid, int cpuid, int group_fd,
                           int flags)
{
    return syscall(SYS_perf_event_open, attr, pid, cpuid, group_fd, flags);
}

namespace internal_energy
{
namespace perf
{

PerfCounter::PerfCounter(uint64_t type, uint64_t config, uint64_t config1, std::string name)
{
    struct perf_event_attr attr;
    memset((void*)&attr, 0, sizeof(struct perf_event_attr));

    attr.size = sizeof(attr);
    attr.sample_period = 0;
    attr.type = 16;
    attr.config = 2;
    attr.exclude_kernel = 0;
    attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;

    fd_ = perf_event_open(&attr, -1, 0, -1, 0);

    if (fd_ == -1)
    {
        throw std::system_error(errno, std::system_category());
    }
}

PerfCounter::~PerfCounter()
{
    close(fd_);
}

RaplAmdPkgCounter::RaplAmdPkgCounter() : PerfCounter(16, 2, 0, "RAPL_ENERGY_PKG")
{
}

double RaplAmdPkgCounter::read_energy()
{
    UserspaceReadFormat fmt;
    if (::read(fd_, &fmt, sizeof(fmt)) != sizeof(UserspaceReadFormat))
    {
        throw std::system_error(errno, std::system_category());
    }
    return fmt.value * pow(2, -32);
}
} // namespace perf
} // namespace internal_energy
