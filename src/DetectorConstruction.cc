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

//#include "G4SystemOfUnits.hh"

//#include "G4ios.hh"

//------------------------------------------------------------------------- 

DetectorConstruction * DetectorConstruction::fgInstance = 0;

DetectorConstruction* DetectorConstruction::Instance(void)
{
  return fgInstance;
}
 
DetectorConstruction::DetectorConstruction()
: 
  fLogicPresampler(nullptr),
  fPresamplerMaterial(nullptr), 
  fStepLimit(nullptr),
  fCheckOverlaps(true)
{
  fMessenger = new DetectorMessenger(this);
  fMagField  = new MagneticField();

  //fNbOfChambers = 1;
  //fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
  fgInstance = this;
}

//------------------------------------------------------------------------- 
 
DetectorConstruction::~DetectorConstruction()
{
  //delete [] fLogicChamber; 
  delete fMagField;
  delete fStepLimit;
  delete fMessenger;
}

//------------------------------------------------------------------------- 
 
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//------------------------------------------------------------------------- 

void DetectorConstruction::DefineMaterials()
{
  // Material definition 

  G4NistManager* nistManager = G4NistManager::Instance();

  G4bool fromIsotopes = false;

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR", fromIsotopes);
  
  // Lead defined using NIST Manager
  fPresamplerMaterial  = nistManager->FindOrBuildMaterial("G4_Pb", fromIsotopes);
  nistManager->FindOrBuildMaterial("G4_Cu", fromIsotopes);
  nistManager->FindOrBuildMaterial("G4_KAPTON", fromIsotopes);
  

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//------------------------------------------------------------------------- 

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  fPresamplerWidth = Cfg.psm_width * mm; //presampler width
  fPsmGemLength = Cfg.psm_gem_length * mm; //distance between presampler and gem
  G4double psm_radius = Cfg.psm_size*mm / 2.0; //Radius of the presampler
  G4double world_size_xy = 2 * psm_radius * 1.2;
  G4double world_size_z = 47 * m;
  G4double air_length = 23 * m;
  G4double vacuum_chamber_size_z = 20 * m;
  G4double vacuum_chamber_size_xy = world_size_xy * 0.9;
  G4double vacuum_wall_size_xy = vacuum_chamber_size_xy;
  G4double vacuum_wall_size_z = 0.5 * mm;
  fAirSensWidth = 1 * mm;
  G4double mirror_size_xy = 20 * cm;
  G4double mirror_size_z = 0.5 * cm; 
  
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(world_size_z);

  GEM.reset(new GEMDetector);
  
  //G4ThreeVector gem_position = G4ThreeVector(0,0,presampler_front_position+fPresamplerWidth+psm_gem_length+GEM->GetWidth()/2.0);
  G4ThreeVector psm_position = G4ThreeVector(0, 0, -fPsmGemLength-fPresamplerWidth/2.0 + air_length);
  G4ThreeVector airsens_position = G4ThreeVector(0,0,-fPsmGemLength -fPresamplerWidth - fAirSensWidth/2.0 + air_length);
  G4ThreeVector gem_position = G4ThreeVector(0, 0, GEM->GetWidth() / 2.0 + air_length);
  G4ThreeVector vacuum_chamber_position = G4ThreeVector(0, 0, -vacuum_chamber_size_z / 2.0);
  G4ThreeVector mirror_position = G4ThreeVector(0, 0, -0.5 * m); 

  G4cout << "fPsmGemLength=" << fPsmGemLength/mm << "mm,   psm.z = " << psm_position.z()/mm  << " mm"<< G4endl;

  //presampler_front_position=-psm_gem_length-fPresamplerWidth;
  
  //------------------------------------------------------------------------- 
  // World
  //-------------------------------------------------------------------------
  
  G4Material* air  = G4Material::GetMaterial("G4_AIR");
  G4cout << "Computed tolerance = "
    << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
    << " mm" << G4endl;

  G4Box* worldS = new G4Box(
    "world", 
    world_size_xy / 2.0,
    world_size_xy / 2.0,
    world_size_z / 2.0
  );

  worldLV = new G4LogicalVolume( 
    worldS,
    air,
    "World"
  ); 
  
  //Must place the World Physical volume unrotated at (0,0,0).
  G4VPhysicalVolume* worldPV = new G4PVPlacement(
    0,               // no rotation
    G4ThreeVector(), // at (0,0,0)
    worldLV,         // its logical volume
    "World",         // its name
    0,               // its mother  volume
    false,           // no boolean operations
    0,               // copy number
    fCheckOverlaps   // checking overlaps 
  ); 

  //-------------------------------------------------------------------------  
  // Vacuum chamber
  //------------------------------------------------------------------------- 

  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* vacuum_mat = nistManager->FindOrBuildMaterial("G4_Galactic");

  G4Box* vacuum_chamber_solid = new G4Box(
    "vacuum_chamber", 
    vacuum_chamber_size_xy / 2.0,
    vacuum_chamber_size_xy / 2.0, 
    vacuum_chamber_size_z / 2.0
  );
                
  G4LogicalVolume* vacuum_chamber_logic = new G4LogicalVolume(
    vacuum_chamber_solid,
    vacuum_mat, 
    "Vacuum_chamber"
    );             
               
  new G4PVPlacement(
    0,                                 //no rotation
    vacuum_chamber_position,           //at position
    vacuum_chamber_logic,              //its logical volume
    "Vacuum_chamber",                  //its name
    worldLV,                           //its mother  volume
    false,                             //no boolean operation
    0,                                 //copy number
    fCheckOverlaps                     //overlaps checking
  );

  //------------------------------------------------------------------------- 
  // Vacuum chamber wall
  //------------------------------------------------------------------------- 

  G4Material* vacuum_wall_mat = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  
  G4Box* vacuum_wall_solid = new G4Box(
    "vacuum_wall", 
    vacuum_wall_size_xy / 2.0,
    vacuum_wall_size_xy / 2.0, 
    vacuum_wall_size_z / 2.0
  );
                
  G4LogicalVolume* vacuum_wall_logic = new G4LogicalVolume(
    vacuum_wall_solid,
    vacuum_wall_mat, 
    "Vacuum_wall"
    );
         
  new G4PVPlacement(
    0,                                 //no rotation
    G4ThreeVector(),                   //at position
    vacuum_wall_logic,                 //its logical volume
    "Vacuum_wall",                     //its name
    worldLV,                           //its mother  volume
    false,                             //no boolean operation
    0,                                 //copy number
    fCheckOverlaps                     //overlaps checking
  );
  
  //------------------------------------------------------------------------- 
  // Mirror
  //-------------------------------------------------------------------------
  
  G4String name, symbol;             // a=mass of a mole;
  G4double a, z, density;            // z=mean number of protons;  
  G4int ncomponents, natoms; 
  
  a = 28.09*g/mole;  
  G4Element* elSi = new G4Element(name="Silicon", symbol="Si", z=14., a);  // create Si

  a = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);  // create O

  density = 2.200*g/cm3;
  G4Material* SiO2 = new G4Material(name="quartz", density, ncomponents=2); // create quartz
  SiO2->AddElement(elSi, natoms=1);
  SiO2->AddElement(elO , natoms=2);

  G4Material* mirror_mat = nistManager->FindOrBuildMaterial("quartz");
  
  G4RotationMatrix *rm = new G4RotationMatrix;
  rm->rotateX(-45*deg);		// rotation      
  
  G4Box* mirror_solid = new G4Box(
    "mirror",
    mirror_size_xy / 2.0,
    mirror_size_xy / 2.0, 
    mirror_size_z / 2.0
  );
                
  G4LogicalVolume* mirror_logic = new G4LogicalVolume(
    mirror_solid,
    mirror_mat,
    "Mirror"
  );
               
  new G4PVPlacement(
    rm,
    mirror_position,
    mirror_logic,
    "Mirror",
    worldLV,
    false,
    0,
    fCheckOverlaps
  );          
                             
  //-------------------------------------------------------------------------
  // Presampler
  //-------------------------------------------------------------------------

  G4Tubs* psm_solid = new G4Tubs(
    "presampler", 
    0.,
    psm_radius,
    fPresamplerWidth / 2.,
    0.*deg, 360.*deg
  );
  
  fLogicPresampler = new G4LogicalVolume(
    psm_solid,
    fPresamplerMaterial,
    "Presampler", 
    0, 0, 0
  );
  
  fPresampler = new G4PVPlacement(
    0,
    psm_position,
    fLogicPresampler,
    "Presampler",
    worldLV,
    false,
    10,
    fCheckOverlaps
  );
  
  //-------------------------------------------------------------------------
  // AirSens
  //-------------------------------------------------------------------------

  G4Tubs* AirSensSolid = new G4Tubs("AirSens",0.,psm_radius,fAirSensWidth/2.0,0.*deg,360.*deg);
  fLogicAirSens = new G4LogicalVolume(AirSensSolid,air ,"AirSens",0,0,0);
  fAirSens = new G4PVPlacement(
    0,
    airsens_position,
    fLogicAirSens,
    "AirSens",
    worldLV,
    false,
    666,
    fCheckOverlaps
  );
  
  //-------------------------------------------------------------------------
  // GEM
  //-------------------------------------------------------------------------

  fGem = new G4PVPlacement(
    0,
    gem_position,
    GEM->GetLogicalVolume(),
    "GEM",
    worldLV,
    false,
    0,
    fCheckOverlaps
  );
      
  //------------------------------------------------------------------------- 

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
  //fLogicPresampler->SetSensitiveDetector(fGEMSensitiveDetector);
  //fLogicPresampler->SetUserLimits(new G4UserLimits(fPresamplerWidth/10.));
  //GEM->SetUserLimits(new G4UserLimits(0.1*mm));
  GEM->GetLogicalVolume()->SetVisAttributes(chamberVisAtt);
  fLogicAirSens->SetSensitiveDetector(fGEMSensitiveDetector);
  fLogicAirSens->SetUserLimits(new G4UserLimits(1*mm));
  //BGO 145 22
  // Always return the physical world
  return worldPV;
}

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}  

void DetectorConstruction::SetPresamplerWidth(G4double width)
{
  SetPresamplerGeometry(width, fPsmGemLength);
  G4cout << "Set new presampler width " << fPresamplerWidth/mm << " mm" << G4endl;
}

void DetectorConstruction::SetPsmGemLength(G4double length)
{
  SetPresamplerGeometry(fPresamplerWidth, length);
  G4cout << "Set new distance from presampler to GEM " << fPsmGemLength/mm << " mm" << G4endl;
}

void DetectorConstruction::SetPresamplerGeometry(G4double width, G4double distance_to_gem)
{
  G4Tubs * solid = (G4Tubs*)fLogicPresampler->GetSolid();
  Cfg.psm_width=width/mm;
  Cfg.psm_gem_length=distance_to_gem/mm;
  fPresamplerWidth=width; //configure new width
  fPsmGemLength=distance_to_gem; //configure new distance from presampler to gem
  G4ThreeVector position(0,0,-fPresamplerWidth/2.0-fPsmGemLength); //set new position
  //open geometry for modification
  G4GeometryManager * geometry=G4GeometryManager::GetInstance();
  geometry->OpenGeometry(fPresampler);
  //set new width
  solid->SetZHalfLength(fPresamplerWidth/2.0);
  //set new position
  fPresampler->SetTranslation(position);
  //close geometry
  geometry->CloseGeometry(fPresampler);

  //airsens position before presampler
  position = position + G4ThreeVector(0,0,-fPresamplerWidth/2.0 - fAirSensWidth/2.0);
  geometry->OpenGeometry(fAirSens);
  fAirSens->SetTranslation(position);
  geometry->CloseGeometry(fAirSens);

}
