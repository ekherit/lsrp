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
    std::unique_ptr<G4UIcmdWithAString>   fRootFileCmd;
    std::unique_ptr<G4UIdirectory>    fDirectory;

    template<typename DataType> 
    struct CmdItem_t
    {
        std::unique_ptr<G4UIcommand>  command;
        DataType * data;
        std::string name;
    };

    std::map<G4UIcommand*,  CmdItem_t <double> > fCmdMapDouble; //continer with the G4UIcommands
    std::map<G4UIcommand*,  CmdItem_t <std::string> > fCmdMapString; //continer with the G4UIcommands

    void AddCmdDouble(double & par, const std::string & name, const std::string & title, const std::string & unit);
    void AddCmdString(std::string & par, const std::string & name, const std::string & title);

};
#endif
