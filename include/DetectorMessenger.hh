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

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(void);
    virtual ~DetectorMessenger();
    static DetectorMessenger * Instance(void);
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    static DetectorMessenger * fgInstance;
    //DetectorConstruction*  fDetectorConstruction;

    std::unique_ptr<G4UIdirectory>    fLaserDirectory;
    std::unique_ptr<G4UIdirectory>    fBeamDirectory;
    std::unique_ptr<G4UIdirectory>    fDetectorDirectory;
    std::unique_ptr<G4UIdirectory>    fPadDirectory;

    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fStepMaxCmd;
    std::unique_ptr<G4UIcmdWithAnInteger> fPhotonNumberCmd;
    std::unique_ptr<G4UIcmdWithAString>   fRootFileCmd;



    //std::unique_ptr<G4UIdirectory>    fDirectory;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fGEMSizeX;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fGEMSizeY;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeXCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPadSizeYCmd;

    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fWorldSizeX;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fWorldSizeY;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fWorldSizeZ;

    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fGEMWorldDistance;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fConverterWidth;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fConverterGEMDistance;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fConverterSize;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fFlangeGEMDistance;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fFlangeWidth;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMirrorFlangeDistance;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fPhotonFlightLength;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fVacuumChamberSize;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMirrorSizeX;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMirrorSizeY;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMirrorWidth;



    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fHighSensWidthXCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fHighSensWidthYCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fRoughSizeXCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fRoughSizeYCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamSigmaYCmd; //vertical angular spread at 1.55 GeV
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamSigmaXCmd; //radial angular spread at 1.55 GeV
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamCurrentCmd; //mA
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fBeamEnergyCmd; 
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserWaveLengthCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserPulseEnergyCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserPulseTimeCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserFrequencyCmd;
    //std::unique_ptr<G4UIcmdWithADoubleAndUnit> fLaserPulseSizeCmd;

    std::map<G4UIcommand*, double *> fMap; //id of the Cmd is connected with the variable disired to set

    template<typename DataType> 
    struct CmdItem_t
    {
        std::unique_ptr<G4UIcommand>  command;
        //G4UIcommand *  command;
        DataType * data;
    };

    std::map<G4UIcommand*,  CmdItem_t <double> > fCmdMapDouble;

    void AddCmdDouble(double & par, const std::string & name, const std::string & title, const std::string & unit);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
