//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include <memory>

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4UserLimits;

class DetectorMessenger;
class MagneticField;
class GEMSensitiveDetector;

#include "GEMDetector.hh"
/// Detector construction class to define materials and geometry.
///
/// In addition a transverse uniform magnetic field is defined in
/// SetMagField() method which can be activated via a command
/// defined in the DetectorMessenger class. 


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

    // Set methods
    void SetMaxStep (G4double );
    void SetCheckOverlaps(G4bool );
    void SetPresamplerWidth(G4double);
    void SetPsmGemLength(G4double);
    void SetPresamplerGeometry(G4double width,G4double distance);

    static DetectorConstruction * Instance();

    //G4double presampler_front_position;
    G4double GetPadZ(void) const { return fPadZPosition; }
    G4double GetFrontZ(void) const { return -fPresamplerWidth-fPsmGemLength; }

  private:
    // methods
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    std::unique_ptr<GEMDetector> GEM;
    G4VPhysicalVolume * fGem;
    //std::unique_ptr<GEMSensitiveDetector> fGEMSensitiveDetector;
    GEMSensitiveDetector * fGEMSensitiveDetector;

    G4LogicalVolume* worldLV;

    G4LogicalVolume*    fLogicPresampler;
    G4VPhysicalVolume*  fPresampler; //physical volume of the presampler
    G4Material*         fPresamplerMaterial;  // pointer to the target  material

    G4LogicalVolume*    fLogicAirSens;
    G4VPhysicalVolume*  fAirSens; //physical volume of the presampler

    G4UserLimits* fStepLimit;            // pointer to user step limits

    DetectorMessenger*  fMessenger;   // messenger
    MagneticField*      fMagField;     // magnetic field
    
    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps 
    static DetectorConstruction * fgInstance;

    G4double fPresamplerWidth;
    G4double fAirSensWidth;
    G4double fPsmGemLength;
    G4double fPadZPosition; //Z position of pad plate

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
