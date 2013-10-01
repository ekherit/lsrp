/*
 * =====================================================================================
 *
 *       Filename:  utils.C
 *
 *    Description:  Utils 
 *
 *        Version:  1.0
 *        Created:  24.09.2013 11:17:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */
//#include <iomanip>
//#include <vector>

double get_fhwm(TH1F * h)
{
  double max = h->GetMaximum();
  cout << "max=" << max << endl;
  int max_bin =  h->GetMaximumBin();
  cout << max_bin << " " << max << endl;
  int upper, lower;
  for(int i=max_bin; i<h->GetNbinsX();i++)
  {
    double y = h->GetBinContent(i);
    cout << i << " " << y << endl;
    if(y<max/2) 
    {
      upper = i;
      break;
    }
  }
  cout << "Upper bin = " << upper << endl;
  for(int i=max_bin; i>=0;i--)
  {
    double y = h->GetBinContent(i);
    cout << i << " " << y << endl;
    if(y<max/2) 
    {
      lower = i;
      break;
    }
  }
  cout << "Lower bin = " << lower << endl;
  double error= h->GetBinWidth(upper)/2.;
  double width = h->GetBinCenter(upper)-h->GetBinCenter(lower);
  cout << h->GetBinCenter(upper) << endl;
  cout << h->GetBinCenter(lower) << endl;
  cout << "Width = " <<  width << " +- "  << error*sqrt(2) << endl;
  return width;
}

inline void DrawAsymmetry(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  //t->SetLineColor(kBlue);
  //t->SetLineWidth(2);
  TH1F *hdown = (TH1F*)gDirectory->Get("hdown");
  TH1F *hup   = (TH1F*)gDirectory->Get("hup");
  if(hdown!=0) delete hdown;
  if(hup!=0) delete hup;
  t->Draw((channel+string(">>hup")).c_str(), Pup&&cut,"goff");
  //t->SetLineColor(kRed);
  //t->SetLineWidth(2);
  t->Draw((channel+string(">>hdown")).c_str(), Pdown&&cut, "goff");
  hdown = (TH1F*)gDirectory->Get("hdown");
  hup   = (TH1F*)gDirectory->Get("hup");
  hup->Print();
  hdown->Print();
  TH1F * hminus = new TH1F(*hup);
  TH1F * hplus = new TH1F(*hup);
  hminus->Add(hdown, -1);
  hplus->Add(hdown, +1);
  TH1F * hasym = new TH1F(*hminus);
  hasym->SetLineColor(t->GetLineColor());
  hasym->SetLineWidth(t->GetLineWidth());
  hasym->Divide(hplus);
  new TCanvas;
  hminus->Draw();
  new TCanvas;
  hplus->Draw();
  new TCanvas;
  hasym->Draw(gopt);
}

inline void CalculateAsymmetry2(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  TCut up = (channel+string(">0")).c_str();
  TCut down = (channel+string("<0")).c_str();
  t->Draw(channel, ((Pup && up) || (Pdown && down)) && cut,"goff");
  double uu = t->GetSelectedRows();
  t->Draw(channel, ((Pup && down) || (Pdown && up)) && cut,"goff");
  double ud = t->GetSelectedRows();
  cout << "uu = " << Long64_t(uu) << endl;
  cout << "ud = " << Long64_t(ud) << endl;
  double eps =(uu-ud)/uu;
  double error = sqrt(1./uu+1./ud)*ud/uu;
  cout << "uu-ud = " << int(uu-ud) << endl;
  //cout << "Assymetry: " <<  eps << " +- " << error << endl;
  cout << "Asymmetry: " <<  eps << " +- " << error << " ("<< error/eps*100.0 << "%)"<< endl;
}

inline double sq(double x) { return x*x;}

struct AsymmetryResult_t
{
  double asymmetry;
  double error;
};

inline  AsymmetryResult_t CalculateAsymmetry(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  TCut up = (channel+string(">0")).c_str();
  TCut down = (channel+string("<0")).c_str();
  t->Draw(channel, ((Pup && up) || (Pdown && down)) && cut,"goff");
  double uu = t->GetSelectedRows();
  t->Draw(channel, ((Pup && down) || (Pdown && up)) && cut,"goff");
  double ud = t->GetSelectedRows();
  cout << "uu = " << Long64_t(uu) << endl;
  cout << "ud = " << Long64_t(ud) << endl;
  double eps = (uu-ud)/(uu+ud);
  double error = 2.0*uu*ud/sq(uu+ud)*sqrt(1./uu+1./ud);
  cout << "uu-ud = " << int(uu-ud)<< endl;
  cout << "uu+ud = " << Long64_t(uu+ud) << endl;
  cout << "Asymmetry: " <<  eps << " +- " << error << " ("<< error/eps*100.0 << "%)"<< endl;
  AsymmetryResult_t ar;
  ar.asymmetry = eps;
  ar.error = error;
  return ar;
}

void drift_comparison(const char * fn1, const char *fn2)
{
  c = new TCanvas;
  TFile * f1 = new TFile(fn1);
  TTree * t1 =(TTree*)f1->Get("lsrp");
  t1->SetTitle("t1");
  TFile * f2 = new TFile(fn2);
  TTree * t2 =(TTree*)f2->Get("lsrp");
  t2->SetTitle("t2");
  t1->SetLineWidth(2);
  t1->SetLineColor(kBlue);
  t1->Draw("pad.q","pad.q<50");
  t2->SetLineWidth(2);
  t2->SetLineColor(kRed);
  t2->Draw("pad.q","pad.q<50","same");
  c->SetLogy();
  new TCanvas;
  TCut cut="abs(pad.yhit)<50";
  t1->Draw("pad.yhit>>hy1",cut,"");
  TH1F *hy1  = (TH1F*)gDirectory->Get("hy1");
  t2->Draw("pad.yhit>>hy2",cut,"same");
  TH1F *hy2  = (TH1F*)gDirectory->Get("hy2");
  double fwhm1 = get_fhwm(hy1);
  double fwhm2 = get_fhwm(hy2);
  cout << "fwhm1 = " <<  fwhm1 << " mm" << endl;
  cout << "fwhm2 = " <<  fwhm2 << " mm" << endl;

  //new TCanvas;
  //cut = "pad.q>2 && abs(pad.yhit) < 20";
  //DrawAsymmetry(t1,"pad.yhit",cut,"");
  //t2->SetLineColor(kRed);
  //DrawAsymmetry(t2,"pad.yhit",cut,"same");

  new TCanvas;
  cut = "abs(gen.y) < 20";
  DrawAsymmetry(t1,"gen.y",cut,"");
  //t2->SetLineColor(kRed);
  //DrawAsymmetry(t2,"hit.y",cut,"same");
}

void utils(void)
{
  ////drift_comparison("drift.root","nodrift.root");
  //TFile * f = new TFile("tmp.root");
  //TTree * t = (TTree*)f->Get("lsrp");
  ////DrawAsymmetry(t,"pad.yhit","abs(pad.yhit)<20");
  //DrawAsymmetry(t,"gen.y","abs(gen.y)<20");
}



TTree *  LoadTree(const char * file)
{
  TFile * f = new TFile(file);
  TTree  *t =  (TTree*)f->Get("lsrp");
  return t;
}

struct OcupancyResult_t
{
  double occupancy;
  double error;
  double mean_error;
};

OcupancyResult_t DrawOccupancy(TTree *t, TCut cut=="")
{
  t->SetLineWidth(2);
  t->Draw("pad.nphot:sqrt(pad.x**2+pad.y**2)>>hocup",cut, "prof");
  double occupancy=hocup->GetBinContent(1);
  double error=hocup->GetBinError(1);
  double binwidth= hocup->GetBinWidth(1);
  double bincenter=hocup->GetBinCenter(1);
  double rmax = bincenter+binwidth/2;
  cout << "rmax = " << rmax << endl;
  char  buf[1024];
  sprintf(buf,"pad.r<%f",rmax);
  TCut bin0 = buf;
  t->Draw("pad.nphot",cut && bin0, "goff");
  double nbin = t->GetSelectedRows();
  cout << "number of event in zero bin: " <<  nbin << endl;
  cout << "Occupancy = " << occupancy  << " +- " << error*sqrt(nbin) <<  "(" << error <<")" <<  endl;
  OcupancyResult_t result;
  result.occupancy = occupancy;
  result.error = error*sqrt(nbin);
  result.mean_error = error;
  return result;
}
#include <iomanip>
void MassiveDrawOccupancy(const char * file_prefix="run")
{
  char filename[1024];
  //double ocup[11]; 
  //double err[11];
  //double mean_error[11];
  int N=11;
  string * Fi = new string[N];
  OcupancyResult_t * Oc = new OcupancyResult_t[N]
  cout << setw(12) << "file" << setw(12) << "occupancy" << setw(12) << "spread" << setw(12) << "mean_error" << endl;
  for(int i=0;i<N;i++)
  {
    sprintf(filename,"%s%d.root",file_prefix,i);
    TTree * t = LoadTree(filename);
    OcupancyResult_t o = DrawOccupancy(t);
    Oc[i]=o;
    Fi[i]=filename;
    cout << setw(12) << filename << setw(12) << o.occupancy << setw(12) << o.error << setw(12) << o.mean_error << endl;
  }

  cout << setw(12) << "file" << setw(12) << "occupancy" << setw(12) << "spread" << setw(12) << "mean_error" << endl;
  for(int i=0;i<N;i++)
  {
    cout << setw(12) << Fi[i] << setw(12) << Oc[i].occupancy << setw(12) << Oc[i].error << setw(12) << Oc[i].mean_error << endl;
  }
}

void MassiveCalculateAsymmetry(const char * file_prefix="run")
{
  char filename[1024];
  int N=11;
  string * Fi = new string[N];
  AsymmetryResult_t * R = new AsymmetryResult_t[N]
  cout << setw(12) << "file" << setw(12) << "occupancy" << setw(12) << "spread"  << endl;
  for(int i=0;i<N;i++)
  {
    sprintf(filename,"%s%d.root",file_prefix,i);
    TTree * t = LoadTree(filename);
    AsymmetryResult_t o = CalculateAsymmetry(t,"pad.y","");
    R[i]=o;
    Fi[i]=filename;
    cout << setw(12) << filename << setw(12) << o.asymmetry << setw(12) << o.error
      << setw(12) << setprecision(3) << R[i].error*100.0/R[i].asymmetry<< endl;
  }

  cout << setw(12) << "file" << setw(12) << "occupancy" << setw(12) << "spread" << setw(12) << endl;
  for(int i=0;i<N;i++)
  {
    cout << setw(12) << Fi[i] << setw(12) << R[i].asymmetry << setw(12) << R[i].error 
      << setw(12) << setprecision(3) << R[i].error*100.0/R[i].asymmetry <<endl;
  }
}

