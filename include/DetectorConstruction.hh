#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include <memory>

class G4LogicalVolume;
class G4VSolid;
class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;

class DetectorMessenger;
class GEMSensitiveDetector;

#include "GEMDetector.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

    // Set methods
    void SetMaxStep (G4double );
    void SetCheckOverlaps(G4bool );
    void UpdateGeometry(void);
    void CalculateGeometry(void);
    static DetectorConstruction * Instance();

    //G4double presampler_front_position;
    const G4ThreeVector  & GetGEMFrontPosition(void) const 
    { 
        return  fGEMFrontPosition;
    }

    const G4ThreeVector  & GetInteractionPointPosition(void) const 
    { 
        return  fInteractionPointPosition;
    }

  private:
    // methods
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    std::unique_ptr<GEMDetector> GEM;
    G4VPhysicalVolume * fGem  = nullptr;
    GEMSensitiveDetector * fGEMSensitiveDetector  = nullptr;

    G4NistManager* nistManager = G4NistManager::Instance();
    
    struct VolumeItem_t
    {
        int shape;//box or tube
        std::shared_ptr<G4VSolid> solid;
        std::shared_ptr<G4LogicalVolume> logic;
        std::shared_ptr<G4VPhysicalVolume> phys;
        void update_geometry(double size_x, double size_y, double size_z, G4ThreeVector pos);
        void open_geometry(void);
        void close_geometry(void);
    };

    std::map<std::string, VolumeItem_t> fVol; //list of all volumes

    G4Material *     fConverterMaterial = nullptr;

    G4UserLimits* fStepLimit = nullptr;            // pointer to user step limits

    DetectorMessenger*  fMessenger;   // messenger
    
    G4bool  fCheckOverlaps  = false; // option to activate checking of volumes overlaps 
    static DetectorConstruction * fgInstance;

    G4double fVacuumChamberLength; 

    //new infrastructure
    G4ThreeVector fGEMPosition;
    G4ThreeVector fGEMFrontPosition;
    G4ThreeVector fGEMPadPosition;

    G4ThreeVector fConverterPosition;
    G4ThreeVector fFlangePosition;
    G4ThreeVector fMirrorPosition;
    G4ThreeVector fVacuumChamberPosition;
    G4ThreeVector fInteractionPointPosition;
    G4ThreeVector fSensBeforeConverterPosition;


  private:
    enum { BOX, TUBE};
    void MakeVolume(
            std::string name, 
            G4Material * material,
            int shape,
            double size_x, 
            double size_y, 
            double size_z, 
            std::string mother_name = "",
            G4ThreeVector position = {0,0,0},
            G4RotationMatrix * rotation_matrix = nullptr,
            int copy_number = 0);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
