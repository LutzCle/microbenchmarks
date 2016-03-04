/*
 * This Source Code Form is subject to the terms of the Mozilla Public License,
 * v. 2.0. If a copy of the MPL was not distributed with this file, You can
 * obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * 
 * Copyright (c) 2016, Lutz, Clemens <lutzcle@cml.li>
 */

#include "pci_bandwidth.hpp"
#include "common.hpp"

#include <vector>
#include <sstream>
#include <iostream>

#include <clext.hpp>

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

void gpubench::PciBandwidth::set_cl_context(cl::Context context) {
    context_ = context;
}

void gpubench::PciBandwidth::set_cl_commandqueue(cl::CommandQueue queue) {
    commandqueue_ = queue;
}

int gpubench::PciBandwidth::run(size_t buffer_bytes) {

    cl_int err;

    size_t buffer_size = buffer_bytes / sizeof(cl_int);

    cl::Event map_event;
    cl::Event unmap_event;
    cl::Event regular_write_event;
    cl::Event regular_read_event;
    cl::Event pinned_write_event;
    cl::Event pinned_read_event;
    cl::Event pinned_copy_read_event;

    std::vector<cl_int> h_regular_buffer(buffer_size);
    cl_int *h_pinned_buffer_ptr = NULL;

    cle::TypedBuffer<cl_int> d_regular_buffer(
            context_,
            CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
            buffer_size,
            h_regular_buffer.data()
            );

    cle::TypedBuffer<cl_int> h_pinned_buffer(
            context_,
            CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE,
            buffer_size,
            NULL
            );

    cle::TypedBuffer<cl_int> d_pinned_buffer(
            context_,
            CL_MEM_READ_WRITE,
            buffer_size,
            NULL
            );

    // Regular read / write tests
    cle_sanitize_val_return(
            commandqueue_.enqueueWriteBuffer(
                d_regular_buffer,
                CL_FALSE,
                0,
                d_regular_buffer.bytes(),
                h_regular_buffer.data(),
                NULL,
                &regular_write_event));

    cle_sanitize_val_return(
            commandqueue_.enqueueReadBuffer(
                d_regular_buffer,
                CL_FALSE,
                0,
                d_regular_buffer.bytes(),
                h_regular_buffer.data(),
                NULL,
                &regular_read_event));

    // Pin memory
    cle_sanitize_ref_return(
            h_pinned_buffer_ptr = (cl_int *) commandqueue_.enqueueMapBuffer(
                h_pinned_buffer,
                CL_TRUE,
                CL_MAP_WRITE_INVALIDATE_REGION,
                0,
                h_pinned_buffer.bytes(),
                0,
                &map_event,
                &err),
            err
            );

    // Pinned memory write test
    cle_sanitize_val_return(
            commandqueue_.enqueueWriteBuffer(
                d_pinned_buffer,
                CL_FALSE,
                0,
                d_pinned_buffer.bytes(),
                h_pinned_buffer_ptr,
                NULL,
                &pinned_write_event));


    // Pinned memory read test
    cle_sanitize_val_return(
            commandqueue_.enqueueReadBuffer(
                d_pinned_buffer,
                CL_FALSE,
                0,
                d_pinned_buffer.bytes(),
                h_pinned_buffer_ptr,
                NULL,
                &pinned_read_event));

    // Unpin memory
    cle_sanitize_val_return(
            commandqueue_.enqueueUnmapMemObject(
                h_pinned_buffer,
                h_pinned_buffer_ptr,
                NULL,
                &unmap_event
                ));

    cle_sanitize_val_return(
            commandqueue_.enqueueCopyBuffer(
                d_pinned_buffer,
                h_pinned_buffer,
                0,
                0,
                h_pinned_buffer.bytes(),
                NULL,
                &pinned_copy_read_event
                ));

    int num_events = 7;
    char const* names[] = {
        "regular read buffer",
        "regular write buffer",
        "pinned map",
        "pinned unmap",
        "pinned read buffer",
        "pinned write buffer",
        "pinned copy read"
    };

    cl::Event const* events[] = {
        &regular_read_event,
        &regular_write_event,
        &map_event,
        &unmap_event,
        &pinned_read_event,
        &pinned_write_event,
        &pinned_copy_read_event
    };

    std::stringstream ss;
    for (int i = 0; i < num_events; ++i) {
        ss << names[i];
        ss << ((i == num_events - 1) ? '\n' : ',');
    }

    for (int i = 0; i < num_events; ++i) {
        uint64_t duration;
        GpuBench::event_nanoseconds(*events[i], duration);

        ss << (buffer_bytes / duration) / 1000 / 1000 / 1000;

        if (i != num_events - 1) {
            ss << ',';
        }
    }

    std::cout << ss.str() << std::endl;

    return 1;
}
