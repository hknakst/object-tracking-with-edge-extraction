#include "tracking.h"
#include <iostream>
#include <math.h>
using namespace std;


Tracking::Tracking()
{
    img=nullptr;
    searchImg=nullptr;
    maskImg=nullptr;
}

Tracking::Tracking(BYTE *img, int Height, int Width,int x1,int y1,int x2,int y2,int l)
{
    this->Height=Height;
    this->Width=Width;
    this->img=img;
    this->x1=x1;
    this->y1=y1;
    this->x2=x2;
    this->y2=y2;
    this->l=l;

    this->searchHeight=(y2+l)-(y1-l);
    this->searchWidth=(x2+l)-(x1-l);
    searchImg=new BYTE[searchHeight*searchWidth];

    this->maskHeight=y2-y1;
    this->maskWidth=x2-x1;
    maskImg=new BYTE[maskHeight*maskWidth];


}


Tracking::~Tracking()
{
    delete []img;
    delete []searchImg;
    delete []maskImg;
}

void Tracking::cropImg()
{
    index=0;
    for(int i=(y1-l); i<(y2+l); i++)
    {
        for(int j=(x1-l); j<(x2+l); j++)
        {
            C=i*(Width)+j;
            searchImg[index]=img[C];
            index++;
        }
    }
    index=0;
    for(int i=y1; i<y2; i++)
    {
        for(int j=x1; j<x2; j++)
        {
            C=i*(Width)+j;
            maskImg[index]=img[C];
            index++;
        }
    }

}
void Tracking::createSearchMask(int sampleWidth, int sampleHeight)
{

    gap[0] = round(maskHeight/sampleHeight);
    gap[1] = round(maskWidth/sampleWidth);


}

void Tracking::searchObject()
{
    int score=0;
    int maxscore=0;
    int StartPoint=0;
    newStartPoint=0;

    for (int i = 0; i < searchHeight; i++)
            for (int j  = 0; j < searchWidth; j++)
            {
               score=0;
               for (int k = 0; k< maskHeight; k+=gap[0])
                   for (int h = 0; h< maskWidth; h+=gap[1])
                   {
                       if(h==0 && k==0)
                           StartPoint=(i* searchWidth + j)+(k * maskWidth + h);

                       score+=(searchImg[(i* searchWidth + j)+(k * searchWidth + h)] == maskImg [k * maskWidth + h]);
                   }

               if(score>maxscore)
               {
                   newStartPoint=StartPoint;
                   maxscore=score;
                }
             }


}

int *Tracking::newArea()
{
    int searchY1= newStartPoint/searchWidth;
    int searchX1=newStartPoint-(searchY1*searchWidth);

    newCoordinate[0]=(x1-l)+searchX1;
    newCoordinate[1]=(y1-l)+searchY1;
    newCoordinate[2]=newCoordinate[0]+(x2-x1);
    newCoordinate[3]=newCoordinate[1]+(y2-y1);

    return newCoordinate;
}


int Tracking::getHeight() const
{
    return Height;
}

void Tracking::setHeight(int value)
{
    Height = value;
}

int Tracking::getWidth() const
{
    return Width;
}

void Tracking::setWidth(int value)
{
    Width = value;
}

BYTE *Tracking::getImg() const
{
    return img;
}

void Tracking::setImg(BYTE *value)
{
    img = value;
}

int Tracking::getSearchHeight() const
{
    return searchHeight;
}

void Tracking::setSearchHeight(int value)
{
    searchHeight = value;
}

int Tracking::getSearchWidth() const
{
    return searchWidth;
}

void Tracking::setSearchWidth(int value)
{
    searchWidth = value;
}

BYTE *Tracking::getSearchImg() const
{
    return searchImg;
}

void Tracking::setSearchImg(BYTE *value)
{
    searchImg = value;
}

int Tracking::getMaskHeight() const
{
    return maskHeight;
}

void Tracking::setMaskHeight(int value)
{
    maskHeight = value;
}

int Tracking::getMaskWidth() const
{
    return maskWidth;
}

void Tracking::setMaskWidth(int value)
{
    maskWidth = value;
}

BYTE *Tracking::getMaskImg() const
{
    return maskImg;
}

void Tracking::setMaskImg(BYTE *value)
{
    maskImg = value;
}
