/* -----------------------------------------------------------------
 * Producer.h
 *
 * A SystemC producer module.
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
#ifndef PRODUCER_H
#define PRODUCER_H

#include <fstream>
#include <systemc.h>

using std::ifstream;

SC_MODULE(Producer){
	// Output FIFO port
	sc_fifo_out<int32_t> outp;

	// File stream for input data
	ifstream inFile;

    // The producer process
	void produceTokens();
	
	void openFile(const char*);

    // The module constructor
	SC_CTOR(Producer) {
		SC_THREAD(produceTokens);
	}
};

#endif
