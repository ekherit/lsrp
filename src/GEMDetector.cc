/*
 * =====================================================================================
 *
 *       Filename:  GEMDetector.cc
 *
 *    Description:  GEM Detector description
 *
 *        Version:  1.0
 *        Created:  21.09.2013 22:15:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#include "GEMDetector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "Config.h"

#include <boost/lexical_cast.hpp>
GEMDetector::AmplificationCascade::AmplificationCascade(G4double size, G4double kapton_width, G4double cuprum_width)
{
  fCuprumWidth=cuprum_width;
  fKaptonWidth=kapton_width;
  fWidth=kapton_width+2.0*cuprum_width;
  fRadius=size/2.0;
  kapton = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
  Cu     = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  G4Tubs * KaptonSV = new G4Tubs("Kapton",0,fRadius, fWidth/2.,0.*deg,360.*deg);
  G4Tubs * CuprumSV = new G4Tubs("Cuprum",0,fRadius, fCuprumWidth/2.,0.*deg,360.*deg);
  //define logical volume for kopton
  LV.reset(new G4LogicalVolume(KaptonSV, kapton,"Kapton"));
  //define cuprum
  G4LogicalVolume * CuprumLV = new G4LogicalVolume(CuprumSV,Cu,"Cuprum");
  //place logical cuprum inside logical kapton
  for(int i=0;i<2;i++)
  {
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,(2*i-1)*(fKaptonWidth+fCuprumWidth)/2.0), //position
        CuprumLV,
        "Cuprum", //the name
        LV.get(), //mother logical volume
        false, //no boolen operations
        i, //copy number
        true); //check overlaps
  }
}


G4Material * EpoxyMaterial(void)
{
  G4String name = "Epoxy";
  G4Material* material = G4Material::GetMaterial(name, false);
  if(material) return material;
  material = new G4Material(name, 1.3*g/cm3,3);
  G4NistManager* nist = G4NistManager::Instance();
  G4Element* H = nist->FindOrBuildElement("H");
  G4Element* C = nist->FindOrBuildElement("C");
  G4Element* O = nist->FindOrBuildElement("O");
  material->AddElement(H, 44);
  material->AddElement(C, 15);
  material->AddElement(O, 7);
  return material;
}

G4Material * StefMaterial(void)
{
  G4String name = "Stef";
  G4Material* material = G4Material::GetMaterial(name, false);
  if(material) return material;
  material = new G4Material(name, 1.7*g/cm3,2);
  G4NistManager* nist = G4NistManager::Instance();
  G4Material * SiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  material->AddMaterial(SiO2, 85.3*perCent);
  material->AddMaterial(EpoxyMaterial(), 14.7*perCent);
  return material;
}



GEMDetector::GEMDetector(void)
{
  //describe main size
  fSizeX = Cfg.gem_size*mm;
  fSizeY = Cfg.gem_size*mm;
  fRadius = std::max(fSizeX, fSizeY)/2.0;
  fStefWidth=1.5*mm;
  fDriftLength = 3*mm;
  fCascadeNumber=Cfg.gem_cascade_number; //should be 3
  fKaptonWidth = 50*um;
  fCuprumWidth = 5*um;
  fCascadeWidth = fKaptonWidth+fCuprumWidth*2;
  fTransferLength = 1.5*mm;
  fInductionLength = 2.0*mm;
  fPadWidth = 15.*um;
  fGEMWidth = fStefWidth*2 + fDriftLength 
    + fCascadeNumber*fCascadeWidth +(fCascadeNumber-1)*fTransferLength
    + fInductionLength + fPadWidth*2;

  G4Tubs * solid_volume = new G4Tubs("GEM",0,fRadius, fGEMWidth/2.,0.*deg,360.*deg);
  Ar = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ar");
  LV.reset(new G4LogicalVolume(solid_volume, Ar,"GEM"));

  //describe corpus which is made from stef
  G4Tubs * StefSV = new G4Tubs("STEF",0.,fRadius,fStefWidth/2.,0.*deg,360.*deg);
  G4LogicalVolume * stefLV = new G4LogicalVolume(StefSV, StefMaterial(),"Stef");
  fStefVolume = stefLV;
  //place front stef
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,-fGEMWidth/2.0+fStefWidth/2.0), //position
      stefLV,
      "Stef", //the name
      LV.get(), //mother logical volume
      false, //no boolen operations
      0, //copy number
      fCheckOverlaps); //check overlaps
  //place back stef
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,+fGEMWidth/2.0-fPadWidth-fStefWidth/2.0), //position
      stefLV,
      "Stef", //the name
      LV.get(), //mother logical volume
      false, //no boolen operations
      1, //copy number
      fCheckOverlaps); //check overlaps

  //create pad and back electrondes
  G4Tubs * PadSV = new G4Tubs("Pads",0.,fRadius,fPadWidth/2.,0.*deg,360.*deg);
  G4Material * Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  G4LogicalVolume * padLV = new G4LogicalVolume(PadSV, Cu,"Pad");
  fPadZposition = fGEMWidth/2.0-fPadWidth-fStefWidth-fPadWidth/2.0;
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,fPadZposition), //position
      padLV,
      "Pad", //the name
      LV.get(), //mother logical volume
      false, //no boolen operations
      0, //copy number
      fCheckOverlaps); //check overlaps
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,fGEMWidth/2.0-fPadWidth/2.0), //position
      padLV,
      "Pad", //the name
      LV.get(), //mother logical volume
      false, //no boolen operations
      1, //copy number
      fCheckOverlaps); //check overlaps

  //describe drift region
  G4Tubs * DriftSV = new G4Tubs("Drift",0.,fRadius,fDriftLength/2.0,0.*deg,360.*deg);
  fDriftVolume.reset(new G4LogicalVolume(DriftSV, Ar,"DriftVolume"));
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,-fGEMWidth/2.0+fStefWidth+fDriftLength/2.0), //position
      fDriftVolume.get(),
      "DriftVolume", //the name
      LV.get(), //mother logical volume
      false, //no boolen operations
      0, //copy number
      fCheckOverlaps); //check overlaps

  //Describe amplification cascade
  fAmplCascade.reset(new AmplificationCascade(2*fRadius, fKaptonWidth, fCuprumWidth));
  for(int i=0;i<fCascadeNumber; i++)
  {
    G4double z0 = -fGEMWidth/2.0+fStefWidth+fDriftLength;
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,z0 + fCascadeWidth/2. + i*(fCascadeWidth+fTransferLength)), //position
        fAmplCascade->GetLogicalVolume(),
        "AmplificationCascadeVolume", //the name
        LV.get(), //mother logical volume
        false, //no boolen operations
        i, //copy number
        fCheckOverlaps); //check overlaps
  }

  //describe TransfereVolume
  G4Tubs * TransferSV = new G4Tubs("Transfere",0.,fRadius,fTransferLength/2.0,0.*deg,360.*deg);
  fTransferVolume.reset(new G4LogicalVolume(TransferSV, Ar,"TransfereVolume"));
  for(int i=0;i<fCascadeNumber-1;i++)
  {
    G4double z0 = -fGEMWidth/2.0+fStefWidth+fDriftLength+fCascadeWidth;
    std::string name = "TransfereVolume"+boost::lexical_cast<std::string>(i+1);
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,z0 + fTransferLength/2. + i*(fCascadeWidth + fTransferLength)), //position
        fTransferVolume.get(),
        name.c_str(),
        //"TransfereVolume"+boost::lexyc, //the name
        LV.get(), //mother logical volume
        false, //no boolen operations
        i+1, //copy number //zero is for DriftVolume
        fCheckOverlaps); //check overlaps
  }
  //fDriftVolume->SetUserLimits(new G4UserLimits(fDriftLength/10.));
  //fTransferVolume->SetUserLimits(new G4UserLimits(fTransferLength/10.));
  PrintGeometry();
}

void GEMDetector::PrintGeometry(void)
{
  G4cout << "GEM total size (X*Y*Z): " << fSizeX/mm << "x" << fSizeY/mm << "x" << fGEMWidth/mm << " mm*mm*mm" << G4endl;
  G4cout << "StefWidth: " << fStefWidth/mm << " mm " << G4endl;
  G4cout << "DriftLength: " << fDriftLength/mm << " mm " << G4endl;
  G4cout << "TransferLength: " << fTransferLength/mm << " mm " << G4endl;
  G4cout << "InductionLength: " << fInductionLength/mm << " mm " << G4endl;
  G4cout << "PadWidth: " << fPadWidth/um << " mkm " << G4endl;
  G4cout << "CuprumWidth: " << fCuprumWidth/um << " mkm " << G4endl;
  G4cout << "KaptonWidth: " << fKaptonWidth/um << " mkm " << G4endl;
  G4cout << "Numbero of amplification cascades: " << fCascadeNumber << G4endl;
}


