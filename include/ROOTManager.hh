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
    //std::unique_ptr<TNtupleD> tree2;
    std::unique_ptr<TFile> file;
    std::unique_ptr<TTree> tree;
    std::unique_ptr<TTree> gen_tree;
    struct generator_t
    {
      Long64_t eventID; //event id
      double P; //polarization of gamma quant
      double Eb; //electron beam energy
      double gamma; //gamma factor
      double omega; //photon energy in r.f. of electron
      double chi; //hardness of photon
      double E; //energy of gamma quant, MeV
      double kx; // x momentum, MeV
      double ky; //
      double kz; //
      double nx;
      double ny;
      double nz;
      double theta; 
      double phi;
      double x; //position
      double y;
      double z; 
    } Gen;

    struct hit_t
    {
      Long64_t trackID;
      Long64_t volumeID;
      double   E;
      double x;
      double y;
      double z;
      double rho;
      double phi;
    } Hit;

    struct Hit_t
    {
      Long64_t nhit; //number of hits
      Long64_t trackID[100];
      Long64_t volumeID[100];
      Double_t E[100];
      double x[100];
      double y[100];
      double z[100];
      double rho[100];
      double phi[100];
    } hit;

  private:
    ROOTManager(); 
    ~ROOTManager(); 
    static ROOTManager* fgInstance;
};
#endif



