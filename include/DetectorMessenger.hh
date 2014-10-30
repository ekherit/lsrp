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
/// \file DetectorMessenger.hh
/// \brief Definition of the DetectorMessenger class

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include <memory>

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Messenger class that defines commands for DetectorConstruction.
///
/// It implements commands:
/// - //det/setTargetMaterial name
/// - //det/setChamberMaterial name
/// - //det/setField value unit
/// - //det/stepMax value unit

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* );
    virtual ~DetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    DetectorConstruction*  fDetectorConstruction;

    std::unique_ptr<G4UIdirectory>    fDirectory;
    std::unique_ptr<G4UIdirectory>    fLaserDirectory;
    std::unique_ptr<G4UIdirectory>    fBeamDirectory;
    std::unique_ptr<G4UIdirectory>    fDetectorDirectory;
    std::unique_ptr<G4UIdirectory>    fPadDirectory;

    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fStepMaxCmd;
    std::unique_ptr<G4UIcmdWithAnInteger> fPhotonNumberCmd;
    std::unique_ptr<G4UIcmdWithAString>   fRootFileCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPresamplerWidthCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPsmGemLengthCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeXCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeYCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fHighSensWidthXCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fHighSensWidthYCmd;
    std::unique_ptr<G4UIcmdWithADouble> fRoughScaleXCmd;
    std::unique_ptr<G4UIcmdWithADouble> fRoughScaleYCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamSigmaYCmd; //vertical angular spread at 1.55 GeV
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamSigmaXCmd; //radial angular spread at 1.55 GeV
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamCurrentCmd; //mA
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamEnergyCmd; 
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserWaveLengthCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserPulseEnergyCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserPulseTimeCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserFrequencyCmd;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPhotonFlightLengthCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
