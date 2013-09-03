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
 
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "MagneticField.hh"
#include "TrackerSD.hh"

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
  fNbOfChambers(0),
  fLogicPresampler(nullptr),
  fLogicChamber(NULL), 
  fPresamplerMaterial(NULL), fChamberMaterial(NULL), 
  fStepLimit(NULL),
 fCheckOverlaps(true)
{
  fMessenger = new DetectorMessenger(this);
  fMagField  = new MagneticField();

  fNbOfChambers = 1;
  fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
  fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
DetectorConstruction::~DetectorConstruction()
{
  delete [] fLogicChamber; 
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
  
  // Lead defined using NIST Manager
  fPresamplerMaterial  = nistManager->FindOrBuildMaterial("G4_Pb", fromIsotopes);
  //fPresamplerMaterial  = nistManager->FindOrBuildMaterial("G4_AIR", fromIsotopes);

  // Xenon gas defined using NIST Manager
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Ar", fromIsotopes);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{

  //my sizes
  G4double psm_width  = 0.5*cm; //width of the Pb presampler
  G4double psm_radius =  20*cm; //Radius of the presampler
  G4double gem_radius = 20*cm;
  G4double gem_width = 1*cm;
  G4double psm_gem_length=1*cm; //distance between presampler and gem
  G4double world_width=(psm_width+gem_width+psm_gem_length)*1.2;
  G4double world_size=2*psm_radius*1.2;

  G4ThreeVector psm_position = G4ThreeVector(0,0,0);
  presampler_front_position = psm_position[3] - psm_width/2;
  G4ThreeVector gem_position = G4ThreeVector(0,0,psm_width/2+gem_width/2+psm_gem_length) + psm_position;

  // Sizes of the principal geometrical components (solids)
  
  //G4double chamberSpacing = 2*cm; // from chamber center to center!

  //G4double chamberWidth = 1.0*cm; // width of the chambers
  //G4double targetLength = 1*cm; // full length of Target
  //G4double targetRadius = 20*cm;   // Radius of Target
  //
  //G4double trackerLength = (fNbOfChambers+1)*chamberSpacing;

  //G4double worldLength = 1.2 * (2*targetLength + trackerLength);


  //targetLength = 0.5*targetLength;             // Half length of the Target  
  //G4double trackerSize   = 0.5*trackerLength;  // Half length of the Tracker

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(world_size);
  G4Material* air  = G4Material::GetMaterial("G4_AIR");
  G4cout << "Computed tolerance = "
    << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
    << " mm" << G4endl;

  G4Box* worldS = new G4Box("world",                                    //its name
      world_size/2,world_size/2,world_width/2); //its size

  G4LogicalVolume* worldLV = 
    new G4LogicalVolume
    (
     worldS,   //its solid
     air,      //its material
     "World"
    ); //its name

  //  Must place the World Physical volume unrotated at (0,0,0).
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
  G4Tubs* psm_solid = new G4Tubs("presampler",0.,psm_radius,psm_width/2,0.*deg,360.*deg);
  fLogicPresampler = new G4LogicalVolume(psm_solid, fPresamplerMaterial,"Presampler",0,0,0);
  new G4PVPlacement(0,               // no rotation
      psm_position,  // at (x,y,z)
      fLogicPresampler,    // its logical volume
      "Presampler",        // its name
      worldLV,         // its mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // checking overlaps 


  //G4Tubs* targetS
  //  = new G4Tubs("target",0.,targetRadius,targetLength,0.*deg,360.*deg);
  //fLogicTarget
  //  = new G4LogicalVolume(targetS, fTargetMaterial,"Target",0,0,0);
  //new G4PVPlacement(0,               // no rotation
  //                  positionTarget,  // at (x,y,z)
  //                  fLogicTarget,    // its logical volume
  //                  "Target",        // its name
  //                  worldLV,         // its mother volume
  //                  false,           // no boolean operations
  //                  0,               // copy number
  //                  fCheckOverlaps); // checking overlaps 

  //G4cout << "Target is " << 2*targetLength/cm << " cm of "
  //       << fTargetMaterial->GetName() << G4endl;

  // Tracker = gem
  G4Tubs* gem_solid = new G4Tubs("gem",0,gem_radius,gem_width/2,0.*deg, 360.*deg);
  fLogicGem = new G4LogicalVolume(gem_solid, fChamberMaterial, "Gem",0,0,0);  
  new G4PVPlacement(0,               // no rotation
                    gem_position, // at (x,y,z)
                    fLogicGem,       // its logical volume
                    "Tracker",       // its name
                    worldLV,         // its mother  volume
                    false,           // no boolean operations
                    1,               // copy number
                    fCheckOverlaps); // checking overlaps 

  // Visualization attributes

  G4VisAttributes* boxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

  worldLV      ->SetVisAttributes(boxVisAtt);
  fLogicPresampler ->SetVisAttributes(boxVisAtt);
  fLogicGem    ->SetVisAttributes(boxVisAtt);

  // Sensitive detectors

  G4String trackerChamberSDname = "slrp/TrackerChamberSD";
  TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector( aTrackerSD );
  fLogicGem->SetSensitiveDetector( aTrackerSD );
  //fLogicPresampler->SetSensitiveDetector(aTrackerSD);
  fLogicGem->SetVisAttributes(chamberVisAtt);
  //BGO 145 22

  // Tracker segments

  //G4cout << "There are " << fNbOfChambers << " chambers in the tracker region. "
  //       << "\nThe chambers are " << chamberWidth/cm << " cm of "
  //       << fChamberMaterial->GetName() << "\nThe distance between chamber is "
  //       << chamberSpacing/cm << " cm" << G4endl;
  
  //G4double firstPosition = -trackerSize + chamberSpacing;
  //G4double firstLength   = trackerLength/10;
  //G4double lastLength    = trackerLength;

  //G4double halfWidth = 0.5*chamberWidth;
  //G4double rmaxFirst = 0.5 * firstLength;

  //G4double rmaxIncr = 0.0;
  //if( fNbOfChambers > 0 ){
  //  rmaxIncr =  0.5 * (lastLength-firstLength)/(fNbOfChambers-1);
  //  if (chamberSpacing  < chamberWidth) {
  //     G4Exception("DetectorConstruction::DefineVolumes()",
  //                 "InvalidSetup", FatalException,
  //                 "Width>Spacing");
  //  }
  //}

  //for (G4int copyNo=0; copyNo<fNbOfChambers; copyNo++)
  //{
  //    G4Tubs* chamberS
  //      = new G4Tubs("chamber", 0, rmax, halfWidth, 0.*deg, 360.*deg);

  //    fLogicChamber[copyNo] =
  //             new G4LogicalVolume(chamberS,fChamberMaterial,"Chamber",0,0,0);

  //    fLogicChamber[copyNo]->SetSensitiveDetector( aTrackerSD );
  //    fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);

  //    new G4PVPlacement(0,                            // no rotation
  //                      G4ThreeVector(0,0,Zposition), // at (x,y,z)
  //                      fLogicChamber[copyNo],        // its logical volume
  //                      "Chamber",                    // its name
  //                      trackerLV,                    // its mother  volume
  //                      false,                        // no boolean operations
  //                      copyNo,                       // copy number
  //                      fCheckOverlaps);              // checking overlaps 

  //}

  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  //G4double maxStep = 0.5*chamberWidth;
  //fStepLimit = new G4UserLimits(maxStep);
  //trackerLV->SetUserLimits(fStepLimit);
 
  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  // Always return the physical world

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();
  G4bool fromIsotopes = false;

  G4Material* pttoMaterial = 
              nistManager->FindOrBuildMaterial(materialName, fromIsotopes);

  if (fPresamplerMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fPresamplerMaterial = pttoMaterial;
        if (fLogicPresampler) fLogicPresampler->SetMaterial(fPresamplerMaterial);
        G4cout << "\n----> The target is made of " << materialName << G4endl;
     } else {
        G4cout << "\n-->  WARNING from SetTargetMaterial : "
               << materialName << " not found" << G4endl;
     }
  }
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetChamberMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();
  G4bool fromIsotopes = false;

  G4Material* pttoMaterial =
    nistManager->FindOrBuildMaterial(materialName, fromIsotopes);

  if (fChamberMaterial != pttoMaterial)
  {
    if ( pttoMaterial )
    {
      fChamberMaterial = pttoMaterial;
      if (fLogicGem) fLogicGem-> SetMaterial(fChamberMaterial);
    }
    G4cout << "\n----> The chambers are made of " << materialName << G4endl;
  } 
  else 
  {
    G4cout << "\n-->  WARNING from SetChamberMaterial : "
      << materialName << " not found" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void DetectorConstruction::SetMagField(G4double fieldValue)
{
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
