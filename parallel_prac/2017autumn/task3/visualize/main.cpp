#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>
#include "EasyBMP.h"

int main(int argc, char *argv[])
{
    if (argc < 6) return -1;

    const char *input = argv[1];
    const int n1 = atoi(argv[2]);
    const int n2 = atoi(argv[3]);
    const int k = atoi(argv[4]);
    const char *output = argv[5];

    std::map<int, RGBApixel> domainToRGB;

    {
        RGBApixel rgb;

        // HSV model
        double h, s, v;
        
        for (int i = 0; i < k; ++i) {
            s = v = 100.0;
            const int j = (i * (k / 2 + 1)) % k;
            h = 360.0 * j / k;
            
            const int h_i = (((int) h) / 60 ) % 6;

            double v_min = (100 - s) * v / 100;
            double a = (((int) h) % 60) / 60.0 * (v - v_min);
            double v_inc = v_min + a;
            double v_dec = v - a;

            v /= 100;
            v_min /= 100;
            v_inc /= 100;
            v_dec /= 100;

            rgb.Alpha = 255;
            rgb.Red = 255;
            rgb.Green = 255;
            rgb.Blue = 255;

            switch (h_i) {
                case 0:
                    rgb.Red *= v;
                    rgb.Green *= v_inc;
                    rgb.Blue *= v_min;
                    break;
                case 1:
                    rgb.Red *= v_dec;
                    rgb.Green *= v;
                    rgb.Blue *= v_min;
                    break;
                case 2:
                    rgb.Red *= v_min;
                    rgb.Green *= v;
                    rgb.Blue *= v_inc;
                    break;
                case 3:
                    rgb.Red *= v_min;
                    rgb.Green *= v_dec;
                    rgb.Blue *= v;
                    break;
                case 4:
                    rgb.Red *= v_inc;
                    rgb.Green *= v_min;
                    rgb.Blue *= v;
                    break;
                case 5:
                    rgb.Red *= v;
                    rgb.Green *= v_min;
                    rgb.Blue *= v_dec;
                    break;
                default:
                    break;
            }

            domainToRGB[i] = rgb;
        }
    }

    BMP image;
    image.SetSize(n1, n2);

    {
        int i, j, domain;
        float x, y;
        
        std::ifstream infile(input);
        while (infile >> i >> j >> x >> y >> domain) {
            image.SetPixel(i, j, domainToRGB[domain]);
        }
    }

    
    image.WriteToFile(output);

    return 0;
}