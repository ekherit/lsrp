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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "ROOTManager.hh"
#include "DetectorConstruction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic

  G4ParticleDefinition* particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("gamma");

  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(6.0*MeV);
  fCompton.reset(new ibn::phys::compton(5000,2.5e-6,1,1));
  time_t seed = time(0);
  G4cout << "Set seed" << seed << endl;
  CLHEP::HepRandom::setTheSeed(seed);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ThreeVector variate_agnle(G4ThreeVector & v, double DX, double DY)
{
  double nx = G4RandGauss::shoot(v.x(), DX);
  double ny = G4RandGauss::shoot(v.y(), DY);
  //G4cout << nx - v.x() << " " << ny - v.y() << endl;
  double nz = v.z();
  G4ThreeVector v2=G4ThreeVector(nx,ny,nz);
  return v2/v2.mag();
}

void variate_agnle(ibn::phys::compton & C, double dx , double dy)
{
  G4ThreeVector k(C.kx, C.ky, C.kz);
  G4ThreeVector n = k/k.mag();
  G4ThreeVector nnew=variate_agnle(n, dx, dy);
  C.kx = C.E*nnew.x();
  C.ky = C.E*nnew.y();
  C.kz = C.E*nnew.z();
  //G4cout << C.kx/C.E << " " << C.ky << " " << C.kz << G4endl;
  C.theta = nnew.theta();
  C.phi = nnew.phi();
}



void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  //Simulate Compton Backscattering
  double Pg = (G4UniformRand()-0.5) > 0 ? 1 : -1; //variate photon polarization
  fCompton->SetPhotonPolarization(Pg);
  //generate compton backscattering
  double theta_max = 20.0/5000.;
  fCompton->generate([](double xmin, double xmax) { return (xmax-xmin)*G4UniformRand()+xmin; },theta_max);
  double dx = 1e-4*fCompton->Ee/1850.0;
  double kz_kx = 0.2;
  variate_agnle(*fCompton,dx,dx*kz_kx); 
  //no angular distribution
  fCompton->kx=0;
  fCompton->ky=0;
  fCompton->kz=fCompton->E;

  fParticleGun->SetParticleEnergy(fCompton->E*MeV);
  //fill internal class members
  auto & G = ROOTManager::Instance()->Gen;
  G.eventID = anEvent->GetEventID();
  G.P = fCompton->Pg*fCompton->Pe;
  G.Eb = fCompton->Ee;
  G.gamma = fCompton->gamma;
  G.omega = fCompton->omega;
  G.chi = fCompton->chi;
  G.E = fCompton->E;
  G.kx = fCompton->kx;
  G.ky = fCompton->ky;
  G.kz = fCompton->kz;
  G.nx = fCompton->kx/fCompton->E;
  G.ny = fCompton->ky/fCompton->E;
  G.nz = fCompton->kz/fCompton->E;
  G.theta = fCompton->theta;
  G.phi = fCompton->phi;

  //calculate position
  G4double flight_length=5000*cm; //50 m
  G.x = G.kx/G.kz*flight_length/mm;
  G.y = G.ky/G.kz*flight_length/mm;
  //G4cout << "detector construction instance: " << DetectorConstruction::Instance() << endl;
  G.z = DetectorConstruction::Instance()->presampler_front_position/mm;

  //set position
  fParticleGun->SetParticlePosition(G4ThreeVector(G.x*mm, G.y*mm, G.z*mm));

  //set direction
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(G.nx,G.ny,G.nz));
    


  //set branch for tree
  ROOTManager::Instance()->gen_tree->Fill();
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
