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
/// \file GEMSensitiveDetector.cc
/// \brief Implementation of the GEMSensitiveDetector class

#include <ibn/math.h>
#include "GEMSensitiveDetector.hh"
#include "ROOTManager.hh"
#include "TrackInformation.hh"

#include "Randomize.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMSensitiveDetector::GEMSensitiveDetector(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMSensitiveDetector::~GEMSensitiveDetector() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection = new GEMHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

std::list<Pad> GetHitPadList(G4ThreeVector hit_pos, double total_charge, double spot_size, unsigned N=2500)
{
  std::list<Pad> Pads;
  for(unsigned i=0;i<N;i++)
  {
    G4double x = G4RandGauss::shoot(hit_pos.x(), spot_size);
    G4double y = G4RandGauss::shoot(hit_pos.y(), spot_size);
    Pad pad(Cfg.pad_xsize, Cfg.pad_ysize, x,y);
    //std::cout << hit_pos.x() << "," << hit_pos.y() << " ->     " << x << "," << y << " spot_size=" << spot_size/mm << " mm";
    //std::cout << "     " << pad.x() << "," << pad.y() << std::endl;
    auto p = std::find(std::begin(Pads),std::end(Pads), pad);
    if(p==Pads.end()) 
    {
      Pads.push_back(pad);
    }
    else
    {
      p->nhit++;
    }
  }
  //calculate charge fraction
  for(auto & p : Pads) 
  {
    p.charge = double(p.nhit)/double(N)*total_charge;
    p.nhit=1;
  }
  return Pads;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool GEMSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double charge = edep/(85.7*eV);
  G4ThreeVector r = aStep->GetPostStepPoint()->GetPosition(); //hit position 

  if (edep==0.) return false;

  GEMHit* newHit = new GEMHit();

  auto track = aStep->GetTrack();
  TrackInformation* info = (TrackInformation*)(track->GetUserInformation());

  newHit->SetTrackID  (track->GetTrackID());
  newHit->SetOriginalTrackID (info->GetOriginalTrackID());
  newHit->SetParticleID(track->GetParticleDefinition()->GetPDGEncoding());
  newHit->SetVolumeID(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit->SetEdep(edep);
  newHit->SetPos(r);
  newHit->SetMomentum(track->GetVertexMomentumDirection());
  //G4cout<< "Before find pad" << G4endl;
  newHit->FindPad();
  newHit->SetCharge(charge*GetAmplification(newHit->GetVolumeID()));
  std::list<Pad> pad_list;
  if(Cfg.drift_spread)
  {
    // calculate drift length
    G4double  drift_length = fPadZPosition - r.z();
    G4double  drift_speed = 15*um/mm;
    G4double  spot_size = drift_speed*drift_length;
    pad_list = GetHitPadList(r, newHit->GetCharge(), spot_size);
  }
  else 
  {
    pad_list.push_back(newHit->GetPad());
  }
  //Fill information about original photon
  for(auto & p : pad_list)
  {
    p.tracks.clear();
    p.tracks.push_back(info->GetOriginalTrackID());
    //no there is only one id in each pad.
  }
  newHit->SetPads(pad_list);
  //std::cout << "initial pad: " << newHit->GetPad().x() << "," << newHit->GetPad().y() << std::endl;
  //for(auto & p : newHit->GetPads())
  //{
  //  std::cout << "               " << p.x() << "," << p.y() << " frac: " << setprecision(3) << p.charge/newHit->GetPad().charge*100 << std::endl;
  //}

  fHitsCollection->insert( newHit );
  //newHit->Print();
  return true;
}

G4double GEMSensitiveDetector::GetAmplification(int volume_id)
{
  double K0 = Cfg.gem_amplification; //maximum amplification coeff
  double Nc = Cfg.gem_cascade_number; //number amplif cascades
  double K =  pow(K0, (Nc-volume_id)/Nc);
  return K;
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
