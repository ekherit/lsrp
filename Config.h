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

struct Config_t
{
	double photon_number=0; //number of photons in one pulse

	//geometry
    double world_size_x = 0.1*m;
    double world_size_y = 0.1*m;
    double world_size_z = 100*m;
    double gem_world_distance = 10*cm; //the position of GEM from right (z>0) edge of the world
	double gem_width; //gem width  will calculated from GEM
    double gem_size = 50*cm; //gem size
	double gem_size_x=50*cm; //xy size of the gem in mm 
	double gem_size_y=50*cm; //xy size of the gem in mm 
	double converter_gem_distance=3*mm; //distance between converter and GEM detector
	double converter_width=12*mm; //presampler width in mm
	double converter_size=50*cm;  //size of the presampler
	//double air_length = 15*m; //air length from gem detector to flange
    double flange_gem_distance = 19*m; //distance between vacuum chamber flange and detector
    double flange_width = 0.5*mm; //flange width
    double mirror_flange_distance = 1*m; //mirror flange distance
    double mirror_width = 4*mm; //the vacuum mirrror width
    double mirror_size_x = 10*cm; //The mirror size x
    double mirror_size_y = 10*cm; //The mirror size y
	double photon_flight_length=25*m; //Photon flight length in mm
    double vacuum_chamber_size = 10*cm; 

    std::string converter_material = "G4_Pb";
    std::string mirror_material    = "quartz" ;
    std::string world_material     = "G4_AIR";
    std::string flange_material    = "G4_STAINLESS-STEEL";

    double sens_before_converter_width = 1*mm;

    double step_max=1000*mm; //Max allowed step

	double gem_amplification;  //1e4
	double pad_size = 1*mm;  //size of signe pad in mm
	double pad_xsize = 1*mm;  //x size of signe pad in mm
	double pad_ysize = 1*mm;  //y size of signe pad in mm
	unsigned gem_cascade_number; //number of amplification cascades 3
	std::string output_file = "tmp.root"; //output file name
	unsigned test_beam = 0;
	unsigned drift_spread;
	unsigned long seed;
	unsigned long run;
	double pad_high_sens_xwidth=64;
	double pad_high_sens_ywidth=20;
	double pad_rough_size_x=2*mm;
	double pad_rough_size_y=2*mm;

	struct 
	{
		double I=1*mA; //current
		double E=5*GeV; //energy
		double sigmaY=21.2*mkrad;
		double sigmaX=104*mkrad;
		double revolution_frequency=818.924*kHz; //Hz
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
};
extern  Config_t Cfg;

#endif 
