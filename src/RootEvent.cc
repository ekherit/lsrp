/*
 * =====================================================================================
 *
 *       Filename:  RootEvent.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19.09.2013 10:21:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#include "RootEvent.hh"
#include <ibn/phys/compton.h>
#include "Config.h"

ClassImp(RootEvent)

RootEvent::RootEvent(void) { }

ClassImp(GeneratorEvent)

GeneratorEvent makeGeneratorEvent(const ibn::phys::compton &c)
{
  GeneratorEvent g;
  g.P = c.Pg*c.Pe;
  g.Eb = c.Ee;
  g.gamma = c.gamma;
  g.omega = c.omega;
  g.chi = c.chi;
  g.E = c.E;
  g.kx = c.kx;
  g.ky = c.ky;
  g.kz = c.kz;
  g.nx = c.kx/c.E;
  g.ny = c.ky/c.E;
  g.nz = c.kz/c.E;
  g.theta = c.theta;
  g.phi = c.phi;
  return g;
}

//GeneratorEvent::GeneratorEvent(const ibn::phys::compton & c)
//{
//    P = c.Pg*c.Pe;
//    Eb = c.Ee;
//    gamma = c.gamma;
//    omega = c.omega;
//    chi = c.chi;
//    E = c.E;
//    kx = c.kx;
//    ky = c.ky;
//    kz = c.kz;
//    nx = c.kx/c.E;
//    ny = c.ky/c.E;
//    nz = c.kz/c.E;
//    theta = c.theta;
//    phi = c.phi;
//}

GeneratorEvent::GeneratorEvent(void) { }

ClassImp(HitEvent)
HitEvent::HitEvent(void) { }
ClassImp(PadEvent)
PadEvent::PadEvent(void) { }
