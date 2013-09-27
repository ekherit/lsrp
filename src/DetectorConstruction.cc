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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "Config.h"
 
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "MagneticField.hh"
#include "GEMSensitiveDetector.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4SDManager.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

//#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction * DetectorConstruction::fgInstance = 0;

DetectorConstruction* DetectorConstruction::Instance(void)
{
  return fgInstance;
}
 
DetectorConstruction::DetectorConstruction()
: 
  fLogicPresampler(nullptr),
  fPresamplerMaterial(NULL), fChamberMaterial(NULL), 
  fStepLimit(NULL),
 fCheckOverlaps(true)
{
  fMessenger = new DetectorMessenger(this);
  fMagField  = new MagneticField();

  //fNbOfChambers = 1;
  //fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
  fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
DetectorConstruction::~DetectorConstruction()
{
  //delete [] fLogicChamber; 
  delete fMagField;
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Material definition 

  G4NistManager* nistManager = G4NistManager::Instance();

  G4bool fromIsotopes = false;

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR", fromIsotopes);
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Ar", fromIsotopes);
  
  // Lead defined using NIST Manager
  fPresamplerMaterial  = nistManager->FindOrBuildMaterial("G4_Pb", fromIsotopes);
  nistManager->FindOrBuildMaterial("G4_Cu", fromIsotopes);
  nistManager->FindOrBuildMaterial("G4_KAPTON", fromIsotopes);

  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Ar", fromIsotopes);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  fPresamplerWidth = Cfg.psm_width*mm; //presampler width
  G4double fPsmGemLength=Cfg.psm_gem_length*mm; //distance between presampler and gem
  G4double psm_radius =  Cfg.psm_size*mm/2.0; //Radius of the presampler
  G4double world_size=2*psm_radius*1.2;
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(world_size);

  GEM.reset(new GEMDetector);

  //G4ThreeVector psm_position = G4ThreeVector(0,0,+fPresamplerWidth/2.0+presampler_front_position);
  //G4ThreeVector gem_position = G4ThreeVector(0,0,presampler_front_position+fPresamplerWidth+psm_gem_length+GEM->GetWidth()/2.0);
  G4ThreeVector psm_position = G4ThreeVector(0,0,-fPsmGemLength-fPresamplerWidth/2.0);
  G4ThreeVector gem_position = G4ThreeVector(0,0,GEM->GetWidth()/2.0);
  //presampler_front_position=-psm_gem_length-fPresamplerWidth;

  // World
  G4Material* air  = G4Material::GetMaterial("G4_AIR");
  G4cout << "Computed tolerance = "
    << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
    << " mm" << G4endl;

  G4Box* worldS = new G4Box("world",                                    //its name
      world_size/2,world_size/2,world_size/2); //its size

  worldLV = 
    new G4LogicalVolume ( worldS,air,"World"); 
  //Must place the World Physical volume unrotated at (0,0,0).
  G4VPhysicalVolume* worldPV = 
    new G4PVPlacement
    (
     0,               // no rotation
     G4ThreeVector(), // at (0,0,0)
     worldLV,         // its logical volume
     "World",         // its name
     0,               // its mother  volume
     false,           // no boolean operations
     0,               // copy number
     fCheckOverlaps
    ); // checking overlaps 

  // Presampler
  G4Tubs* psm_solid = new G4Tubs("presampler",0.,psm_radius,fPresamplerWidth/2,0.*deg,360.*deg);
  fLogicPresampler = new G4LogicalVolume(psm_solid, fPresamplerMaterial,"Presampler",0,0,0);
  fPresampler = new G4PVPlacement(0,               // no rotation
      psm_position,  // at (x,y,z)
      fLogicPresampler,    // its logical volume
      "Presampler",        // its name
      worldLV,         // its mother volume
      false,           // no boolean operations
      10,               // copy number
      fCheckOverlaps); // checking overlaps 


  fGem = new G4PVPlacement(0,               // no rotation
                    gem_position, // at (x,y,z)
                    GEM->GetLogicalVolume(),       // its logical volume
                    "GEM",       // its name
                    worldLV,         // its mother  volume
                    false,           // no boolean operations
                    0,               // copy number
                    fCheckOverlaps); // checking overlaps 

  fPadZPosition = GEM->GetPadZ() + gem_position.z();
  // Visualization attributes

  G4VisAttributes* boxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

  worldLV      ->SetVisAttributes(boxVisAtt);
  fLogicPresampler ->SetVisAttributes(boxVisAtt);
  GEM->GetLogicalVolume()    ->SetVisAttributes(boxVisAtt);

  // Sensitive detectors
  G4String trackerChamberSDname = "slrp/GEMSD";
  fGEMSensitiveDetector = new GEMSensitiveDetector(trackerChamberSDname, "GEMHitsCollection");
  fGEMSensitiveDetector->SetPadZ(fPadZPosition);
  G4SDManager::GetSDMpointer()->AddNewDetector(fGEMSensitiveDetector);
  GEM->GetDriftVolume()->SetSensitiveDetector(fGEMSensitiveDetector);
  GEM->GetTransferVolume()->SetSensitiveDetector(fGEMSensitiveDetector);
  //fLogicPresampler->SetSensitiveDetector(aGEMSensitiveDetector);
  //fLogicPresampler->SetUserLimits(new G4UserLimits(fPresamplerWidth/10.));
  GEM->GetLogicalVolume()->SetVisAttributes(chamberVisAtt);

  //BGO 145 22
  // Always return the physical world
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void DetectorConstruction::SetTargetMaterial(G4String )
{

  //G4NistManager* nistManager = G4NistManager::Instance();
  //G4bool fromIsotopes = false;

  //G4Material* pttoMaterial = 
  //            nistManager->FindOrBuildMaterial(materialName, fromIsotopes);

  //if (fPresamplerMaterial != pttoMaterial) {
  //   if ( pttoMaterial ) {
  //      fPresamplerMaterial = pttoMaterial;
  //      if (fLogicPresampler) fLogicPresampler->SetMaterial(fPresamplerMaterial);
  //      G4cout << "\n----> The target is made of " << materialName << G4endl;
  //   } else {
  //      G4cout << "\n-->  WARNING from SetTargetMaterial : "
  //             << materialName << " not found" << G4endl;
  //   }
  //}
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetChamberMaterial(G4String)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void DetectorConstruction::SetMagField(G4double fieldValue)
{
  G4cout << "Set Magnetic field: " << fieldValue/tesla << " T" << G4endl;
  fMagField->SetMagFieldValue(fieldValue);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}  



void DetectorConstruction::SetPresamplerWidth(G4double width)
{
  SetPresamplerGeometry(width, fPsmGemLength);
}

void DetectorConstruction::SetPsmGemLength(G4double length)
{
  SetPresamplerGeometry(fPresamplerWidth, length);
}

void DetectorConstruction::SetPresamplerGeometry(G4double width, G4double distance_to_gem)
{
  G4Tubs * solid = (G4Tubs*)fLogicPresampler->GetSolid();
  Cfg.psm_width=width/mm;
  Cfg.psm_gem_length=distance_to_gem/mm;
  fPresamplerWidth=width; //configure new width
  fPsmGemLength=distance_to_gem; //configure new distance from presampler to gem
  G4ThreeVector position(0,0,-fPresamplerWidth/2.0-fPsmGemLength); //set new position
  G4cout << "Set psm width = " << Cfg.psm_width << " mm" << G4endl;
  G4cout << "Set psm_gem length = " << Cfg.psm_gem_length << " mm" << G4endl;

  //open geometry for modification
  G4GeometryManager * geometry=G4GeometryManager::GetInstance();
  geometry->OpenGeometry(fPresampler);
  //set new width
  solid->SetZHalfLength(fPresamplerWidth/2.0);
  //set new position
  fPresampler->SetTranslation(position);
  //close geometry
  geometry->CloseGeometry(fPresampler);
}
