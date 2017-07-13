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
#include "Config.hh"
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

void EventAction::BeginOfEventAction(const G4Event* )
{  
  //fPads.clear();
  //G4int eventID = event->GetEventID();
  //if ( eventID % fPrintModulo == 0) 
  //{ 
  //  G4cout << "\n---> Begin of event: " << eventID << G4endl;
  //  CLHEP::HepRandom::showEngineStatus();
  //}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  G4int eventID = event->GetEventID();
  //if ( eventID % fPrintModulo == 0)
  //  G4cout << "---> End of event: " << eventID << G4endl;

  G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
  std::list<int> phot_list; //list of registered photons

  auto RM = ROOTManager::Instance();
  auto & Revent = RM->event;
  Revent.d=Cfg.converter.width;
  Revent.l=Cfg.converter_gem_distance;
  Revent.psx=Cfg.gem.pad.size_x;
  Revent.psy=Cfg.gem.pad.size_y;
  Revent.run=Cfg.run;
  Revent.eventID = eventID;
	auto PGA = PrimaryGeneratorAction::Instance();
  Revent.Nphot = PGA->fPhotonNumber;
  Revent.nphot = PGA->fRealPhotonNumber;
  Revent.gen.resize(Revent.nphot);
  Revent.Eb = PGA->fBeamEnergy;
  Revent.P = PGA->fPolarization;
  Revent.nhit = hc->GetSize();

  std::list<Pad> fPads; //collection of pads
  //Make total hit pad list
  //One photon can produce many hits
  //each hit can hit many pads
  for(unsigned i=0; i< hc->GetSize();i++)
  {
    for(auto & pad : static_cast<GEMHit*>(hc->GetHit(i))->GetPads())
    {
      auto p = std::find(std::begin(fPads),std::end(fPads), pad);
      if(p==fPads.end()) //new hit pad
      {
        fPads.push_back(pad);//here nhit=1
      }
      else //already find the pad with hit
      { 
        p->nhit++; //increase number of hits
        p->charge+= pad.charge; //collect charge
        //register the track ids
        p->tracks.insert(std::end(p->tracks), std::begin(pad.tracks), std::end(pad.tracks));
      }
    }
  }
  //Now we have pad list with number of hits and with the photon track list in each pad
  std::map <unsigned,std::list<Pad> > tracks;  // photon trackID -> padlist
  for(auto & p : fPads)
  {
    //keep only unique track for each pad
    p.tracks.sort();
    p.tracks.unique();
    for(auto track_id : p.tracks)
    {
      tracks[track_id].push_back(p);
    }
  };

  if(Cfg.root.one_pad_per_track == 1)  //refill the pad list keep only signle pad for each registered track
  {
    fPads.clear();
    for(auto & item : tracks)
    {
      ///auto track_id = item.first;
      auto & pad_list = item.second;
      pad_list.sort([](const Pad & p1, const Pad & p2){return p1.charge < p2.charge; });
      //pad_list.erase(++fPads.begin(), fPads.end());
      fPads.push_back(pad_list.front()); //keep only pad with highest charge
    }
  }

  if(Cfg.root.save_hits == 1)
  {
    //Loop over all hit collection
    Revent.hit.resize(hc->GetSize());
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
      Revent.hit[i].rho = hypot(Revent.hit[i].y,Revent.hit[i].y);
      Revent.hit[i].phi = hit->GetPos().phi();
      //Revent.hit[i].q = hit->GetCharge()/coulomb*1e15;
      Revent.hit[i].q = hit->GetCharge();
    }
  }

  Revent.npad = fPads.size();
  Revent.pad.resize(fPads.size());
  int i=0;
  for(auto & p : fPads) 
  {
    PadEvent & epad=Revent.pad[i];
    //tracks should be already unique
    //p.tracks.sort(); //sort initial photon track identificators
    //p.tracks.unique(); //remove doubles
    phot_list.insert(std::end(phot_list), std::begin(p.tracks), std::end(p.tracks)); //write tracks id to global registered photon list
    epad.X = p.x();
    epad.Y = p.y();
    epad.R = hypot(epad.X,epad.Y);
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
    } while (Pad(epad.x, epad.y) != p);
    epad.r = hypot(epad.x,epad.y);
    //variate amplification
    do { epad.q = p.charge*(1+0.3*G4RandGauss::shoot()); } while(epad.q <=0);
    //std::cout << "epad.q = " << epad.q << std::endl;
    //std::cout << "p.charge = " << p.charge << std::endl;
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

  if(RM->tree != nullptr) RM->tree->Fill();

  // periodic printing

  static long geometry_print_index=0;
  auto isprint = [](int id, int period)  -> bool
  {
    return (id < period*10) && (id % period == 0);
  };
  class index_print
  {
    int a;

  };

  bool ispr1 = isprint(eventID,1);
  bool ispr10 = isprint(eventID,10);
  bool ispr100 = isprint(eventID,100);
  bool ispr1000 = isprint(eventID,1000);
  bool ispr10000 = isprint(eventID,10000);

  //bool ispr100 = eventID < 1000 && eventID%100==0;
  //bool ispr1000 = eventID < 10000 && eventID%1000==0;
  //bool ispr10k = eventID%10000==0;
  if(eventID==0) geometry_print_index=0;
  if (ispr1 || ispr100 || ispr10000)
  {
    G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << ">>> Event: " << eventID  <<  ".  ";
    G4cout << Revent.ndet << " photons, "  
      << hc->GetSize() << " hits and " 
      << Revent.npad << " pads stored in this event.";
    if(geometry_print_index % 10 ==0)
    {
      G4cout << " ( Nphot=" << Revent.nphot << ", dPb=" << Cfg.converter.width/mm << " mm" 
             << ", l="  << Cfg.converter_gem_distance/mm<< " mm"
             << ", psx=" << Cfg.gem.pad.size_x/mm << " mm, psy=" << Cfg.gem.pad.size_y/mm << " mm )" ;
    }
    geometry_print_index++;
    G4cout<< G4endl;
  }
  Revent.clear();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
