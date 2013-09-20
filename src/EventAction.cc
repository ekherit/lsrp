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

#include <algorithm>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(),
  fPrintModulo(1000)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{  
  fPads.clear();
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
  //RM->hit.nhit = hc->GetSize();
  RM->event.nphot=Cfg.photon_number;
  RM->event.nhit = hc->GetSize();
  RM->event.hit.resize(hc->GetSize());
  for(unsigned i=0; i< hc->GetSize();i++)
  {
    TrackerHit * hit = (TrackerHit*)hc->GetHit(i);
    RM->event.hit[i].trackID = hit->GetTrackID();
    RM->event.hit[i].volumeID = hit->GetChamberNb();
    RM->event.hit[i].E = hit->GetEdep()/MeV;
    RM->event.hit[i].x = hit->GetPos().x()/mm;
    RM->event.hit[i].y = hit->GetPos().y()/mm;
    RM->event.hit[i].z = hit->GetPos().z()/mm;
    RM->event.hit[i].rho = sqrt(ibn::sq(RM->event.hit[i].y) + ibn::sq(RM->event.hit[i].y));
    RM->event.hit[i].phi = hit->GetPos().phi();
    const Pad  & pad = hit->GetPad();
    auto p = std::find(std::begin(fPads),std::end(fPads), pad);
    if(p==fPads.end()) 
    {
      fPads.push_back(pad);
      fPads.back().nhit=1;
      fPads.back().xhit=hit->GetPos().x();
      fPads.back().yhit=hit->GetPos().y();
    }
    else
    {
      p->charge+=hit->GetCharge();
      p->nhit++;
    }
  }
  fPads.sort();
  //amplification of the charge
  RM->event.npad = fPads.size();
  RM->event.pad.resize(fPads.size());
  int i=0;
  for(auto & p : fPads) 
  {
    double K0 = 1e4;
    double dK0 = K0*0.3;
    double K = G4RandGauss::shoot(K0,dK0);
    //p.second*=K/coulomb;
    RM->event.pad[i].x = p.x();
    RM->event.pad[i].y = p.y();
    RM->event.pad[i].nx = p.fnx;
    RM->event.pad[i].ny = p.fny;
    RM->event.pad[i].q = p.charge*K/coulomb;
    RM->event.pad[i].nhit = p.nhit;
    RM->event.pad[i].xhit = p.xhit;
    RM->event.pad[i].yhit = p.yhit;
    i++;
  }
  RM->tree->Fill();
  RM->event.clear();

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
