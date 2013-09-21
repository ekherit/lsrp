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
/// \file example.cc
/// \brief Main program of the  example

#include "Config.h"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include "G4StepLimiterBuilder.hh"

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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Config_t Cfg;

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
    ("psm_width", po::value<double>(&Cfg.psm_width)->default_value(0.56), "Presampler width, cm")
    ("gem_width", po::value<double>(&Cfg.gem_width)->default_value(0.3), "GEM width, cm")
    ("psm_gem_length", po::value<double>(&Cfg.psm_gem_length)->default_value(1.0), "Distance between presampler and GEM, in cm")
    ("pad_size", po::value<double>(&Cfg.pad_size)->default_value(1), "hexagonal pad size, mm")
    ("photon_number", po::value<unsigned>(&Cfg.photon_number)->default_value(1), "Number of photons per pulse")
    ("output", po::value<std::string>(&Cfg.output_file)->default_value("tmp.root"), "Output file name")
    ("help", "Print this help")
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
    cout << "psm_width=" <<  Cfg.psm_width << endl;
    cout << "gem_width=" <<  Cfg.gem_width << endl;
    cout << "psm_gem_length=" <<  Cfg.psm_gem_length << endl;
    cout << "pad_size=" <<  Cfg.pad_size << endl;
    cout << "photon_number=" <<  Cfg.photon_number << endl;
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

  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  //set seed
  CLHEP::HepRandom::setTheSeed(time(0));
  
  // Construct the default run manager
  
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes

  runManager->SetUserInitialization(new DetectorConstruction());

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  //G4VModularPhysicsList * physicsList  = new QGSP_BERT_HP;
  physicsList->RegisterPhysics(new G4StepLimiterBuilder());
  runManager->SetUserInitialization(physicsList);
    
  // Set user action classes

  runManager->SetUserAction(new PrimaryGeneratorAction());
  runManager->SetUserAction(new RunAction());
  runManager->SetUserAction(new EventAction());
  
  // Initialize G4 kernel

  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
    }
  else
    {  // interactive mode : define UI session
#ifdef G4UI_USE
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute init_vis.mac");
#else
        UImanager->ApplyCommand("/control/execute init.mac");
#endif
      if (ui->IsGUI())
         UImanager->ApplyCommand("/control/execute gui.mac");
      ui->SessionStart();
      delete ui;
#endif
    }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
