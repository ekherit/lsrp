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

GEMDetector::AmplificationCascade::AmplificationCascade(G4double size, G4double kapton_width, G4double cuprum_width)
{
  fCuprumWidth=cuprum_width;
  fKaptonWidth=kapton_width;
  fWidth=kapton_width+2.0*cuprum_width;
  fRadius=size;
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
        i, //copy number
        true); //check overlaps
  }
}

GEMDetector::GEMDetector(void)
{
  //describe main size
  fSizeX = 20*cm;
  fSizeY = 20*cm;
  fRadius = std::max(fSizeX, fSizeY)/2.0;
  fStefWidth=1.5*mm;
  fDriftLength = 3*mm;
  fCascadeNumber=3;
  fKaptonWidth = 50e-6*m;
  fCuprumWidth = 5e-6*m;
  fCascadeWidth = fKaptonWidth+fCuprumWidth*2;
  fTransferLength = 1.5*mm;
  fInductionLength = 2.0*mm;
  fGEMWidth = fStefWidth*2 + fDriftLength 
    + fCascadeNumber*fCascadeWidth +(fCascadeNumber-1)*fTransferLength
    + fInductionLength;
  G4Tubs * solid_volume = new G4Tubs("GEM",0,fRadius, fGEMWidth/2.,0.*deg,360.*deg);
  Ar = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ar");
  LV.reset(new G4LogicalVolume(solid_volume, Ar,"GEM"));

  //describe corpus which is made from stef
  G4Tubs * StefSV = new G4Tubs("STEF",0.,fRadius,fStefWidth/2.,0.*deg,360.*deg);
  stef = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  G4LogicalVolume * stefLV = new G4LogicalVolume(StefSV, stef,"Stef");
  for(int i=0;i<2;i++)
  {
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,(2*i-1)*(fGEMWidth-fStefWidth)/2.0), //position
        stefLV,
        "Stef", //the name
        LV.get(), //mother logical volume
        i, //copy number
        true); //check overlaps
  }
  //describe drift region
  G4Tubs * DriftSV = new G4Tubs("Drift",0.,fRadius,fDriftLength/2.0,0.*deg,360.*deg);
  fDriftVolume.reset(new G4LogicalVolume(DriftSV, Ar,"DriftVolume"));
  new G4PVPlacement(0, //no rotation
      G4ThreeVector(0,0,-fGEMWidth/2.0+fStefWidth+fDriftLength/2.0), //position
      fDriftVolume.get(),
      "DriftVolume", //the name
      LV.get(), //mother logical volume
      0, //copy number
      true); //check overlaps

  //Describe amplification cascade
  fAmplCascade.reset(new AmplificationCascade(2*fRadius, fKaptonWidth, fCuprumWidth));
  for(int i=0;i<fCascadeNumber; i++)
  {
    G4double z0 = -fGEMWidth/2.0+fStefWidth+fDriftLength;
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,z0 + fCascadeWidth/2. + i*(fCascadeWidth+fTransferLength)), //position
        fDriftVolume.get(),
        "AmplificationCascadeVolume", //the name
        LV.get(), //mother logical volume
        i, //copy number
        true); //check overlaps
  }

  //describe TransfereVolume
  G4Tubs * TransferSV = new G4Tubs("Transfere",0.,fRadius,fTransferLength/2.0,0.*deg,360.*deg);
  for(int i=0;i<fCascadeNumber-1;i++)
  {
    G4LogicalVolume * lv = new G4LogicalVolume(TransferSV, Ar,"TransfereVolume");
    fTransferVolume.push_back(std::unique_ptr<G4LogicalVolume>(lv));
    G4double z0 = -fGEMWidth/2.0+fStefWidth+fDriftLength+fCascadeWidth;
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(0,0,z0 + fTransferLength/2. + i*(fCascadeWidth + fTransferLength)), //position
        lv,
        "TransfereVolume", //the name
        LV.get(), //mother logical volume
        i, //copy number
        true); //check overlaps
  }
  //Induction volume I can not to discribe becase it is inside mother GEM volume Ar.
}
