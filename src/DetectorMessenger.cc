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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
 : G4UImessenger(),
   fDetectorConstruction(Det)
{
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

  fStepMaxCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/stepMax",this));
  fStepMaxCmd->SetGuidance("Define a step max");
  fStepMaxCmd->SetParameterName("stepMax",false);
  fStepMaxCmd->SetUnitCategory("Length");
  fStepMaxCmd->AvailableForStates(G4State_Idle);

  fPhotonNumberCmd.reset(new G4UIcmdWithAnInteger("/lsrp/PhotonNumber",this));
  fPhotonNumberCmd->SetGuidance("Set photon number per pulse");
  fPhotonNumberCmd->SetParameterName("level",false);
  fPhotonNumberCmd->SetDefaultValue(1);

  fRootFileCmd.reset(new G4UIcmdWithAString("/lsrp/RootFile",this));
  fRootFileCmd->SetGuidance("Select output ROOT file");
  fRootFileCmd->SetParameterName("choice",false);
  fRootFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  fPresamplerWidthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/PresamplerWidth",this));
  fPresamplerWidthCmd->SetGuidance("Define a width of the presampler");
  fPresamplerWidthCmd->SetParameterName("presamplerWidth",false);
  fPresamplerWidthCmd->SetUnitCategory("Length");
  fPresamplerWidthCmd->AvailableForStates(G4State_Idle);

  fPsmGemLengthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/PsmGemLength",this));
  fPsmGemLengthCmd->SetGuidance("Define distance betwee presampler and gem");
  fPsmGemLengthCmd->SetParameterName("PsmGemLength",false);
  fPsmGemLengthCmd->SetUnitCategory("Length");
  fPsmGemLengthCmd->AvailableForStates(G4State_Idle);

  fPadSizeCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/pad/Size",this));
  fPadSizeCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeCmd->SetParameterName("PadSize",false);
  fPadSizeCmd->SetUnitCategory("Length");
  fPadSizeCmd->AvailableForStates(G4State_Idle);

  fPadSizeXCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/pad/SizeX",this));
  fPadSizeXCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeXCmd->SetParameterName("PadSizeX",false);
  fPadSizeXCmd->SetUnitCategory("Length");
  fPadSizeXCmd->AvailableForStates(G4State_Idle);

  fPadSizeYCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/pad/SizeY",this));
  fPadSizeYCmd->SetGuidance("Size of the hexagonal pad");
  fPadSizeYCmd->SetParameterName("PadSizeY",false);
  fPadSizeYCmd->SetUnitCategory("Length");
  fPadSizeYCmd->AvailableForStates(G4State_Idle);

  fHighSensWidthXCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/pad/HighSensWidthX",this));
  fHighSensWidthXCmd->SetGuidance("Size of the hexagonal pad");
  fHighSensWidthXCmd->SetParameterName("HighSensWidthX",false);
  fHighSensWidthXCmd->SetUnitCategory("Length");
  fHighSensWidthXCmd->AvailableForStates(G4State_Idle);

  fHighSensWidthYCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/det/pad/HighSensWidthY",this));
  fHighSensWidthYCmd->SetGuidance("Size of the hexagonal pad");
  fHighSensWidthYCmd->SetParameterName("HighSensWidthY",false);
  fHighSensWidthYCmd->SetUnitCategory("Length");
  fHighSensWidthYCmd->AvailableForStates(G4State_Idle);

  fRoughScaleXCmd.reset(new G4UIcmdWithADouble("/lsrp/det/pad/RoughScaleX",this));
  fRoughScaleXCmd->SetGuidance("X Scale of big pad");
  fRoughScaleXCmd->SetParameterName("RoughScaleX",false);
  fRoughScaleXCmd->AvailableForStates(G4State_Idle);

  fRoughScaleYCmd.reset(new G4UIcmdWithADouble("/lsrp/det/pad/RoughScaleY",this));
  fRoughScaleYCmd->SetGuidance("Y scale of big pad");
  fRoughScaleYCmd->SetParameterName("RoughScaleY",false);
  fRoughScaleYCmd->AvailableForStates(G4State_Idle);

  fBeamSigmaYCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/beam/SigmaY",this));
  fBeamSigmaYCmd->SetGuidance("Vertical beam angular spread");
  fBeamSigmaYCmd->SetParameterName("BeamSigmaY",false);
  fBeamSigmaYCmd->SetUnitCategory("Angle");
  fBeamSigmaYCmd->AvailableForStates(G4State_Idle);

  fBeamSigmaXCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/beam/SigmaX",this));
  fBeamSigmaXCmd->SetGuidance("Vertical beam angular spread");
  fBeamSigmaXCmd->SetParameterName("BeamSigmaX",false);
  fBeamSigmaXCmd->SetUnitCategory("Angle");
  fBeamSigmaXCmd->AvailableForStates(G4State_Idle);

  fBeamCurrentCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/beam/Current",this));
  fBeamCurrentCmd->SetGuidance("Beam current");
  fBeamCurrentCmd->SetParameterName("BeamCurrent",false);
  fBeamCurrentCmd->SetUnitCategory("Electric current");
  fBeamCurrentCmd->AvailableForStates(G4State_Idle);

  fBeamEnergyCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/beam/Energy",this));
  fBeamEnergyCmd->SetGuidance("Beam energy");
  fBeamEnergyCmd->SetParameterName("BeamEnergy",false);
  fBeamEnergyCmd->SetUnitCategory("Energy");
  fBeamEnergyCmd->AvailableForStates(G4State_Idle);

  fLaserWaveLengthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/laser/WaveLength",this));
  fLaserWaveLengthCmd->SetGuidance("Laser wave length");
  fLaserWaveLengthCmd->SetParameterName("LaserWaveLength",false);
  fLaserWaveLengthCmd->SetUnitCategory("Length");
  fLaserWaveLengthCmd->AvailableForStates(G4State_Idle);


  fLaserPulseEnergyCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/laser/PulseEnergy",this));
  fLaserPulseEnergyCmd->SetGuidance("Laser wave length");
  fLaserPulseEnergyCmd->SetParameterName("LaserPulseEnergy",false);
  fLaserPulseEnergyCmd->SetUnitCategory("Energy");
  fLaserPulseEnergyCmd->AvailableForStates(G4State_Idle);

  fLaserPulseTimeCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/laser/PulseTime",this));
  fLaserPulseTimeCmd->SetGuidance("Laser wave length");
  fLaserPulseTimeCmd->SetParameterName("LaserPulseTime",false);
  fLaserPulseTimeCmd->SetUnitCategory("Time");
  fLaserPulseTimeCmd->AvailableForStates(G4State_Idle);

  
  fLaserFrequencyCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/laser/Frequency",this));
  fLaserFrequencyCmd->SetGuidance("Laser wave length");
  fLaserFrequencyCmd->SetParameterName("LaserFrequency",false);
  fLaserFrequencyCmd->SetUnitCategory("Frequency");
  fLaserFrequencyCmd->AvailableForStates(G4State_Idle);


  fPhotonFlightLengthCmd.reset(new G4UIcmdWithADoubleAndUnit("/lsrp/PhotonFlightLength",this));
  fPhotonFlightLengthCmd->SetGuidance("Photon flight length");
  fPhotonFlightLengthCmd->SetParameterName("PhotonFlightLength",false);
  fPhotonFlightLengthCmd->SetUnitCategory("Length");
  fPhotonFlightLengthCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fStepMaxCmd.get() )
  {
    fDetectorConstruction->SetMaxStep(fStepMaxCmd->GetNewDoubleValue(newValue));
  }   

  if( command == fPresamplerWidthCmd.get())
  {
    fDetectorConstruction->SetPresamplerWidth(fPresamplerWidthCmd->GetNewDoubleValue(newValue));
  }   

  if( command == fPsmGemLengthCmd.get())
  {
    fDetectorConstruction->SetPsmGemLength(fPsmGemLengthCmd->GetNewDoubleValue(newValue));
  }   

  if(command == fPhotonNumberCmd.get())
  {
    Cfg.photon_number = fPhotonNumberCmd->GetNewIntValue(newValue);
  }

  if( command == fRootFileCmd.get() )
  { 
    ROOTManager::Instance()->SetRootFile(newValue);
  }
  
  if( command == fPadSizeCmd.get())
  {
    Cfg.pad_size = fPadSizeCmd->GetNewDoubleValue(newValue);
    Cfg.pad_xsize = Cfg.pad_size;
    Cfg.pad_ysize = Cfg.pad_size;
  }
  if( command == fPadSizeXCmd.get())
  {
    Cfg.pad_xsize = fPadSizeXCmd->GetNewDoubleValue(newValue);
  }
  if( command == fPadSizeYCmd.get())
  {
    Cfg.pad_ysize = fPadSizeYCmd->GetNewDoubleValue(newValue);
  }
  if( command == fHighSensWidthXCmd.get())
  {
    Cfg.pad_high_sens_xwidth = fHighSensWidthXCmd->GetNewDoubleValue(newValue);
  }
  if( command == fHighSensWidthYCmd.get())
  {
    Cfg.pad_high_sens_ywidth = fHighSensWidthYCmd->GetNewDoubleValue(newValue);
  }
  if( command == fRoughScaleXCmd.get())
  {
    Cfg.pad_rough_xscale = fRoughScaleXCmd->GetNewDoubleValue(newValue);
  }
  if( command == fRoughScaleYCmd.get())
  {
    Cfg.pad_rough_yscale = fRoughScaleYCmd->GetNewDoubleValue(newValue);
  }
  if( command == fBeamSigmaYCmd.get())
  {
    Cfg.beam.sigmaY = fBeamSigmaYCmd->GetNewDoubleValue(newValue);
  }
  if( command == fBeamSigmaXCmd.get())
  {
    Cfg.beam.sigmaX = fBeamSigmaXCmd->GetNewDoubleValue(newValue);
  }
  if( command == fBeamCurrentCmd.get())
  {
    Cfg.beam.I = fBeamCurrentCmd->GetNewDoubleValue(newValue);
    std::cout  << "Beam Current (Messanger) = " << Cfg.beam.I << std::endl;
  }
  if( command == fBeamEnergyCmd.get())
  {
    Cfg.beam.E = fBeamEnergyCmd->GetNewDoubleValue(newValue);
    std::cout  << "Cfg.beam.E = " << Cfg.beam.E << std::endl;
  }

  if( command == fLaserWaveLengthCmd.get())
  {
    Cfg.laser.lambda = fLaserWaveLengthCmd->GetNewDoubleValue(newValue);
  }
  if( command == fLaserPulseEnergyCmd.get())
  {
    Cfg.laser.pulse_energy = fLaserPulseEnergyCmd->GetNewDoubleValue(newValue);
  }
  if( command == fLaserPulseTimeCmd.get())
  {
    Cfg.laser.pulse_time = fLaserPulseTimeCmd->GetNewDoubleValue(newValue);
  }

  if( command == fLaserFrequencyCmd.get())
  {
    Cfg.laser.frequency = fLaserFrequencyCmd->GetNewDoubleValue(newValue);
  }

  if( command == fPhotonFlightLengthCmd.get())
  {
    Cfg.photon_flight_length = fPhotonFlightLengthCmd->GetNewDoubleValue(newValue);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
