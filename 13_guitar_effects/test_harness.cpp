#include "effects.h"
#include <cstdio>
#include <cstdint>

int main()
{

  FILE *inf = fopen("testfile", "r");
  FILE *outf = fopen("outfile", "w");
  effects e; 
  e.initialise();

  while(!feof(inf))
  {
    int16_t sample = fgetc(inf);
    sample |= fgetc(inf) << 8;
    sample -= 32768;

    printf("%i \n", sample);
    e.process_sample(sample);

    fputc((sample & 0xff), outf);
    fputc((sample >> 8), outf);
  }  
  fclose(inf);
  fclose(outf);

  return 0;
}
