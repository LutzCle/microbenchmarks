#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

namespace GpuBench {
    uint64_t event_nanoseconds(cl::Event const& event, uint64_t& time);
}

#endif /* COMMON_HPP */
