/*
 * =====================================================================================
 *
 *       Filename:  GEM.h
 *
 *    Description:  GEM construction
 *
 *        Version:  1.0
 *        Created:  17.09.2013 12:11:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#ifndef IBN_LSRP_GEM_H
#define IBN_LSRP_GEM_H
#include <memory>
#include <vector>
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

class GEMDetector 
{
  G4double fSizeX; // GEM size X
  G4double fSizeY; // GEM size Y
  G4double fRadius; // GEM radius 50 mm
  G4int fCascadeNumber; //number of amplification cascades
  G4double fStefWidth; //width of the stef 1.5 mm
  G4double fDriftLength; //drift length 3 mm
  G4double fKaptonWidth; //kapton width 50 mkm
  G4double fCuprumWidth;//cuprum width on capton (one size) 5 mkm
  G4double fCascadeWidth; // fKaptonWidth+fCuprumWidth*2; (60 mkm)
  G4double fTransferLength; //1.5 mm
  G4double fInductionLength; //induction length
  G4double fPadWidth;   //Pad width 15 mkm
  G4double fGEMWidth;
  std::unique_ptr<G4LogicalVolume> LV;
  std::unique_ptr<G4LogicalVolume> fDriftVolume;
  class AmplificationCascade  //amplification cascade
  {
    G4double fWidth;
    G4double fCuprumWidth;
    G4double fKaptonWidth;
    G4double fRadius;
    G4Material * kapton;
    G4Material * Cu;
    std::unique_ptr<G4LogicalVolume>  LV;
    public:
    G4LogicalVolume * GetLogicalVolume(void) { return LV.get(); }
    AmplificationCascade(G4double size, G4double kapton_width=50e-6*m, G4double cuprum_width=5e-6*m);
    G4double GetWidth(void) const { return fKaptonWidth+2*fCuprumWidth;}
    G4double GetKaptonWidth(void) const { return fKaptonWidth; }
    G4double GetCuprumWidth(void) const { return fCuprumWidth; }
  };
  std::unique_ptr<AmplificationCascade> fAmplCascade;
  std::vector<std::unique_ptr<G4LogicalVolume>> fTransferVolume;
  G4Material * Ar;
  G4Material * stef;
  public:
    GEMDetector(void);
    G4LogicalVolume * GetLogicalVolume(void) {return LV.get();}
    G4double GetWidth(void) { return fGEMWidth; }
};

#endif
