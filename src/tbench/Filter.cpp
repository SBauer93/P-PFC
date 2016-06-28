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

// @minimp3.c selfcoded - SBLIMIT
#define SBLIMIT 32
// Just a comment to check, whether the Git integration works on my computer

// @minimp3.c Z.59 - FIXHR
#define FIXHR(a) ((int)((a) * (1LL<<32) + 0.5))
// @minimp3.c Z.57 - FIXR
//			  Z.55 - FRAC_ONE
//			  Z.45 - FRAC_BITS 
#define FRAC_BITS   15
#define FRAC_ONE    (1 << FRAC_BITS)
#define FIXR(a)   ((int)((a) * FRAC_ONE + 0.5))

// @minimp3.c Z.777ff - Cx, icos36, icos36h
#define C1 FIXHR(0.98480775301220805936/2)
#define C2 FIXHR(0.93969262078590838405/2)
#define C3 FIXHR(0.86602540378443864676/2)
#define C4 FIXHR(0.76604444311897803520/2)
#define C5 FIXHR(0.64278760968653932632/2)
#define C6 FIXHR(0.5/2)
#define C7 FIXHR(0.34202014332566873304/2)
#define C8 FIXHR(0.17364817766693034885/2)

static const int icos36[9] = {
	FIXR(0.50190991877167369479),
	FIXR(0.51763809020504152469), //0
	FIXR(0.55168895948124587824),
	FIXR(0.61038729438072803416),
	FIXR(0.70710678118654752439), //1
	FIXR(0.87172339781054900991),
	FIXR(1.18310079157624925896),
	FIXR(1.93185165257813657349), //2
	FIXR(5.73685662283492756461),
};

static const int icos36h[9] = {
	FIXHR(0.50190991877167369479 / 2),
	FIXHR(0.51763809020504152469 / 2), //0
	FIXHR(0.55168895948124587824 / 2),
	FIXHR(0.61038729438072803416 / 2),
	FIXHR(0.70710678118654752439 / 2), //1
	FIXHR(0.87172339781054900991 / 2),
	FIXHR(1.18310079157624925896 / 4),
	FIXHR(1.93185165257813657349 / 4), //2
 //    FIXHR(5.73685662283492756461),
};

// @libc.h Z.16 - INLINE
#define INLINE __forceinline


 // @minimp3.c - MULH
#ifndef _MSC_VER 
#define MULL(a,b) (((int64_t)(a) * (int64_t)(b)) >> FRAC_BITS)
#define MULH(a,b) (((int64_t)(a) * (int64_t)(b)) >> 32)
#else
static INLINE int MULL(int a, int b) {
	int res;
	__asm {
		mov eax, a
		imul b
		shr eax, 15
		shl edx, 17
		or eax, edx
		mov res, eax
	}
	return res;
}
static INLINE int MULH(int a, int b) {
	int res;
	__asm {
		mov eax, a
		imul b
		mov res, edx
	}
	return res;
}
#endif

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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// IMDCT //


static void imdct12(int *out, int *in)
{
	int in0, in1, in2, in3, in4, in5, t1, t2;

	in0 = in[0 * 3];
	in1 = in[1 * 3] + in[0 * 3];
	in2 = in[2 * 3] + in[1 * 3];
	in3 = in[3 * 3] + in[2 * 3];
	in4 = in[4 * 3] + in[3 * 3];
	in5 = in[5 * 3] + in[4 * 3];
	in5 += in3;
	in3 += in1;

	in2 = MULH(2 * in2, C3);
	in3 = MULH(4 * in3, C3);

	t1 = in0 - in4;
	t2 = MULH(2 * (in1 - in5), icos36h[4]);

	out[7] =
		out[10] = t1 + t2;
	out[1] =
		out[4] = t1 - t2;

	in0 += in4 >> 1;
	in4 = in0 + in2;
	in5 += 2 * in1;
	in1 = MULH(in5 + in3, icos36h[1]);
	out[8] =
		out[9] = in4 + in1;
	out[2] =
		out[3] = in4 - in1;

	in0 -= in2;
	in5 = MULH(2 * (in5 - in3), icos36h[7]);
	out[0] =
		out[5] = in0 - in5;
	out[6] =
		out[11] = in0 + in5;
}

static void imdct36(int *out, int *buf, int *in, int *win)
{
	int i, j, t0, t1, t2, t3, s0, s1, s2, s3;
	int tmp[18], *tmp1, *in1;

	for (i = 17;i >= 1;i--)
		in[i] += in[i - 1];
	for (i = 17;i >= 3;i -= 2)
		in[i] += in[i - 2];

	for (j = 0;j<2;j++) {
		tmp1 = tmp + j;
		in1 = in + j;
		t2 = in1[2 * 4] + in1[2 * 8] - in1[2 * 2];

		t3 = in1[2 * 0] + (in1[2 * 6] >> 1);
		t1 = in1[2 * 0] - in1[2 * 6];
		tmp1[6] = t1 - (t2 >> 1);
		tmp1[16] = t1 + t2;

		t0 = MULH(2 * (in1[2 * 2] + in1[2 * 4]), C2);
		t1 = MULH(in1[2 * 4] - in1[2 * 8], -2 * C8);
		t2 = MULH(2 * (in1[2 * 2] + in1[2 * 8]), -C4);

		tmp1[10] = t3 - t0 - t2;
		tmp1[2] = t3 + t0 + t1;
		tmp1[14] = t3 + t2 - t1;

		tmp1[4] = MULH(2 * (in1[2 * 5] + in1[2 * 7] - in1[2 * 1]), -C3);
		t2 = MULH(2 * (in1[2 * 1] + in1[2 * 5]), C1);
		t3 = MULH(in1[2 * 5] - in1[2 * 7], -2 * C7);
		t0 = MULH(2 * in1[2 * 3], C3);

		t1 = MULH(2 * (in1[2 * 1] + in1[2 * 7]), -C5);

		tmp1[0] = t2 + t3 + t0;
		tmp1[12] = t2 + t1 - t0;
		tmp1[8] = t3 - t1 - t0;
	}

	i = 0;
	for (j = 0;j<4;j++) {
		t0 = tmp[i];
		t1 = tmp[i + 2];
		s0 = t1 + t0;
		s2 = t1 - t0;

		t2 = tmp[i + 1];
		t3 = tmp[i + 3];
		s1 = MULH(2 * (t3 + t2), icos36h[j]);
		s3 = MULL(t3 - t2, icos36[8 - j]);

		t0 = s0 + s1;
		t1 = s0 - s1;
		out[(9 + j)*SBLIMIT] = MULH(t1, win[9 + j]) + buf[9 + j];
		out[(8 - j)*SBLIMIT] = MULH(t1, win[8 - j]) + buf[8 - j];
		buf[9 + j] = MULH(t0, win[18 + 9 + j]);
		buf[8 - j] = MULH(t0, win[18 + 8 - j]);

		t0 = s2 + s3;
		t1 = s2 - s3;
		out[(9 + 8 - j)*SBLIMIT] = MULH(t1, win[9 + 8 - j]) + buf[9 + 8 - j];
		out[(j)*SBLIMIT] = MULH(t1, win[j]) + buf[j];
		buf[9 + 8 - j] = MULH(t0, win[18 + 9 + 8 - j]);
		buf[+j] = MULH(t0, win[18 + j]);
		i += 4;
	}

	s0 = tmp[16];
	s1 = MULH(2 * tmp[17], icos36h[4]);
	t0 = s0 + s1;
	t1 = s0 - s1;
	out[(9 + 4)*SBLIMIT] = MULH(t1, win[9 + 4]) + buf[9 + 4];
	out[(8 - 4)*SBLIMIT] = MULH(t1, win[8 - 4]) + buf[8 - 4];
	buf[9 + 4] = MULH(t0, win[18 + 9 + 4]);
	buf[8 - 4] = MULH(t0, win[18 + 8 - 4]);
}

static void compute_imdct(
	mp3_context_t *s, granule_t *g, int32_t *sb_samples, int32_t *mdct_buf)
{
	int32_t *ptr, *win, *win1, *buf, *out_ptr, *ptr1;
	int32_t out2[12];
	int i, j, mdct_long_end, v, sblimit;

	/* find last non zero block */
	ptr = g->sb_hybrid + 576;
	ptr1 = g->sb_hybrid + 2 * 18;
	while (ptr >= ptr1) {
		ptr -= 6;
		v = ptr[0] | ptr[1] | ptr[2] | ptr[3] | ptr[4] | ptr[5];
		if (v != 0)
			break;
						}
	sblimit = ((ptr - g->sb_hybrid) / 18) + 1;

	if (g->block_type == 2) {
		/* XXX: check for 8000 Hz */
		if (g->switch_point)
			mdct_long_end = 2;
		else
			mdct_long_end = 0;
	}
	else {
		mdct_long_end = sblimit;
	}

	buf = mdct_buf;
	ptr = g->sb_hybrid;
	for (j = 0;j<mdct_long_end;j++) {
		/* apply window & overlap with previous buffer */
		out_ptr = sb_samples + j;
		/* select window */
		if (g->switch_point && j < 2)
			win1 = mdct_win[0];
		else
			win1 = mdct_win[g->block_type];
		/* select frequency inversion */
		win = win1 + ((4 * 36) & -(j & 1));
		imdct36(out_ptr, buf, ptr, win);
		out_ptr += 18 * SBLIMIT;
		ptr += 18;
		buf += 18;
	}
	for (j = mdct_long_end;j<sblimit;j++) {
		/* select frequency inversion */
		win = mdct_win[2] + ((4 * 36) & -(j & 1));
		out_ptr = sb_samples + j;

		for (i = 0; i<6; i++) {
			*out_ptr = buf[i];
			out_ptr += SBLIMIT;
		}
		imdct12(out2, ptr + 0);
		for (i = 0;i<6;i++) {
			*out_ptr = MULH(out2[i], win[i]) + buf[i + 6 * 1];
			buf[i + 6 * 2] = MULH(out2[i + 6], win[i + 6]);
			out_ptr += SBLIMIT;
		}
		imdct12(out2, ptr + 1);
		for (i = 0;i<6;i++) {
			*out_ptr = MULH(out2[i], win[i]) + buf[i + 6 * 2];
			buf[i + 6 * 0] = MULH(out2[i + 6], win[i + 6]);
			out_ptr += SBLIMIT;
		}
		imdct12(out2, ptr + 2);
		for (i = 0;i<6;i++) {
			buf[i + 6 * 0] = MULH(out2[i], win[i]) + buf[i + 6 * 0];
			buf[i + 6 * 1] = MULH(out2[i + 6], win[i + 6]);
			buf[i + 6 * 2] = 0;
		}
		ptr += 18;
		buf += 18;
	}
	/* zero bands */
	for (j = sblimit;j<SBLIMIT;j++) {
		/* overlap */
		out_ptr = sb_samples + j;
		for (i = 0;i<18;i++) {
			*out_ptr = buf[i];
			buf[i] = 0;
			out_ptr += SBLIMIT;
		}
		buf += 18;
	}
}
