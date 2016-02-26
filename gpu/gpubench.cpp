/*
 * This Source Code Form is subject to the terms of the Mozilla Public License,
 * v. 2.0. If a copy of the MPL was not distributed with this file, You can
 * obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * 
 * Copyright (c) 2016, Lutz, Clemens <lutzcle@cml.li>
 */

#include "pci_bandwidth.hpp"

#include <iostream>

#include <clext.hpp>
#include <boost/program_options.hpp>

#ifndef GPUBENCH_NAME
#define GPUBENCH_NAME ""
#endif

namespace po = boost::program_options;

class CmdOptions {
public:
    enum class Mode {GpuMemBandwidth, PciBandwidth};

    int parse(int argc, char **argv) {
        char help_msg[] =
            "Usage: " GPUBENCH_NAME " [OPTION]\n"
            "Options"
            ;

        po::options_description cmdline(help_msg);
        cmdline.add_options()
            ("help", "Produce help message")
            ("gpumembw", "GPU Memory Bandwidth")
            ("pcibw", "PCI Bandwidth")
            ;

        po::variables_map vm;
        po::store(
                po::command_line_parser(argc, argv).options(cmdline).run(),
                vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << cmdline <<std::endl;
            return -1;
        }

        if (vm.count("gpumembw")) {
            mode_ = Mode::GpuMemBandwidth;
        }

        if (vm.count("pcibw")) {
            mode_ = Mode::PciBandwidth;
        }

        return 1;
    }

    Mode get_mode() const {
        return mode_;
    }

private:
    Mode mode_;
};

int main(int argc, char **argv) {

    int ret = 0;

    CmdOptions options;

    ret = options.parse(argc, argv);
    if (ret < 0) {
        return 1;
    }

    switch (options.get_mode()) {
        case CmdOptions::Mode::GpuMemBandwidth:

            break;
        case CmdOptions::Mode::PciBandwidth:
            GpuBench::PciBandwidth pcibw;

            ret = pcibw.run();
            if (ret < 0) {
                return 1;
            }

            break;
    }

    return 0;
}
