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
//describes hexagonal pad
#include <cmath>
class Pad
{
  double fa; // pad size
  double fx,fy; //coordinate of the center of the pad
  public:
  long nhit;
  double xhit,yhit;
  double charge;
  long fnx, fny; //index of pads
  double x(void)const  { return fx;}
  double y(void) const { return fy;}
  double size(void) const {return fa;}
  Pad(double a=1)
  {
    Init(a);
  }

  Pad(double a, double x, double y)
  {
    Init(a);
    FindIndex(x,y);
  }
  
  void Init(double a=1)
  {
    fa = a;
    fx=0;
    fy=0;
    fnx=0;
    fny=0;
    charge=0;
    nhit=1; //Already have 1 hit
    xhit=0;
    yhit=0;
  }


  void FindPosition(long nx, long ny)
  {
    int even = fabs(nx%2);
    fx = (floor(nx/2.)*1.5 + 0.75*even)*fa;
    fy = (ny + 0.5*even)*sqrt(3.0)/2.0*fa;
  }

  void FindIndex(double x, double y)
  {
    xhit=x;
    yhit=y;
    //step 1 normalize position on size
    x = x/fa;
    y=y/(sqrt(3.0)/2.0*fa);
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

inline bool operator==(const Pad & p1, const Pad & p2) 
{
  return p1.fnx == p2.fnx && p1.fny==p2.fny;
}

inline bool operator<(const Pad & p1, const Pad & p2) 
{
  if(p1.fnx< p2.fnx) return  true;
  if(p1.fnx>p2.fnx)  return false;
  return p1.fny<p2.fny;
}

//inline bool operator>(const Pad & p1, const Pad & p2) 
//{
//  return p1.fnx > p2.fnx || p1.fny>p2.fny;
//}
//inline bool operator<=(const Pad & p1, const Pad & p2) 
//{
//  return p1.fnx <= p2.fnx && p1.fny<=p2.fny;
//}
//
//inline bool operator>=(const Pad & p1, const Pad & p2) 
//{
//  return p1.fnx >= p2.fnx && p1.fny>=p2.fny;
//}

#endif

