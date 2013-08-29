/*
 * =====================================================================================
 *
 *       Filename:  ROOTManager.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/29/13 17:34:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#include "ROOTManager.hh"

ROOTManager * ROOTManager::fgInstance = 0;

ROOTManager* ROOTManager::Instance(void)
{
  if (fgInstance == 0) fgInstance = new ROOTManager();
  return fgInstance;
}

ROOTManager::ROOTManager(void)
{
  tree.reset(new TNtupleD("lsrp","Laser polarimeter simulation","zx:E:kx:ky:kz:phi:Edep"));
  file.reset(new TFile("tmp.root","RECREATE"));
}

ROOTManager::~ROOTManager()
{
}
