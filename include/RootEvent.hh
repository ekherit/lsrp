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
//#include "G4ThreeVector.hh"
#include "Pad.hh"

#include <vector>

#include <ibn/phys/compton.h>
//namespace ibn 
//{ 
//  namespace phys 
//  {
//    class compton;
//  }
//}

using namespace ibn::phys;

class PadEvent : public TObject
{
  public:
  PadEvent(void);
  Long64_t nx;
  Long64_t ny;
  Double_t x; //mm
  Double_t y; //mm
  Double_t q;  //collected charge coulomb
  Long64_t nhit; //number of hit for this pad
  Long64_t nphot; //number of different photons in this pad
  Double_t xhit; //mm
  Double_t yhit; //mm
  ClassDef(PadEvent, 1); //The class title
};

class GeneratorEvent : public TObject
{
  public:
  GeneratorEvent(void);
  //GeneratorEvent(const compton & );
  virtual ~GeneratorEvent(void){}
  double P; //polarization of gamma quant
  double Eb; //electron beam energy
  double gamma; //gamma factor
  double omega; //photon energy in r.f. of electron
  double chi; //hardness of photon
  double E; //energy of gamma quant, MeV
  double kx; // x momentum, MeV
  double ky; //
  double kz; //
  double nx;
  double ny;
  double nz;
  double theta; 
  double phi;
  double x; //position
  double y;
  double z; 
  std::vector<PadEvent*> pad;
  ClassDef(GeneratorEvent, 1); //The class title
};

GeneratorEvent makeGeneratorEvent(const compton & );


class HitEvent : public TObject 
{
  public:
  HitEvent(void);
  Long64_t trackID;
  Long64_t OrigTrackID;
  Long64_t pid;
  Long64_t volumeID;
  Double_t E;
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t rho;
  Double_t phi;
  Double_t q;  //collected charge
  ClassDef(HitEvent, 1); //The class title
};


class RootEvent : public TObject
{
  public:
  RootEvent(void);
  virtual ~RootEvent(void){}
  Long64_t eventID; //event id
  Double_t P; //polarization of gamma quant
  Double_t Eb; //electron beam energy
  Long64_t nphot; //number of photons
  Long64_t nhit;
  Long64_t npad;
  std::vector<HitEvent> hit;
  std::vector<PadEvent> pad;
  std::vector<GeneratorEvent> gen;
  void clear(void)
  {
    Eb=-1;
    nphot=-1;
    nhit=-1;
    hit.clear();
    pad.clear();
    gen.clear();
  }
  ClassDef(RootEvent, 1); //The class title
};

#endif


