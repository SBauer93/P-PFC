/* -----------------------------------------------------------------
 * Testbench.h
 *
 * A SystemC testbench module.
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
#ifndef TESTBENCH_H
#define TESTBENCH_H

#include "Filter.h"
#include "Consumer.h"
#include "Producer.h"

#include <systemc.h>

SC_MODULE(Testbench) {

    // The modules
	Filter	 filter;
	Producer producer;
	Consumer consumer;

    // The communication channels
	sc_fifo<char> channelIn;
	sc_fifo<char> channelOut;

	void setInFile(const char *name) { producer.openFile(name); }
	void setOutFile(const char *name) { consumer.openFile(name); }

    // The testbench constructor
	SC_CTOR(Testbench)
	    // Initialize modules and channels
		: filter	("filter")
		, producer	("producer")
		, consumer	("consumer")
		, channelIn	("channelIn", 4096)
		, channelOut("channelOut", 4096) {

    		//sc_trace_file *tf = sc_create_vcd_trace_file("wave");
    		//sc_trace(tf, channelIn, "channel");
		
		// Connect producer and filter via channelIn
		producer.outp(channelIn);
		filter.inp(channelIn);

		// Connect filter and consumer via channelOut
		filter.outp(channelOut);
		consumer.inp(channelOut);
	}
};

#endif
