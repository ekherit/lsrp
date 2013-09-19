/*
 * =====================================================================================
 *
 *       Filename:  RootEvent.hh
 *
 *    Description:  Root Event class
 *
 *        Version:  1.0
 *        Created:  19.09.2013 10:20:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#ifndef IBN_LSRP_ROOT_EVENT_H
#define IBN_LSRP_ROOT_EVENT_H

#include <TObject.h>

#include <vector>
class RootEvent : public TObject
{
  ClassDef(RootEvent, 1); //The class title
  public:
  RootEvent(void);
  virtual ~RootEvent(void){}
  Double_t Eb; //beam energy
  Long64_t Ng; //number of photons
  std::vector<Double_t> E;
  struct S
  {
    double A;
    double B;
  } s;
};

#endif


