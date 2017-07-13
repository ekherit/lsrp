/*
 * =====================================================================================
 *
 *       Filename:  compton.h
 *
 *    Description:  Compton backscattering formula
 *
 *        Version:  1.0
 *        Created:  28.08.2013 14:55:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */
#ifndef IBN_PHYS_COMPTON_H
#define IBN_PHYS_COMPTON_H
#include <iostream>
#include <math.h>
using namespace std;
namespace ibn
{
  namespace phys
  {
    class compton
    {
      public:
      double Eg; //energy of gamma quant, MeV
      double Ee; //energy of electron beam, MeV
      double Pe; //electron polarization
      double Sx; //electron x polarization
      double Sy; //electron y polarization
      double Sz; //electron z polarization
      double Pg; //photon circular polarization
      double gamma;
      double V; //velocity of electron in lab frame
      double omega; //photon energy in r.f. of electron
      double chi; //zhestkost parameter = omega/me (in the electron rest frame)
      //Scattered parameters of gamma quant 
      double kx;
      double ky;
      double kz;
      double E;
      double theta;
      double phi;
      compton(double ee, double eg, double pe, double pg)
      {
        SetEnergy(ee,eg);
        SetPolarization(pe,pg);
      }

      void SetElectronEnergy(double ee)
      {
        Ee=ee;
        gamma=Ee/ME;
        V = sqrt(1.0-1.0/(gamma*gamma));
        SetPhotonEnergy(Eg);
      }

      void SetPhotonEnergy(double eg)
      {
        Eg=eg;
        omega = gamma*Eg*(1.0+V);
        chi = omega/ME;
      }

      void SetEnergy(double ee, double eg)
      {
        SetElectronEnergy(ee);
        SetPhotonEnergy(eg);
      }

      void SetPhotonPolarization(double pg) 
      {
        Pg=pg;
      }
      void SetElectronPolarization(double pe) 
      {
        Pe=pe;
	Sx=0;
	Sz=0;
	Sy=pe;
      }

      void SetElectronPolarization(double sx,  double sy,  double sz) 
      {
				double mod = sqrt(sx*sx+sy*sy+sz*sz);
				Sx = sx/mod;
				Sy = sy/mod;
				Sz = sz/mod;
      }
      void SetPolarization(double pe, double pg)
      {
        SetElectronPolarization(pe);
        SetPhotonPolarization(pg);
      }


      ~compton(void){}

      double cross_section(double x, double phi=0)
      {
        double b=1./x-1.;
        double bchi = (b/chi);
        return 1./x+x+ bchi*(bchi - 2) - Pe*Pg*polarized_cross_section(x,phi);
      }

      double operator()(double x)
      {
        return cross_section(x,0);
      }

      double polarized_cross_section(double x, double phi)
      {
        return (1.0-x)*(Sz*cosx(x)  + Sy*sin(phi)*sinx(x) + Sx*cos(phi)*sinx(x));
      }


			//calculate cos(theta) as function of undimentional parameter
			//x = omega'/omega (photon energy factor)
			//check this formula at 2014-04-09
      double cosx(double x)
      {
        return (1./x-1.)/chi-1.0;
      }
      double sinx(double x)
      {
        double c = cosx(x);
        return sqrt(1.0 - c*c);
      }

      double xcos(double c)
      {
        return 1./(1.+chi*(1.+c));
      }

      double cos_rf(double cos_lf) //cosine in rest frame calculated from lab frame
      {
        return (cos_lf -V)/(1.-V*cos_lf);
      }

      double cos_lf(double cos_rf) //cosine in rest frame calculated from lab frame
      {
        return (cos_rf + V)/(1.+V*cos_rf);
      }

      double sin_lf(void)
      {
        return hypot(kx,ky)/E;
      }

      template <class UniformFunction> 
        void generate(UniformFunction  Uniform, double *x, double *phi, const double xmax=1)
        {
          const double xmin = 1.0/(1.+2.*chi); //inverse minimum photon energy 
          const double ymax=1./xmin+1; //maximum of cross section
          double y; //cross section in units 0.5*r0^2/chi
          do
          {
            y=Uniform(0,ymax);
            *x=Uniform(xmin,xmax);
            *phi = Uniform(-M_PI,M_PI);
          } while(y>cross_section(*x,*phi));
        }

      template <class UniformFunction> 
        void generate(UniformFunction  Uniform, const double theta_max=1)
        {
          double xmax = 1./(1.+chi*(1.0 + cos_rf(cos(theta_max))));
          double x;
          generate(Uniform,&x,&phi,xmax);
          double c_rf = cosx(x); //cosine in rest frame
          double c_lf = cos_lf(c_rf); //cosine if lab frame
          double s_lf = sqrt(1.0-c_lf*c_lf); //sine in lab frame
          E = gamma*x*chi*ME*(1.0+c_rf);
          kx = E*s_lf*cos(phi);
          ky = E*s_lf*sin(phi);
          kz = E*c_lf; 
        }
      private:
      const double ME=0.510998928; // ± 0.000000011 electron mass PDG 2013
    };

    
  }
}
#endif
