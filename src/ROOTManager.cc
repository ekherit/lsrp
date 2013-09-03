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
  //tree2.reset(new TNtupleD("lsrp2","Laser polarimeter simulation","zx:E:kx:ky:kz:phi:Edep"));
  file.reset(new TFile("tmp.root","RECREATE"));
  tree.reset(new TTree("lsrp","Laser polarimeter simulation"));
  tree->Branch("gen",&Gen,"eventID/l:P/D:Eb:gamma:omega:chi:E:kx:ky:kz:nx:ny:nz:theta:phi:x:y:z");
  tree->Branch("hit",&Hit,"trackID/l:volumeID:E/D:x:y:z:rho:phi");
  gen_tree.reset(new TTree("gen_tree","Compton initial events"));
  gen_tree->Branch("gen", &Gen,"eventID/l:P/D:Eb:gamma:omega:chi:E:kx:ky:kz:nx:ny:nz:theta:phi:x:y:z");
}

ROOTManager::~ROOTManager()
{
}
