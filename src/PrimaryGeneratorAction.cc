#include "PrimaryGeneratorAction.hh"
#include "ROOTManager.hh"
#include "DetectorConstruction.hh"
#include "Config.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
//#include "G4SystemOfUnits.hh"
#include "G4Electron.hh"

#include "Randomize.hh"
#include "PhysConstant.h"
extern GeneratorEvent makeGeneratorEvent(const ibn::phys::compton &c);

PrimaryGeneratorAction * PrimaryGeneratorAction::fgInstance = 0;

PrimaryGeneratorAction* PrimaryGeneratorAction::Instance(void)
{
  if (fgInstance == 0) fgInstance = new PrimaryGeneratorAction();
  return fgInstance;
}


PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
{
  fgInstance = this;
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic

  fGamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  //fElectron  = G4ParticleTable::GetParticleTable()->FindParticle("electron");
  fElectron = G4Electron::Definition();

  Init();

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::Init(void)
{
  G4cout << "Beam parameters: " << G4endl;
  //beam parameters
  fBeamEnergy = Cfg.beam.E;
  G4cout << "Beam energy: " << fBeamEnergy/GeV  << " GeV " << G4endl;
  fSigmaX = Cfg.beam.sigmaX;
  G4cout << "Beam radial angular spread: " << fSigmaX/mkrad << " mkrad (E=1.55 GeV)" << G4endl;
  fSigmaY = Cfg.beam.sigmaY;
  G4cout << "Beam vertical angular spread: " << fSigmaY/mkrad << " mkrad (E=1.55 GeV)" << G4endl;
  fSigmaX = Cfg.beam.sigmaX*fBeamEnergy/(1.55*GeV);
  G4cout << "Beam radial angular spread =  " << fSigmaX/mkrad << " mkrad (E="<<fBeamEnergy/GeV << " GeV)" << G4endl;
  fSigmaY = Cfg.beam.sigmaY*fBeamEnergy/(1.55*GeV);
  G4cout << "Beam vertical angular spread = " << fSigmaY/mkrad << " mkrad (E="<<fBeamEnergy/GeV << " GeV)" << G4endl;


  fBeamCurrent = Cfg.beam.I;
  G4cout << "Beam current: " << fBeamCurrent/mA  << " mA " << G4endl;
  G4double f0 = Cfg.beam.revolution_frequency; //Hz revolution frequency
  G4cout << "Beam revolution frequency: " << f0/kHz << " kHz" << G4endl;
  G4double Ne = fBeamCurrent/f0/eplus; // number of electrons in beam
  G4cout << "Beam number of elctrons = " << Ne << G4endl;

  //laser parameters
  G4double lambda = Cfg.laser.lambda; //in meters
  G4cout << "Laser wave length: " << Cfg.laser.lambda / micrometer << " mkm" << G4endl;
  G4double  laser_frequency = Cfg.laser.frequency;
  G4cout << "Laser frequency: " << laser_frequency/kHz << " kHz" << G4endl;
  G4double pulse_time = Cfg.laser.pulse_time; //sec
  G4cout << "Laser pulse time: " << pulse_time/ns << " ns" << G4endl;
  G4double pulse_energy = Cfg.laser.pulse_energy; //pulse energy, Joul
  G4cout << "Laser pulse energy: " << pulse_energy / mkJ << " mkJ" << G4endl;
  G4double pulse_length= pulse_time*(ibn::phys::c*m/s);
  G4cout << "Laser pulse length = " << pulse_length/m << " m" << G4endl;
  G4cout << "Laser focus length = " << Cfg.laser.focus_length/cm << " cm" << G4endl;
  G4double pulse_size = Cfg.laser.pulse_size; //m
  G4cout << "Laser pulse size = " << pulse_size/mm << " mm" << G4endl;
  G4double photon_energy = 2*M_PI*hc/lambda; 
  G4cout << "Laser photon energy = " << photon_energy/eV << " eV" << G4endl;// eV, should be 2.48 eV
  G4double Ngamma = pulse_energy/photon_energy;
  G4cout << "Laser photon number = " << Ngamma << G4endl;
  G4double ngamma = Ngamma/(pulse_length*pulse_size*pulse_size*M_PI/4.0);
  G4cout << "Laser photon density = " << ngamma*(cm*cm*cm) << " cm^-3" <<G4endl;

  //const double hc = 197.326968e-15; // MeV*m;
  G4cout << "Scattering parameters: " << G4endl;
  //Ymax should be smaller than detector size in order to supress outside world volume hit
  fFlightLength = Cfg.photon_flight_length;
  G4cout << "Photon flight length: " << fFlightLength/m << " m" << G4endl;
  G4double Ymax = 0.49*(Cfg.converter.size*mm/2.);
  G4cout << "\tYmax = " << fFlightLength*fThetaMax/cm << " cm" << G4endl;
  fThetaMax = Ymax/fFlightLength;
  G4cout << "\tfThetaMax = " << fThetaMax/mrad << " mrad"  << G4endl;
  G4cout << "\tCharacteristic spot size on detector: " << G4endl;
  G4cout << "\t\tsize_x = " << 2*hypot(fSigmaX*fFlightLength, 0.511*MeV/fBeamEnergy)/mm << " mm" << G4endl;
  G4cout << "\t\tsize_y = " << 2*hypot(fSigmaY*fFlightLength,0.511*MeV/fBeamEnergy)/mm << " mm" << G4endl;
  fCompton.reset(new ibn::phys::compton(fBeamEnergy/MeV,photon_energy/MeV,0,0));
  double cos_rf = fCompton->cos_rf(cos(fThetaMax));
  double xmax = fCompton->xcos(cos_rf);
  double xmin = 1./(1.0+2.0*fCompton->chi);
  G4cout << "\txmin = " << xmin << G4endl;
  G4cout << "\txmax = " << xmax << G4endl;
  //double Integral  = ibn::dgaus(*fCompton,xmin,xmax,1e-13);
  //G4cout << "\tIntegral = " << Integral << endl;
  //G4double sigma = M_PI*ibn::sq(ibn::phys::r_e)/fCompton->chi*Integral*m*m; //m^2
  G4double sigma = 0.65e-24*cm*cm;
  G4cout << "\tsigma = " << sigma/(cm*cm) << " cm^2" << " (" << sigma/barn << " b)"<<G4endl;
  G4double nu = sigma*(ibn::phys::c*m/s)*Ne*ngamma;
  G4cout << "\tRate = " << nu/GHz << " GHz" << G4endl;
  fPhotonNumber = nu*Cfg.laser.focus_length/pulse_length*Cfg.laser.pulse_time;
  G4cout << "Scattered photon number = " << fPhotonNumber<< " per pulse" << G4endl; //двойка из-за встречного движения пучка
  G4double rate = fPhotonNumber*laser_frequency;
  G4cout << "Scattered photon number which hit detector = " << rate/kHz <<" kHz" <<  G4endl;
  if(Cfg.photon_number != 0 ) 
  {
    fPhotonNumber = Cfg.photon_number;
    G4cout << "Redefine photon number by config: " << fPhotonNumber << G4endl;
  }
  fCompton.reset(new ibn::phys::compton(fBeamEnergy/MeV,photon_energy/MeV,1,1));
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
  long eventID=anEvent->GetEventID();
  fParticleGun->SetParticleDefinition(fGamma);
  // This function is called at the begining of event
  fPolarization = 2.0*(eventID%2)-1.0; //calculate polarization
  fCompton->SetPhotonPolarization(fPolarization);
  //G4cout << fPhotonNumber << G4endl;
  fRealPhotonNumber = CLHEP::RandPoissonQ::shoot(fPhotonNumber);
  auto & IP =  DetectorConstruction::Instance()->GetInteractionPointPosition();
  for(int i=0;i<fRealPhotonNumber;i++)
  {
    //generate compton backscattering
    fCompton->generate([](double xmin, double xmax) { return (xmax-xmin)*G4UniformRand()+xmin; },fThetaMax);
    variate_agnle(*fCompton,fSigmaX,fSigmaY); 
    G4ThreeVector k(fCompton->kx, fCompton->ky, fCompton->kz);
    fParticleGun->SetParticleEnergy(fCompton->E*MeV);
    fParticleGun->SetParticlePosition(IP);
    fParticleGun->SetParticleMomentumDirection(k);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    GeneratorEvent gevent= makeGeneratorEvent(*fCompton);
    gevent.x = IP.x()/mm;
    gevent.y = IP.y()/mm;
    gevent.z = IP.z()/mm;
    gevent.npad=0;
    ROOTManager::Instance()->event.gen.push_back(gevent);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
