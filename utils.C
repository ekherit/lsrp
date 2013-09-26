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

inline void CalculateAsymmetry(TTree *t, const char * channel, TCut cut, const char * gopt="")
{
  TCut Pup="P>0";
  TCut Pdown = "P<0";
  TCut up = (channel+string(">0")).c_str();
  TCut down = (channel+string("<0")).c_str();
  t->Draw(channel, ((Pup && up) || (Pdown && down)) && cut,"goff");
  Long64_t uu = t->GetSelectedRows();
  t->Draw(channel, ((Pup && down) || (Pdown && up)) && cut,"goff");
  Long64_t ud = t->GetSelectedRows();
  cout << "uu = " << uu << endl;
  cout << "ud = " << ud << endl;
  cout << "uu-ud = " << double(uu)-double(ud) << endl;
  cout << "Assymetry: " << double(uu-ud)/double(uu) << endl;
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
  //drift_comparison("drift.root","nodrift.root");
  TFile * f = new TFile("tmp.root");
  TTree * t = (TTree*)f->Get("lsrp");
  //DrawAsymmetry(t,"pad.yhit","abs(pad.yhit)<20");
  DrawAsymmetry(t,"gen.y","abs(gen.y)<20");
}

