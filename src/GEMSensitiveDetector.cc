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
   fHitsCollection(nullptr)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GEMSensitiveDetector::~GEMSensitiveDetector() 
{
  std::cout <<"GEMSensitiveDetector::~GEMSensitiveDetector " << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection = new GEMHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce
  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  GetHitPadList
 *  Description:  Spread the total charge over many pads by using monte carlo
 * =====================================================================================
 */
std::list<Pad> GetHitPadList(G4ThreeVector hit_pos, double total_charge, double spot_size, unsigned N=400)
{
  std::list<Pad> Pads;
  for(unsigned i=0;i<N;i++)
  {
    G4double x = G4RandGauss::shoot(hit_pos.x(), spot_size);
    G4double y = G4RandGauss::shoot(hit_pos.y(), spot_size);
    Pad pad(x,y);
    auto p = std::find(std::begin(Pads),std::end(Pads), pad);
    if(p==Pads.end()) 
    {
      //new pad
      Pads.push_back(pad);
    }
    else
    {
      //alrady hit
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
  int volid = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
  // energy deposit in the volume
  G4double Edep = aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit();
  if(Edep<=0) return false;
  //G4double charge = edep/(85.7*eV);
  G4double charge = Edep/(26*eV);
  G4ThreeVector r = aStep->GetPostStepPoint()->GetPosition(); //hit position 

  GEMHit* newHit = new GEMHit(); //create a hit
  newHit->SetEdep(Edep);
  newHit->SetPos(r);

  auto track = aStep->GetTrack();
  newHit->SetMomentum(track->GetVertexMomentumDirection());

  TrackInformation* info = (TrackInformation*)(track->GetUserInformation());
  newHit->SetTrackID  (track->GetTrackID());
  newHit->SetOriginalTrackID (info->GetOriginalTrackID());
  newHit->SetParticleID(track->GetParticleDefinition()->GetPDGEncoding());
  newHit->SetVolumeID(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  
  newHit->FindPad(); //get pad which correspond the hit (first approximation without drift and spread

  newHit->SetCharge(charge*GetAmplification(newHit->GetVolumeID()));
  //std::cout << " charge = " << charge << " Ampl charge = " << newHit->GetCharge() <<  "  Edep = " << Edep/keV << "   total = " << aStep->GetTotalEnergyDeposit()/keV << "   nonionazing = " << aStep->GetNonIonizingEnergyDeposit() <<std::endl;

  std::list<Pad> pad_list;
  if(Cfg.gem.drift_spread)
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

  /*  
   *  As I have seen in the G4THitCoolections code. Hit Collection is realized through the vector.  
   *  It delete it's content in destructor. So I dont need to delete GEMHit by myself
   */
  fHitsCollection->insert( newHit );
  return true;
}

G4double GEMSensitiveDetector::GetAmplification(int volume_id)
{
  double K0 = Cfg.gem.amplification; //maximum amplification coeff
  double Nc = Cfg.gem.cascade_number; //number amplif cascades
  //amplification for the volume_id
  //0 : K0
  //1 : K0^{2/3}
  //2 : K0^{1/3}
  double K =  pow(K0, (Nc-volume_id)/Nc); 
  return K;
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GEMSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 )
  { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
  //delete fHitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
