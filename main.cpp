#include "aes.h"
#include <iostream>

using namespace std;

int main() {
  uint32_t p[4] = {0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734};
  uint32_t k[4] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c};
  uint32_t c[4];

  cout << "plaintext : " << endl;

  for(int i = 0; i < 4; i++) {
    cout << hex << (int)p[i] << "\t";
  }

  cout << "\n" << "key : " << endl;

  for(int i = 0; i < 4; i++) {
    cout << hex << (int)k[i] << "\t";
  }

  enCipher(p, c, k);

  cout << "\n" << "ciphertext : " << endl;

  for(int i = 0; i < 4; i++) {
    cout << hex << (int)c[i] << "\t";
  }

  cout << endl;

  return 0;
}
