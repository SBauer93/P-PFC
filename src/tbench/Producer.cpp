/* -----------------------------------------------------------------
 * Producer.cpp
 *
 * Systemc producer module.
 *
 * Author:  Benz Rudolf (Uni Rostock)
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

#define SBLIMIT 32
#define FIFO_IN "HW_DATA_IN"

typedef struct _granule {
	uint8_t scfsi;
	int part2_3_length;
	int big_values;
	int global_gain;
	int scalefac_compress;
	uint8_t block_type;
	uint8_t switch_point;
	int table_select[3];
	int subblock_gain[3];
	uint8_t scalefac_scale;
	uint8_t count1table_select;
	int region_size[3];
	int preflag;
	int short_start, long_end;
	uint8_t scale_factors[40];
	int32_t sb_hybrid[SBLIMIT * 18];
} granule_t;

typedef unsigned char  uint8_t;
typedef   signed int   int32_t;
typedef   signed short int16_t;

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
	granule_t g;
	int32_t sb_samples, mdct_buf;
	int16_t mdct_win;								

	// Pointer to the "granule_t" struct
	unsigned char *ptr = (unsigned char*) &g;
	
	// Read the parameters from the FIFO
	FILE *fifo_in = fopen(FIFO_IN, "r");
	fread(&sb_samples, sizeof(int32_t), 1, fifo_in);
	fread(&mdct_buf, sizeof(int32_t), 1, fifo_in);
	fread(&mdct_win, sizeof(int16_t), 1, fifo_in);
	for (int i = 0; i < sizeof(g); ++i) {
		fread((unsigned char*)ptr[i], sizeof(int32_t), 1, fifo_in);
	}
	
		
		// Try to write onto FIFO. If FIFO is full,
		// then wait for 10 ns
	while (!outp.nb_write(sb_samples))
		wait(10, SC_NS);
	while (!outp.nb_write(mdct_buf))
		wait(10, SC_NS);
	while (!outp.nb_write(mdct_win))
		wait(10, SC_NS);
	for (int i = 0; i < sizeof(granule_t); ++i) {
		while (!outp.nb_write((int32_t)ptr[i]))
			wait(10, SC_NS);
	}



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
