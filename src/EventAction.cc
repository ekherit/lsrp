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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "ROOTManager.hh"
#include "Pad.hh"
#include "TrackerHit.hh"
#include <ibn/math.h>

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(),
  fPrintModulo(1000)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{  
  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0) { 
    G4cout << "\n---> Begin of event: " << eventID << G4endl;
    CLHEP::HepRandom::showEngineStatus();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // print per event (modulo n)

  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0)
    G4cout << "---> End of event: " << eventID << G4endl;

  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
  auto RM = ROOTManager::Instance();
  RM->hit.nhit = hc->GetSize();
  RM->event.E.resize(hc->GetSize());
  for(unsigned i=0; i< hc->GetSize();i++)
  {
    TrackerHit * hit = (TrackerHit*)hc->GetHit(i);
    RM->hit.trackID[i] = hit->GetTrackID();
    RM->hit.volumeID[i] = hit->GetChamberNb();
    RM->hit.E[i] = hit->GetEdep()/MeV;
    Pad pad(1.0*mm, hit->GetPos().x(), hit->GetPos().y());
    RM->hit.x[i] = pad.x();
    RM->hit.y[i] = pad.y();
    RM->hit.z[i] = hit->GetPos().z()/mm;
    RM->hit.rho[i] = sqrt(ibn::sq(RM->hit.y[i]) + ibn::sq(RM->hit.y[i]));
    RM->hit.phi[i] = hit->GetPos().phi();
    RM->event.E[i] = RM->hit.E[i];
    RM->event.s.A+=RM->hit.E[i];
    RM->event.s.B+=RM->hit.E[i]*RM->hit.E[i];
  }
  RM->event.s.A/=hc->GetSize();
  RM->event.s.B/=hc->GetSize();
  ROOTManager::Instance()->tree->Fill();

  // periodic printing

  if ( eventID < 100 || eventID % 100 == 0) {
    G4cout << ">>> Event: " << eventID  << G4endl;
    if ( trajectoryContainer ) {
      G4cout << "    " << n_trajectories
             << " trajectories stored in this event." << G4endl;
    }
    G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << "    "  << hc->GetSize() << " hits stored in this event" << G4endl;
  }
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
