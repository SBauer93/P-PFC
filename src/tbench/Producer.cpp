/* -----------------------------------------------------------------
 * Producer.cpp
 *
 * Systemc producer module.
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
#include "Producer.h"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <systemc.h>

using std::cout;
using std::endl;

void Producer::openFile(const char *name) {
    inFile.open(name);
}

// The producer process
void Producer::produceTokens() {

	// Say hello, if you're alive
	cout << sc_time_stamp()
		 << ":\tproduceTokens(): Starting up!"
		 << endl;

	// Token buffer
    char token = '\0';

	// Put every token onto the output FIFO
	while (token != '9') {
		inFile.get(token);
		
		// Try to write onto FIFO. If FIFO is full,
		// then wait for 10 ns
		while (!outp.nb_write(token))
			wait(10, SC_NS);

		// Say hello, if token is on FIFO
		//cout << sc_time_stamp()
		//     << ":\tproduceTokens(): Wrote <"
		//	 << token << "> onto FIFO!"
		//	 << endl;
		
		wait(10, SC_NS);
	}
	
	// Say hello, if you're done
	cout << "produceTokens(): All tokens on FIFO!"
		 << endl;
}
