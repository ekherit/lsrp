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
#include "GEMHit.hh"
#include <ibn/math.h>
#include "Config.h"

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
  std::list<int> phot_list; //list of registered photons

  auto RM = ROOTManager::Instance();
  RM->event.d=Cfg.psm_width;
  RM->event.l=Cfg.psm_gem_length;
  RM->event.psx=Cfg.pad_xsize;
  RM->event.psy=Cfg.pad_ysize;
  RM->event.run=Cfg.run;
  RM->event.eventID = eventID;
  RM->event.Eb = RM->event.gen[0].Eb;
  RM->event.P = RM->event.gen[0].P;
  RM->event.nphot=Cfg.photon_number;
  RM->event.nhit = hc->GetSize();
  RM->event.hit.resize(hc->GetSize());
  for(unsigned i=0; i< hc->GetSize();i++)
  {
    GEMHit * hit = (GEMHit*)hc->GetHit(i);
    RM->event.hit[i].trackID = hit->GetTrackID();
    RM->event.hit[i].OrigTrackID = hit->GetOriginalTrackID();
    RM->event.hit[i].pid = hit->GetParticleID();
    RM->event.hit[i].volumeID = hit->GetVolumeID();
    RM->event.hit[i].E = hit->GetEdep()/MeV;
    RM->event.hit[i].x = hit->GetPos().x()/mm;
    RM->event.hit[i].y = hit->GetPos().y()/mm;
    RM->event.hit[i].z = hit->GetPos().z()/mm;
    RM->event.hit[i].rho = sqrt(ibn::sq(RM->event.hit[i].y) + ibn::sq(RM->event.hit[i].y));
    RM->event.hit[i].phi = hit->GetPos().phi();
    RM->event.hit[i].q = hit->GetCharge()/coulomb*1e15;
    for(auto & pad : hit->GetPads())
    {
      auto p = std::find(std::begin(fPads),std::end(fPads), pad);
      if(p==fPads.end()) 
      {
        fPads.push_back(pad);
      }
      else
      {
        p->nhit++;
        p->charge+= pad.charge;
        p->tracks.insert(std::end(p->tracks), std::begin(pad.tracks), std::end(pad.tracks));
      }
    }
  }
  fPads.sort();
  //amplification of the charge
  RM->event.npad = fPads.size();
  RM->event.pad.resize(fPads.size());
  int i=0;
  for(auto & p : fPads) 
  {
    PadEvent & epad=RM->event.pad[i];
    p.tracks.sort(); //sort initial photon track identificators
    p.tracks.unique(); //remove doubles
    phot_list.insert(std::end(phot_list), std::begin(p.tracks), std::end(p.tracks)); //write tracks id to global registered photon list
    epad.X = p.x();
    epad.Y = p.y();
    epad.R = sqrt(ibn::sq(epad.X)+ibn::sq(epad.Y));
    epad.nx = p.nx();
    epad.ny = p.ny();
    epad.xhit = p.xhit;
    epad.yhit = p.yhit;
    //spread hit position over pad
    do {
      epad.x = p.x() + p.size()*(G4UniformRand()-0.5);
      epad.y = p.y() + p.size()*(G4UniformRand()-0.5)*sqrt(3.0)/2.0;
    } while (Pad(p.xsize(),p.ysize(), epad.x, epad.y)!= p);
    epad.r = ibn::rho(epad.x,epad.y);
    //variate amplification
    do { epad.q = p.charge*(1+0.3*G4RandGauss::shoot()); } while(epad.q <=0);
    //scale charge
    epad.q = epad.q/coulomb*1e15; //fC or femtocoulomb 
    epad.nhit = p.nhit;
    epad.nphot = p.tracks.size();
    for(auto & track : p.tracks) 
    {
      if(track>Cfg.photon_number)
      {
        G4cout << "Original track more then photon number: " << track << " > " << Cfg.photon_number << G4endl;
        continue;
      }
      RM->event.gen[track-1].pad.push_back(epad);
      RM->event.gen[track-1].npad++;
    }
    i++;
  }
  phot_list.sort();
  phot_list.unique();
  RM->event.ndet=phot_list.size();
  RM->tree->Fill();

  // periodic printing

  static long geometry_print_index=0;
  bool ispr1 = eventID < 100;
  bool ispr100 = eventID < 1000 && eventID%100==0;
  bool ispr10k = eventID%10000==0;
  if(eventID==0) geometry_print_index=0;
  if (ispr1 || ispr100 || ispr10k)
  {
    G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << ">>> Event: " << eventID  <<  ".  ";
    G4cout << RM->event.ndet << " photons, "  
      << hc->GetSize() << " hits and " 
      << RM->event.npad << " pads stored in this event.";
    if(geometry_print_index % 10 ==0)
    {
      G4cout << " ( Nphot=" << RM->event.nphot << ", dPb=" << Cfg.psm_width/mm << " mm" 
             << ", l="  << Cfg.psm_gem_length/mm<< " mm"
             << ", psx=" << Cfg.pad_xsize/mm << " mm, psy=" << Cfg.pad_ysize/mm << " mm )" ;
    }
    geometry_print_index++;
    G4cout<< G4endl;
  }
  RM->event.clear();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
