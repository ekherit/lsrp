#include "Config.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "ROOTManager.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BERT_HP.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <boost/program_options.hpp>

#include <regex>
#include <cstring>
#include <ctime>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void clean_argc_argv(int * ARGC, char ** ARGV, boost::program_options::variables_map & opt)
{
  int argc=0;
  char ** argv = new char*[*ARGC];
  for(int i=0; i<*ARGC;i++)
  {
    string s = ARGV[i];
    bool ismatched=false;
    for(auto & o : opt)
    {
      regex r(".*"+o.first+".*");
      std::smatch match;
      if(std::regex_match(s,match,r)) 
      {
        ismatched=true;
      }
    }
    if(!ismatched)
    {
      argv[argc] = new char[s.size()+1];
      strncpy(argv[argc], s.c_str(),s.size()+1);
      argc++;
    }
  }
  //for(int i=0;i<*ARGC;i++) delete[] ARGV[i];
  *ARGC = argc;
  for(int i=0;i<*ARGC;i++)
  {
    ARGV[i] = argv[i];
  }
}
int main(int argc,char** argv)
{
  namespace po=boost::program_options;
  po::options_description opt_desc("Allowed options");
  opt_desc.add_options()
    ("help", "Print this help")
    ("seed",po::value<unsigned long>(&Cfg.seed),"Seed of the engine")
    ;
  po::positional_options_description pos;
  //pos.add("output",-1);
  po::variables_map opt; //options container
  try
  {
    //po::store(po::command_line_parser(argc, argv).options(opt_desc).positional(pos).run(), opt);
    po::store(parse_command_line(argc, argv, opt_desc), opt);
    //std::ifstream config_file("fit.cfg");
    //po::store(po::parse_config_file(config_file,opt_desc,true), opt);
    po::notify(opt);
    if(opt.count("help"))
    {
      std::clog << opt_desc;
      return 0;
    }
  } 
  catch (boost::program_options::unknown_option & o)
  {
  }
  catch (boost::program_options::error & po_error)
  {
    cerr << "WARGNING: configuration: "<< po_error.what() << endl;
    exit(0);
  }
  //remove my own program options
  clean_argc_argv(&argc, argv, opt);
  for(int i=0;i<argc;i++)
  {
    cout << i << ": " << argv[i] << endl;
  }

  std::ofstream mem_usage("mem_usage.txt");
  time_t begin_time = time(0);
  mem_usage << "lsrp begintime (" << begin_time << ") " << ctime(&begin_time); 
  mem_usage.close();
  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  if(Cfg.seed==0) Cfg.seed = begin_time;
  G4cout << "Random seed: " << Cfg.seed << G4endl;
  CLHEP::HepRandom::setTheSeed(Cfg.seed);
  
  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  auto detector_construction = new DetectorConstruction();
  runManager->SetUserInitialization(detector_construction);

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  runManager->SetUserInitialization(physicsList);
    
  // Set user action classes
  runManager->SetUserAction(new PrimaryGeneratorAction());
  runManager->SetUserAction(new RunAction());
  runManager->SetUserAction(new EventAction());
  runManager->SetUserAction(new TrackingAction());
  
  // Initialize G4 kernel
  runManager->Initialize();

  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  G4String command = "/control/execute ";
  G4String fileName = argv[1];
  UImanager->ApplyCommand(command+fileName);


  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
#ifdef G4VIS_USE
  delete visManager;
#else
  delete runManager;
#endif

  delete  ROOTManager::Instance(); //in order to remove TFile before ROOT garbage collection start
  return 0;
}
