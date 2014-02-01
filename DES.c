#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "des.h"

static void ComputeRoundKey(uint8_t roundKey[56], uint8_t key[56]);
static void RotateRoundKeyLeft(uint8_t roundKey[56]);
static void RotateRoundKeyRight(uint8_t roundKey[56]);
static void ComputeIP(uint8_t L[32], uint8_t R[32], uint8_t inBlk[64]);
static void ComputeFP(uint8_t outBlk[64], uint8_t L[32], uint8_t R[32]);
static void ComputeF(uint8_t fout[32], uint8_t R[32], uint8_t roundKey[56]);
static void ComputeP(uint8_t output[32], uint8_t input[32]);
static void ComputeS_Lookup(int k, uint8_t output[4], uint8_t input[6]);
static void ComputePC2(uint8_t subkey[48], uint8_t roundKey[56]);
static void ComputeExpansionE(uint8_t expandedBlock[48], uint8_t R[32]);
static void DumpBin(char *str, uint8_t *b, int bits);
static void Exchange_L_and_R(uint8_t L[32], uint8_t R[32]);


static int table_DES_IP[64] = {
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25,
    32,  0, 40,  8, 48, 16, 56, 24
};



static int table_DES_FP[64] = {
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7,
    56, 48, 40, 32, 24, 16,  8,  0,
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6
};


static int table_DES_PC1[56] = {
    27, 19, 11, 31, 39, 47, 55,
    26, 18, 10, 30, 38, 46, 54,
    25, 17,  9, 29, 37, 45, 53,
    24, 16,  8, 28, 36, 44, 52,
    23, 15,  7,  3, 35, 43, 51,
    22, 14,  6,  2, 34, 42, 50,
    21, 13,  5,  1, 33, 41, 49,
    20, 12,  4,  0, 32, 40, 48
};


static int table_DES_PC2[48] = {
    24, 27, 20,  6, 14, 10,  3, 22,
     0, 17,  7, 12,  8, 23, 11,  5,
    16, 26,  1,  9, 19, 25,  4, 15,
    54, 43, 36, 29, 49, 40, 48, 30,
    52, 44, 37, 33, 46, 35, 50, 41,
    28, 53, 51, 55, 32, 45, 39, 42
};


static int table_DES_E[48] = {
    31,  0,  1,  2,  3,  4,  3,  4,
     5,  6,  7,  8,  7,  8,  9, 10,
    11, 12, 11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20, 19, 20,
    21, 22, 23, 24, 23, 24, 25, 26,
    27, 28, 27, 28, 29, 30, 31,  0
};


static int table_DES_P[32] = {
    11, 17,  5, 27, 25, 10, 20,  0,
    13, 21,  3, 28, 29,  7, 18, 24,
    31, 22, 12,  6, 26,  2, 16,  8,
    14, 30,  4, 19,  1,  9, 15, 23
};


static int table_DES_S[8][64] = {
    /* table S[0] */
        {   13,  1,  2, 15,  8, 13,  4,  8,  6, 10, 15,  3, 11,  7,  1,  4,
            10, 12,  9,  5,  3,  6, 14, 11,  5,  0,  0, 14, 12,  9,  7,  2,
             7,  2, 11,  1,  4, 14,  1,  7,  9,  4, 12, 10, 14,  8,  2, 13,
             0, 15,  6, 12, 10,  9, 13,  0, 15,  3,  3,  5,  5,  6,  8, 11  },
    /* table S[1] */
        {    4, 13, 11,  0,  2, 11, 14,  7, 15,  4,  0,  9,  8,  1, 13, 10,
             3, 14, 12,  3,  9,  5,  7, 12,  5,  2, 10, 15,  6,  8,  1,  6,
             1,  6,  4, 11, 11, 13, 13,  8, 12,  1,  3,  4,  7, 10, 14,  7,
            10,  9, 15,  5,  6,  0,  8, 15,  0, 14,  5,  2,  9,  3,  2, 12  },
    /* table S[2] */
        {   12, 10,  1, 15, 10,  4, 15,  2,  9,  7,  2, 12,  6,  9,  8,  5,
             0,  6, 13,  1,  3, 13,  4, 14, 14,  0,  7, 11,  5,  3, 11,  8,
             9,  4, 14,  3, 15,  2,  5, 12,  2,  9,  8,  5, 12, 15,  3, 10,
             7, 11,  0, 14,  4,  1, 10,  7,  1,  6, 13,  0, 11,  8,  6, 13  },
    /* table S[3] */
        {    2, 14, 12, 11,  4,  2,  1, 12,  7,  4, 10,  7, 11, 13,  6,  1,
             8,  5,  5,  0,  3, 15, 15, 10, 13,  3,  0,  9, 14,  8,  9,  6,
             4, 11,  2,  8,  1, 12, 11,  7, 10,  1, 13, 14,  7,  2,  8, 13,
            15,  6,  9, 15, 12,  0,  5,  9,  6, 10,  3,  4,  0,  5, 14,  3  },
    /* table S[4] */
        {    7, 13, 13,  8, 14, 11,  3,  5,  0,  6,  6, 15,  9,  0, 10,  3,
             1,  4,  2,  7,  8,  2,  5, 12, 11,  1, 12, 10,  4, 14, 15,  9,
            10,  3,  6, 15,  9,  0,  0,  6, 12, 10, 11,  1,  7, 13, 13,  8,
            15,  9,  1,  4,  3,  5, 14, 11,  5, 12,  2,  7,  8,  2,  4, 14  },
    /* table S[5] */
        {   10, 13,  0,  7,  9,  0, 14,  9,  6,  3,  3,  4, 15,  6,  5, 10,
             1,  2, 13,  8, 12,  5,  7, 14, 11, 12,  4, 11,  2, 15,  8,  1,
            13,  1,  6, 10,  4, 13,  9,  0,  8,  6, 15,  9,  3,  8,  0,  7,
            11,  4,  1, 15,  2, 14, 12,  3,  5, 11, 10,  5, 14,  2,  7, 12  },
    /* table S[6] */
        {   15,  3,  1, 13,  8,  4, 14,  7,  6, 15, 11,  2,  3,  8,  4, 14,
             9, 12,  7,  0,  2,  1, 13, 10, 12,  6,  0,  9,  5, 11, 10,  5,
             0, 13, 14,  8,  7, 10, 11,  1, 10,  3,  4, 15, 13,  4,  1,  2,
             5, 11,  8,  6, 12,  7,  6, 12,  9,  0,  3,  5,  2, 14, 15,  9  },
    /* table S[7] */
        {   14,  0,  4, 15, 13,  7,  1,  4,  2, 14, 15,  2, 11, 13,  8,  1,
             3, 10, 10,  6,  6, 12, 12, 11,  5,  9,  9,  5,  0,  3,  7,  8,
             4, 15,  1, 12, 14,  8,  8,  2, 13,  4,  6,  9,  2,  1, 11,  7,
            15,  5, 12, 11,  9,  3,  7, 14,  3, 10, 10,  0,  5,  6,  0, 13  }
};






void EncryptDES(uint8_t key[56], uint8_t outBlk[64], uint8_t inBlk[64]) {
  int i,round;
  uint8_t R[32], L[32], fout[32];
  uint8_t roundKey[56];

  ComputeRoundKey(roundKey, key);
  ComputeIP(L,R,inBlk);

  for (round = 0; round < 16; round++) {
    RotateRoundKeyLeft(roundKey);
    if (round != 0 && round != 1 && round != 8 && round != 15) RotateRoundKeyLeft(roundKey);
    ComputeF(fout, R, roundKey);
    for (i = 0; i < 32; i++) L[i] ^= fout[i];
    Exchange_L_and_R(L,R);
  }

  Exchange_L_and_R(L,R);

  ComputeFP(outBlk,L,R);
}



void DecryptDES(uint8_t key[56], uint8_t outBlk[64], uint8_t inBlk[64]) {
  int i,round;
  uint8_t R[32], L[32], fout[32];
  uint8_t roundKey[56];


  ComputeRoundKey(roundKey, key);

  ComputeIP(L,R,inBlk);

  for (round = 0; round < 16; round++) {

    ComputeF(fout, R, roundKey);
    for (i = 0; i < 32; i++) L[i] ^= fout[i];
    Exchange_L_and_R(L,R);

    RotateRoundKeyRight(roundKey);
    if (round != 0 && round != 7 && round != 14 && round != 15)
      RotateRoundKeyRight(roundKey);
  }

  Exchange_L_and_R(L,R);

  ComputeFP(outBlk,L,R);
}




static void ComputeRoundKey(uint8_t roundKey[56], uint8_t key[56]) {
  int i;

  for (i = 0; i < 56; i++)
    roundKey[table_DES_PC1[i]] = key[i];
}



static void RotateRoundKeyLeft(uint8_t roundKey[56]) {
  uint8_t temp1, temp2;
  int i;

  temp1 = roundKey[27];
  temp2 = roundKey[55];
  for (i = 27; i >= 1; i--) {
    roundKey[i] = roundKey[i-1];
    roundKey[i+28] = roundKey[i+28-1];
  }
  roundKey[ 0] = temp1;
  roundKey[28] = temp2;
}



static void RotateRoundKeyRight(uint8_t roundKey[56]) {
  uint8_t temp1, temp2;
  int i;

  temp1 = roundKey[0];
  temp2 = roundKey[28];
  for (i = 0; i < 27; i++) {
    roundKey[i] = roundKey[i+1];
    roundKey[i+28] = roundKey[i+28+1];
  }
  roundKey[27] = temp1;
  roundKey[55] = temp2;
}



static void ComputeIP(uint8_t L[32], uint8_t R[32], uint8_t inBlk[64]) {
  uint8_t output[64];
  int i;


  for (i = 63; i >= 0; i--)
    output[table_DES_IP[i]] = inBlk[i];

  
  for (i = 63; i >= 0; i--) {
    if (i >= 32)
      L[i-32] = output[i];
    else
      R[i] = output[i];
  }
}



static void ComputeFP(uint8_t outBlk[64], uint8_t L[32], uint8_t R[32]) {
  uint8_t input[64];
  int i;


  for (i = 63; i >= 0; i--)
    input[i] = (i >= 32) ? L[i - 32] : R[i];

  for (i = 63; i >= 0; i--)
    outBlk[table_DES_FP[i]] = input[i];
}




static void ComputeF(uint8_t fout[32], uint8_t R[32], uint8_t roundKey[56]) {
  uint8_t expandedBlock[48], subkey[48], sout[32];
  int i,k;


  ComputeExpansionE(expandedBlock, R);


  ComputePC2(subkey, roundKey);


  for (i = 0; i < 48; i++)
    expandedBlock[i] ^= subkey[i];

  for (k = 0; k < 8; k++)
    ComputeS_Lookup(k, sout+4*k, expandedBlock+6*k);

  ComputeP(fout, sout);
}



static void ComputeP(uint8_t output[32], uint8_t input[32]) {
  int i;

  for (i = 0; i < 32; i++)
    output[table_DES_P[i]] = input[i];
}



static void ComputeS_Lookup(int k, uint8_t output[4], uint8_t input[6]) {
  int inputValue, outputValue;


  inputValue = input[0] + 2*input[1] + 4*input[2] + 8*input[3] +
          16*input[4] + 32*input[5];


  outputValue = table_DES_S[k][inputValue];


  output[0] = (outputValue & 1) ? 1 : 0;
  output[1] = (outputValue & 2) ? 1 : 0;
  output[2] = (outputValue & 4) ? 1 : 0;
  output[3] = (outputValue & 8) ? 1 : 0;
}


static void ComputePC2(uint8_t subkey[48], uint8_t roundKey[56]) {
  int i;

  for (i = 0; i < 48; i++)
    subkey[i] = roundKey[table_DES_PC2[i]];
}



static void ComputeExpansionE(uint8_t expandedBlock[48], uint8_t R[32]) {
  int i;

  for (i = 0; i < 48; i++)
    expandedBlock[i] = R[table_DES_E[i]];
}



static void Exchange_L_and_R(uint8_t L[32], uint8_t R[32]) {
  int i;

  for (i = 0; i < 32; i++)
    L[i] ^= R[i] ^= L[i] ^= R[i];                 /* exchanges L[i] and R[i] */
}




