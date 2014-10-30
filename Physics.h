/*
 * =====================================================================================
 *
 *       Filename:  Physics.h
 *
 *    Description:  Calculation of physics results
 *
 *        Version:  1.0
 *        Created:  20.09.2013 14:43:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */
#include <TTree.h>
#include <string>
#include <iostream>
#include <TCut.h>

//#ifndef IBN_LSRP_PHYSICS_H
//#define IBN_LSRP_PHYSICS_H

//inline ibn::valer<double> get_epsilon(Long64_t N1, Long64_t N2)
//{
//  ibn::valer<double> eps;
//  eps.value = double(N1-N2)/double(N1+N2);
//  eps.error = sqrt(ibn::sq(2.0*N2/ibn::sq(double(N1+N2)))*N1 + ibn::sq(2.0*N1/ibn::sq(double(N1+N2)))*N2);
//  return eps;
//}

//inline void Asymmetry(TTree * t, const char * channel)
//{
//  TCut Pup="P>0";
//  TCut Pdown = "P<0";
//  TCut up = (string(channel) + string(">0")).c_str();
//  TCut down = (string(channel) + string("<0")).c_str();
//  Long64_t Nplus  = t->GetEntries(Pup && up) + t->GetEntries(Pdown && down);
//  Long64_t Nminus = t->GetEntries(Pup && up) + t->GetEntries(Pdown && down);
//  double eps = double(Nplus-Nminus)/double(Nplus+Nminus);
//  cout << "Asymmetry = " << eps << endl;
//  //Long64_t Nup0 = RM->gen_tree->GetEntries("y>0&&P>0") + RM->gen_tree->GetEntries("y<0&&P<0");
//  //Long64_t Ndown0 = RM->gen_tree->GetEntries("y<0&&P>0")+ RM->gen_tree->GetEntries("y>0&&P<0");
//  //ibn::valer<double> eps0 = get_epsilon(Nup0,Ndown0);
//  //Long64_t Nup = RM->tree->GetEntries("hit.y>0 && volumeID==1&& P>0") +  RM->tree->GetEntries("hit.y<0 && volumeID==1&& P<0");
//  //Long64_t Ndown = RM->tree->GetEntries("hit.y<0 && volumeID==1 && P>0") +  RM->tree->GetEntries("hit.y>0 && volumeID==1 && P<0");
//  //ibn::valer<double> eps = get_epsilon(Nup,Ndown);
//  //RM->gen_tree->Write();
//  //G4cout << "Initial assymetry: " << eps0.value << "+-" << eps0.error << " ("<< eps0.error/eps0.value*100<< "%)" << G4endl;
//  //G4cout << "Gem assymetry:     " << eps.value << "+-" << eps.error << " ("<< eps.error/eps.value*100<< "%)" << G4endl;
//}

inline void DrawAsymmetry(TTree *t, const char * channel, TCut cut)
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  t->SetLineColor(kBlue);
  t->SetLineWidth(2);
  t->Draw((channel+string(">>hup")).c_str(), Pup&&cut);
  t->SetLineColor(kRed);
  t->SetLineWidth(2);
  t->Draw((channel+string(">>hdown")).c_str(), Pdown&&cut, "same");
  TH1F *hdown = (TH1F*)gDirectory->Get("hdown");
  TH1F *hup   = (TH1F*)gDirectory->Get("hup");
  hup->Print();
  hdown->Print();
  TH1F * hminus = new TH1F(*hup);
  TH1F * hplus = new TH1F(*hup);
  hminus->Add(hdown, -1);
  hplus->Add(hdown, +1);
  TH1F * hasym = new TH1F(*hminus);
  hasym->Divide(hplus);
  new TCanvas;
  hasym->Draw();
}
//#endif
