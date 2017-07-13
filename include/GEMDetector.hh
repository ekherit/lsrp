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
#include <list>
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

#include "G4SystemOfUnits.hh"

class G4UserLimits;

class GEMDetector 
{
  G4double fSizeX; // GEM size X
  G4double fSizeY; // GEM size Y
  G4double fRadius; // GEM radius 50 mm
  G4int fCascadeNumber = 3; //number of amplification cascades
  G4double fStefWidth = 1.5*mm; //width of the stef 1.5 mm
  G4double fDriftLength = 3*mm; //drift length 3 mm
  G4double fKaptonWidth = 50*um; //kapton width 50 mkm
  G4double fCuprumWidth = 5*um;//cuprum width on capton (one size) 5 mkm
  G4double fCascadeWidth = fKaptonWidth+fCuprumWidth*2; // fKaptonWidth+fCuprumWidth*2; (60 mkm)
  G4double fTransferLength = 1.5*cm; //1.5 mm
  G4double fInductionLength  = 2.0*mm; //induction length
  G4double fPadWidth = 15.*um;   //Pad width 15 mkm
  G4double fGEMWidth;   //total GEM width calculated from other parameters
  G4double fPadZposition; // z position of the pad
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
    std::list<std::unique_ptr<G4VSolid>>  SolidVolumeListList;
    std::list<std::unique_ptr<G4VPhysicalVolume>>  PhysicalVolumeListList;
    public:
    G4LogicalVolume * GetLogicalVolume(void) { return LV.get(); }
    AmplificationCascade(G4double size, G4double kapton_width=50e-6*CLHEP::m, G4double cuprum_width=5e-6*CLHEP::m);
    G4double GetWidth(void) const { return fKaptonWidth+2*fCuprumWidth;}
    G4double GetKaptonWidth(void) const { return fKaptonWidth; }
    G4double GetCuprumWidth(void) const { return fCuprumWidth; }
    void open_geometry(void);
    void update_geometry(double size);
    void close_geometry(void);
  };
  std::unique_ptr<AmplificationCascade> fAmplCascade;
  std::unique_ptr<G4LogicalVolume> fTransferVolume;
  std::list<std::unique_ptr<G4VSolid>>  SolidVolumeListList;
  std::list<std::unique_ptr<G4VPhysicalVolume>>  PhysicalVolumeListList;
  G4Material * Ar;
  public:
    GEMDetector(void);
    G4double GetWidth(void) { return fGEMWidth; }
    G4LogicalVolume * GetLogicalVolume(void) {return LV.get();}
    G4LogicalVolume * GetDriftVolume(void) { return fDriftVolume.get(); }
    G4LogicalVolume * GetTransferVolume(void) { return fTransferVolume.get(); }
    void PrintGeometry(void);
    G4int GetCascadeNumber(void) const { return fCascadeNumber; }
    G4LogicalVolume * fStefVolume;
    G4double GetPadZ(void) const {return fPadZposition;}
    bool fCheckOverlaps=true;
    void SetUserLimits(G4UserLimits * );

    void open_geometry(void);
    void update_geometry(double size);
    void close_geometry(void);
};

#endif
