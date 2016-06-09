/* -----------------------------------------------------------------
 * Consumer.h
 *
 * Systemc consumer module.
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
#ifndef CONSUMER_H
#define CONSUMER_H

#include <fstream>
#include <systemc.h>

using std::ofstream;

SC_MODULE(Consumer){
	// The input FIFO port
	sc_fifo_in<char> inp;

	// The output file stream
	ofstream outFile;

    // The consumer process
	void consumeTokens();

	void openFile(const char*);

    // The module constructor
	SC_CTOR(Consumer) {
		SC_THREAD(consumeTokens);
	}
};

#endif
