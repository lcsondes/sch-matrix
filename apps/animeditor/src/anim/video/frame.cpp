/*
 *  This file is part of SCH Matrix.
 *
 *  SCH Matrix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SCH Matrix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SCH Matrix. If not, see <https://www.gnu.org/licenses/>.
 */

#include "frame.h"

QRgb Frame::alphaBlend(QRgb source,QRgb target)
{
    //TODO check
    int a=qAlpha(target),sa=qAlpha(source);
    return qRgba(qRed  (source)*(255-a)/255+qRed  (target)*a/255,
                 qGreen(source)*(255-a)/255+qGreen(target)*a/255,
                 qBlue (source)*(255-a)/255+qBlue (target)*a/255,
                 sa+a*(255-sa)/255);
}

void Frame::ctor(bool black/*=false*/)
{
    pixels_=new QRgb*[width_];
    for(uint i=0;i<width_;++i)
    {
        pixels_[i]=new QRgb[height_];
        if(!black)
        {
            //transparent
            memset(pixels_[i],0,height_*sizeof(QRgb));
        }
        else
        {
            //black
            for(uint j=0;j<height_;++j)
            {
                pixels_[i][j]=0xFF000000;
            }
        }
    }
}

Frame::Frame(uint w,uint h)
    :width_(w),height_(h),delay_(0)
{
    ctor();
}

Frame::Frame(uint w,uint h,Frame::Black)
    :width_(w),height_(h),delay_(0)
{
    ctor(true);
}

Frame::Frame(const Frame& o)
    :width_(o.width_),height_(o.height_),delay_(o.delay_)
{
    pixels_=new QRgb*[width_];
    for(uint i=0;i<width_;++i)
    {
        pixels_[i]=new QRgb[height_];
        memcpy(pixels_[i],o.pixels_[i],height_*sizeof(QRgb));
    }
}

Frame::Frame(Frame&& o)
    :width_(o.width_),height_(o.height_),delay_(o.delay_),pixels_(o.pixels_)
{
    o.pixels_=NULL;
}

Frame::~Frame()
{
    dtor();
}

void Frame::dtor()
{
    if(!pixels_)return;//mozgató konstruktor
    for(uint i=0;i<width_;++i)
    {
        delete[] pixels_[i];
    }
    delete[] pixels_;
}

bool Frame::operator==(const Frame& o) const
{
	if(width_!=o.width_||height_!=o.height_)return false;
	for(uint x=0;x<width_;++x)
	{
		if(memcmp(pixels_[x],o.pixels_[x],sizeof(QRgb)*height_))return false;
	}
	return true;
}

QRgb Frame::pixelAt(uint x,uint y) const
{
    Q_ASSERT(x<width_&&y<height_);
    return pixels_[x][y];
}

void Frame::setPixel(uint x,uint y,QRgb color)
{
    Q_ASSERT(x<width_&&y<height_);
    pixels_[x][y]=color;
}

QRgb& Frame::pixelAt(uint x,uint y)
{
    Q_ASSERT(x<width_&&y<height_);
    return pixels_[x][y];
}

uint Frame::delay() const
{
    return delay_;
}

uint Frame::width() const
{
    return width_;
}

uint Frame::height() const
{
    return height_;
}

void Frame::setDelay(uint d)
{
    delay_=d;
}

void Frame::render(Frame* tgt,const QTransform& trans) const
{
    for(uint x=0;x<width_;++x)
    {
        for(uint y=0;y<height_;++y)
        {
            int tx,ty;
            trans.map(x,y,&tx,&ty);
            if(uint(tx)>=tgt->width()||uint(ty)>=tgt->height())
            {
                continue;
            }
            tgt->pixelAt(tx,ty)=alphaBlend(tgt->pixelAt(tx,ty),pixels_[x][y]);
        }
    }
}

void Frame::shiftPixels(int dx,int dy)
{
    Frame temp(*this);
    for(uint x=0;x<width_;++x)
    {
        memset(pixels_[x],0,sizeof(QRgb)*height_);
    }
    //TODO ez régen renderrel ment, de ha átlátszó a pixel, beszarik a mainen. bug.
    int width=width_; //warningokat esznek
    int height=height_;
    for(int x=0;x<width;++x)
    {
        for(int y=0;y<height;++y)
        {
            int x2=x-dx,y2=y-dy;
            if(x2<0||y2<0||x2>=width||y2>=height)continue;
            pixels_[x][y]=temp.pixels_[x2][y2];
        }
    }
}
