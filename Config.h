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
  double psm_width; //presampler width in cm
  double psm_gem_length; //distance between presampler and gem
  double gem_width; //gem width
  double pad_size;  //size of signe pad in mm
  unsigned photon_number; //number of photons in one pulse
  std::string output_file; //output file name
};

extern  Config_t Cfg;

#endif 
