/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  Configuration for the simulation
 *
 *        Version:  1.0
 *        Created:  09/06/13 14:59:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */


#ifndef IBN_LSRP_CONFIG_H
#define IBN_LSRP_CONFIG_H

#include <string>

#include <TObject.h>
#include <TRef.h>

#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
const G4double Hz  = 1.0/second;
const G4double kHz = 1e3*Hz;
const G4double MHz = 1e6*Hz;
const G4double GHz = 1e9*Hz;
const G4double mA =  milliampere;
const G4double mkJ = joule*1e-6;
const G4double mJ =  joule*1e-3;
const G4double mkm = micrometer;
const G4double hc = 197.326968e-15*MeV*m;
const G4double mkrad = 1e-6*radian;

struct Object
{
  struct { double x,y,z; } size;
  struct { double x=0,y=0,z=0; } rotation;
  std::string   material;
};

class Config_t : public TObject
{
  public:
  Config_t(void){};
  virtual ~Config_t(void){}
  unsigned long seed=0;
  unsigned long run=0;
  double photon_number=0; //number of photons in one pulse
  std::string root_file = "tmp.root"; //output file name

  //geometry
  double world_size_x = 0.1*m;
  double world_size_y = 0.1*m;
  double world_size_z = 100*m;
  double gem_world_distance = 10*cm;   // the position of GEM from right (z>0) edge of the world
  double converter_gem_distance=3*mm;  // distance between converter and GEM detector
  double flange_gem_distance = 19*m;   // distance between vacuum chamber flange and detector
  double mirror_flange_distance = 1*m; // mirror flange distance
  double photon_flight_length=25*m;    // Photon flight length in mm

  std::string world_material     = "G4_AIR";
  std::string vacuum_chamber_material = "G4_Galactic";

  double sens_before_converter_width = 1*mm;

  double step_max=1000*mm; //Max allowed step

  struct 
  {
    unsigned cascade_number; //number of amplification cascades 3
    double   amplification=1e4;  //1e4
    unsigned drift_spread;
    struct
    {
      double size         = 1*mm;  //size of signe pad in mm
      double size_x       = 1*mm;  //x size of signe pad in mm
      double size_y       = 1*mm;  //y size of signe pad in mm
      double rough_size_x = 2*mm;
      double rough_size_y = 2*mm;
    } pad;
    double high_sens_size_x=64;
    double high_sens_size_y=20;
    double width;                    // gem width  will calculated from GEM
    double size = 50*cm;             // gem size
    double size_x=50*cm;             // xy size of the gem in mm
    double size_y=50*cm;             // xy size of the gem in mm
  } gem;

  struct
  {
    double width=12*mm;        // converter width in mm
    double size=50*cm;         // size of the converter
    double size_x = 50*cm;
    double size_y = 50*cm;
    std::string material = "G4_Pb";
  } converter;


  struct
  {
    double width = 0.5*mm;   
    double size=20*cm; //the size of flange and vacuum chamber (vacuum chamber is round);
    std::string material    = "G4_STAINLESS-STEEL";
  } flange;

  struct
  {
    double width = 4*mm;          // the vacuum mirrror width
    double size_x = 10*cm;        // The mirror size x
    double size_y = 10*cm;        // The mirror size y
    double rotation_y =  45*deg;  // Rotation around axis y
    std::string material    = "quartz" ;
  } mirror;

  unsigned test_beam = 0;

  struct 
  {
    double I=1*mA; //current
    double E=5*GeV; //energy
    double sigmaY=21.2*mkrad;
    double sigmaX=104*mkrad;
    double revolution_frequency=818.924*kHz; //Hz
    double x = 0*mm,y =0*mm; //bunch position x and y
  } beam;

  struct
  {
    double lambda=527*nm; //metr laser wave length
    double pulse_energy= 100*mkJ;  //laser pulse energy, Joule
    double pulse_time =  6*ns;   //laser pulse time
    double pulse_size =  3*mm; //meters
    double frequency=4*kHz; //laser frequency, Hz
    double focus_length=15*cm; //laser focuse length
  } laser;

  struct
  {
    std::string file; //the name of ROOT file to save
    std::string auto_generate_root_files = "no";
  } root;

  ClassDef(Config_t, 1); //The class title
};
extern  Config_t Cfg;

#endif 
