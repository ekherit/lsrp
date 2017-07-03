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
  //fLogicConverter(nullptr),
  fConverterMaterial(nullptr), 
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
    fConverterMaterial  = nistManager->FindOrBuildMaterial("G4_Pb", fromIsotopes);
    nistManager->FindOrBuildMaterial("G4_Cu", fromIsotopes);
    nistManager->FindOrBuildMaterial("G4_KAPTON", fromIsotopes);
  
    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//------------------------------------------------------------------------- 

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(Cfg.world_size_z);
    GEM.reset(new GEMDetector);
    CalculateGeometry();
    G4cout << "Computed tolerance = " << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm << " mm" << G4endl;
  
    //------------------------------------------------------------------------- 
    // World
    //-------------------------------------------------------------------------
  
    G4Material* air  = G4Material::GetMaterial("G4_AIR");


    G4NistManager* nistManager = G4NistManager::Instance();
    auto mater = [nistManager](const char * mat_name)
    {
        return nistManager->FindOrBuildMaterial(mat_name);
    };


    MakeVolume("World",G4Material::GetMaterial("G4_AIR"), BOX, Cfg.world_size_x, Cfg.world_size_y, Cfg.world_size_z);
    MakeVolume("VacuumChamber",mater("G4_Galactic"), TUBE, Cfg.vacuum_chamber_size, 0, fVacuumChamberLength, "World", fVacuumChamberPosition);
    MakeVolume("Flange",mater("G4_STAINLESS-STEEL"), TUBE, Cfg.vacuum_chamber_size, 0, Cfg.flange_width, "VacuumChamber", fFlangePosition);

  
    //------------------------------------------------------------------------- 
    // Mirror
    // SiO2 or Cu
    //-------------------------------------------------------------------------
  
    int mirror_type = 1; // 1 - SiO2 , 2 - Cu
    G4Material* mirror_mat;
  
    switch(mirror_type)
    {
        case 1:
        {
            Cfg.mirror_width = 0.5 * cm;
      
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

            mirror_mat = nistManager->FindOrBuildMaterial("quartz");
        } break;
        case 2:
        {
            Cfg.mirror_width = 1 * cm; 
            mirror_mat = nistManager->FindOrBuildMaterial("G4_Cu");
        } break;
    }


  
    G4RotationMatrix *rm = new G4RotationMatrix;
    rm->rotateX(-45*deg);		// rotation      
    MakeVolume("Mirror",mirror_mat, BOX, Cfg.mirror_size_x, Cfg.mirror_size_y, Cfg.mirror_width, "VacuumChamber", fMirrorPosition, rm);
    MakeVolume("Converter", fConverterMaterial, BOX, Cfg.converter_size, Cfg.converter_size,Cfg.converter_width, "World", fConverterPosition);

    MakeVolume("SensBeforeConverter", G4Material::GetMaterial("G4_AIR"), BOX, Cfg.converter_size, Cfg.converter_size, 1*mm,"World", fSensBeforeConverterPosition, nullptr, 666);
  
  //-------------------------------------------------------------------------
  // GEM
  //-------------------------------------------------------------------------
  //
    fGem = new G4PVPlacement(
        0,
        fGEMPosition,
        GEM->GetLogicalVolume(),
        "GEM",
        fVol["World"].logic.get(),
        false,
        0,
        fCheckOverlaps
    );

    fPadZPosition = GEM->GetPadZ() + fGEMPosition.z();
    // Visualization attributes

    G4VisAttributes* boxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

    fVol["World"].logic -> SetVisAttributes(boxVisAtt);
    fVol["Converter"].logic->SetVisAttributes(boxVisAtt);
    GEM->GetLogicalVolume()->SetVisAttributes(boxVisAtt);

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
    fVol["SensBeforeConverter"].logic->SetSensitiveDetector(fGEMSensitiveDetector);
    fVol["SensBeforeConverter"].logic->SetUserLimits(new G4UserLimits(1*mm));

    return fVol["World"].phys.get(); 
}

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
    if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
    fCheckOverlaps = checkOverlaps;
}  


void DetectorConstruction::CalculateGeometry(void)
{
    auto world_wall = Cfg.world_size_z/2;
    auto gem_half_width = GEM->GetWidth()/2.0;
    fGEMPosition = G4ThreeVector(0, 0, world_wall - gem_half_width - Cfg.gem_world_distance);
    fConverterPosition = fGEMPosition - G4ThreeVector(0, 0, gem_half_width + Cfg.converter_gem_distance + Cfg.converter_width/2.0);
    fFlangePosition = fGEMPosition - G4ThreeVector(0, 0, gem_half_width + Cfg.flange_gem_distance + Cfg.flange_width/2.0);
    fMirrorPosition = fFlangePosition - G4ThreeVector(0, 0, Cfg.flange_width/2.0 + Cfg.mirror_flange_distance + Cfg.mirror_width/2.0);

    fInteractionPointPosition  = fGEMPosition - G4ThreeVector(0, 0, gem_half_width + Cfg.photon_flight_length);

    //the length of vacuum chamber includes the flange
    fVacuumChamberLength =  ((Cfg.flange_width/2.0 + fFlangePosition.z()) -  (-Cfg.world_size_z/2.0));
    auto vacuum_chamber_pos =  0.5*((Cfg.flange_width/2.0 + fFlangePosition.z()) +  (-Cfg.world_size_z/2.0));
    fVacuumChamberPosition  = G4ThreeVector(0,0, vacuum_chamber_pos);
    fSensBeforeConverterPosition =  fConverterPosition - G4ThreeVector(0,0, Cfg.converter_width/2.0  + Cfg.sens_before_converter_width/2.0);
}

void DetectorConstruction::UpdateGeometry(void)
{
    CalculateGeometry();

    G4GeometryManager * geometry=G4GeometryManager::GetInstance();
    //GEM detector
    geometry->OpenGeometry(fGem);
    fGem->SetTranslation(fGEMPosition);
    geometry->CloseGeometry(fGem);

    fVol["Converter"].update_geometry(Cfg.converter_size,Cfg.converter_size, Cfg.converter_width, fConverterPosition);
    fVol["SensBeforeConverter"].update_geometry(Cfg.converter_size,Cfg.converter_size, Cfg.sens_before_converter_width, fSensBeforeConverterPosition);
    fVol["VacuumChamber"].update_geometry(Cfg.vacuum_chamber_size,Cfg.vacuum_chamber_size, fVacuumChamberLength, fVacuumChamberPosition);
    fVol["Flange"].update_geometry(Cfg.vacuum_chamber_size, Cfg.vacuum_chamber_size, Cfg.flange_width, fFlangePosition);
    fVol["Mirror"].update_geometry(Cfg.mirror_size_x, Cfg.mirror_size_y, Cfg.mirror_width, fMirrorPosition);
    
}


void DetectorConstruction::MakeVolume 
        ( 
             std::string name, 
             G4Material * material,
             int shape,
             double size_x, 
             double size_y, 
             double size_z, 
             std::string mother_name,
             G4ThreeVector position,
             G4RotationMatrix * rotation_matrix,
             int copy_number
        )
{
    auto & volume = fVol[name]; //create new VolumeItem with empty volumes
    G4VSolid * solid =nullptr;
    switch (shape)
    {
        case BOX:
            solid = (G4VSolid*) new G4Box
                ( 
                 name.c_str(), 
                 size_x/2.0, 
                 size_y/2.0, 
                 size_z/2.0
                );
            break;
        case TUBE:
            solid = (G4VSolid*) new G4Tubs
                (
                 name.c_str(),
                 0,
                 size_x/2.0,
                 size_z/2.0,
                 0*deg,
                 360*deg
                );
            break;
    };
    volume.solid.reset (solid);
    volume.shape = shape;
    //Create world logical volume
    volume.logic.reset
        (
         new G4LogicalVolume
         (
          volume.solid.get(), 
          material,  //All world has air
          name.c_str()
         )
        ); 

    G4LogicalVolume * mother_volume = mother_name == "" ? nullptr : fVol[mother_name].logic.get();
    //Must place the World Physical volume unrotated at (0,0,0).
    volume.phys.reset
        (
         new G4PVPlacement
         (
          rotation_matrix,               // no rotation
          position,        // at (0,0,0)
          volume.logic.get(),    // its logical volume
          name.c_str(),         // its name
          mother_volume,        // its mother  volume
          false,           // no boolean operations
          copy_number,               // copy number
          fCheckOverlaps   // checking overlaps 
         )
        ); 
}

void DetectorConstruction::VolumeItem_t::update_geometry(double size_x, double size_y, double size_z, G4ThreeVector pos)
{
    G4GeometryManager * geometry=G4GeometryManager::GetInstance();
    geometry->OpenGeometry(phys.get());
    switch (shape)
    {
        case BOX:
            {
                G4Box * box = (G4Box*)(solid.get());
                box->SetZHalfLength(size_z/2.0);
                box->SetXHalfLength(size_x/2.0);
                box->SetYHalfLength(size_y/2.0);
            }
            break;
        case TUBE:
            {
                G4Tubs * tube = (G4Tubs*)(solid.get());
                tube->SetOuterRadius(size_x/2.0);
                tube->SetZHalfLength(size_z/2.0);
            }
            break;
    }
    phys->SetTranslation(pos);
    geometry->CloseGeometry(phys.get());
}
