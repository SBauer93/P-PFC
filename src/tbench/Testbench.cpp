/* -----------------------------------------------------------------
 * Testbench.cpp
 *
 * SystemC testbench with simulation driver.
 *
 * Author:  Jens Rudolf (Uni Rostock)
 * Date:    2015-29-05
 *
 *
 * Changelog:
 *      2015-29-05 - Initial version
 *
 *
 * Copyright (c) 2015 by Jens Rudolf (Uni Rostock). All rights
 * reserved. For educational use only. NO WARRANTY!
 *
 * Report bugs to:
 *      jens.rudolf@uni-rostock.de
 *
 * -----------------------------------------------------------------
 */
#include "Testbench.h"

#include <iostream>
#include <systemc.h>

using std::cerr;
using std::endl;

int sc_main(int argc, char *argv[]) {

    if (argc < 3) {
        cerr << "ERROR: Input and output files must be supplied!\n"
             << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
        return -1;
    }

    Testbench tb("Testbench");
    tb.setInFile(argv[1]);
    tb.setOutFile(argv[2]);
    
    //sc_trace_file *tf = sc_create_vcd_trace_file("wave");
    //tb.channelIn.trace(tf);

    sc_start(1500, SC_NS);
    return 0;
}
