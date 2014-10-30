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

struct valer
{
  valer(void){}
  valer(double v, double e)
  {
    value = v;
    error = e;
  }
  double value=0;
  double error=0;
};


double get_fwhm(TH1F * h, double * error_result=0)
{
  double max = h->GetMaximum();
//  cout << "max=" << max << endl;
  int max_bin =  h->GetMaximumBin();
//  cout << max_bin << " " << max << endl;
  int upper, lower;
  for(int i=max_bin; i<h->GetNbinsX();i++)
  {
    double y = h->GetBinContent(i);
 //   cout << i << " " << y << endl;
    if(y<max/2) 
    {
      upper = i;
      break;
    }
  }
  //cout << "Upper bin = " << upper << endl;
  for(int i=max_bin; i>=0;i--)
  {
    double y = h->GetBinContent(i);
   // cout << i << " " << y << endl;
    if(y<max/2) 
    {
      lower = i;
      break;
    }
  }
  //cout << "Lower bin = " << lower << endl;
  double error= h->GetBinWidth(upper)/2.;
  double width = h->GetBinCenter(upper)-h->GetBinCenter(lower);
  //cout << h->GetBinCenter(upper) << endl;
  //cout << h->GetBinCenter(lower) << endl;
  //cout << "Width = " <<  width << " +- "  << error*sqrt(2) << endl;
  if(error_result) *error_result = error*sqrt(2.0);
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
//Рисует ассимметрию с нормировкой на полное число событий.
inline void DrawAsymmetry2(TTree *t, const char * channel, TCut cut, const char * gopt="")
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
  TH1F * hplus = new TH1F(*hup);
  hplus->Add(hdown, +1);
	hplus->Scale(0.5);
  TH1F * hminus = new TH1F(*hup);
  hminus->Add(hplus, -1);
  //TH1F * hasym = new TH1F(*hminus);
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

inline valer CalculateEffect3(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  TCut up = (channel+string(">0")).c_str();
  TCut down = (channel+string("<0")).c_str();
  t->Draw(channel, ((Pup && up) || (Pdown && down)) && cut,"goff");
  double uu = t->GetSelectedRows();
  t->Draw(channel, ((Pup && down) || (Pdown && up)) && cut,"goff");
  double ud = t->GetSelectedRows();
  //cout << "uu = " << Long64_t(uu) << endl;
  //cout << "ud = " << Long64_t(ud) << endl;
  double sum = uu+ud;
  double eps =(uu-ud)/(uu+ud);
  double error = 2.0*uu*ud/(sum*sum)*sqrt(1.0/uu+1.0/ud);
  //cout << "uu-ud = " << int(uu-ud) << endl;
  //cout << "Assymetry: " <<  eps << " +- " << error << endl;
  return valer(eps,error);
}
//Расчёт эффекта с учётом вычисления центра распределения
inline valer CalculateEffect4(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
	t->Draw(channel,  cut,  "goff");
	TH1F * h = (TH1F*) t->GetHistogram();
	double mean = h->GetMean();
	//cout << "mean = " << mean << endl;
  TCut Pup="P>0";
  TCut Pdown = "P<0";
	char buf[1024];
	sprintf(buf, "%s>%f", channel,  mean);
  TCut up = buf;
	sprintf(buf, "%s<%f", channel,  mean);
  TCut down = buf;
  t->Draw(channel, ((Pup && up) || (Pdown && down)) && cut,"goff");
  double uu = t->GetSelectedRows();
  t->Draw(channel, ((Pup && down) || (Pdown && up)) && cut,"goff");
  double ud = t->GetSelectedRows();
  //cout << "uu = " << Long64_t(uu) << endl;
  //cout << "ud = " << Long64_t(ud) << endl;
  double sum = uu+ud;
  double eps =(uu-ud)/(uu+ud);
  double error = 2.0*uu*ud/(sum*sum)*sqrt(1.0/uu+1.0/ud);
  //cout << "uu-ud = " << int(uu-ud) << endl;
  //cout << "Assymetry: " <<  eps << " +- " << error << endl;
  return valer(eps,error);
}

inline void CalculateAsymmetry3(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  valer v = CalculateEffect3(t,channel,cut,gopt);
  cout << "Asymmetry: " <<  v.value << " +- " << v.error << " ("<< v.error/v.value*100.0 << "%)"<< endl;
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

void DrawAsymVSWidth(TTree * t,const char * chan="pad.y", const char * cut="pad.q>0")
{
  char sel_up1[1024];
  char sel_down1[1024];
  char sel_up2[1024];
  char sel_down2[1024];
  sprintf(sel_up1, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up1", chan, cut, cut);
  sprintf(sel_down1, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down1", chan, cut, cut);
  sprintf(sel_up2, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up2", chan, cut, cut);
  sprintf(sel_down2, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down2", chan, cut, cut);

  t->Draw(sel_up1,"P>0","prof");
  TProfile * puu = new TProfile(*((TProfile*)gDirectory->Get("prof_up1")));
  t->Draw(sel_down1,"P<0","prof");
  TProfile * pdd = new TProfile(*((TProfile*)gDirectory->Get("prof_down1")));

  t->Draw(sel_up2,"P<0","prof");
  TProfile * pud = new TProfile(*((TProfile*)gDirectory->Get("prof_up2")));
  t->Draw(sel_down2,"P>0","prof");
  TProfile * pdu = new TProfile(*((TProfile*)gDirectory->Get("prof_down2")));

  TProfile * p1 = new TProfile(*puu);
  p1->Add(pdd,1);
  TProfile * p2 = new TProfile(*pud);
  p2->Add(pdu,1);
  TProfile * pdif = new TProfile(*p1);
  pdif->Add(p2,-1);
  TProfile * psum = new TProfile(*p1);
  psum->Add(p2,1);
  TProfile * pasym = new TProfile(*pdif);
  cout << "1" << endl;
  //pasym->Divide((TH1*)psum,1);
  cout << "2" << endl;
  TCanvas * c = new TCanvas;
  c->Divide(2,3);
  c->cd(1);
  p1->Draw();
  c->cd(2);
  p2->Draw();
  c->cd(3);
  pdif->Draw();
  c->cd(4);
  psum->Draw();
  c->cd(5);
  //pasym->Draw();
}

void DrawDyVSWidth(TTree * t,const char * chan="pad.y", const char * cut="pad.q>0")
{
  char sel_up1[1024];
  char sel_down1[1024];
  char sel_up2[1024];
  char sel_down2[1024];
  sprintf(sel_up1, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up1", chan, cut, cut);
  sprintf(sel_down1, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down1", chan, cut, cut);
  sprintf(sel_up2, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up2", chan, cut, cut);
  sprintf(sel_down2, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down2", chan, cut, cut);

  t->Draw(sel_up1,"P>0","prof");
  TProfile * puu = new TProfile(*((TProfile*)gDirectory->Get("prof_up1")));
  t->Draw(sel_down1,"P<0","prof");
  TProfile * pdd = new TProfile(*((TProfile*)gDirectory->Get("prof_down1")));

  t->Draw(sel_up2,"P<0","prof");
  TProfile * pud = new TProfile(*((TProfile*)gDirectory->Get("prof_up2")));
  t->Draw(sel_down2,"P>0","prof");
  TProfile * pdu = new TProfile(*((TProfile*)gDirectory->Get("prof_down2")));

  TProfile * p1 = new TProfile(*puu);
  p1->Add(pdd,1);
  TProfile * p2 = new TProfile(*pud);
  p2->Add(pdu,1);
  TProfile * pdif = new TProfile(*p1);
  pdif->Add(p2,-1);
  TProfile * psum = new TProfile(*p1);
  psum->Add(p2,1);
  TProfile * pasym = new TProfile(*pdif);
  cout << "1" << endl;
  //pasym->Divide((TH1*)psum,1);
  cout << "2" << endl;
  TCanvas * c = new TCanvas;
  c->Divide(2,3);
  c->cd(1);
  p1->Draw();
  c->cd(2);
  p2->Draw();
  c->cd(3);
  pdif->Draw();
  c->cd(4);
  psum->Draw();
  c->cd(5);
  //pasym->Draw();
}

void DrawDyVSWidth2(TTree * t, const char * cut="pad.q>0")
{
  //char sel_up1[1024];
  //char sel_down1[1024];
  //char sel_up2[1024];
  //char sel_down2[1024];
  //sprintf(sel_up1, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up1", chan, cut, cut);
  //sprintf(sel_down1, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down1", chan, cut, cut);
  //sprintf(sel_up2, "Sum$((%s>0)*(%s))/Sum$(%s):d>>prof_up2", chan, cut, cut);
  //sprintf(sel_down2, "Sum$((%s<0)*(%s))/Sum$(%s):d>>prof_down2", chan, cut, cut);
  

  t->SetAlias("MY","Sum$(pad.y)/Length$(pad.y)");
  t->SetAlias("MY2","Sum$(pad.y**2)/Length$(pad.y)");


  TCanvas * c = new TCanvas;
  c->Divide(2,2);
  c->cd(1);
  t->SetLineWidth(2);
  t->SetLineColor(kBlue);
  t->Draw("MY/sqrt(MY2-MY*MY):d>>pup","P>0","prof");
  t->SetLineColor(kRed);
  t->Draw("MY/sqrt(MY2-MY*MY):d>>pdown","P<0","prof");
  TProfile * pup = new TProfile(*((TProfile*)gDirectory->Get("pup")));
  TProfile * pdown = new TProfile(*((TProfile*)gDirectory->Get("pdown")));
  c->cd(2);
  pup->Draw();
  pdown->Draw("same");
  TProfile * pr = new TProfile(*pup);
  pr->Add(pdown,-1);
  c->cd(3);
  pr->Draw();
}

void DrawDyVSWidth3(TTree * t, const char * xaxis="d", const char * pad_cut="pad.q>1")
{
  char MY[1024];
  char MY2[1024];
  sprintf(MY, "Sum$(-P*pad.y*(%s))/Sum$(%s)", pad_cut, pad_cut);
  sprintf(MY2, "Sum$(pad.y*pad.y*(%s))/Sum$(%s)", pad_cut, pad_cut);
  t->SetAlias("MY",MY);
  t->SetAlias("MY2",MY2);
  char sel[1024];
  sprintf(sel,"MY/sqrt(MY2-MY*MY):%s>>prof", xaxis);
  t->Draw(sel,"","prof");
  TProfile * p = new TProfile(*((TProfile*)gDirectory->Get("prof")));
  int max_bin = p->GetMaximumBin();
  int min_bin = p->GetMinimumBin();
  TGraphErrors * g = new TGraphErrors;
  int i=0;
  for(int bin=0; bin<100000; bin++)
  {
    double y= p->GetBinContent(bin);
    double dy = p->GetBinError(bin);
    double x = p->GetBinCenter(bin);
    if(dy!=0)
    {
      g->SetPoint(i, x, y/dy);
      g->SetPointError(i, 0, 1);
      i++;
    }
    cout << bin << " " << y << " " << dy << endl;
  }
  new TCanvas;
  g->Draw("a*");
}



void MassiveDrawWidth(const char * file_prefix="run", int N = 10, const char * xaxis, const char * yaxis, const char * pad_cut="pad.q>0", const char * main_cut="1")
{
  char filename[1024];
  string * Fi = new string[N];
  cout << setw(12) << "file" << setw(12) << "fwhm, mm" << setw(12) << "error, mm" << endl;
  TGraphErrors * g = new TGraphErrors;
  Long64_t run;
  char xaxis_sel[1024];
  char yaxis_sel[1024];
  sprintf(yaxis_sel,"%s>>hyaxis",yaxis);
  TGraphErrors * ygraph = new TGraphErrors;
  for(int i=0;i<N;i++)
  {
    sprintf(filename,"%s%d.root",file_prefix,i);
    TTree * t = LoadTree(filename);
    sprintf(xaxis_sel,"%s>>hxaxis",xaxis);
    t->Draw(xaxis_sel,"","goff");
    double x = ((TH1F*)gDirectory->Get("hxaxis"))->GetMean();
    TCut pcut = pad_cut;
    TCut mcut = main_cut;
    t->Draw(yaxis_sel,pcut&&mcut,"");
    TH1F * h = (TH1F*)gDirectory->Get("hyaxis");
    double y = h->GetMean();
    double dy = h->GetMeanError();
    double width_error=1;
    double width = get_fwhm(h,&width_error);
    cout <<setw(12) << filename << setw(12) << x << setw(12) << run << setw(12) << y << setw(12) << dy << setw(12) << width << setw(12) << width_error << endl;
    ygraph->SetPoint(i, x, width);
    ygraph->SetPointError(i, 0, width_error);
  }
  TCanvas * c = new TCanvas;
  ygraph->Draw("a*");
}

valer CalculateEffect(TTree * t, const char * channel="pad.y", const char * pad_cut="pad.q>0", const char * gopt="goff")
{
  char MY[1024];
  char MY2[1024];
  sprintf(MY,  "Sum$(-P*(%s)*(%s))/Sum$(%s)", channel, pad_cut, pad_cut);
  sprintf(MY2, "Sum$((%s)**2*(%s))/Sum$(%s)", channel, pad_cut, pad_cut);
  t->SetAlias("MY",MY);
  t->SetAlias("MY2",MY2);
  char sel[1024];
  t->Draw("MY/sqrt(MY2-MY*MY)>>heff", "", gopt);
  TH1F * heff = (TH1F*)gDirectory->Get("heff");
  valer effect(heff->GetMean(), heff->GetMeanError());
  delete heff;
  return effect;
}

valer CalculateFWHM(TTree *t, const char * channel, TCut cut, const char * gopt="goff")
{
  char hname[1024];
  sprintf(hname, "h%s",channel);
  TH1F * hwidth = (TH1F*)gDirectory->Get(hname);
  if(hwidth) delete hwidth;
  char sel[1024];
  sprintf(sel, "%s>>%s", channel, hname);
  t->Draw(sel, cut, gopt);
  hwidth = (TH1F*)gDirectory->Get(hname);
  double error;
  double width=get_fwhm(hwidth, &error);
  return valer(width,error);
}


valer CalculateOccupancy(TTree *t, TCut cut=="", const char * gopt="goff")
{
  t->Draw("pad.nphot:sqrt(pad.x**2+pad.y**2)>>hocup",cut, "profgoff");
  double occupancy=hocup->GetBinContent(1);
  double error=hocup->GetBinError(1);
  double binwidth= hocup->GetBinWidth(1);
  double bincenter=hocup->GetBinCenter(1);
  double rmax = bincenter+binwidth/2;
  //cout << "rmax = " << rmax << endl;
  char  buf[1024];
  sprintf(buf,"pad.r<%f",rmax);
  TCut bin0 = buf;
  t->Draw("pad.nphot",cut && bin0, gopt);
  double nbin = t->GetSelectedRows();
  cout << "number of event in zero bin: " <<  nbin << endl;
  cout << "Occupancy = " << occupancy  << " +- " << error*sqrt(nbin) <<  "(" << error <<")" <<  endl;
  valer result(occupancy, error);
  return result;
}

Double_t poissonf(Double_t*x,Double_t*par)                                         
{                                                                              
  return par[0]*TMath::Poisson(x[0],par[1]);
}                                                                              


valer CalculateAverageOccupancy(TTree *t, TCut pad_cut, double sigmax, double sigmay, double r=1, const char * gopt="goff")
{
  char  buf[1024];
  sprintf(buf,"sqrt((pad.x/%f)**2 + (pad.y/%f)**2)<%f",sigmax,sigmay, r);
  TCut center_cut = buf;
  t->Draw("pad.nphot",pad_cut && center_cut &&"pad.nphot==1", "goff");
  double n1 = t->GetSelectedRows();
  t->Draw("pad.nphot",pad_cut && center_cut &&"pad.nphot==2", "goff");
  double n2 = t->GetSelectedRows();
  t->Draw("pad.nphot",pad_cut && center_cut &&"pad.nphot==3", "goff");
  double n3 = t->GetSelectedRows();
  t->Draw("pad.nphot",pad_cut && center_cut &&"pad.nphot==4", "goff");
  double n4 = t->GetSelectedRows();
  double lambda1 = 2.0*n2/n1;
  double N0 = n1/TMath::Poisson(1.0,lambda1);
  double lambda2 = sqrt(6.0*n3/n1);
  double lambda3 = pow(24.0*n4/n1, 1.0/3.0);
  cout << "lambda1 = " << lambda1 << "   lambda2 = " << lambda2 << " lambda3 = " << lambda3 << endl;
  TF1 * pois = new TF1("pois",poissonf,1,10,2); // x in [0;10], 2 parameters                  
  pois->SetParName(0,"Const");                                                
  pois->SetParName(1,"#lambda"); 
  pois->SetParameter(0,n1);
  pois->SetParameter(1,lambda1);
  t->Fit("pois","pad.nphot",pad_cut && center_cut, gopt,"R");
  //valer result(hnphot->GetMean(), hnphot->GetMeanError());
  double lambda = pois->GetParameter(1);
  double lambda_error = pois->GetParError(1);
  cout << lambda << "+-" << lambda_error << endl;
  cout << pois->GetChisquare() << "  ndf = " << pois->GetNDF() << endl;
  //return valer(lambda, lambda_error*sqrt(pois->GetChisquare()/pois->GetNDF()));
  //return valer(lambda, lambda_error);
  return valer(lambda1, lambda1*sqrt(1./n2 + 1./n1));
}


void Draw(TTree * t, TCut pad_cut="")
{
  TCanvas * c = new TCanvas;
  c->Divide(2,2);
  c->cd(1);
  valer fwhm_y = CalculateFWHM(t,"pad.y", pad_cut && TCut("abs(pad.y)<15"), "");
  char ybuf[1024];
  sprintf(ybuf,"fwhm = %5.1f mm", fwhm_y.value);
  TLatex * ytex = new TLatex(0,0,ybuf);
  ytex->Draw();
  c->cd(2);
  valer fwhm_x = CalculateFWHM(t,"pad.x", pad_cut && TCut("abs(pad.x)<100"), "");
  char xbuf[1024];
  sprintf(xbuf,"fwhm = %5.1f mm", fwhm_x.value);
  TLatex * xtex = new TLatex(0,0,xbuf);
  xtex->Draw();
  c->cd(3);
  valer occup = CalculateAverageOccupancy(t, pad_cut,fwhm_x.value/2.0, fwhm_y.value/2.0, 1, "");
  c->cd(4);
  t->Draw("pad.q","pad.q<1e5");
	gPad->SetLogy();
}

void MassiveDraw(const char * file_prefix="run", int N = 10, const char * xaxis, const char * pad_cut="pad.q>0", const char * main_cut="1")
{
  char filename[1024];
  string * Fi = new string[N];
  TGraphErrors * g = new TGraphErrors;
  Long64_t run;
  char xaxis_sel[1024];
  TGraphErrors * ygraph = new TGraphErrors;
  TGraphErrors * effect_graph = new TGraphErrors;
  TGraphErrors * confidence_graph = new TGraphErrors;
  TGraphErrors * occupancy_graph = new TGraphErrors;
  cout << setw(12) << "file" << setw(5) << "run" <<  setw(6) << xaxis;
  cout << setw(24) << "fwhm y" << flush;
  cout << setw(24) << "fwhm x" << flush;
  cout << setw(24) << "effect" << flush;
  cout << setw(25) << "occupancy" << flush;
  cout << endl;
  for(int i=0;i<N;i++)
  {
    sprintf(filename,"%s%d.root",file_prefix,i);
    cout << "Loading tree" << endl;
    if(gSystem->AccessPathName(filename)) continue;
    TTree * t = LoadTree(filename);
    cout << "Calculating x axis value" << endl;
    sprintf(xaxis_sel,"%s>>hxaxis",xaxis);
    t->Draw(xaxis_sel,"","goff",10);
    double x = ((TH1F*)gDirectory->Get("hxaxis"))->GetMean();
    cout <<setw(12) << filename << setw(5) << run << setw(6) << x  << flush;
    TCut pcut = pad_cut;
    TCut mcut = main_cut;
    valer fwhm_y = CalculateFWHM(t,"pad.y", pcut && TCut("abs(pad.y)<15"));
    cout << setw(12) << fwhm_y.value << "+-" << fwhm_y.error << flush;
    valer fwhm_x = CalculateFWHM(t,"pad.x", pcut && TCut("abs(pad.x)<100"));
    cout << setw(12) << fwhm_x.value << "+-" << fwhm_x.error << flush;
    valer effect = CalculateEffect(t,"pad.y", pcut);
    cout << setw(12) << effect.value << "+-" << effect.error << flush;
    //valer occup = CalculateOccupancy(t, pad_cut);
    valer occup = CalculateAverageOccupancy(t, pad_cut,fwhm_x.value/2.0, fwhm_y.value/2.0,1);
    cout << setw(12) << occup.value << "+-" << occup.error << flush;
    cout << endl;
    ygraph->SetPoint(i, x, fwhm_y.value);
    ygraph->SetPointError(i, 0, fwhm_y.error);

    effect_graph->SetPoint(i,x, effect.value);
    effect_graph->SetPointError(i,0, effect.error);

    confidence_graph->SetPoint(i,x, effect.value/effect.error);
    confidence_graph->SetPointError(i,0, 1);

    occupancy_graph->SetPoint(i,x, occup.value);
    occupancy_graph->SetPointError(i,0, occup.error);
  }
  TCanvas * c1 = new TCanvas;
  ygraph->Draw("a*");
  TCanvas * c2 = new TCanvas;
  effect_graph->Draw("a*");
  TCanvas * c3 = new TCanvas;
  confidence_graph->Draw("a*");
  TCanvas * c4 = new TCanvas;
  occupancy_graph->Draw("a*");
}


void DrawChargeThresholdEffect(TTree * t, double qmin=0, double qmax=5e4, double dq=1e4,  const char * main_cut="1")
{
  TGraphErrors * delta_graph =new TGraphErrors;
  TGraphErrors * effect_graph=new TGraphErrors;
  int i=0;
  cout  << "charge, fC" << "   effect " <<  " confidence level " << endl;
  printf("%15s%15s%16s\n", "charge,(Ne) ", "effect, %", " CL, sigma");
	TCut MCut = main_cut;
	
  for(double q=qmin;q<=qmax;q+=dq)
  {
    char cut[1024];
    sprintf(cut, "pad.q>%f", q);
    valer effect;
    valer effect = CalculateEffect4(t, "pad.y", cut && MCut);
    delta_graph->SetPoint(i, q, fabs(effect.value)*100);
    delta_graph->SetPointError(i, 0, effect.error*100);
    effect_graph->SetPoint(i, q, fabs(effect.value/effect.error));
    effect_graph->SetPointError(i, 0, 0);
    printf("%15.1f%9.2f+-%4.2f%16.2f\n", q, effect.value*100, effect.error*100, fabs(effect.value/effect.error));
    i++;
  }
  TCanvas * c = new TCanvas;
  c->Divide(2,1);
  c->cd(1);
  delta_graph->Draw("a*");
	delta_graph->GetXaxis()->SetTitle("charge");
	delta_graph->GetYaxis()->SetTitle("(N^{+}-N^{-}) / (N^{+}+N^{-}),  %");
	delta_graph->SetTitle("depolarization effect");
  c->cd(2);
	gPad->SetLogx();
  effect_graph->Draw("apc");
	effect_graph->SetMarkerStyle(21);
	effect_graph->GetXaxis()->SetTitle("charge");
	effect_graph->SetTitle("Confidence level");
	effect_graph->GetYaxis()->SetTitle("#varepsilon/#sigma_{#varepsilon}");

}

void DrawOccupancyPoisson(TTree *t, int n1, int n2)
{
  TGraphErrors * mu_graph=new TGraphErrors;
  TF1 * pois = new TF1("pois",poissonf,1,10,2); // x in [0;10], 2 parameters                  
  pois->SetParName(0,"Const");                                                
  pois->SetParName(1,"#lambda"); 
  int i=0;
  Long64_t N0 = lsrp->GetEntries();
  for(int n=n1 ; n<=n2; n++)
  {
    char cut[1024];
    sprintf(cut, "(pad.nx==0 || pad.nx==-1) && pad.ny==%d",n);
    Long64_t N = lsrp->GetEntries(cut); //number of pad with nphot>0
    double P0 = double(N0-N)/double(N0);
    double lambda0 = -log(P0);
    //remaining number 
    pois->SetParameter(0,N);
    pois->SetParameter(1,lambda0);
    t->Draw("pad.Y",cut,"goff",10);
    TH1F * h = t->GetHistogram();
    //cout << h << endl;
    //h->Print();
    //cout << h->GetMean() << endl;
    double y = h->GetMean(); //((TH1F*)gDirectory->Get("htemp"))->GetMean();
    cout << "Ymean =  " << y << endl;
    new TCanvas;
    t->Fit("pois","pad.nphot",cut, "","R");
    double lambda = pois->GetParameter(1);
    double lambda_error = pois->GetParError(1);
    mu_graph->SetPoint(i, y, lambda);
    mu_graph->SetPointError(i, 0, lambda_error);
    cout << lambda0 << endl;
    cout << lambda << "+-" << lambda_error << endl;
    cout << pois->GetChisquare() << "  ndf = " << pois->GetNDF() << endl;
    i++;
  }
  new TCanvas;
  mu_graph->Draw("*ac");
}


double theory_asymmetry(double y,  double chi)
{
	double r =  - 2*chi*y*(1+y*y)/(2*chi*chi*(1+y*y) + (1+y*y+2*chi)*(1+y*y*y*y));
	return r;
}

double theory_asymmetry(double *x,  double *p)
{
	return theory_asymmetry(y, chi);
}


