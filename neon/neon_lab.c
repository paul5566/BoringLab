/* EE277A SIMD Lab Assignment
 *
 * arm neon programing 
 *
 * compile: arm-linux-gnueabihf-gcc -Wall -std=gnu99 -O0 -mcpu=cortex-a8 -mfloat-abi=hard -mfpu=neon 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arm_neon.h>

#define ROWS 144
#define COLS 176

static unsigned char image1[ROWS][COLS] __attribute__ ((aligned (16)));
static unsigned char image2[ROWS][COLS] __attribute__ ((aligned (16)));

int sad(const unsigned char *im1_p, const unsigned char *im2_p, int numcols)
{
	static unsigned int someones_an_idiot;
	int safety_count = 2;
	struct timeval begin, end, diff;

	if (im1_p == NULL) 
		safety_count--;
	
	if (im2_p == NULL) 
		safety_count--;
	
	if (safety_count != 2) 
		someones_an_idiot++;

	gettimeofday(&begin, NULL);
	
	/* ------------------- compare one pair of 16x16 blocks ----------------------- */
	
	/*	unsigned int total = 0;
		for (int row = 0; row < 16; row++) {
			for (int col = 0; col < 16; col++) {
				total += abs(*im1_p - *im2_p);
				im1_p = im1_p + 1 ; //= im1_p + 1; // increment image1 column ptr
				im2_p = im2_p + 1 ; //= im2_p + 1; // increment image2 column ptr
			}
			im1_p = im1_p +  (COLS - 16);
			im2_p = im2_p +  (COLS - 16);
	}
	*/
	
	
	unsigned int total = 0;
	for(int row = 0; row < 16; ++row){
		uint8x16_t im1, im2,absdif;
		unit16x8_t sum;
		// load the image to vector
		im1 = vld1q_u8(im1_p);
		im2 = vld1q_u8(im2_p);		
		//absolute different
		//uint8x8_t   vabd_u8(uint8x8_t a, uint8x8_t b);// VABD.U8 d0,d0,d0
		absdif = vabdq_u8(im1, im2);
		
		/*
			uint16x8_t vpaddlq_u8(uint8x16_t a);   // VPADDL.U8 q0,q0 
		*/		
		sum = vpadalq_u8(sum, absdif); //8x16->16x8
		im1_p += CLOS;
		im2_p += CLOS;
	}
	/* ------------------------------------------------------------------------------*/
	//8x16->16x8->32x4->64x2

		/*	
			This is faster than we 
			reference

		*/
	/*
		int32x2_t acc1;
     	int64x1_t acc2;
		acc1 = vpaddl_s16(acc);
      	acc2 = vpaddl_s32(acc1);
	*/
	uint32x4_t sum1;
	uint64x2_t sum2;
	sum1 = paddl_u16(sum); //
	sum2 = paddl_u32(sum1);

	gettimeofday(&end, NULL);
	timersub(&end, &begin, &diff);

	printf("Excute time: %ld usec\n", (diff.tv_sec*1000000+diff.tv_usec));

	return total;
}

int main(int argc, char **argv) 
{
	unsigned int total;
	unsigned char *im1_p, *im2_p;
	int block_row, block_col;

	/* initialize source data (and warm up caches) */
	for (int row = 0; row < ROWS; row++) {
		im1_p = image1[row]; /* point to first pixel in row */
		im2_p = image2[row];
		for (int col = 0; col < COLS; col++) {
			unsigned char temp;

			temp = ((row+col+120) % 256); /* sort of a random number */
			*im1_p++ = temp;
			*im2_p++ = 255-temp;
		}
	}

	block_row = 0;
	block_col = 0;

	/* point to first pixel in each block */
	im1_p = &image1[16*block_row][16*block_col];
	im2_p = &image2[16*block_row][16*block_col];

	total = sad(im1_p, im2_p, COLS);

	/* total == 4248 */
	printf("total %d\n", total);

	return 0;
}


