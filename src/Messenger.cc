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
#include "Messenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "Config.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Messenger::Messenger(G4RunManager * theRunManager)
 : G4UImessenger(),
   fRunManager(theRunManager)
{
  fDirectory.reset(new G4UIdirectory("/lsrp/"));
  fDirectory->SetGuidance("UI commands specific to this example.");

  //fDetDirectory = new G4UIdirectory("/lsrp/det/");
  //fDetDirectory->SetGuidance("Detector construction control");

  fPhotonNumberCmd.reset(new G4UIcmdWithAnInteger("/lsrp/PhotonNumber",this));
  fPhotonNumberCmd->SetGuidance("Set photon number per pulse");
  fPhotonNumberCmd->SetParameterName("level",false);
  fPhotonNumberCmd->SetDefaultValue(1);

  fRootFileCmd.reset(new G4UIcmdWithAString("/lsrp/RootFile",this));
  fRootFileCmd->SetGuidance("Select output ROOT file");
  fRootFileCmd->SetParameterName("choice",false);
  fRootFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Messenger::~Messenger()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Messenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if(command == fPhotonNumberCmd.get())
  {
    Cfg.photon_number = fPhotonNumberCmd->GetNewIntValue(newValue);
    G4cout << "Set photon number: " <<  Cfg.photon_number << G4cout;
  }

  if( command == fRootFileCmd.get() )
  { 
    ROOTManager::Instance()->SetRootFile(newValue);
    G4cout << "Set RootFile: " <<  Cfg.output_file << G4cout;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
