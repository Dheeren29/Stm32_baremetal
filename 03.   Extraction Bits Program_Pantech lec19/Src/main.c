/*
Extract bit positions from 9th to 14th [14:9] in a given data and save it in to another variable
Data =0x BC10;
Binary Value for 0XBC10 = 1011 1100 0001 0000;
Data to be extracted =011 110
Binary Value for 0XBC10 after shift =  0000 0000 01 01 1110;
Mask data = 0000 0000 00 11 1111 (3f)
Output Data = 01 1110 (1E);
*/

#include <stdint.h>

int main(void)
{	uint32_t data = 0xbc10;
	uint32_t extracted_bits = (data >> 9) & 0x3f;

}
