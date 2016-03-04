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
            ("platform",
             po::value<unsigned int>(&platform_)->default_value(0),
             "OpenCL platform number")
            ("device",
             po::value<unsigned int>(&device_)->default_value(0),
             "OpenCL device number")
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

        if (vm.count("platform")) {
            platform_ = vm["platform"].as<unsigned int>();
        }

        if (vm.count("device")) {
            device_ = vm["device"].as<unsigned int>();
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

    unsigned int cl_platform() const {
        return platform_;
    }

    unsigned int cl_device() const {
        return device_;
    }

private:
    Mode mode_;
    unsigned int platform_;
    unsigned int device_;
};

int main(int argc, char **argv) {

    int ret = 0;

    CmdOptions options;

    ret = options.parse(argc, argv);
    if (ret < 0) {
        return 1;
    }

    cle::CLInitializer initializer;
    if (initializer.init(options.cl_platform(), options.cl_device()) < 0) {
        return 1;
    }

    switch (options.get_mode()) {
        case CmdOptions::Mode::GpuMemBandwidth:

            break;
        case CmdOptions::Mode::PciBandwidth:
            gpubench::PciBandwidth pcibw;
            pcibw.set_cl_context(initializer.get_context());
            pcibw.set_cl_commandqueue(initializer.get_commandqueue());

            ret = pcibw.run();
            if (ret < 0) {
                return 1;
            }

            break;
    }

    return 0;
}
