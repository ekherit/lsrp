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
#include <ibn/integral.h>
#include <ibn/phys/constant.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic

  fGamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  fElectron  = G4ParticleTable::GetParticleTable()->FindParticle("electron");

  fParticleGun->SetParticleDefinition(fGamma);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(6.0*MeV);

  const G4double Hz = 1.0/s;
  const G4double kHz = 1e3/s;
  const G4double MHz = 1e6/s;
  const G4double GHz = 1e9/s;
  const G4double mA = 1e-3; //A

  fFlightLength = 50000*mm;
  fThetaMax = 20.0*cm/fFlightLength;
  fBeamEnergy = 5*GeV;
  double lambda = 0.5e-6; //in meters
  //const double hc = 197.326968e-15; // MeV*m;
  double photon_energy = 2*M_PI*ibn::phys::hc/lambda*MeV; //MeV, should be 2.48e-6 MeV
  fCompton.reset(new ibn::phys::compton(fBeamEnergy/MeV,photon_energy,1,1));
  time_t seed = time(0);
  G4cout << "Set seed" << seed << endl;
  CLHEP::HepRandom::setTheSeed(seed);
  double y = 50*mm; //mm
  double theta_max = y/fFlightLength;
  double cos_rf = fCompton->cos_rf(cos(theta_max));
  double xmax = fCompton->xcos(cos_rf);
  double xmin = 1./(1.0+2.0*fCompton->chi);
  double Integral  = ibn::dgaus(*fCompton,xmin,xmax,1e-10);
  double I = 1*mA; //Amper
  G4cout << "Beam current: " << I/mA << " mA" << G4endl;
  double f0 = 818.924*kHz; //Hz revolution frequency
  G4cout << "Revolution frequency: " << f0/kHz << " kHz" << G4endl;
  double Ne = I/(f0/Hz)/ibn::phys::e_SI; // number of electrons in beam
  G4cout << "Number of elctrons in beam: " << Ne << G4endl;
  double puls_time = 6e-9*s; //sec
  double puls_length= puls_time*(ibn::phys::c*m/s);
  double puls_size = 3e-3*m; //m
  G4cout << "puls time = " << puls_time/ns << " ns" << G4endl;
  G4cout << "puls length = " << puls_length/m << " m" << G4endl;
  G4cout << "puls size = " << puls_size/mm << " mm" << G4endl;
  G4double pulse_energy = 100e-6; //pulse energy, Joul
  G4cout << "pulse energy = " << pulse_energy << " mkJ" << G4endl;
  G4cout << "photon energy = " << photon_energy/eV << " eV" << G4endl;
  G4double Ngamma = pulse_energy/ibn::phys::e_SI/(photon_energy/eV);
  G4double ngamma = Ngamma/(puls_length*ibn::sq(puls_size));
  G4cout << "number of initial photons: " << Ngamma << G4endl;
  G4cout << "density of photon beam: " << ngamma*mm*mm*mm << " 1/mm^3" << G4endl;
  G4cout << "Ymax = " << fFlightLength*theta_max/cm << " cm" << G4endl;
  G4cout << "theta_max = " << theta_max << G4endl;
  G4cout << "xmin = " << xmin << endl;
  G4cout << "xmax = " << xmax << endl;
  G4cout << "Integral = " << Integral << endl;
  G4double sigma = M_PI*ibn::sq(ibn::phys::r_e)/fCompton->chi*Integral*m*m; //m^2
  G4cout << "sigma = " << sigma/(m*m) << " m^2" << " (" << sigma/(1e-24*cm*cm) << " b)"<<G4endl;
  G4double nu = sigma*(ibn::phys::c*m/s)*Ne*ngamma;
  G4cout << "Rate: " << nu/GHz << " GHz" << G4endl;
  G4cout << "number of scattered photons: " << nu*puls_time << " per pulse" << G4endl;
  G4double  laser_frequency = 5*kHz;
  G4cout << "Laser frequency: " << laser_frequency/kHz << " kHz" << G4endl;
  G4cout << "Photon rate to detector: " << laser_frequency*nu*puls_time/kHz <<" kHz" <<  G4endl;
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

  for(int i=0;i<10;i++)
  {
    //Simulate Compton Backscattering
    double Pg = (G4UniformRand()-0.5) > 0 ? 1 : -1; //variate photon polarization
    fCompton->SetPhotonPolarization(Pg);
    //generate compton backscattering
    fCompton->generate([](double xmin, double xmax) { return (xmax-xmin)*G4UniformRand()+xmin; },fThetaMax);
    double dx = 1e-4*fCompton->Ee/1850.0;
    double kz_kx = 0.2;
    variate_agnle(*fCompton,dx,dx*kz_kx); 
    //no angular distribution
    //fCompton->kx=0;
    //fCompton->ky=0;
    //fCompton->kz=fCompton->E;

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
    G.x = G.kx/G.kz*fFlightLength/mm;
    G.y = G.ky/G.kz*fFlightLength/mm;
    //G4cout << "detector construction instance: " << DetectorConstruction::Instance() << endl;
    G.z = DetectorConstruction::Instance()->presampler_front_position/mm;

    //set position
    fParticleGun->SetParticlePosition(G4ThreeVector(G.x*mm, G.y*mm, G.z*mm));

    //set direction
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(G.nx,G.ny,G.nz));



    //set branch for tree
    ROOTManager::Instance()->gen_tree->Fill();
    //fParticleGun->SetParticleDefinition(fGamma);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    //fParticleGun->SetParticleDefinition(fElectron);
    //fParticleGun->SetParticleEnergy(10*GeV);
    //fParticleGun->SetParticlePosition(G4ThreeVector(5*cm, 5*cm, G.z*mm));
    //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,0));
    //fParticleGun->GeneratePrimaryVertex(anEvent);
    //ROOTManager::Instance()->gen_tree->Fill();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
