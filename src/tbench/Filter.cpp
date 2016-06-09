/* -----------------------------------------------------------------
 * Filter.cpp
 *
 * A SystemC filter module.
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
#include "Filter.h"

#include <iostream>
#include <systemc.h>

using std::cout;
using std::endl;

// The filter process
void Filter::filterTokens() {

	// Say hello to the world
	cout << sc_time_stamp()
		 << ":\tfilterTokens():  Starting up!"
	     << endl;
	
	// Token buffer and sample flag
	char token  = '\0';
	bool sample = true;

	while (true) {

		// Try to read token from input FIFO
		while (!inp.nb_read(token))
			wait(10, SC_NS);

        // Sample every second token
        if (!(sample = !sample)) continue;

		// Try to write token to output FIFO
		while (!outp.nb_write(token))
			wait(10, SC_NS);
	}
}
