//*************************************************
//  Lightweight 1:2 Gaussian interpolation for
//  small displays
//  (F) Dzl 2019
// http://blog.dzl.dk/2019/06/08/compact-gaussian-interpolation-for-small-displays/
//  Optimized for simplicity and does not account for
//  screen edge ghosting.
//*************************************************
//  Image dimensions
#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 24

//#define SIGMA_1
//#define SIGMA_05
#define SIGMA_2

//  Different kernels:
#if defined(SIGMA_1)
//Sigma=1
#define P0 (0.077847)
#define P1 (0.123317+0.077847)
#define P2 (0.195346+0.123317+0.123317+0.077847)
#elif defined(SIGMA_05)
  //Sigma=0.5
  #define P0 (0.024879)
  #define P1 (0.107973+0.024879)
  #define P2 (0.468592+0.107973+0.107973+0.024879)
#elif defined(SIGMA_2)

  //Sigma=2
  #define P0 (0.102059)
  #define P1 (0.115349+0.102059)
  #define P2 (0.130371+0.115349+0.115349+0.102059)
#endif

class GBlur
{
    //-Offsets
    const int offsets[4][4] =
    {
      { -IMAGE_WIDTH - 1, -IMAGE_WIDTH, -1, 0},
      { -IMAGE_WIDTH, -IMAGE_WIDTH + 1, 0, 1},
      { -1, 0, IMAGE_WIDTH, IMAGE_WIDTH + 1},
      {0, 1, IMAGE_WIDTH, IMAGE_WIDTH + 1}
    };
    const float kernel[4][4] =
    {
      {P0, P1, P1, P2},
      {P1, P0, P2, P1},
      {P1, P2, P0, P1},
      {P2, P1, P1, P0}
    };

  public:
    //*************************************************
    //  This method takes two pixel arrays:
    //  'source' and 'dest'.
    //  For speed 'IMAGE_WIDTH' and 'IMAGE_HEIGHT'
    //  (original image size) are pre defiend in the top.
    //  'source' is the original (monochrome) pixels
    //  and 'dest' is the interpolated pixels.
    //  the size of 'source' is IMAGE_WIDTH*IMAGE_HEIGHT
    //  and 'dest' is IMAGE_WIDTH*IMAGE_HEIGHT*4
    //*************************************************
    void calculate(float *source, float *dest)
    {
      float pix;
      //For rest of  output pixel:
      for (int i = 1; i < IMAGE_WIDTH * IMAGE_HEIGHT * 4; i++)
      {
        int sourceAddress = ((i >> 1) & 0x1f) + ((i & 0xffffff80) >> 2);
        pix = 0;
        int q = (i & 0x00000001) + ((i & 0x00000040) >> 5);   //Calculation to perform
        for (int z = 0; z < 4; z++)
        {
          int sa = sourceAddress + offsets[q][z];
          if (sa > 0 && sa < IMAGE_WIDTH * IMAGE_HEIGHT)
            pix += kernel[q][z] * source[sa];
          dest[i] = pix;
        }
      }
    }

    void calculate(float *source, float *dest, int source_width, int source_height)
    {
        float pix;

        const int offsets2[4][4] =
        {
            { -source_width - 1, -source_width, -1, 0},
            { -source_width, -source_width + 1, 0, 1},
            { -1, 0, source_width, source_width + 1},
            {0, 1, source_width, source_width + 1}
        };
        //For rest of  output pixel:
        for (int i = 1; i < source_width * source_height * 4; i++)
        {
            int sourceAddress = ((i >> 1) & 0x1f) + ((i & 0xffffff80) >> 2);
            pix = 0;
            int q = (i & 0x00000001) + ((i & 0x00000040) >> 5);   //Calculation to perform
            for (int z = 0; z < 4; z++)
            {
                int sa = sourceAddress + offsets2[q][z];
                if (sa > 0 && sa < source_width * source_height)
                    pix += kernel[q][z] * source[sa];
                dest[i] = pix;
            }
        }
    }
};