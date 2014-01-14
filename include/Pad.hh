/*
 * =====================================================================================
 *
 *       Filename:  Pad.hh
 *
 *    Description:  Pad description and utils
 *
 *        Version:  1.0
 *        Created:  18.09.2013 13:25:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */
#ifndef IBN_LSRP_PAD_H
#define IBN_LSRP_PAD_H
#include <cmath>
#include <list>
#include <iostream>
#include "Config.h"

class BasePad
{
  protected:
  int fpad_type=0; //pad type 0,1,2...
  double fxsize; // pad size x
  double fysize; // pad size y
  double fx=0;
  double fy=0; //coordinate of the center of the pad
  long fnx=0; //x index of pad
  long fny=0; //y index of pad
  double farea=0; // area of the pad
  public:
  BasePad(double ax, double ay) : fxsize(ax), fysize(ay)
  {
    farea = fxsize*fysize;
  }
  BasePad(double a=1) : BasePad(a,a)
  {
  }
  long nhit=1;
  double xhit=0;
  double yhit=0;
  double charge=0;
  double x(void)const  { return fx;}
  double y(void) const { return fy;}
  long   nx(void) const { return fnx;}
  long   ny(void) const { return fny;}
  double size(void) const {return std::max(fxsize, fysize);}
  double xsize(void) const {return fxsize;}
  double ysize(void) const {return fysize;}
  double area(void) const { return farea;}
  int type(void) const {return fpad_type;}
  std::list<unsigned> tracks; //original track ids
};


class SquarePad : public BasePad
{
  public:
    SquarePad(void) : BasePad()
    {
    }

    SquarePad(double X, double Y, double q=0) : SquarePad(Cfg.pad_xsize,Cfg.pad_ysize)
    {
      charge=q;
      FindIndex(X,Y);
    }
  protected:

    void FindIndex(double x,double y)
    {
      xhit=x;
      yhit=y;
      x = x/fxsize;
      y=y/fysize;
      fnx = floor(x+0.5);
      fny = floor(y+0.5);
      FindPosition(fnx,fny);
    }
    void FindPosition(long nx, long ny)
    {
      fx = (nx+0.5)*fxsize;
      fy = (ny+0.5)*fysize;
    }
};

class VariableSquarePad : public BasePad
{
  double fHighSensWidthX=64; //high sensetive region width in x
  double fHighSensWidthY=20; //high sensitive region width in y
  double fRoughScaleX=2;
  double fRoughScaleY=2;
  enum { FINE_PAD=0, ROUGH_PAD=1};
  public:
    VariableSquarePad(void) : BasePad()
    {
    }

    VariableSquarePad(double X, double Y, double q=0)
    {
      double ax = Cfg.pad_xsize;
      double ay = Cfg.pad_ysize;
      fHighSensWidthX = Cfg.pad_high_sens_xwidth;
      fHighSensWidthY = Cfg.pad_high_sens_ywidth;
      fRoughScaleX = Cfg.pad_rough_xscale;
      fRoughScaleY = Cfg.pad_rough_yscale;
      if(fabs(X) <= fHighSensWidthX*0.5 && fabs(Y)<=fHighSensWidthY*0.5)
      {
        fxsize=ax;
        fysize=ay;
        fpad_type = FINE_PAD;
      }
      else
      {
        fxsize=ax*fRoughScaleX;
        fysize=ay*fRoughScaleY;
        fpad_type = ROUGH_PAD;
      }
      farea=fxsize*fysize;
      charge=q;
      FindIndex(X,Y);
    }


    void FindIndex(double x,double y)
    {
      xhit=x;
      yhit=y;
      x = x/fxsize;
      y = y/fysize;
      fnx = floor(x);
      fny = floor(y);
      FindPosition(fnx,fny);
    }

    void FindPosition(long nx, long ny)
    {
      fx = (nx+0.5)*fxsize;
      fy = (ny+0.5)*fysize;
    }
};


class HexPad : public BasePad
{
  public:
  HexPad() : BasePad()
  {
  }

  HexPad(double X, double Y) : HexPad(Cfg.pad_xsize, Cfg.pad_ysize)
  {
    FindIndex(X,Y);
  }

  protected:
  void FindPosition(long nx, long ny)
  {
    int even = fabs(nx%2);
    fx = (floor(nx/2.)*1.5 + 0.75*even)*fxsize;
    fy = (ny + 0.5*even)*sqrt(3.0)/2.0*fysize;
  }

  void FindIndex(double x, double y)
  {
    xhit=x;
    yhit=y;
    //step 1 normalize position on size
    x = x/fxsize;
    y=y/(sqrt(3.0)/2.0*fysize);
    //step 2 found number of cell
    long Nx = floor((x+0.5)/1.5);
    long Ny = floor((y+0.5));
    //step 3 found couurdinates inside cell
    x = x - 1.5*Nx;
    y = y - Ny;
    //step 4 found hexagon
    enum {A,B,C,D,E} hex;
    if(y>0)
    {
      if(y<0.5+2*(x+0.25) && y<0.5-2*(x-0.25)) hex=A;
      else hex = x>0 ? B : E;
    }
    else 
    {
      if(y>=-0.5-2*(x+0.25) && y>=-0.5+2*(x-0.25)) hex=A;
      else hex = x>0 ? C : D;
    }
    //step 5 calculate index;
    switch(hex)
    {
      case A: fnx = 2*Nx;   fny = Ny;   break;
      case B: fnx = 2*Nx+1; fny = Ny;   break;
      case C: fnx = 2*Nx+1; fny = Ny-1; break;
      case D: fnx = 2*Nx-1; fny = Ny-1; break;
      case E: fnx = 2*Nx-1; fny = Ny;   break;
    }
    FindPosition(fnx,fny);
  }
};



//typedef SquarePad  Pad;
//typedef HexPad  Pad;
typedef VariableSquarePad Pad;
inline bool operator==(const Pad & p1, const Pad & p2) 
{
  return p1.nx() == p2.nx() && p1.ny()==p2.ny() && p1.type()==p2.type();
}

inline bool operator!=(const Pad & p1, const Pad & p2) 
{
  return ! (p1 == p2);
}

inline bool operator<(const Pad & p1, const Pad & p2) 
{
  if(p1.type()<p2.type()) return true;
  if(p1.type()>p2.type()) return false;
  //go there is pad type are equal
  //now compare pads using x coord
  if(p1.nx()< p2.nx()) return  true;
  if(p1.nx()>p2.nx())  return false;
  //go there if xpad are equal
  //now compare y position
  return p1.ny()<p2.ny();
}
#endif

