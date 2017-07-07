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

#include <boost/algorithm/string.hpp>

#include "ROOTManager.hh"
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "Config.h"

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
DetectorMessenger::DetectorMessenger(void)
 : G4UImessenger()
{
  fgInstance = this;
  DefineUnits();
  fDirectory.reset(new G4UIdirectory("/lsrp/"));
  fDirectory->SetGuidance("UI commands for Laser Polarimeter Simulation");

  fRootFileCmd.reset(new G4UIcmdWithAString("/lsrp/RootFile",this));
  fRootFileCmd->SetGuidance("Select output ROOT file");
  fRootFileCmd->SetParameterName("choice",false);
  fRootFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  AddCmdDouble(Cfg.photon_number, "/lsrp/PhotonNumberPerPulse","Number of photons per pulse", "");
  AddCmdDouble(Cfg.step_max, "/lsrp/StepMax","Miximum allowed step", "Length");
  AddCmdDouble(Cfg.pad_size,   "/lsrp/GEM/pad/Size","Size of the pad", "Length");
  AddCmdDouble(Cfg.pad_xsize, "/lsrp/GEM/pad/SizeX","X size of the pad", "Length");
  AddCmdDouble(Cfg.pad_ysize, "/lsrp/GEM/pad/SizeY","Y size of the pad", "Length");
  AddCmdDouble(Cfg.pad_rough_size_x, "/lsrp/GEM/pad/RoughSizeX","Big pad size x","Length");
  AddCmdDouble(Cfg.pad_rough_size_y, "/lsrp/GEM/pad/RoughSizeY","Big pad size y", "Length");
  AddCmdDouble(Cfg.pad_high_sens_xwidth, "/lsrp/GEM/HighSensSizeX","X High sensitive width", "Length");
  AddCmdDouble(Cfg.pad_high_sens_ywidth, "/lsrp/GEM/HighSensSizeY","Y High sensitive width","Length");
  AddCmdDouble(Cfg.beam.sigmaX, "/lsrp/beam/SigmaX","Beam X angular spread", "Angle");
  AddCmdDouble(Cfg.beam.sigmaY, "/lsrp/beam/SigmaY","Beam Y angular spread", "Angle");
  AddCmdDouble(Cfg.beam.I, "/lsrp/beam/Current","Beam current", "Electric current");
  AddCmdDouble(Cfg.beam.E, "/lsrp/beam/Energy","Beam energy", "Energy");
  AddCmdDouble(Cfg.beam.x, "/lsrp/beam/x","Bunch X positon", "Length");
  AddCmdDouble(Cfg.beam.y, "/lsrp/beam/y","Bunch Y positon", "Length");
  AddCmdDouble(Cfg.laser.lambda, "/lsrp/laser/WaveLength","Laser wave length", "Length");
  AddCmdDouble(Cfg.laser.pulse_energy, "/lsrp/laser/PulseEnergy","Laser pulse energy", "Energy");
  AddCmdDouble(Cfg.laser.pulse_time, "/lsrp/laser/PulseTime","Laser pulse time", "Time");
  AddCmdDouble(Cfg.laser.frequency, "/lsrp/laser/PulseFrequency","Laser pulse frequency", "Frequency");
  AddCmdDouble(Cfg.laser.pulse_size, "/lsrp/laser/PulseSize","Laser pulse size", "Length");

  AddCmdDouble(Cfg.world_size_x, "/lsrp/World/SizeX","World size x", "Length");
  AddCmdDouble(Cfg.world_size_y, "/lsrp/World/SizeY","World size y", "Length");
  AddCmdDouble(Cfg.world_size_z, "/lsrp/World/SizeZ","World size z", "Length");

  AddCmdDouble(Cfg.gem_size, "/lsrp/GEM/Size","GEM size", "Length");
  AddCmdDouble(Cfg.gem_size_x, "/lsrp/GEM/SizeX","GEM size x", "Length");
  AddCmdDouble(Cfg.gem_size_x, "/lsrp/GEM/SizeY","GEM size y", "Length");
  AddCmdDouble(Cfg.gem_world_distance, "/lsrp/GEM/DistanceToWorldEdge","GEM-world distance", "Length");

  AddCmdDouble(Cfg.converter_width, "/lsrp/Converter/Width","Converter width", "Length");
  AddCmdDouble(Cfg.converter_size,   "/lsrp/Converter/Size","Converter size (x,y)", "Length");

  AddCmdDouble(Cfg.converter_gem_distance, "/lsrp/Converter/DistanceToGEM","Converter - GEM distance", "Length");

  AddCmdDouble(Cfg.flange_gem_distance, "/lsrp/Flange/DistanceToGEM","The distance from flange to GEM detector", "Length");
  AddCmdDouble(Cfg.flange_width, "/lsrp/Flange/Width","Flange width", "Length");

  AddCmdDouble(Cfg.mirror_flange_distance, "/lsrp/MirrorFlangeDistance","Mirror - flange distance", "Length");
  AddCmdDouble(Cfg.mirror_width, "/lsrp/Mirror/Width","Mirror width",   "Length");
  AddCmdDouble(Cfg.mirror_size_x, "/lsrp/Mirror/SizeX","Mirror size x", "Length");
  AddCmdDouble(Cfg.mirror_size_y, "/lsrp/Mirror/SizeY","Mirror size y", "Length");
  AddCmdDouble(Cfg.vacuum_chamber_size, "/lsrp/VacuumChamber/Size","Vacuum chamber size", "Length");
  AddCmdDouble(Cfg.photon_flight_length, "/lsrp/PhotonFlightLength","Photon Flight length", "Length");

  AddCmdString(Cfg.world_material, "/lsrp/World/Material","World material");
  AddCmdString(Cfg.converter_material, "/lsrp/Converter/Material","Converter material");
  AddCmdString(Cfg.mirror_material, "/lsrp/Mirror/Material","Mirror material");
  AddCmdString(Cfg.flange_material, "/lsrp/Flange/Material","Flange material");
  AddCmdString(Cfg.vacuum_chamber_material, "/lsrp/VacuumChamber/Material","Vacuum chamber material");
}


DetectorMessenger::~DetectorMessenger() { }

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  try
  {
    auto p = fCmdMapDouble.find(command);
    if (  p!=fCmdMapDouble.end() ) 
    {
        *p->second.data = static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue);
        std::cout << "newValue = " << newValue << " " << *p->second.data << "  for name = " << p->second.name << std::endl;
    }
  }
  catch(...)
  {
  }

  try
  {
    auto p = fCmdMapString.find(command);
    if (  p!=fCmdMapString.end() ) 
    {
        *p->second.data = newValue;
        std::cout << "newValue = " << newValue << " " << *p->second.data << "  for name = " << p->second.name << std::endl;
    }
  }
  catch(...)
  {
  }

  if( command == fRootFileCmd.get() )
  { 
    ROOTManager::Instance()->SetRootFile(newValue);
  }
}

void DetectorMessenger::AddCmdDouble(double & par, const std::string & name, const std::string & title, const  std::string & unit)
{
    auto command = new G4UIcmdWithADoubleAndUnit(name.c_str(), this); //create a command
    command->SetGuidance(title.c_str()); //set description
    std::vector<std::string> nms;
    boost::split(nms,name,boost::is_any_of("/")); //find parameter name (without dirs)
    command->SetParameterName(nms.back().c_str(), false); //set parameter name
    command->SetUnitCategory(unit.c_str()); //set units
    command->AvailableForStates(G4State_Idle); //???
    fCmdMapDouble.emplace //register in the list of known command
        (
            std::make_pair<G4UIcommand*,  CmdItem_t<double> > 
            (
                static_cast<G4UIcommand*>(command),
                {std::unique_ptr<G4UIcommand>(command), &par, name}
            )
        ); 
}

void DetectorMessenger::AddCmdString(std::string & par, const std::string & name, const std::string & title)
{
    auto command = new G4UIcmdWithAString(name.c_str(), this); //create a command
    command->SetGuidance(title.c_str()); //set description
    std::vector<std::string> nms;
    boost::split(nms,name,boost::is_any_of("/")); //find parameter name (without dirs)
    command->SetParameterName(nms.back().c_str(), false); //set parameter name
    //command->SetUnitCategory(unit.c_str()); //set units
    command->AvailableForStates(G4State_Idle); //???
    fCmdMapString.emplace //register in the list of known command
        (
            std::make_pair<G4UIcommand*,  CmdItem_t<std::string> > 
            (
                static_cast<G4UIcommand*>(command),
                {std::unique_ptr<G4UIcommand>(command), &par, name}
            )
        ); 
}


