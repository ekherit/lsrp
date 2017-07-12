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


    SquarePad(double X, double Y, double q=0) : BasePad(Cfg.gem.pad.size_x, Cfg.gem.pad.size_y)
    {
      charge=q;
      FindIndex(X,Y);
    }
  protected:

    void FindIndex(double X,double Y)
    {
      xhit=X;
      yhit=Y;
      X = X/fxsize;
      Y=Y/fysize;
      fnx = floor(X+0.5);
      fny = floor(Y+0.5);
      FindPosition(fnx,fny);
    }
    void FindPosition(long NX, long NY)
    {
      fx = (NX+0.5)*fxsize;
      fy = (NY+0.5)*fysize;
    }
};

class VariableSquarePad : public BasePad
{
  double fHighSensWidthX=64; //high sensetive region width in x
  double fHighSensWidthY=20; //high sensitive region width in y
  double fRoughSizeX=2;
  double fRoughSizeY=2;
  enum { FINE_PAD=0, ROUGH_PAD=1};
  public:
    VariableSquarePad(void) : BasePad()
    {
    }

    VariableSquarePad(double X, double Y, double q=0)
    {
      double ax = Cfg.gem.pad.size_x;
      double ay = Cfg.gem.pad.size_y;
      fHighSensWidthX = Cfg.gem.high_sens_size_x;
      fHighSensWidthY = Cfg.gem.high_sens_size_y;
      fRoughSizeX = Cfg.gem.pad.rough_size_x;
      fRoughSizeY = Cfg.gem.pad.rough_size_y;
      if(fabs(X) <= fHighSensWidthX*0.5 && fabs(Y)<=fHighSensWidthY*0.5)
      {
        fxsize=ax;
        fysize=ay;
        fpad_type = FINE_PAD;
      }
      else
      {
        fxsize=fRoughSizeX;
        fysize=fRoughSizeY;
        fpad_type = ROUGH_PAD;
      }
      farea=fxsize*fysize;
      charge=q;
      FindIndex(X,Y);
    }


    void FindIndex(double X,double Y)
    {
      xhit=X;
      yhit=Y;
      X = X/fxsize;
      Y = Y/fysize;
      fnx = floor(X);
      fny = floor(Y);
      FindPosition(fnx,fny);
    }

    void FindPosition(long NX, long NY)
    {
      fx = (NX+0.5)*fxsize;
      fy = (NY+0.5)*fysize;
    }
};


class HexPad : public BasePad
{
  public:
  HexPad() : BasePad()
  {
  }

  HexPad(double X, double Y) : BasePad(Cfg.gem.pad.size_x, Cfg.gem.pad.size_y)
  {
    FindIndex(X,Y);
  }

  protected:
  void FindPosition(long NX, long NY)
  {
    int even = fabs(NX%2);
    fx = (floor(NX/2.)*1.5 + 0.75*even)*fxsize;
    fy = (NY + 0.5*even)*sqrt(3.0)/2.0*fysize;
  }

  void FindIndex(double X, double Y)
  {
    xhit=X;
    yhit=Y;
    //step 1 normalize position on size
    X = X/fxsize;
    Y=Y/(sqrt(3.0)/2.0*fysize);
    //step 2 found number of cell
    long Nx = floor((X+0.5)/1.5);
    long Ny = floor((Y+0.5));
    //step 3 found couurdinates inside cell
    X = X - 1.5*Nx;
    Y = Y - Ny;
    //step 4 found hexagon
    enum {A,B,C,D,E} hex;
    if(Y>0)
    {
      if(Y<0.5+2*(X+0.25) && Y<0.5-2*(X-0.25)) hex=A;
      else hex = X>0 ? B : E;
    }
    else 
    {
      if(Y>=-0.5-2*(X+0.25) && Y>=-0.5+2*(X-0.25)) hex=A;
      else hex = X>0 ? C : D;
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

