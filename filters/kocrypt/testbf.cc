#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blowfish.h"
#include "cbc.h"


int main() {
BlockCipher *bf;
char data[] = "This is a test.";
char expect[] = "\x22\x30\x7e\x2f\x42\x28\x44\x01\xda\xdf\x5a\x81\xd7\xe5\x7c\xd0";
char key[] = "testkey";
unsigned long et[] = {0x11223344};

  printf("%d:  0x11 == %d and 0x44 == %d\n", ((unsigned char *)et)[0],
                                             0x11, 0x44);
  bf = new BlowFish();
//  bf = new CipherBlockChain(new BlowFish());

  bf->setKey((void *)key, 7*8);

  if (!bf->readyToGo()) {
     printf("Error: not ready to go!\n");
     return -1;
  }

  printf("About to encrypt...\n"); fflush(stdout);
  if (-1 == bf->encrypt((void *)data, 8)) {
     printf("Error: encrypt failed!\n");
     return -1;
  }
  printf("About to encrypt part 2...\n"); fflush(stdout);
  bf->encrypt((void *)(data+8), 8);

  printf("Encryption done.  data[] is now: ");
  for (int i = 0; i < 16; i++) {
     printf("0x%x ", data[i]&0xff);
     if ((data[i]&0xff) != (expect[i]&0xff)) {
        printf("Error.  This byte failed the comparison.  It should have been 0x%x.\n", expect[i]&0xff);
        return -1;
     }
  }
  printf("\n");

  delete bf;
  bf = new BlowFish();
//  bf = new CipherBlockChain(new BlowFish());
  bf->setKey((void *)key, 7*8);

  printf("About to decrypt...\n"); fflush(stdout);
  if (-1 == bf->decrypt((void *)data, 16)) {
     printf("Error: decrypt failed!\n");
     return -1;
  }
  //bf->decrypt((void *)(data+8), 8);

  printf("All done!  Result...  data[] = \"%s\"\n", data);
  if (strcmp(data, "This is a test.")) {
      printf("ERROR. Decryption failed.\n");
      return -1;
  }

  delete bf;
}



