/* -----------------------------------------------------------------
 * Filter.h
 *
 * Systemc filter module.
 *
 * Author:  Jens Rudolf (Uni Rostock)
 * Date:    2015-24-05
 *
 *
 * Changelog:
 *      2015-24-05 - Initial version
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
#ifndef FILTER_H
#define FILTER_H

#include <systemc.h>

SC_MODULE(Filter) {
    // Input and output FIFO ports
	sc_fifo_in<char>  inp;
	sc_fifo_out<char> outp;

    // The filter process
	void filterTokens();

    // The module constructor
	SC_CTOR(Filter) {
		SC_THREAD(filterTokens);
	}
};

#endif
