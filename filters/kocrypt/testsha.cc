#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"


int main() {
SHA1 *sha1;
unsigned char data[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
unsigned long et[] = {0x11223344};
int rc;

  printf("%d:  0x11 == %d and 0x44 == %d\n", ((unsigned char *)et)[0],
                                             0x11, 0x44);
  sha1 = new SHA1();

  if (!sha1->readyToGo()) {
     printf("Error: not ready to go!\n");
     return -1;
  }

  printf("About to process [%s]\n", data);
  rc = sha1->process(data, strlen((char *)data));

  if (rc != strlen((char *)data)) {
     printf("Error processing the data.  rc=%d\n", rc);
  } else printf("Done.\n");

const unsigned char *res = sha1->getHash();

  if (res) {
     for (int i = 0; i < 20; i++) {
        printf("%.2X", *res++);
        if (i>0 && (i-1)%2 == 0) printf(" ");
     }
     printf("\n");
  } else printf("Error - getHash() returned NULL!\n");

  delete sha1;
}



