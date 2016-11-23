void simple_effect(TTree *t, TCut cut="")
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
	
    TCanvas * c = new TCanvas;

    t->SetLineColor(kBlue);
    t->Draw("pad.y>>hl",cut && "P>0");
    TH1 * hl = (TH1*) t->GetHistogram();
    double y1 = hl->GetMean();
    double dy1 = hl->GetMeanError();
    t->SetLineColor(kRed);
    t->Draw("pad.y>>hr",cut && "P<0","same");
    TH1 * hr = (TH1*) t->GetHistogram();
    double y2 = hr->GetMean();
    double dy2 = hr->GetMeanError();
    std::cout << y1 << " +- " << dy1 << std::endl;
    std::cout << y2 << " +- " << dy2 << std::endl;
    std::cout << y2-y1 << " +- " << sqrt(dy2*dy2+dy1*dy1) << std::endl;
    
	//--------------------------------------------------------------------------
	
	TCanvas * c1 = new TCanvas;
	
	t->SetLineColor(kBlue);
	t->Draw("pad.y>>all",cut && "true");
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
