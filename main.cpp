#include "mainwindow.h"
#include "canny.h"
#include "tracking.h"
#include <iostream>
#include <QApplication>
#include <bmp.h>
#include <time.h>
typedef unsigned char BYTE;

using namespace std;


//    clock_t begin = clock();
//    clock_t end = clock();
//    double time = double(end-begin)/CLOCKS_PER_SEC;

BYTE* ConvertBMPToIntensity(BYTE* Buffer, int unsigned width, int unsigned height)
{
    // first make sure the parameters are valid
    if ((NULL == Buffer) || (width == 0) || (height == 0))
        return NULL;

    // find the number of padding bytes

    int padding = 0;
    int scanlinebytes = width * 3;
    while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
        padding++;
    // get the padded scanline width
    int psw = scanlinebytes + padding;

    // create new buffer
    BYTE* newbuf = new BYTE[width*height];

    // now we loop trough all bytes of the original buffer,
    // swap the R and B bytes and the scanlines
    long bufpos = 0;
    long newpos = 0;
    for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++) {
            newpos = row * width + column;
            bufpos = (height - row - 1) * psw + column * 3;
            newbuf[newpos] = BYTE(0.11*Buffer[bufpos + 2] + 0.59*Buffer[bufpos + 1] + 0.3*Buffer[bufpos]);
        }

    return newbuf;
}//ConvetBMPToIntensity

BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height, long* newsize)
{
    // first make sure the parameters are valid
    if ((NULL == Buffer) || (width == 0) || (height == 0))
        return NULL;

    // now we have to find with how many bytes
    // we have to pad for the next DWORD boundary

    int padding = 0;
    int scanlinebytes = width * 3;
//    while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
//        padding++;
    // get the padded scanline width
    int psw = scanlinebytes + padding;
    // we can already store the size of the new padded buffer
    *newsize = height * psw;

    // and create new buffer
    BYTE* newbuf = new BYTE[*newsize];

    // fill the buffer with zero bytes then we dont have to add
    // extra padding zero bytes later on
    memset(newbuf, 0, *newsize);

    // now we loop trough all bytes of the original buffer,
    // swap the R and B bytes and the scanlines
    long bufpos = 0;
    long newpos = 0;
    for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++) {
            bufpos = row * width + column;     // position in original buffer
            newpos = (height - row - 1) * psw + column * 3;           // position in padded buffer
            //newpos = (row)* psw + column * 3;
            newbuf[newpos] = Buffer[bufpos];       //  blue
            newbuf[newpos + 1] = Buffer[bufpos];   //  green
            newbuf[newpos + 2] = Buffer[bufpos];   //  red
        }

    return newbuf;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    SimpleBMP bmp;
    bmp.load("frame/0.bmp");

    BYTE *Raw_Intensity;
    Raw_Intensity = ConvertBMPToIntensity(bmp.getPixels(), bmp.width, bmp.height);
     clock_t begin = clock();
    Tracking tracking(Raw_Intensity,bmp.height,bmp.width,100,60,300,400,50);
    tracking.cropImg();

    Canny  nesne(tracking.getSearchImg(),tracking.getSearchHeight(),tracking.getSearchWidth());
    nesne.verticalDerivative();
    nesne.horizontalDerivative();
    nesne.edgeImage();

    BYTE *deneme =new BYTE[(nesne.getHeight() - 2)*(nesne.getWidth() - 2)];
    deneme=nesne.nonmaximumSuppresion();

    Canny nesne2(tracking.getMaskImg(),tracking.getMaskHeight(),tracking.getMaskWidth());
    nesne2.verticalDerivative();
    nesne2.horizontalDerivative();
    nesne2.edgeImage();

    BYTE *deneme2 =new BYTE[(nesne2.getHeight() - 2)*(nesne2.getWidth() - 2)];
    deneme2=nesne2.nonmaximumSuppresion();

   //2. framede 1. framden aldığım nesneyi arıcam





    tracking.createSearchMask(10,10);
    tracking.searchObject();

    clock_t end = clock();
    double time = double(end-begin)/CLOCKS_PER_SEC;

    long new_size;
    BYTE *temp_buffer= ConvertIntensityToBMP(deneme, nesne.getWidth()-2, nesne.getHeight()-2,&new_size);
    long new_size2;
    BYTE *temp_buffer2= ConvertIntensityToBMP(deneme2, nesne2.getWidth()-2, nesne2.getHeight()-2,&new_size2);



    bmp.height=(nesne.getHeight()-2);
    bmp.width=(nesne.getWidth()-2);
    bmp.pixels=temp_buffer;
    bmp.save("output.bmp");


    bmp.height=(nesne2.getHeight()-2);
    bmp.width=(nesne2.getWidth()-2);
    bmp.pixels=temp_buffer2;
    bmp.save("output2.bmp");



    return a.exec();
}
