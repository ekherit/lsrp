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

#include <TSystem.h>
#include "ROOTManager.hh"
#include "Config.h"

#include <stdio.h>

ROOTManager * ROOTManager::fgInstance = 0;

ROOTManager* ROOTManager::Instance(void)
{
  if (fgInstance == 0) fgInstance = new ROOTManager();
  return fgInstance;
}

ROOTManager::ROOTManager(void)
{
  SetRootFile(Cfg.root_file.c_str()); //set default ROOT file
}

ROOTManager::~ROOTManager()
{
  CleanOldFilesAndTree();
}

void ROOTManager::SetRootFile(const char * root_file_name)
{
  CleanOldFilesAndTree();
  std::cout << "Set new ROOT file " << root_file_name << std::endl;
  filename = root_file_name;
  file = new TFile(filename.c_str(),"RECREATE");
  InitTree();
}

void ROOTManager::CleanOldFilesAndTree(void)
{
  if(file != nullptr ) 
  {
    auto events_in_tree  = tree != nullptr  ? tree->GetEntries() : 0;
    file->Close();
    //tree will automatically be deleted while destroing TFile no need to delete tree manually ??? strange
    delete file;
    if(events_in_tree == 0)  //Remove file with zero number of events
    {
      remove(filename.c_str());
    }
  }
}

void ROOTManager::InitTree(void)
{
  tree = new TTree("lsrp","Laser polarimeter simulation");
  tree->Branch("event",&event);
}

void ROOTManager::Write(void)
{
    if(tree) tree->Write();
}

