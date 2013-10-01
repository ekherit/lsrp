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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class

#include "ROOTManager.hh"
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "Config.h"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
 : G4UImessenger(),
   fDetectorConstruction(Det)
{
  fDirectory.reset(new G4UIdirectory("/lsrp/"));
  fDirectory->SetGuidance("UI commands for Laser Polarimeter Simulation");

  fStepMaxCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/stepMax",this));
  fStepMaxCmd->SetGuidance("Define a step max");
  fStepMaxCmd->SetParameterName("stepMax",false);
  fStepMaxCmd->SetUnitCategory("Length");
  fStepMaxCmd->AvailableForStates(G4State_Idle);

  fPhotonNumberCmd.reset(new G4UIcmdWithAnInteger("/lsrp/PhotonNumber",this));
  fPhotonNumberCmd->SetGuidance("Set photon number per pulse");
  fPhotonNumberCmd->SetParameterName("level",false);
  fPhotonNumberCmd->SetDefaultValue(1);

  fRootFileCmd.reset(new G4UIcmdWithAString("/lsrp/RootFile",this));
  fRootFileCmd->SetGuidance("Select output ROOT file");
  fRootFileCmd->SetParameterName("choice",false);
  fRootFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  fPresamplerWidthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PresamplerWidth",this));
  fPresamplerWidthCmd->SetGuidance("Define a width of the presampler");
  fPresamplerWidthCmd->SetParameterName("presamplerWidth",false);
  fPresamplerWidthCmd->SetUnitCategory("Length");
  fPresamplerWidthCmd->AvailableForStates(G4State_Idle);

  fPsmGemLengthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PsmGemLength",this));
  fPsmGemLengthCmd->SetGuidance("Define distance betwee presampler and gem");
  fPsmGemLengthCmd->SetParameterName("PsmGemLength",false);
  fPsmGemLengthCmd->SetUnitCategory("Length");
  fPsmGemLengthCmd->AvailableForStates(G4State_Idle);

  fPadSizeCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PadSize",this));
  fPadSizeCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeCmd->SetParameterName("PadSize",false);
  fPadSizeCmd->SetUnitCategory("Length");
  fPadSizeCmd->AvailableForStates(G4State_Idle);

  fPadSizeXCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PadSizeX",this));
  fPadSizeXCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeXCmd->SetParameterName("PadSizeX",false);
  fPadSizeXCmd->SetUnitCategory("Length");
  fPadSizeXCmd->AvailableForStates(G4State_Idle);

  fPadSizeYCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PadSizeY",this));
  fPadSizeYCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeYCmd->SetParameterName("PadSizeY",false);
  fPadSizeYCmd->SetUnitCategory("Length");
  fPadSizeYCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fStepMaxCmd.get() )
  {
    fDetectorConstruction->SetMaxStep(fStepMaxCmd->GetNewDoubleValue(newValue));
  }   

  if( command == fPresamplerWidthCmd.get())
  {
    fDetectorConstruction->SetPresamplerWidth(fPresamplerWidthCmd->GetNewDoubleValue(newValue));
  }   

  if( command == fPsmGemLengthCmd.get())
  {
    fDetectorConstruction->SetPsmGemLength(fPsmGemLengthCmd->GetNewDoubleValue(newValue));
  }   

  if(command == fPhotonNumberCmd.get())
  {
    Cfg.photon_number = fPhotonNumberCmd->GetNewIntValue(newValue);
  }

  if( command == fRootFileCmd.get() )
  { 
    ROOTManager::Instance()->SetRootFile(newValue);
  }
  
  if( command == fPadSizeCmd.get())
  {
    Cfg.pad_size = fPadSizeCmd->GetNewDoubleValue(newValue)/mm;
    Cfg.pad_xsize = Cfg.pad_size;
    Cfg.pad_ysize = Cfg.pad_size;
  }
  if( command == fPadSizeXCmd.get())
  {
    Cfg.pad_xsize = fPadSizeXCmd->GetNewDoubleValue(newValue)/mm;
  }
  if( command == fPadSizeYCmd.get())
  {
    Cfg.pad_ysize = fPadSizeYCmd->GetNewDoubleValue(newValue)/mm;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
