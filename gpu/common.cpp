#include "common.hpp"

#include <cstdint>

#include <clext.hpp>

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

uint64_t GpuBench::event_nanoseconds(cl::Event const& event, uint64_t& time) {
    cl_ulong start, end;

    cle_sanitize_val_return(
            event.wait()
            );

    cle_sanitize_val_return(
            event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start)
            );

    cle_sanitize_val_return(
            event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end)
            );

    time = end - start;

    return CL_SUCCESS;
}
