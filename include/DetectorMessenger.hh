/*
 * =====================================================================================
 *
 *       Filename:  DetectorMessenger.cc
 *
 *    Description:  G4UIcommand proceedings
 *
 *        Version:  1.0
 *        Created:  13.07.2017 21:59:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (ekherit), I.B.Nikolaev@inp.nsk.su
 *   Organization:  Budker Insitute of Nuclear Physics
 *
 * =====================================================================================
 */
#pragma once

#include <memory>
#include <typeinfo>
#include <map>

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;

#include <boost/variant.hpp>

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(void);
    virtual ~DetectorMessenger();
    static DetectorMessenger * Instance(void);
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    static DetectorMessenger * fgInstance;
    std::unique_ptr<G4UIdirectory>    fDirectory;

    //all G4UIcommand variose types in the map with boost::variant 
    std::map<G4UIcommand*,  boost::variant<int *, double*,  std::string*>  >  fCmdMap;
    //add command to fCmdMap
    template<typename Type> void AddCmd(Type & par, const std::string & name, const std::string & title, const std::string unit = "");

};
