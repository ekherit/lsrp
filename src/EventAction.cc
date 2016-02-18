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
#include "PrimaryGeneratorAction.hh"

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
  //G4cout << "EndofEventAction:" << G4endl;

  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0)
    G4cout << "---> End of event: " << eventID << G4endl;

  G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
  std::list<int> phot_list; //list of registered photons

  auto RM = ROOTManager::Instance();
  auto & Revent = RM->event;
  Revent.d=Cfg.psm_width;
  Revent.l=Cfg.psm_gem_length;
  Revent.psx=Cfg.pad_xsize;
  Revent.psy=Cfg.pad_ysize;
  Revent.run=Cfg.run;
  Revent.eventID = eventID;
	auto PGA = PrimaryGeneratorAction::Instance();
  Revent.Nphot = PGA->fPhotonNumber;
  Revent.nphot = PGA->fRealPhotonNumber;
  Revent.gen.resize(Revent.nphot);
  Revent.Eb = PGA->fBeamEnergy;
  Revent.P = PGA->fPolarization;
//G4cout << "Before hc->GetSIze()" << G4endl;
  Revent.nhit = hc->GetSize();
  Revent.hit.resize(hc->GetSize());
//G4cout << "After hc->GetSize() = " << hc->GetSize() << G4endl;

  for(unsigned i=0; i< hc->GetSize();i++)
  {
    GEMHit * hit = (GEMHit*)hc->GetHit(i);
    Revent.hit[i].trackID = hit->GetTrackID();
    Revent.hit[i].OrigTrackID = hit->GetOriginalTrackID();
    Revent.hit[i].pid = hit->GetParticleID();
    Revent.hit[i].volumeID = hit->GetVolumeID();
    Revent.hit[i].E = hit->GetEdep()/MeV;
    Revent.hit[i].x = hit->GetPos().x()/mm;
    Revent.hit[i].y = hit->GetPos().y()/mm;
    Revent.hit[i].z = hit->GetPos().z()/mm;
    Revent.hit[i].rho = sqrt(ibn::sq(Revent.hit[i].y) + ibn::sq(Revent.hit[i].y));
    Revent.hit[i].phi = hit->GetPos().phi();
    //Revent.hit[i].q = hit->GetCharge()/coulomb*1e15;
    Revent.hit[i].q = hit->GetCharge();
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

  std::map <unsigned,std::list<Pad> > tracks;  // photon trackID -> padlist
  for(auto & p : fPads)
  {
    p.tracks.sort();
    p.tracks.unique();
    for(auto track_id : p.tracks)
    {
      tracks[track_id].push_back(p);
    }
  };

  fPads.clear();
  for(auto & item : tracks)
  {
    auto track_id = item.first;
    auto & pad_list = item.second;
    pad_list.sort([](const Pad & p1, const Pad & p2){return p1.charge < p2.charge; });
    //pad_list.erase(++fPads.begin(), fPads.end());
    fPads.push_back(pad_list.front());
  }
	



  //fPads.sort(); //fPads.global pad list
  //sort by charge
  //fPads.sort([](const Pad & p1, const Pad & p2){return p1.charge < p2.charge; });
  //fPads.erase(++fPads.begin(), fPads.end());

  Revent.npad = fPads.size();
  Revent.pad.resize(fPads.size());
  int i=0;
  for(auto & p : fPads) 
  {
    PadEvent & epad=Revent.pad[i];
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
    epad.s = p.area();
    epad.type = p.type();
    //spread hit position over pad
    do
    {
      epad.x = p.x() + p.xsize()*(G4UniformRand()-0.5);
      epad.y = p.y() + p.ysize()*(G4UniformRand()-0.5);
      //epad.x = p.x() + p.xsize()*(G4UniformRand());
      //epad.y = p.y() + p.ysize()*(G4UniformRand());
      //std::cout << "fPads: spread: "<< p.x()<<","<<p.y()<<","<<p.type()<< " " << epad.x << "," << epad.y  << std::endl;
      //Pad newpad(epad.x, epad.y);
      //std::cout << "newpad: " << newpad.x() << " " << newpad.y() << " " << newpad.type() << std::endl;
    } while (Pad(epad.x, epad.y) != p);
    epad.r = ibn::rho(epad.x,epad.y);
    //variate amplification
    do { epad.q = p.charge*(1+0.3*G4RandGauss::shoot()); } while(epad.q <=0);
    //scale charge
    //epad.q = epad.q/coulomb*1e15; //fC or femtocoulomb 
    epad.nhit = p.nhit;
    epad.nphot = p.tracks.size();
    for(auto & track : p.tracks) 
    {
      if(track>Revent.nphot)
      {
        G4cout << "Original track more then real photon number: " << track << " > " << Revent.nphot << G4endl;
        continue;
      }
      Revent.gen[track-1].pad.push_back(epad);
      Revent.gen[track-1].npad++;
    }
    i++;
  }
  phot_list.sort();
  phot_list.unique();
  Revent.ndet=phot_list.size();

  double nup=0;
  double ndown=0;
  double ysum=0;
  double xsum=0;
  double y2sum=0;
  double x2sum=0;
  for(const auto & pad : Revent.pad)
  {
    if(pad.y > 0) nup++;
    if(pad.y < 0) ndown++;
    ysum+=pad.y;
    xsum+=pad.x;
    y2sum+=pad.y*pad.y;
    x2sum+=pad.x*pad.x;
  }
  //find average y
  double My = ysum/Revent.pad.size();
  double Mx = xsum/Revent.pad.size();
  double My2 = y2sum/Revent.pad.size();
  double Mx2 = x2sum/Revent.pad.size();
  Revent.My = My;
  Revent.Mx = Mx;
  Revent.xRMS = sqrt(Mx2 - Mx*Mx);
  Revent.yRMS = sqrt(My2 - My*My);
  Revent.asym = Revent.P*(nup-ndown)/(nup+ndown);
  //calculate asym2
  nup=0;
  ndown=0;
  for(const auto & pad : Revent.pad)
  {
    if(pad.y > My) nup++;
    if(pad.y < Mx) ndown++;
  }
  Revent.asym2 = Revent.P*(nup-ndown)/(nup+ndown);



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
    G4cout << Revent.ndet << " photons, "  
      << hc->GetSize() << " hits and " 
      << Revent.npad << " pads stored in this event.";
    if(geometry_print_index % 10 ==0)
    {
      G4cout << " ( Nphot=" << Revent.nphot << ", dPb=" << Cfg.psm_width/mm << " mm" 
             << ", l="  << Cfg.psm_gem_length/mm<< " mm"
             << ", psx=" << Cfg.pad_xsize/mm << " mm, psy=" << Cfg.pad_ysize/mm << " mm )" ;
    }
    geometry_print_index++;
    G4cout<< G4endl;
  }
  Revent.clear();
  //G4cout << "End end of event action" << G4endl;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
