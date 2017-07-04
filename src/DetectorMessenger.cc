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

  fLaserDirectory.reset(new G4UIdirectory("/lsrp/laser/"));
  fLaserDirectory->SetGuidance("Laser parameters");

  fBeamDirectory.reset(new G4UIdirectory("/lsrp/beam/"));
  fBeamDirectory->SetGuidance("Beam parameters");

  fDetectorDirectory.reset(new G4UIdirectory("/lsrp/det/"));
  fDetectorDirectory->SetGuidance("Detector parameters");

  fPadDirectory.reset(new G4UIdirectory("/lsrp/det/pad/"));
  fPadDirectory->SetGuidance("GEM pad parameters");

  fRootFileCmd.reset(new G4UIcmdWithAString("/lsrp/RootFile",this));
  fRootFileCmd->SetGuidance("Select output ROOT file");
  fRootFileCmd->SetParameterName("choice",false);
  fRootFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPhotonNumberCmd.reset(new G4UIcmdWithAnInteger("/lsrp/PhotonNumber",this));
  fPhotonNumberCmd->SetGuidance("Set photon number per pulse");
  fPhotonNumberCmd->SetParameterName("level",false);
  fPhotonNumberCmd->SetDefaultValue(1);

  auto setCmd  = [this](auto & CmdPtr, double & par, std::string dir, std::string title, std::string name, std::string unit)
  {
      CmdPtr.reset(new G4UIcmdWithADoubleAndUnit((dir+"/"+name).c_str(), this));
      CmdPtr->SetGuidance(title.c_str());
      CmdPtr->SetParameterName(name.c_str(), false);
      CmdPtr->SetUnitCategory(unit.c_str());
      CmdPtr->AvailableForStates(G4State_Idle);
      fMap[static_cast<G4UIcommand*>(CmdPtr.get())] = & par;
  };

  AddCmdDouble(test_parameter, "/lsrp/TestParameter"," This is the test parameter", "Length");
  AddCmdDouble(Cfg.step_max, "/lsrp/StepMax","Miximum allowed step", "Length");
  setCmd(fStepMaxCmd,Cfg.step_max,   "/lsrp",, "stepMax","Length");

  setCmd(fPadSizeCmd,Cfg.pad_size,   "/lsrp/GEM/pad","Size of the pad", "Size","Length");
  setCmd(fPadSizeXCmd,Cfg.pad_xsize, "/lsrp/GEM/pad","X size of the pad", "SizeX","Length");
  setCmd(fPadSizeYCmd,Cfg.pad_ysize, "/lsrp/GEM/pad","Y size of the pad", "SizeY","Length");
  setCmd(fRoughSizeXCmd,Cfg.pad_rough_size_x, "/lsrp/GEM/pad","Big pad size x", "RoughSizeX","Length");
  setCmd(fRoughSizeYCmd,Cfg.pad_rough_size_y, "/lsrp/GEM/pad","Big pad size y", "RoughSizeY","Length");
  setCmd(fHighSensWidthXCmd,Cfg.pad_high_sens_xwidth, "/lsrp/GEM","X High sensitive width", "HighSensSizeX","Length");
  setCmd(fHighSensWidthYCmd,Cfg.pad_high_sens_ywidth, "/lsrp/GEM","Y High sensitive width", "HighSensSizeY","Length");

  setCmd(fBeamSigmaXCmd,Cfg.beam.sigmaX, "/lsrp/beam","Beam X angular spread", "SigmaX","Angle");
  setCmd(fBeamSigmaYCmd,Cfg.beam.sigmaY, "/lsrp/beam","Beam Y angular spread", "SigmaY","Angle");
  setCmd(fBeamCurrentCmd,Cfg.beam.I, "/lsrp/beam","Beam current", "Current","Electric current");
  setCmd(fBeamEnergyCmd,Cfg.beam.E, "/lsrp/beam","Beam energy", "Energy","Energy");

  setCmd(fLaserWaveLengthCmd,Cfg.laser.lambda, "/lsrp/laser","Laser wave length", "WaveLength","Length");
  setCmd(fLaserPulseEnergyCmd,Cfg.laser.pulse_energy, "/lsrp/laser","Laser pulse energy", "PulseEnergy","Energy");
  setCmd(fLaserPulseTimeCmd,Cfg.laser.pulse_time, "/lsrp/laser","Laser pulse time", "PulseTime","Time");
  setCmd(fLaserFrequencyCmd,Cfg.laser.frequency, "/lsrp/laser","Laser pulse frequency", "PulseFrequency","Frequency");
  setCmd(fLaserPulseSizeCmd,Cfg.laser.pulse_size, "/lsrp/laser","Laser pulse size", "PulseSize","Length");

  setCmd(fWorldSizeX,Cfg.world_size_x, "/lsrp/World","World size x", "SizeX","Length");
  setCmd(fWorldSizeY,Cfg.world_size_y, "/lsrp/World","World size y", "SizeY","Length");
  setCmd(fWorldSizeZ,Cfg.world_size_z, "/lsrp/World","World size z", "SizeZ","Length");

  setCmd(fWorldSizeX,Cfg.world_size_x, "/lsrp/GEM","GEM size x", "SizeX","Length");
  setCmd(fWorldSizeX,Cfg.world_size_x, "/lsrp/GEM","GEM size y", "SizeY","Length");
  setCmd(fGEMWorldDistance, Cfg.gem_world_distance, "/lsrp/GEM","GEM-world distance", "DistanceToWorldEdge","Length");

  setCmd(fConverterWidth,Cfg.converter_width, "/lsrp/Converter","Converter width", "Width","Length");
  setCmd(fConverterSize,Cfg.converter_size,   "/lsrp/Converter","Converter size (x,y)", "Size","Length");

  setCmd(fConverterGEMDistance,Cfg.converter_gem_distance, "/lsrp/Converter","Converter - GEM distance", "DistanceToGEM","Length");

  setCmd(fFlangeGEMDistance, Cfg.flange_gem_distance, "/lsrp/Flange","The distance from flange to GEM detector", "DistanceToGEM","Length");
  setCmd(fFlangeWidth, Cfg.flange_width, "/lsrp/Flange","Flange width", "Width","Length");

  setCmd(fMirrorFlangeDistance, Cfg.mirror_flange_distance, "/lsrp","Mirror - flange distance", "MirrorFlangeDistance","Length");
  setCmd(fMirrorWidth, Cfg.mirror_width, "/lsrp/Mirror","Mirror width",   "Width","Length");
  setCmd(fMirrorSizeX, Cfg.mirror_size_x, "/lsrp/Mirror","Mirror size x", "SizeX","Length");
  setCmd(fMirrorSizeY, Cfg.mirror_size_y, "/lsrp/Mirror","Mirror size y", "SizeY","Length");
  setCmd(fVacuumChamberSize, Cfg.vacuum_chamber_size, "/lsrp/VacuumChamber","Vacuum chamber size", "Size","Length");
  setCmd(fPhotonFlightLength, Cfg.photon_flight_length, "/lsrp","Photon Flight length", "PhotonFlightLength","Length");

}


DetectorMessenger::~DetectorMessenger() { }

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  try
  {
    auto p = fCmdMapDouble.find(command);
    if (  p!=fCmdMapDouble.end() ) 
    {
        std::cout << "FIND TEST COMMAND: " << std::endl;
        *p->second.data = static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue);
        std::cout << "newValue = " << newValue << " " << *p->second.data << std::endl;
    }
  }
  catch(...)
  {
  }
  try
  {
    auto p = fMap.find(command);
    if (  p!=fMap.end() ) 
    {
        *p->second = static_cast<G4UIcmdWithADoubleAndUnit*>(command)->GetNewDoubleValue(newValue);
    }
  }
  catch(...)
  {
  }

  if( command == fStepMaxCmd.get() )
  {
      DetectorConstruction::Instance()->SetMaxStep(Cfg.step_max);
  }   

  if(command == fPhotonNumberCmd.get())
  {
    Cfg.photon_number = fPhotonNumberCmd->GetNewIntValue(newValue);
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
                {std::unique_ptr<G4UIcommand>(command), &par}
            )
        ); 
}
