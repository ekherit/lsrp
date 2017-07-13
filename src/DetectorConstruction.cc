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

#include "Config.hh"

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
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

//#include "G4VisAttributes.hh"
//#include "G4Colour.hh"

DetectorConstruction * DetectorConstruction::fgInstance = nullptr;

DetectorConstruction* DetectorConstruction::Instance(void)
{
  if( fgInstance == nullptr ) new DetectorConstruction();
  return fgInstance;
}

DetectorConstruction::DetectorConstruction()  
{
  fgInstance = this;
  fMessenger = DetectorMessenger::Instance();
}

//------------------------------------------------------------------------- 

DetectorConstruction::~DetectorConstruction()
{
  std::cout << "DetectorConstruction::~DetectorConstruction()"<< std::endl;
  delete fStepLimit;
  delete fGEMSensitiveDetector;
  delete fGem;
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
  // Lead defined using NIST Manager
  fConverterMaterial  = nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_Cu");
  nistManager->FindOrBuildMaterial("G4_KAPTON");
  //create the  quartz material
  {

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
  }

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//------------------------------------------------------------------------- 

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  //for(int i=0;i<10;i++)
  //std::cout << "###################################################" << std::endl;
  //std::cout << "                    IN DEFINE VOLUMES " << std::endl;
  //for(int i=0;i<10;i++)
  //std::cout << "###################################################" << std::endl;
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(Cfg.world_size_z);
  GEM.reset(new GEMDetector);
  CalculateGeometry();
  G4cout << "Computed tolerance = " << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm << " mm" << G4endl;

  //------------------------------------------------------------------------- 
  // World
  //-------------------------------------------------------------------------

  auto mater = [this](const std::string & mat_name)
  {
    return nistManager->FindOrBuildMaterial(mat_name.c_str());
  };


  MakeVolume("World",mater(Cfg.world_material), BOX, Cfg.world_size_x, Cfg.world_size_y, Cfg.world_size_z);
  MakeVolume("VacuumChamber",mater(Cfg.vacuum_chamber_material), TUBE, Cfg.flange.size, 0, fVacuumChamberLength, "World", fVacuumChamberPosition);
  MakeVolume("Flange",mater(Cfg.flange.material), TUBE, Cfg.flange.size, 0, Cfg.flange.width, "VacuumChamber", fFlangePosition - fVacuumChamberPosition);
  std::cout << "The positions  vac. cham z =" << fVacuumChamberPosition.z() + fVacuumChamberLength/2.0 << " flange z = " <<  fFlangePosition.z() << std::endl;

  G4RotationMatrix *rm = new G4RotationMatrix;
  rm->rotateY(45*deg);		// rotation      
  MakeVolume("Mirror",mater(Cfg.mirror.material), BOX, Cfg.mirror.size_x, Cfg.mirror.size_y, Cfg.mirror.width, "VacuumChamber", fMirrorPosition - fVacuumChamberPosition, rm);
  MakeVolume("Converter", mater(Cfg.converter.material), BOX, Cfg.converter.size, Cfg.converter.size,Cfg.converter.width, "World", fConverterPosition);
  MakeVolume("SensBeforeConverter",  mater(Cfg.world_material), BOX, Cfg.converter.size, Cfg.converter.size, 1*mm,"World", fSensBeforeConverterPosition, nullptr, 666);

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

  // Sensitive detectors
  G4String trackerChamberSDname = "slrp/GEMSD";
  fGEMSensitiveDetector = new GEMSensitiveDetector(trackerChamberSDname, "GEMHitsCollection");
  fGEMSensitiveDetector->SetPadZ(fGEMPadPosition.z());
  G4SDManager::GetSDMpointer()->AddNewDetector(fGEMSensitiveDetector);
  GEM->GetDriftVolume()->SetSensitiveDetector(fGEMSensitiveDetector);
  GEM->GetTransferVolume()->SetSensitiveDetector(fGEMSensitiveDetector);
  //fLogicPresampler->SetSensitiveDetector(fGEMSensitiveDetector);
  //fLogicPresampler->SetUserLimits(new G4UserLimits(fPresamplerWidth/10.));
  //GEM->SetUserLimits(new G4UserLimits(0.1*mm));
  //GEM->GetLogicalVolume()->SetVisAttributes(chamberVisAtt);
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
  std::cout << "Calculate geometry" << std::endl;
  std::vector<double> ysizes{Cfg.world_size_y, Cfg.flange.size,Cfg.mirror.size_y,Cfg.converter.size, Cfg.gem.size_y, Cfg.gem.size};
  std::vector<double> xsizes{Cfg.world_size_x, Cfg.flange.size,Cfg.mirror.size_x,Cfg.converter.size, Cfg.gem.size_x, Cfg.gem.size};
  //calculate world xy size:
  Cfg.world_size_y = *std::max_element(ysizes.begin(), ysizes.end());
  Cfg.world_size_x = *std::max_element(xsizes.begin(), xsizes.end());
  Cfg.world_size_z =  GEM->GetWidth() + 2*Cfg.gem_world_distance + Cfg.photon_flight_length;
  auto world_wall = Cfg.world_size_z/2;
  auto gem_half_width = GEM->GetWidth()/2.0;
  fGEMPosition = G4ThreeVector(0, 0, world_wall - gem_half_width - Cfg.gem_world_distance);
  fGEMFrontPosition  = fGEMPosition - G4ThreeVector(0, 0, gem_half_width);
  fGEMPadPosition = G4ThreeVector(0,0,GEM->GetPadZ()) + fGEMPosition;
  if (fGEMSensitiveDetector != nullptr) fGEMSensitiveDetector->SetPadZ(fGEMPadPosition.z());
  std::cout << "GEM: z = " <<  fGEMPosition.z() << "  width = " <<  GEM->GetWidth() << std::endl;
  fConverterPosition = fGEMPosition - G4ThreeVector(0, 0, gem_half_width + Cfg.converter_gem_distance + Cfg.converter.width/2.0);
  std::cout  << "Converter: z = " << fConverterPosition.z() << " width = "  <<  Cfg.converter.width << std::endl;
  fFlangePosition = fGEMPosition - G4ThreeVector(0, 0, gem_half_width + Cfg.flange_gem_distance + Cfg.flange.width/2.0);
  std::cout  << "Flange: z = " << fFlangePosition.z() << " width = "  <<  Cfg.flange.width << std::endl;
  fMirrorPosition = fFlangePosition - G4ThreeVector(0, 0, Cfg.flange.width/2.0 + Cfg.mirror_flange_distance + Cfg.mirror.width/2.0);
  std::cout  << "Mirror: z = " << fMirrorPosition.z() << " width = "  <<  Cfg.mirror.width << std::endl;
  fInteractionPointPosition  = fGEMPosition + G4ThreeVector(Cfg.beam.x, Cfg.beam.y, - (gem_half_width + Cfg.photon_flight_length));

  //the length of vacuum chamber includes the flange
  std::cout << "half world " << Cfg.world_size_z/2.0 << std::endl;
  std::cout << "vacuum ch. right = " << Cfg.flange.width/2.0 + fFlangePosition.z() << std::endl;
  std::cout << "vacuum ch. left = " <<  - Cfg.world_size_z/2.0 << std::endl;
  fVacuumChamberLength =  ((Cfg.flange.width/2.0 + fFlangePosition.z()) -  (-Cfg.world_size_z/2.0));
  auto vacuum_chamber_pos =  0.5*((Cfg.flange.width/2.0 + fFlangePosition.z()) +  (-Cfg.world_size_z/2.0));
  fVacuumChamberPosition  = G4ThreeVector(0,0, vacuum_chamber_pos);
  std::cout  << "VacuumChamber: z = " << fVacuumChamberPosition.z() << " width = "  <<  fVacuumChamberLength << std::endl;
  fSensBeforeConverterPosition =  fConverterPosition - G4ThreeVector(0,0, Cfg.converter.width/2.0  + Cfg.sens_before_converter_width/2.0);
  std::cout  << "SensBeforeConverter: z = " << fSensBeforeConverterPosition.z() << " width = "  <<  Cfg.sens_before_converter_width << std::endl;;
}

void DetectorConstruction::UpdateGeometry(void)
{
  CalculateGeometry();
  fCheckOverlaps = false;
  for (auto & p : fVol) p.second.open_geometry();
  GEM->open_geometry();
  fGem->SetTranslation(fGEMPosition);
  GEM->update_geometry(Cfg.gem.size);

  fVol["World"].update_geometry(Cfg.world_size_x,Cfg.world_size_y, Cfg.world_size_z, {0,0,0});
  fVol["World"].logic->SetMaterial(nistManager->FindOrBuildMaterial(Cfg.world_material.c_str()));
  fVol["Converter"].update_geometry(Cfg.converter.size,Cfg.converter.size, Cfg.converter.width, fConverterPosition);
  fVol["Converter"].logic->SetMaterial(nistManager->FindOrBuildMaterial(Cfg.converter.material.c_str()));
  fVol["SensBeforeConverter"].update_geometry(Cfg.converter.size,Cfg.converter.size, Cfg.sens_before_converter_width, fSensBeforeConverterPosition);
  fVol["SensBeforeConverter"].logic->SetMaterial(nistManager->FindOrBuildMaterial(Cfg.world_material.c_str()));
  fVol["VacuumChamber"].update_geometry(Cfg.flange.size,Cfg.flange.size, fVacuumChamberLength, fVacuumChamberPosition);
  fVol["Flange"].update_geometry(Cfg.flange.size, Cfg.flange.size, Cfg.flange.width, fFlangePosition-fVacuumChamberPosition);
  fVol["Flange"].logic->SetMaterial(nistManager->FindOrBuildMaterial(Cfg.flange.material.c_str()));
  fVol["Mirror"].update_geometry(Cfg.mirror.size_x, Cfg.mirror.size_y, Cfg.mirror.width, fMirrorPosition-fVacuumChamberPosition);
  fVol["Mirror"].logic->SetMaterial(nistManager->FindOrBuildMaterial(Cfg.mirror.material.c_str()));
  for (auto & p : fVol) p.second.close_geometry();
  GEM->close_geometry();
  fCheckOverlaps = true;
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

void DetectorConstruction::VolumeItem_t::open_geometry(void)
{
  G4GeometryManager * geometry=G4GeometryManager::GetInstance();
  geometry->OpenGeometry(phys.get());
}

void DetectorConstruction::VolumeItem_t::close_geometry(void)
{
  G4GeometryManager * geometry=G4GeometryManager::GetInstance();
  geometry->CloseGeometry(phys.get());
}

void DetectorConstruction::VolumeItem_t::update_geometry(double size_x, double size_y, double size_z, G4ThreeVector pos)
{
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
}
