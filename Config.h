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
struct Config_t
{
  unsigned photon_number; //number of photons in one pulse
  double photon_flight_length; //Photon flight length in mm
  //geometry
  double psm_width; //presampler width in cm
  double psm_size;  //size of the presampler
  double psm_gem_length; //distance between presampler and gem
  double gem_width; //gem width
  double gem_size; //size of the gem in mm
  double gem_amplification;  //1e4
  double pad_size;  //size of signe pad in mm
  unsigned gem_cascade_number; //number of amplification cascades 3
  std::string output_file; //output file name
  unsigned test_beam;
  unsigned drift_spread;
  unsigned long seed;
  unsigned long run;
};

extern  Config_t Cfg;

#endif 
