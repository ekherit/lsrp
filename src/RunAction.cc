#include "RunAction.hh"
#include "ROOTManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "Config.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include <boost/filesystem.hpp>

#include "Utils.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  Cfg.run = aRun->GetRunID();

  //inform the runManager to save random number seed

  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  /////Before new run we have to update output file and detector geometry
  std::string file_prefix = Cfg.root_file;
  namespace  fs = boost::filesystem;
  fs::path path(Cfg.root_file.c_str());
  if ( Cfg.root.auto_generate_root_files == "yes" ) 
  {
    auto file = path.stem();
    char suffix[128];
    sprintf(suffix,"-%03ld",Cfg.run);
    file+=suffix;
    file+=".root";
    path = file;
  }
  ROOTManager::Instance()->SetRootFile(path.c_str());
  DetectorConstruction::Instance()->UpdateGeometry(); 
  PrimaryGeneratorAction::Instance()->Init();
  print_memory_usage(aRun->GetRunID());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//ibn::valer<double> get_epsilon(Long64_t N1, Long64_t N2)
//{
//  ibn::valer<double> eps;
//  eps.value = double(N1-N2)/double(N1+N2);
//  eps.error = sqrt(ibn::sq(2.0*N2/ibn::sq(double(N1+N2)))*N1 + ibn::sq(2.0*N1/ibn::sq(double(N1+N2)))*N2);
//  return eps;
//}

void RunAction::EndOfRunAction(const G4Run*)
{
  ROOTManager::Instance()->Write();
  //if(RM->tree !=nullptr) RM->tree->Write();
  //Long64_t Nup0 = RM->gen_tree->GetEntries("y>0&&P>0") + RM->gen_tree->GetEntries("y<0&&P<0");
  //Long64_t Ndown0 = RM->gen_tree->GetEntries("y<0&&P>0")+ RM->gen_tree->GetEntries("y>0&&P<0");
  //ibn::valer<double> eps0 = get_epsilon(Nup0,Ndown0);
  //Long64_t Nup = RM->tree->GetEntries("hit.y>0 && volumeID==1&& P>0") +  RM->tree->GetEntries("hit.y<0 && volumeID==1&& P<0");
  //Long64_t Ndown = RM->tree->GetEntries("hit.y<0 && volumeID==1 && P>0") +  RM->tree->GetEntries("hit.y>0 && volumeID==1 && P<0");
  //ibn::valer<double> eps = get_epsilon(Nup,Ndown);
  //RM->gen_tree->Write();
  //G4cout << "Initial assymetry: " << eps0.value << "+-" << eps0.error << " ("<< eps0.error/eps0.value*100<< "%)" << G4endl;
  //G4cout << "Gem assymetry:     " << eps.value << "+-" << eps.error << " ("<< eps.error/eps.value*100<< "%)" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
