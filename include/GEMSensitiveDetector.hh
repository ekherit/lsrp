#ifndef GEMSensitiveDetector_h
#define GEMSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"

#include "GEMHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Tracker sensitive detector class
///
/// In Initialize(), it creates one hit for each tracker volume
///
/// The values are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step.

class GEMSensitiveDetector : public G4VSensitiveDetector
{
  public:
    GEMSensitiveDetector(const G4String& name, 
                const G4String& hitsCollectionName);
    virtual ~GEMSensitiveDetector();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
    void SetPadZ(G4double z) { fPadZPosition = z; }
    G4double GetAmplification(int volume);

  private:
    GEMHitsCollection* fHitsCollection;
    G4double fPadZPosition;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
