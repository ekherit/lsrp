#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1


#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

#include "Compton.h"

#include <memory>

class G4ParticleGun;
class G4Event;

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the Tracker 
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class 
/// (see the macros provided with this example).

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    static PrimaryGeneratorAction* Instance(void);
    unique_ptr<ibn::phys::compton> fCompton; 
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* );

    G4ParticleGun* GetParticleGun() {return fParticleGun;}

    void SetRandomFlag(G4bool );
    
    G4double fFlightLength; //Distance from scattering point to detector
    G4double fThetaMax; //Maximum angle for generator
    G4double fBeamEnergy; // beam energy
    G4double fBeamCurrent; //beam current
    G4double fSigmaY; // vertical angular spread
    G4double fSigmaX; //horizontal angular spread
    G4double fPhotonNumber; //number of scattered photons (average)
    G4double fPolarization; //number of scattered photons (average)
    G4int fRealPhotonNumber; //real scattered photon number in event
    G4ParticleDefinition * fElectron;
    G4ParticleDefinition * fGamma;


    void Init(void);
  private:
    G4ParticleGun*          fParticleGun; // G4 particle gun
    static PrimaryGeneratorAction* fgInstance;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
