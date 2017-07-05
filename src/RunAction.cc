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
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "ROOTManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "Config.h"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"


#include <ibn/valer.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  Cfg.run = aRun->GetRunID();

  //inform the runManager to save random number seed

  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  DetectorConstruction::Instance()->UpdateGeometry();
  PrimaryGeneratorAction::Instance()->Init();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ibn::valer<double> get_epsilon(Long64_t N1, Long64_t N2)
{
  ibn::valer<double> eps;
  eps.value = double(N1-N2)/double(N1+N2);
  eps.error = sqrt(ibn::sq(2.0*N2/ibn::sq(double(N1+N2)))*N1 + ibn::sq(2.0*N1/ibn::sq(double(N1+N2)))*N2);
  return eps;
}

void RunAction::EndOfRunAction(const G4Run* )
{
  auto RM = ROOTManager::Instance();
  if(RM->tree !=nullptr) RM->tree->Write();
  //Long64_t Nup0 = RM->gen_tree->GetEntries("y>0&&P>0") + RM->gen_tree->GetEntries("y<0&&P<0");
  //Long64_t Ndown0 = RM->gen_tree->GetEntries("y<0&&P>0")+ RM->gen_tree->GetEntries("y>0&&P<0");
  //ibn::valer<double> eps0 = get_epsilon(Nup0,Ndown0);
  //Long64_t Nup = RM->tree->GetEntries("hit.y>0 && volumeID==1&& P>0") +  RM->tree->GetEntries("hit.y<0 && volumeID==1&& P<0");
  //Long64_t Ndown = RM->tree->GetEntries("hit.y<0 && volumeID==1 && P>0") +  RM->tree->GetEntries("hit.y>0 && volumeID==1 && P<0");
  //ibn::valer<double> eps = get_epsilon(Nup,Ndown);
  //RM->gen_tree->Write();
  //G4cout << "Initial assymetry: " << eps0.value << "+-" << eps0.error << " ("<< eps0.error/eps0.value*100<< "%)" << G4endl;
  //G4cout << "Gem assymetry:     " << eps.value << "+-" << eps.error << " ("<< eps.error/eps.value*100<< "%)" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
