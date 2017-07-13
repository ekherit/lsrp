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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class

#include <typeinfo>

#include <boost/algorithm/string.hpp>

#include "ROOTManager.hh"
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "Config.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"

#include "G4UnitsTable.hh"

#include "UnitDefinition.hh"


DetectorMessenger * DetectorMessenger::fgInstance = nullptr;

DetectorMessenger* DetectorMessenger::Instance(void)
{
    if( fgInstance == nullptr ) new DetectorMessenger();
    return fgInstance;
}
 
double test_parameter;
DetectorMessenger::DetectorMessenger(void) : G4UImessenger()
{
  fgInstance = this;
  DefineUnits();
  fDirectory.reset(new G4UIdirectory("/lsrp/"));
  fDirectory->SetGuidance("UI commands for Laser Polarimeter Simulation");

  AddCmd(Cfg.root_file                     , "/lsrp/RootFile"                , "ROOT file with simulation result");
  AddCmd(Cfg.root.auto_generate_root_files , "/lsrp/AutoRootFile"            , "Automaticaly create root file number with newnumber");

  AddCmd(Cfg.photon_number                 , "/lsrp/PhotonNumberPerPulse"    , "Number of photons per pulse"                           , "");
  AddCmd(Cfg.step_max                      , "/lsrp/StepMax"                 , "Miximum allowed step"                                  , "Length");
  AddCmd(Cfg.gem.pad.size                  , "/lsrp/GEM/pad/Size"            , "Size of the pad"                                       , "Length");
  AddCmd(Cfg.gem.pad.size_x                , "/lsrp/GEM/pad/SizeX"           , "X size of the pad"                                     , "Length");
  AddCmd(Cfg.gem.pad.size_y                , "/lsrp/GEM/pad/SizeY"           , "Y size of the pad"                                     , "Length");
  AddCmd(Cfg.gem.pad.rough_size_x          , "/lsrp/GEM/pad/RoughSizeX"      , "Big pad size x"                                        , "Length");
  AddCmd(Cfg.gem.pad.rough_size_y          , "/lsrp/GEM/pad/RoughSizeY"      , "Big pad size y"                                        , "Length");
  AddCmd(Cfg.gem.high_sens_size_x          , "/lsrp/GEM/HighSensSizeX"       , "X High sensitive width"                                , "Length");
  AddCmd(Cfg.gem.high_sens_size_y          , "/lsrp/GEM/HighSensSizeY"       , "Y High sensitive width"                                , "Length");
  AddCmd(Cfg.beam.sigmaX                   , "/lsrp/beam/SigmaX"             , "Beam X angular spread"                                 , "Angle");
  AddCmd(Cfg.beam.sigmaY                   , "/lsrp/beam/SigmaY"             , "Beam Y angular spread"                                 , "Angle");
  AddCmd(Cfg.beam.I                        , "/lsrp/beam/Current"            , "Beam current"                                          , "Electric current");
  AddCmd(Cfg.beam.E                        , "/lsrp/beam/Energy"             , "Beam energy"                                           , "Energy");
  AddCmd(Cfg.beam.x                        , "/lsrp/beam/x"                  , "Bunch X positon"                                       , "Length");
  AddCmd(Cfg.beam.y                        , "/lsrp/beam/y"                  , "Bunch Y positon"                                       , "Length");
  AddCmd(Cfg.laser.lambda                  , "/lsrp/laser/WaveLength"        , "Laser wave length"                                     , "Length");
  AddCmd(Cfg.laser.pulse_energy            , "/lsrp/laser/PulseEnergy"       , "Laser pulse energy"                                    , "Energy");
  AddCmd(Cfg.laser.pulse_time              , "/lsrp/laser/PulseTime"         , "Laser pulse time"                                      , "Time");
  AddCmd(Cfg.laser.frequency               , "/lsrp/laser/PulseFrequency"    , "Laser pulse frequency"                                 , "Frequency");
  AddCmd(Cfg.laser.pulse_size              , "/lsrp/laser/PulseSize"         , "Laser pulse size"                                      , "Length");

  AddCmd(Cfg.world_size_x                  , "/lsrp/World/SizeX"             , "World size x"                                          , "Length");
  AddCmd(Cfg.world_size_y                  , "/lsrp/World/SizeY"             , "World size y"                                          , "Length");
  AddCmd(Cfg.world_size_z                  , "/lsrp/World/SizeZ"             , "World size z"                                          , "Length");

  AddCmd(Cfg.gem.size                      , "/lsrp/GEM/Size"                , "GEM size"                                              , "Length");
  AddCmd(Cfg.gem.size_x                    , "/lsrp/GEM/SizeX"               , "GEM size x"                                            , "Length");
  AddCmd(Cfg.gem.size_x                    , "/lsrp/GEM/SizeY"               , "GEM size y"                                            , "Length");
  AddCmd(Cfg.gem_world_distance            , "/lsrp/GEM/DistanceToWorldEdge" , "GEM-world distance"                                    , "Length");

  AddCmd(Cfg.converter.width               , "/lsrp/Converter/Width"         , "Converter width"                                       , "Length");
  AddCmd(Cfg.converter.size                , "/lsrp/Converter/Size"          , "Converter size xy"                                     , "Length");

  AddCmd(Cfg.converter_gem_distance        , "/lsrp/Converter/DistanceToGEM" , "Converter - GEM distance"                              , "Length");

  AddCmd(Cfg.flange_gem_distance           , "/lsrp/Flange/DistanceToGEM"    , "The distance from flange to GEM detector"              , "Length");
  AddCmd(Cfg.flange.width                  , "/lsrp/Flange/Width"            , "Flange width"                                          , "Length");

  AddCmd(Cfg.mirror_flange_distance        , "/lsrp/MirrorFlangeDistance"    , "Mirror - flange distance"                              , "Length");
  AddCmd(Cfg.mirror.width                  , "/lsrp/Mirror/Width"            , "Mirror width"                                          , "Length");
  AddCmd(Cfg.mirror.size_x                 , "/lsrp/Mirror/SizeX"            , "Mirror size x"                                         , "Length");
  AddCmd(Cfg.mirror.size_y                 , "/lsrp/Mirror/SizeY"            , "Mirror size y"                                         , "Length");
  AddCmd(Cfg.flange.size                   , "/lsrp/VacuumChamber/Size"      , "Vacuum chamber size"                                   , "Length");
  AddCmd(Cfg.photon_flight_length          , "/lsrp/PhotonFlightLength"      , "Photon Flight length"                                  , "Length");

  AddCmd(Cfg.world_material                , "/lsrp/World/Material"          , "World material");
  AddCmd(Cfg.converter.material            , "/lsrp/Converter/Material"      , "Converter material");
  AddCmd(Cfg.mirror.material               , "/lsrp/Mirror/Material"         , "Mirror material");
  AddCmd(Cfg.flange.material               , "/lsrp/Flange/Material"         , "Flange material");
  AddCmd(Cfg.vacuum_chamber_material       , "/lsrp/VacuumChamber/Material"  , "Vacuum chamber material");
  AddCmd(Cfg.root.save_hits                , "/lsrp/SaveAllHits"             , "Save all hits");
  AddCmd(Cfg.root.one_pad_per_track        , "/lsrp/SaveOnePadPerTrack"      , "Save only one pad per track");
  AddCmd(Cfg.gem.pad.threshold             , "/lsrp/GEM/pad/Threshold"       , "The pad threshold");
}


DetectorMessenger::~DetectorMessenger() { }

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  auto p = fCmdMap.find(command);
  if (  p!=fCmdMap.end() ) 
  {
    if ( typeid(*command) == typeid(G4UIcmdWithADoubleAndUnit) ) 
    {
      auto data = *boost::get<double*>(p->second);
      data = static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue);
      std::cout << command->GetCommandPath() << " = " <<  data  << " (" << newValue << ")\n";
    }
    if ( typeid(*command) == typeid(G4UIcmdWithADouble) ) 
    {
      auto & data = *boost::get<double*>(p->second);
      data = static_cast<G4UIcmdWithADouble*>(command)->GetNewDoubleValue(newValue);
      std::cout << command->GetCommandPath() << " = " <<  data  << " (" << newValue << ")\n";
    }
    if ( typeid(*command) == typeid(G4UIcmdWithAnInteger) ) 
    {
      auto & data = *boost::get<int*>(p->second);
      data = static_cast<G4UIcmdWithAnInteger*>(command)->GetNewIntValue(newValue);
      std::cout << command->GetCommandPath() << " = " <<  data  << " (" << newValue << ")\n";
    }
    if ( typeid(*command) == typeid(G4UIcmdWithAString) ) 
    {
      auto & data = *boost::get<std::string*>(p->second);
      data = newValue;
      std::cout << command->GetCommandPath() << " = " <<  data  << " (" << newValue << ")\n";
    }
  }
}


template<typename Type>
void DetectorMessenger::AddCmd(Type & par, const std::string & name, const std::string & title, const std::string unit)
{
  std::vector<std::string> nms;
  boost::split(nms,name,boost::is_any_of("/")); //find parameter name (without dirs)
  const char * parname =  nms.back().c_str();
  G4UIcommand * cmd = nullptr;
  if( typeid(Type) == typeid(double) )
  {
    if(unit.length() != 0) //Has the unit information
    {
      auto c = new G4UIcmdWithADoubleAndUnit(name.c_str(), this); //create a command
      c->SetParameterName(parname, false); //set parameter name
      c->SetUnitCategory(unit.c_str()); //set units
      cmd = static_cast<G4UIcommand*>(c);
    }
    else
    {
      auto c = new G4UIcmdWithADouble(name.c_str(), this); //create a command
      boost::split(nms,name,boost::is_any_of("/")); //find parameter name (without dirs)
      c->SetParameterName(parname, false); //set parameter name
      cmd = static_cast<G4UIcommand*>(c);
    }
  }
  if( typeid(Type) == typeid(int) )
  {
    auto c = new G4UIcmdWithAnInteger(name.c_str(), this); //create a command
    c->SetParameterName(parname, false); //set parameter name
    cmd = static_cast<G4UIcommand*>(c);
  }
  if( typeid(Type) == typeid(std::string) )
  {
    auto c = new G4UIcmdWithAString(name.c_str(), this); //create a command
    c->SetParameterName(parname, false); //set parameter name
    cmd = static_cast<G4UIcommand*>(c);
  }
  cmd->SetGuidance(title.c_str()); //set description
  cmd->AvailableForStates(G4State_Idle); 
  fCmdMap[cmd] = & par;
}
