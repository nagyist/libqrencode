#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrencode_inner.h"
#include "../qrspec.h"
#include "../qrinput.h"

void test_iterate()
{
	int i;
	QRinput *stream;
	char num[9] = "01234567";
	unsigned char *data;
	QRRawCode *raw;
	int err = 0;

	testStart("Test getCode (1-L)");
	stream = QRinput_new();
	QRenc_setVersion(stream, 1);
	QRenc_setErrorCorrectionLevel(stream, QR_ECLEVEL_L);
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);

	raw = QRraw_new(stream);
	data = raw->datacode;
	for(i=0; i<raw->dataLength; i++) {
		if(data[i] != QRraw_getCode(raw)) {
			err++;
		}
	}

	QRinput_free(stream);
	QRraw_free(raw);
	testEnd(err);
}

void test_iterate2()
{
	int i;
	QRinput *stream;
	char num[9] = "01234567";
	QRRawCode *raw;
	int err = 0;
	unsigned char correct[] = {
	0x10, 0x11, 0xec, 0xec, 0x20, 0xec, 0x11, 0x11,
	0x0c, 0x11, 0xec, 0xec, 0x56, 0xec, 0x11, 0x11,
	0x61, 0x11, 0xec, 0xec, 0x80, 0xec, 0x11, 0x11,
	0xec, 0x11, 0xec, 0xec, 0x11, 0xec, 0x11, 0x11,
	0xec, 0x11, 0xec, 0xec, 0x11, 0xec, 0x11, 0x11,
	0xec, 0x11, 0xec, 0xec, 0x11, 0x11,
	0x5c, 0xde, 0x68, 0x68, 0x4d, 0xb3, 0xdb, 0xdb,
	0xd5, 0x14, 0xe1, 0xe1, 0x5b, 0x2a, 0x1f, 0x1f,
	0x49, 0xc4, 0x78, 0x78, 0xf7, 0xe0, 0x5b, 0x5b,
	0xc3, 0xa7, 0xc1, 0xc1, 0x5d, 0x9a, 0xea, 0xea,
	0x48, 0xad, 0x9d, 0x9d, 0x58, 0xb3, 0x3f, 0x3f,
	0x10, 0xdb, 0xbf, 0xbf, 0xeb, 0xec, 0x05, 0x05,
	0x98, 0x35, 0x83, 0x83, 0xa9, 0x95, 0xa6, 0xa6,
	0xea, 0x7b, 0x8d, 0x8d, 0x04, 0x3c, 0x08, 0x08,
	0x64, 0xce, 0x3e, 0x3e, 0x4d, 0x9b, 0x30, 0x30,
	0x4e, 0x65, 0xd6, 0xd6, 0xe4, 0x53, 0x2c, 0x2c,
	0x46, 0x1d, 0x2e, 0x2e, 0x29, 0x16, 0x27, 0x27
	};

	testStart("Test getCode (5-H)");
	stream = QRinput_new();
	QRenc_setVersion(stream, 5);
	QRenc_setErrorCorrectionLevel(stream, QR_ECLEVEL_H);
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);

	raw = QRraw_new(stream);
	for(i=0; i<raw->dataLength; i++) {
		if(correct[i] != QRraw_getCode(raw)) {
			err++;
		}
	}

	QRinput_free(stream);
	QRraw_free(raw);
	testEnd(err);
}

void print_filler(void)
{
	int width;
	int x, y;
	int version = 5;
	unsigned char *frame;

	width = QRspec_getWidth(version);
	frame = QRenc_fillerTest(version);

	for(y=0; y<width; y++) {
		for(x=0; x<width; x++) {
			printf("%02x ", *frame++);
		}
		printf("\n");
	}
}

void test_filler(void)
{
	int i;
	unsigned char *frame;
	int err = 0;
	int j, w, e;

	testStart("Frame fillter test");
	for(i=1; i<=QRSPEC_VERSION_MAX; i++) {
		frame = QRenc_fillerTest(i);
		if(frame == NULL) {
			printf("Something wrong in version %d\n", i);
			err++;
		} else {
			w = QRspec_getWidth(i);
			e = 0;
			for(j=0; j<w*w; j++) {
				if(frame[j] == 0) e++;
			}
			free(frame);
			if(e) {
				printf("Non-filled bit was found in version %d\n", i);
				err++;
			}
		}

	}
	testEnd(err);
}

void print_mask(void)
{
	int mask;
	int x, y;
	int version = 4;
	int width;
	unsigned char *frame, *masked, *p;

	width = QRspec_getWidth(version);
	frame = (unsigned char *)malloc(width * width);
	memset(frame, 0x20, width * width);
	for(mask=0; mask<8; mask++) {
		masked = QRenc_makeMask(width, frame, mask);
		p = masked;
		printf("mask %d:\n", mask);
		for(y=0; y<width; y++) {
			for(x=0; x<width; x++) {
				if(*p & 1) {
					printf("#");
				} else {
					printf(" ");
				}
				p++;
			}
			printf("\n");
		}
		printf("\n");
		free(masked);
	}
	free(frame);
}

void test_format(void)
{
	unsigned char *frame;
	unsigned int format;
	int width;
	int i;
	unsigned int decode;

	testStart("Test format information(level L,mask 0)");
	width = QRspec_getWidth(1);
	frame = QRspec_newFrame(1);
	format = QRspec_getFormatInfo(1, QR_ECLEVEL_L);
	QRenc_writeFormatInformation(width, frame, 1, QR_ECLEVEL_L);
	decode = 0;
	for(i=0; i<8; i++) {
		decode = decode << 1;
		decode |= frame[width * 8 + i + (i > 5)] & 1;
	}
	for(i=0; i<7; i++) {
		decode = decode << 1;
		decode |= frame[width * ((6 - i) + (i < 1)) + 8] & 1;
	}
	if(decode != format) {
		printf("Upper-left format information is invalid.\n");
		printf("%08x, %08x\n", format, decode);
		testEnd(1);
		return;
	}
	decode = 0;
	for(i=0; i<7; i++) {
		decode = decode << 1;
		decode |= frame[width * (width - 1 - i) + 8] & 1;
	}
	for(i=0; i<8; i++) {
		decode = decode << 1;
		decode |= frame[width * 8 + width - 8 + i] & 1;
	}
	if(decode != format) {
		printf("Bottom and right format information is invalid.\n");
		printf("%08x, %08x\n", format, decode);
		testEnd(1);
		return;
	}

	free(frame);

	testEnd(0);
}

#define N1 (3)
#define N2 (3)
#define N3 (40)
#define N4 (10)

void test_eval(void)
{
	unsigned char *frame;
	int w = 6;
	int demerit;

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (all white)");
	memset(frame, 0, w * w);
	demerit = QRenc_evaluateSymbol(w, frame);
	testEndExp(demerit == ((N1 + 1)*w*2 + N2 * (w - 1) * (w - 1)));

	testStart("Test mask evaluation (all black)");
	memset(frame, 1, w * w);
	demerit = QRenc_evaluateSymbol(w, frame);
	testEndExp(demerit == ((N1 + 1)*w*2 + N2 * (w - 1) * (w - 1)));

	free(frame);
}

/* .#.#.#.#.#
 * #.#.#.#.#.
 * ..##..##..
 * ##..##..##
 * ...###...#
 * ###...###.
 * ....####..
 * ####....##
 * .....#####
 * #####.....
 */
void test_eval2(void)
{
	unsigned char *frame;
	int w = 10;
	int demerit;
	int x;

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (run length penalty check)");
	for(x=0; x<w; x++) {
		frame[      x] = x & 1;
		frame[w   + x] = (x & 1) ^ 1;
		frame[w*2 + x] = (x / 2) & 1;
		frame[w*3 + x] = ((x / 2) & 1) ^ 1;
		frame[w*4 + x] = (x / 3) & 1;
		frame[w*5 + x] = ((x / 3) & 1) ^ 1;
		frame[w*6 + x] = (x / 4) & 1;
		frame[w*7 + x] = ((x / 4) & 1) ^ 1;
		frame[w*8 + x] = (x / 5) & 1;
		frame[w*9 + x] = ((x / 5) & 1) ^ 1;
	}
	demerit = QRenc_evaluateSymbol(w, frame);
	testEndExp(demerit == N1 * 4 + N2 * 4);

	free(frame);
}

void test_eval3(void)
{
	unsigned char *frame;
	int w = 15;
	int demerit;
	int x, y;
	static unsigned char pattern[7][15] = {
		{0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0}, // N3x1
		{1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1}, // N3x1
		{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1}, // N3x1
		{1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0}, // 0
		{1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1}, // N3x2
		{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0}, // N3 + (N1+1)
		{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}  // (N1+1)
	};

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (1:1:3:1:1 check)");
	for(y=0; y<5; y++) {
		for(x=0; x<w; x++) {
			frame[w*y*2     + x] = pattern[y][x];
			frame[w*(y*2+1) + x] = pattern[y][x]^1;
		}
	}
	for(x=0; x<w; x++) {
		frame[w*10 + x] = x & 1;
	}
	for(y=5; y<7; y++) {
		for(x=0; x<w; x++) {
			frame[w*(y*2+1) + x] = pattern[y][x];
			frame[w*(y*2+2) + x] = pattern[y][x]^1;
		}
	}
	demerit = QRenc_evaluateSymbol(w, frame);
	testEndExp(demerit == N3 * 6 + (N1 + 1) * 4);

	free(frame);
}

unsigned int m1pat[8][21] = {
	{0x1fc77f, 0x105c41, 0x174c5d, 0x174b5d, 0x175b5d, 0x104241, 0x1fd57f,
 	 0x000000, 0x154512, 0x1a16a2, 0x0376ee, 0x19abb2, 0x04eee1, 0x001442,
	 0x1fc111, 0x10444b, 0x175d5d, 0x174aae, 0x175ae5, 0x1043b8, 0x1fd2e5},
	{0x1fdd7f, 0x104641, 0x17565d, 0x17415d, 0x17415d, 0x105841, 0x1fd57f,
	 0x000a00, 0x146f25, 0x10bc08, 0x09dc44, 0x130118, 0x0e444b, 0x001ee8,
	 0x1fdbbb, 0x104ee1, 0x1747f7, 0x174004, 0x17504f, 0x104912, 0x1fd84f},
	{0x1fcb7f, 0x104f41, 0x17505d, 0x17585d, 0x17575d, 0x105141, 0x1fd57f,
	 0x001300, 0x17c97c, 0x02b52c, 0x046a9f, 0x01083c, 0x03f290, 0x0017cc,
	 0x1fcd60, 0x1057c5, 0x17512c, 0x175920, 0x175694, 0x104036, 0x1fde94},
	{0x1fdb7f, 0x105441, 0x174d5d, 0x17585d, 0x174c5d, 0x104c41, 0x1fd57f,
	 0x001800, 0x16e44b, 0x02b52c, 0x12f1f2, 0x1a258a, 0x03f290, 0x001ca1,
	 0x1fd0d6, 0x1057c5, 0x174a41, 0x175496, 0x175694, 0x104b5b, 0x1fd322},
	{0x1fd37f, 0x104741, 0x17475d, 0x175f5d, 0x175f5d, 0x105941, 0x1fd57f,
	 0x001400, 0x1171f9, 0x0c8dcf, 0x15ed83, 0x108f20, 0x0dca73, 0x001f2f,
	 0x1fda7c, 0x1040d9, 0x1759cf, 0x1741c3, 0x174188, 0x10472a, 0x1fd677},
	{0x1fcd7f, 0x105741, 0x17505d, 0x17545d, 0x17475d, 0x104941, 0x1fd57f,
	 0x001b00, 0x1059ce, 0x05a95d, 0x046a9f, 0x03001c, 0x0e444b, 0x001fec,
	 0x1fcd60, 0x104bb4, 0x17412c, 0x174100, 0x17404f, 0x104816, 0x1fde94},
	{0x1fdd7f, 0x105741, 0x17545d, 0x17445d, 0x17555d, 0x104f41, 0x1fd57f,
	 0x000b00, 0x13fd97, 0x05a95d, 0x00f8d6, 0x028604, 0x0e444b, 0x001f2f,
	 0x1fd9f2, 0x105bb4, 0x175365, 0x175718, 0x17404f, 0x1048d5, 0x1fda06},
	{0x1fc77f, 0x104841, 0x174e5d, 0x174b5d, 0x174f5d, 0x105041, 0x1fd57f,
	 0x000400, 0x12d7a0, 0x1a16a2, 0x0a527c, 0x1d39fb, 0x04eee1, 0x0010d0,
	 0x1fc358, 0x10544b, 0x1749cf, 0x1758e7, 0x174ae5, 0x10472a, 0x1fd0ac}
};

void test_encode(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	unsigned char *frame;
	int err = 0;
	int x, y, w;
	int mask;
	QRcode *qrcode;

	testStart("Test encode (1-M)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	for(mask=0; mask<8; mask++) {
		qrcode = QRcode_encodeMask(stream, 1, QR_ECLEVEL_M, mask);
		w = qrcode->width;
		frame = qrcode->data;
		for(y=0; y<w; y++) {
			for(x=0; x<w; x++) {
				if(((m1pat[mask][y] >> (20-x)) & 1) != (frame[y*w+x]&1)) {
					printf("Diff in mask=%d (%d,%d)\n", mask, x, y);
					err++;
				}
			}
		}
		QRcode_free(qrcode);
	}
	QRinput_free(stream);
	testEnd(err);
}

void print_encode(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	unsigned char *frame;
	int x, y, w;
	QRcode *qrcode;

	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	qrcode = QRcode_encodeInput(stream, 1, QR_ECLEVEL_L);
	w = qrcode->width;
	frame = qrcode->data;
	for(y=0; y<w; y++) {
		for(x=0; x<w; x++) {
			printf("%02x ", frame[y*w+x]);
		}
		printf("\n");
	}
	QRinput_free(stream);
	QRcode_free(qrcode);
}

int main(int argc, char **argv)
{
	test_iterate();
	test_iterate2();
//	print_filler();
	test_filler();
//	print_mask();
	test_format();
	test_eval();
	test_eval2();
	test_eval3();
	test_encode();
//	print_encode();

	report();

	return 0;
}