/*
 * =====================================================================================
 *
 *       Filename:  maximum.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/13 12:35:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

{
  TFile f("tmp.root");
  lsrp->Draw("hit.y>>h", "abs(hit.y)<10");
  double max = h->GetMaximum();
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
}


