#include "mainwindow.h"
#include "canny.h"
#include "tracking.h"
#include <iostream>
#include <QApplication>
#include <bmp.h>
#include <time.h>
#include <string>
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
    Tracking tracking;
    Tracking tracking2;
    int coordinat[4]={30,52,534,228};
    SimpleBMP bmp;
    for(int i=0; i<24; i++)
    {
        string path ="frame1_ornek/" +std::to_string(i) + ".bmp";
        bmp.load(path.c_str());
        bmp.drawRect(coordinat[0],270-coordinat[1],coordinat[2],270-coordinat[3]);

        //bmp.save(path.c_str());

        BYTE *Raw_Intensity;
        Raw_Intensity = ConvertBMPToIntensity(bmp.getPixels(), bmp.width, bmp.height);
         clock_t begin = clock();
        tracking.TrackingSet(Raw_Intensity,bmp.height,bmp.width,coordinat[0],coordinat[1],coordinat[2],coordinat[3],10);
        tracking.cropSearchImg();

        //1. framden hem search alanını hem mask alanını cıkardım canny uyguladım
        Canny  nesne(tracking.getSearchImg(),tracking.getSearchHeight(),tracking.getSearchWidth());
        nesne.verticalDerivative();
        nesne.horizontalDerivative();
        nesne.edgeImage();
        nesne.nonmaximumSuppresion();
        Canny nesne2(tracking.getMaskImg(),tracking.getMaskHeight(),tracking.getMaskWidth());
        nesne2.verticalDerivative();
        nesne2.horizontalDerivative();
        nesne2.edgeImage();
        BYTE *deneme2 =new BYTE[(nesne2.getHeight() - 2)*(nesne2.getWidth() - 2)];
        deneme2=nesne2.nonmaximumSuppresion();

       //2. framede 1. framden aldığım nesneyi arıcam
        string path2 ="frame1_ornek/" +std::to_string(i+2) + ".bmp";
        bmp.load(path2.c_str());

        Raw_Intensity = ConvertBMPToIntensity(bmp.getPixels(), bmp.width, bmp.height);
        tracking2.TrackingSet(Raw_Intensity,bmp.height,bmp.width,coordinat[0],coordinat[1],coordinat[2],coordinat[3],10);
        tracking2.cropSearchImg();
        Canny  nesne3(tracking2.getSearchImg(),tracking2.getSearchHeight(),tracking2.getSearchWidth());
        nesne3.verticalDerivative();
        nesne3.horizontalDerivative();
        nesne3.edgeImage();
        BYTE *deneme =new BYTE[(nesne3.getHeight() - 2)*(nesne3.getWidth() - 2)];
        deneme=nesne3.nonmaximumSuppresion();
    //    Canny nesne4(tracking2.getMaskImg(),tracking2.getMaskHeight(),tracking2.getMaskWidth());
    //    nesne4.verticalDerivative();
    //    nesne4.horizontalDerivative();
    //    nesne4.edgeImage();
    //    nesne4.nonmaximumSuppresion();
       tracking2.setMaskImg(tracking.getMaskImg());


        tracking2.createSearchMask(10,10);
        tracking2.searchObject();
        int *af;
        af=tracking2.newArea();
        bmp.drawRect(af[0],270-af[1],af[2],270-af[3]);
        for(int i=0; i<4; i++)
            coordinat[i]=af[i];
        string path3 ="frame1/" +std::to_string(i+1) + ".bmp";
        bmp.save(path3.c_str());
        clock_t end = clock();
        double time = double(end-begin)/CLOCKS_PER_SEC;





        long new_size;
        BYTE *temp_buffer= ConvertIntensityToBMP(deneme, nesne.getWidth()-2, nesne.getHeight()-2,&new_size);
//        long new_size2;
//        BYTE *temp_buffer2= ConvertIntensityToBMP(deneme2, nesne2.getWidth()-2, nesne2.getHeight()-2,&new_size2);



        bmp.height=(nesne.getHeight()-2);
        bmp.width=(nesne.getWidth()-2);
        bmp.pixels=temp_buffer;
        string path4 ="frame2/" +std::to_string(i+1) + ".bmp";
        bmp.save(path4.c_str());


//        bmp.height=(nesne2.getHeight()-2);
//        bmp.width=(nesne2.getWidth()-2);
//        bmp.pixels=temp_buffer2;
//        bmp.save("output2.bmp");



    }



    return a.exec();
}
