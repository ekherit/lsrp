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
	cout << "asym = " << effect << " +- " << error << " (" << fabs(effect/error) << ")" << endl;
	
  TCanvas * c =new TCanvas;
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

  //long N= t->Draw("pad.y>>his",cut && "P>0","goff");
  //TH1 * his = t->GetHistogram();
	//double mean=0;
	//double M2=0;
	//for(int i=0;i<N;i++) { 
	//	mean+=t->GetV1()[i];
	//	M2+=t->GetV1()[i]*t->GetV1()[i];
	//}
	//mean/=N;
	//M2/=N;



	//double RMS=sqrt(M2-mean*mean);
	//double mean_error = RMS/sqrt(N);
	//cout << "mean = " << mean << " +- " << mean_error << " (" << fabs(mean/mean_error) << ")" <<endl;

  //double skew = his->GetSkewness(1);
  //double skew_error = his->GetSkewness(11);
	//cout << "skew = " << skew << " +- " << skew_error << " (" << fabs(skew/skew_error) << ")" <<endl;
	
	
}

