void simple_effect(TTree *t, TCut cut="", double * result=0, double * result_error=0)
{
	double up1 = t->GetEntries(cut && "pad.y>0 && P>0");
	double up2 = t->GetEntries(cut && "pad.y<0 && P<0");

	double down1 = t->GetEntries(cut && "pad.y<0 && P>0");
	double down2 = t->GetEntries(cut && "pad.y>0 && P<0");
	double N0 = (up1+up2+down1+down2);
    double N1 = up1+up2;
    double N2 = down1+down2;
	double effect = (N1-N2)/N0;
	double error = sqrt(N1*(1.-N1/N0) + N2*(1.0-N2/N0))/N0;
	cout << "asym = " << effect << " +- " << error 
	     << " (" << fabs(effect/error) << ")" << endl;
	
	//--------------------------------------------------------------------------
	
    //TCanvas * c = new TCanvas;

    t->SetLineColor(kBlue);
    t->Draw("pad.y>>hl",cut && "P>0","goff");
    TH1 * hl = (TH1*) t->GetHistogram();
    double y1 = hl->GetMean();
    double dy1 = hl->GetMeanError();
    t->SetLineColor(kRed);
    t->Draw("pad.y>>hr",cut && "P<0","goff");
    TH1 * hr = (TH1*) t->GetHistogram();
    double y2 = hr->GetMean();
    double dy2 = hr->GetMeanError();
    std::cout << "<yleft> = " << y1 << " +- " << dy1 << std::endl;
    std::cout << "<yrigh> = " << y2 << " +- " << dy2 << std::endl;

    double effect_mean = y2 -y1;
    double effect_mean_error = sqrt(dy2*dy2+dy1*dy1);

    std::cout << "Delta <y> = " << effect_mean << " +- " << effect_mean_error << std::endl;

    if(result!=0) *result = effect_mean;
    if(result_error!=0) *result_error = effect_mean_error;
    
	//--------------------------------------------------------------------------
	
	//TCanvas * c1 = new TCanvas;
	
	t->SetLineColor(kBlue);
	t->Draw("pad.y>>all",cut && "1","goff");
	TH1 * all = (TH1*) t->GetHistogram();
	
	double mean = all->GetMean();
	string s = "" + to_string(mean) + " sdf";
	s.c_str();
	char buf1[1024], buf2[1024], buf3[1024], buf4[1024];
	sprintf(buf1, "pad.y> %f && P>0", mean);
	sprintf(buf2, "pad.y< %f && P<0", mean);
	sprintf(buf3, "pad.y< %f && P>0", mean);
	sprintf(buf4, "pad.y> %f && P<0", mean);
	
    double mean_up1 = t->GetEntries(cut && buf1);
	double mean_up2 = t->GetEntries(cut && buf2);

	double mean_down1 = t->GetEntries(cut && buf3);
	double mean_down2 = t->GetEntries(cut && buf4);
	double mean_N0 = (mean_up1+mean_up2+mean_down1+mean_down2);
    double mean_N1 = mean_up1+mean_up2;
    double mean_N2 = mean_down1+mean_down2;
	double mean_effect = (mean_N1-mean_N2)/mean_N0;
	double mean_error = sqrt(mean_N1*(1.-mean_N1/mean_N0) + mean_N2*(1.0-mean_N2/mean_N0))/mean_N0;
	cout << "asym = " << mean_effect << " +- " << mean_error 
	     << " (" << fabs(mean_effect/mean_error) << ")" << endl;
	

}


void calc(const char * file, const char * cut="", double * e=0, double *de=0)
{
    TFile f(file);
    TTree * t = (TTree*) f.Get("lsrp");
    simple_effect(t,cut,e,de);
}


void add_to_graph (TGraphErrors *g, double d, const char * file, const char * cut = "")
{
    double e, de;
    calc(file,cut, &e, &de);
    int n = g->GetN();
    g->SetPoint(n, d, e);
    g->SetPointError(n, 0, de);
}


TGraphErrors * proceed (const char * prefix, int begin=0, int end=100, const char * cut="")
{
    TGraphErrors * g = new TGraphErrors;
    char buf[1024]; 
    for ( i = begin; i <= end; i += 1 ) 
    {
      sprintf(buf, "%s%d.root", prefix, i);
      if(gSystem->AccessPathName(buf) == 0)
      {
        std::cout << buf << std::endl;
        add_to_graph(g,i,buf,cut);
      }
    }
    g->Draw("ap");
    g->GetXaxis()->SetTitle("converter - GEM distance (mm)");
    g->GetXaxis()->SetDecimals();
    g->GetXaxis()->SetTitleOffset(1.2);
    g->GetYaxis()->SetRangeUser(0,0.2);
    g->GetYaxis()->SetTitle("#Delta <y>/PV (mm)");
    g->SetTitle("E=4.1 GeV, L_{IP}=31 m, L_{air}=20 m, quarz mirror 5 mm, GEM size 50 cm");
    g->SetMarkerStyle(21);
    g->Fit("pol1");
    gStyle->SetOptFit();
    return g;
}

//void proceed2 (const char * prefix, const char * cut = "")
//{
//    TGraphErrors * g = new TGraphErrors;
//
//    for(int i=0;i<26;i++)
//    {
//        char buf[1024];
//        sprintf(buf,"%d.root",i);
//        add_to_graph(g,i,buf,cut);
//    }
//    g->Draw("a*");
//}
