/*
 * This Source Code Form is subject to the terms of the Mozilla Public License,
 * v. 2.0. If a copy of the MPL was not distributed with this file, You can
 * obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * 
 * Copyright (c) 2016, Lutz, Clemens <lutzcle@cml.li>
 */

#ifndef PCI_BANDWIDTH_HPP
#define PCI_BANDWIDTH_HPP

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

namespace gpubench {
    class PciBandwidth {
    public:
        void set_cl_context(cl::Context context);
        void set_cl_commandqueue(cl::CommandQueue queue);

        int run();

    private:
        cl::Context context_;
        cl::CommandQueue commandqueue_;
    };
}

#endif /* PCI_BANDWIDTH_HPP */
