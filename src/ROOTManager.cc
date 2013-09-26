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
#include "Config.h"

ROOTManager * ROOTManager::fgInstance = 0;

ROOTManager* ROOTManager::Instance(void)
{
  if (fgInstance == 0) fgInstance = new ROOTManager();
  return fgInstance;
}

ROOTManager::ROOTManager(void)
{
  SetRootFile(Cfg.output_file.c_str());
}

ROOTManager::~ROOTManager()
{
}

void ROOTManager::SetRootFile(const char * root_file_name)
{
  Cfg.output_file = root_file_name;
  if(tree) tree->Write(); //save tree
  //tree will be deleted while destroing TFile
  file.reset(new TFile(Cfg.output_file.c_str(),"RECREATE"));
  InitTree();
}

void ROOTManager::InitTree(void)
{
  tree = new TTree("lsrp","Laser polarimeter simulation");
  tree->Branch("event",&event);
}

