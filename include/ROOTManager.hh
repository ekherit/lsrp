/*
 * =====================================================================================
 *
 *       Filename:  ROOTManager.hh
 *
 *    Description:  Manger collecting information ans saving it into ROOT file
 *
 *        Version:  1.0
 *        Created:  08/29/13 17:29:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */
#ifndef LSRP_ROOT_MANGER_H
#define LSRP_ROOT_MANGER_H

#include <TTree.h>
#include <TNtupleD.h>
#include <TFile.h>

#include <memory>

class ROOTManager
{
  public:
    static ROOTManager* Instance(void);
    std::unique_ptr<TNtupleD> tree;
    std::unique_ptr<TFile> file;
  private:
    ROOTManager(); 
    ~ROOTManager(); 
    static ROOTManager* fgInstance;
};
#endif



