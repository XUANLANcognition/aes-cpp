#include "aes.h"
#include <iostream>

using namespace std;

/*
  round keys
*/
uint32_t Rkey[44];

void subByte(uint8_t *t) {
  uint8_t tmp1 = (*t >> 4) * 16;
  uint8_t tmp2 = (*t & 0x0f);
  *t = Sbox[(int)(tmp2 + tmp1)];
}

void subWord(uint32_t *t) {
  uint8_t temp;
  for (int i = 3; i >= 0; i--) {
    temp = (*t >> (i * 8)) & 0xff;
    subByte(&temp);
    *t &= ~(0xff << i * 8);
    *t |= temp << (i * 8);
  }
}

/*
  circularly shift
  t : origin
  count
*/

void RotWord(uint32_t *t, int count) {
  uint8_t temp;

  for (int i = 0; i < count; i++) {
    temp = (*t >> 24) & 0xff;
    *t <<= 8;
    *t |= temp;
  }
}

/*
  t1 : left
  t2 : right
  t3 : length
*/

void addRoundKey(uint32_t *t1, uint32_t *t2, int t3) {
  for (int i= 0; i < t3; i++) {
    t1[i] ^= t2[i];
  }
}

/*
  generate round keys
*/

void geneRoundKeys(uint32_t key[4], uint32_t roundKeys[NK * (ROUND + 1)]) {
  uint32_t temp;

  // copy init key to the frist round
  memcpy(roundKeys, key, sizeof(uint32_t) * 4);

  for (int i = 4; i < NK * (ROUND + 1); i++) {
    temp = roundKeys[i - 1];
    if(i % NK == 0) {
      RotWord(&temp, 1); 
      subWord(&temp);
      temp ^= Rcon[i / 4 - 1];
    }
    roundKeys[i] = roundKeys[i - NK] ^ temp;
  }
}

/*
  t1 : in
  t2 : out
*/

void turn(uint32_t t1[4], uint32_t t2[4]) {

  uint32_t buf[4];

  for(int i = 0; i < 4; i++) {
    buf[i] = t1[i];
  }

  uint32_t temp = 0x00000000;

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      temp = buf[j] & (0xff000000 >> (i * 8));
      temp = temp << (i * 8);
      temp >>= (j * 8);
      t2[i] = t2[i] & ~(0xff000000 >> (j * 8));
      t2[i] |= temp;
    }
  } 
}

void shiftRow(uint32_t t[4]) {
  for(int i = 0; i < 4; i++) {
    RotWord(&t[i], i);
  }
}

void mixColumns(uint32_t t[4]) {
  turn(t, t);
  uint32_t temp[4];

  memcpy(temp, t, sizeof(uint32_t) * 4);

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      t[i] &= ~(0xff000000 >> (j * 8));
      t[i] |= ((uint32_t)(GFmulWord(mcBox[i], temp[j])) << ((3 - j) * 8));
    }
  }
}


/*
  main interface
  ******************************************************************************
  in : plaintext
  out : ciphertext
  key : key
  ******************************************************************************
*/

void enCipher(uint32_t in[4], uint32_t out[4], uint32_t key[4]) {
  
  geneRoundKeys(key, Rkey);
  addRoundKey(in, Rkey, 4);

  turn(in, in);

  for (int r = 1; r < 10; r++) {
    // subByte
    for(int i = 0; i < 4; i++) {
      subWord(in + i);
    }
    //shiftRow
    shiftRow(in);
    //mixColumns
    mixColumns(in);
    //addRoundKey
    turn(in, in);
    addRoundKey(in, Rkey + (r * 4), 4);
    turn(in, in);
  }
  turn(in, in);
  for(int i = 0; i < 4; i++) {
    subWord(in + i);
  }
  turn(in, in);
  shiftRow(in);
  turn(in, in);
  addRoundKey(in, Rkey + 40, 4);
  turn(in, in);
  memcpy(out, in, sizeof(uint32_t) * 4);
}

uint8_t GFmulWord(uint32_t f1, uint32_t f2) {
  uint8_t t = 0x00;

  for(int i = 0; i < 4; i++) {
    t ^= GFmul((f1 >> (i * 8)) & 0xff, (f2 >> (i * 8)) & 0xff);
  }

  return t;
}

/*
  multiple on GF(2 ^ 8)
  f1 : first
  f2 : second
*/

uint8_t GFmul(uint8_t f1, uint8_t f2) {
  
  if(f1 == 0) {
    return 0;
  }

  if(f1 == 0x01) {
    return f2;
  }

  uint8_t mask = 0x80;
  uint8_t r = 0x00;
  uint8_t temp = f2;
  for(int i = 0; i < 7; i++) {
    if((f1 & (mask >> i)) != 0) {
      for(int j = 0; j < (7 - i); j++) {
        if((temp & 0x80) != 0) {
          temp = (temp << 1) ^ 0x1b;
        }
        else {
          temp = temp << 1;
        }
      }
      r = r ^ temp;
      temp = f2;
    }
  }

  if((f1 & 0x01) != 0) {
    r = r ^ f2;
  }

  return r;
}