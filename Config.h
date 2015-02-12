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
	unsigned photon_number=0; //number of photons in one pulse
	double photon_flight_length=25*m; //Photon flight length in mm
	//geometry
	double psm_width; //presampler width in cm
	double psm_size;  //size of the presampler
	double psm_gem_length; //distance between presampler and gem
	double gem_width; //gem width
	double gem_size; //size of the gem in mm
	double gem_amplification;  //1e4
	double pad_size;  //size of signe pad in mm
	double pad_xsize;  //x size of signe pad in mm
	double pad_ysize;  //y size of signe pad in mm
	unsigned gem_cascade_number; //number of amplification cascades 3
	std::string output_file; //output file name
	unsigned test_beam;
	unsigned drift_spread;
	unsigned long seed;
	unsigned long run;
	double pad_high_sens_xwidth=64;
	double pad_high_sens_ywidth=20;
	double pad_rough_xscale=2;
	double pad_rough_yscale=2;

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
		double lambda=0.5*mkm; //metr laser wave length
		double pulse_energy= 100*mkJ;  //laser pulse energy, Joule
		double pulse_time =  6*ns;   //laser pulse time
		double pulse_size =  3*mm; //meters
		double frequency=10*kHz; //laser frequency, Hz
		double focus_length=15*cm; //laser focuse length
	} laser;
};
extern  Config_t Cfg;

#endif 
