/*
 * =====================================================================================
 *
 *       Filename:  calc.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  31.08.2013 16:40:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

inline double sq(double x) { return x*x;}
double get_eps(double N1, double N2)
{
  return (N1-N2)/(N1+N2);
};

double get_eps_error(double N1, double N2)
{
  return sqrt(sq(2*N2/sq(N1+N2))*N1 + sq(2*N1/sq(N1+N2))*N2);
};

void calc(void)
{
  TFile f("tmp.root");
  TTree * hit = f.Get("lsrp");
  TTree * gen = f.Get("gen_tree");
  Long64_t Nup = gen->GetEntries("y>0&&P>0") + gen->GetEntries("y<0&&P<0");
  Long64_t Ndown = gen->GetEntries("y<0&&P>0")+ gen->GetEntries("y>0&&P<0");
  cout << "initial assymetry: " << get_eps(Nup, Ndown) << " +- " << get_eps_error(Nup, Ndown) <<  " (" << fabs(get_eps_error(Nup,Ndown)/get_eps(Nup,Ndown))*100<< "%)" << endl;
  Long64_t nup = hit->GetEntries("hit.y>0 && volumeID==1&& P>0") +  hit->GetEntries("hit.y<0 && volumeID==1&& P<0");
  Long64_t ndown = hit->GetEntries("hit.y<0 && volumeID==1 && P>0") +  hit->GetEntries("hit.y>0 && volumeID==1 && P<0");
  cout << "  final assymetry: " << get_eps(nup, ndown) << " +- " << get_eps_error(nup, ndown) << " (" << fabs(get_eps_error(nup,ndown)/get_eps(nup,ndown))*100<< "%)" <<endl;
  TF1 * sinf = new TF1("asym","[0]+[1]*sin(x)",-TMath::Pi(), +TMath::Pi());
  lsrp->Fit("asym","hit.phi", "P>0");
}

