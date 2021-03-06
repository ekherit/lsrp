#ifndef GEMHit_h
#define GEMHit_h 1

#include "Config.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "Pad.hh"

#include <list>

class GEMHit : public G4VHit
{
  public:
    GEMHit();
    GEMHit(const GEMHit&);
    virtual ~GEMHit();

    // operators
    const GEMHit& operator=(const GEMHit&);
    G4int operator==(const GEMHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID  (G4int track)      { fTrackID = track; };
    void SetOriginalTrackID  (G4int track)      { fOriginalTrackID = track; };
    void SetParticleID  (G4int pid)      { fParticleID = pid; };
    void SetVolumeID (G4int volume)      { fVolumeID = volume; };
    void SetEdep     (G4double de)      { fEdep = de; };
    void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
    void SetMomentum (const G4ThreeVector & k ) { fMomentum = k;}
    void SetCharge   (G4double charge)  { fCharge = charge; fPad.charge=fCharge; }
    void FindPad(void) 
    { 
      fPad = Pad(fPos.x(), fPos.y());
      fPad.charge=fCharge;
    }
    void SetPads(const std::list<Pad> & pads ) { fPads = pads;}

    // Get methods
    G4int GetTrackID() const     { return fTrackID; };
    G4int GetOriginalTrackID() const     { return fOriginalTrackID; };
    G4int GetParticleID() const     { return fParticleID; };
    G4int GetVolumeID() const   { return fVolumeID; };
    G4double GetEdep() const     { return fEdep; };
    G4double GetCharge() const   { return fCharge;};
    G4ThreeVector GetPos() const { return fPos; };
    G4ThreeVector GetMomentum() const { return fMomentum; };
    const Pad & GetPad(void) const { return fPad; };
    const std::list<Pad> & GetPads(void) const { return fPads; };


  private:

      G4int         fTrackID;
      G4int         fOriginalTrackID;
      G4int         fParticleID;
      G4int         fVolumeID;
      G4double      fEdep;
      G4ThreeVector fPos;
      G4ThreeVector fMomentum; 
      G4double      fCharge;
      Pad           fPad;
      std::list<Pad> fPads; // list of pads
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<GEMHit> GEMHitsCollection;

extern G4Allocator<GEMHit> GEMHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* GEMHit::operator new(size_t)
{
  void *hit;
  hit = (void *) GEMHitAllocator.MallocSingle();
  return hit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void GEMHit::operator delete(void *hit)
{
  GEMHitAllocator.FreeSingle((GEMHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
