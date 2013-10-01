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
/// \file GEMHit.cc
/// \brief Implementation of the GEMHit class

#include "GEMHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>


G4Allocator<GEMHit> GEMHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMHit::GEMHit()
 : G4VHit(),
   fTrackID(-1),
   fOriginalTrackID(-1),
   fParticleID(0),
   fVolumeID(-1),
   fEdep(0.),
   fPos(G4ThreeVector()),
   fMomentum(G4ThreeVector()),
   fCharge(0)
   //fPad()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMHit::~GEMHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMHit::GEMHit(const GEMHit& right)
  : G4VHit()
{
  fTrackID   = right.fTrackID;
  fOriginalTrackID = right.fOriginalTrackID;
  fParticleID       = right.fParticleID;
  fVolumeID = right.fVolumeID;
  fEdep      = right.fEdep;
  fPos       = right.fPos;
  fPad       = right.fPad;
  fCharge    = right.fCharge;
  fMomentum       = right.fMomentum;
  fPads      = right.fPads;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GEMHit& GEMHit::operator=(const GEMHit& right)
{
  fTrackID   = right.fTrackID;
  fParticleID = right.fParticleID;
  fVolumeID = right.fVolumeID;
  fEdep      = right.fEdep;
  fPos       = right.fPos;
  fPad       = right.fPad;
  fCharge       = right.fCharge;
  fMomentum       = right.fMomentum;
  fPads      = right.fPads;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int GEMHit::operator==(const GEMHit& right) const
{
  return ( this == &right ) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMHit::Print()
{
  G4cout
     << "  trackID: " << fTrackID << " volumeID: " << fVolumeID
     << "Edep: "
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " Position: "
     << std::setw(7) << G4BestUnit( fPos,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
