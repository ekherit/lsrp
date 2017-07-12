/*
 * =====================================================================================
 *
 *       Filename:  RootEvent.hh
 *
 *    Description:  Root Event class
 *
 *        Version:  1.0
 *        Created:  19.09.2013 10:20:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#ifndef IBN_LSRP_ROOT_EVENT_H
#define IBN_LSRP_ROOT_EVENT_H

#include <TObject.h>
#include <TVector3.h>
#include <TRef.h>
#include "Pad.hh"

#include <vector>

#include "Config.h"


class PadEvent : public TObject
{
  public:
  PadEvent(void);
  Double_t s=-99999;//area of the pad
  Double_t type=-99999;//pad type
  Double_t X=-99999; //mm //center of the pad
  Double_t Y=-99999; //mm
  Double_t xhit=-99999;//mm
  Double_t yhit=-99999;//mm
  Double_t x=-99999; //random place at pad
  Double_t y=-99999; 
  Double_t r=-99999; //sqrt(x^2+y^2)
  Double_t R=-99999; //sqrt(X^2+Y^2)
  Long64_t nx=-99999; //index of the pad
  Long64_t ny=-99999;
  Double_t q=-99999;  //collected charge coulomb
  Long64_t nhit=-99999; //number of different hit for this pad
  Long64_t nphot=-99999; //number of different photons in this pad
  ClassDef(PadEvent, 1); //The class title
};

class GeneratorEvent : public TObject
{
  public:
  GeneratorEvent(void);
  virtual ~GeneratorEvent(void){}
  double P=-99999; //polarization of gamma quant
  double Eb=-99999; //electron beam energy
  double gamma=-99999; //gamma factor
  double omega=-99999; //photon energy in r.f. of electron
  double chi=-99999; //hardness of photon
  double E=-99999; //energy of gamma quant, MeV
  double kx=-99999; // x momentum, MeV
  double ky=-99999; //
  double kz=-99999; //
  double nx=-99999;
  double ny=-99999;
  double nz=-99999;
  double theta=-99999; 
  double phi=-99999;
  double x=-99999; //position,mm
  double y=-99999;
  double z=-99999; 
  //Long64_t nphot=-9999; //number of generated photons
  unsigned npad;
  std::vector<PadEvent> pad;
  ClassDef(GeneratorEvent, 1); //The class title
};



class HitEvent : public TObject 
{
  public:
  HitEvent(void);
  Long64_t trackID=-99999;
  Long64_t OrigTrackID=-99999;
  Long64_t pid=0;
  Long64_t volumeID=-99999;
  Double_t E=-99999; //deposit energy, MeV
  Double_t x=-99999; //position, mm
  Double_t y=-99999;
  Double_t z=-99999;
  Double_t rho=-99999;
  Double_t phi=-99999;
  Double_t q=-99999;  //collected charge
  ClassDef(HitEvent, 1); //The class title
};

class RootEvent : public TObject
{
  public:
  RootEvent(void);
  virtual ~RootEvent(void){}
  //geometry information
  Double_t d=-99999; //presampler  width, mm
  Double_t l=-99999; //distance from presampler to GEM,mm
  Double_t psx=-99999; //x pad size, mm
  Double_t psy=-99999; //y pad size, mm
  Long64_t run=-99999; //run id
  Long64_t eventID=-99999; //event id
  //beam information
  Double_t P=0; //polarization of gamma quant
  Double_t Eb=-99999; //electron beam energy
  Double_t Nphot=-99999; //expected number of photons
  Long64_t nphot=-99999; //number of generated photons
  Long64_t ndet=-99999; //nomber of registered photons
  //detector information
  Long64_t nhit=-99999;
  Long64_t npad=-99999;
  std::vector<HitEvent> hit;
  std::vector<PadEvent> pad;
  std::vector<GeneratorEvent> gen;
  Double_t asym=-99999; //asymmetry
  Double_t asym2=-99999; //asymmetry
  Double_t Mx=-99999; //average y
  Double_t My=-99999; //average x
  Double_t xRMS=-99999;
  Double_t yRMS=-99999;
  void clear(void)
  {
    d=-99999;
    l=-99999;
    psx=-99999;
    psy=-99999;
    run=-99999;
    eventID=-99999;
    P=0;
    Eb=-99999;
    nphot=-99999;
    nhit=-99999;
    npad=-99999;
    asym=-99999;
    asym2=-99999;
    Mx=-99999;
    My=-99999;
    xRMS=-99999;
    yRMS=-99999;
    hit.clear();
    pad.clear();
    gen.clear();
  }
  ClassDef(RootEvent, 1); //The class title
};

#endif


