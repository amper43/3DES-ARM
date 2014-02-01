#ifndef _DES_H
#define _DES_H

#include <stdint.h>

void EncryptDES(uint8_t key[56], uint8_t outBlk[64], uint8_t inBlk[64]);
void DecryptDES(uint8_t key[56], uint8_t outBlk[64], uint8_t inBlk[64]);

#endif
