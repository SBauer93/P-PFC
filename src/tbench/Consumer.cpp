/* -----------------------------------------------------------------
 * Consumer.cpp
 *
 * A SystemC consumer module.
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
#include "Consumer.h"

#include <fstream>
#include <iostream>
#include <systemc.h>

using std::cout;
using std::endl;

void Consumer::openFile(const char *name) {
    outFile.open(name);
}

// The consumer process
void Consumer::consumeTokens() {
	
	// Say hello to the world	
	cout << sc_time_stamp()
		 << ":\tconsumeTokens(): Starting up!"
	     << endl;

	// The token buffer
	char token = '\0';

	while (true) {
		// Try to read token from input FIFO
		// If not succeeded, wait 10 ns
		while (!inp.nb_read(token))
			wait(10, SC_NS);

        // Write token to output file
        outFile.put(token);
        outFile.flush();

		// Tell the world about the new token
		//cout << sc_time_stamp()
		//	 << ":\tconsumeTokens(): Read  <"
		//	 << token << "> from FIFO!"
	    //     << endl;

	    wait(10, SC_NS);
	}
}
