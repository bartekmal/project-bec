
void PlotFourTH1FwErrors(TH1F* His1, 
                         TH1F* His2,
                         TH1F* His3,
                         TH1F* His4)
{
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(4);
  
  His1->SetLineColor(2);
  His1->SetMarkerColor(2);
  His1->SetMarkerStyle(24);
  His1->SetMarkerSize(0.9);
  His1->GetYaxis()->SetRangeUser(0.0, 1.5);
  His1->SetYTitle("Efficiency");
  His1->SetXTitle("Momentum (MeV/c)");
  //His1->GetYaxis()->SetTitleFont(62);
  His1->Draw("E");
  gPad->Modified();
  His2->SetLineColor(1);
  His2->SetMarkerColor(1);
  His2->SetMarkerStyle(25);
  His2->SetMarkerSize(0.9);
  His2->GetYaxis()->SetRangeUser(0.0, 1.5);
  His2->SetYTitle("Efficiency");
  His2->SetXTitle("Momentum (MeV/c)");
  His2->GetYaxis()->SetTitleFont(62);
  His2->Draw("Esames");
  gPad->Modified();
  His3->SetLineColor(2);
  His3->SetMarkerColor(2);
  His3->SetMarkerStyle(20);
  His3->SetMarkerSize(0.9);
  His3->GetYaxis()->SetRangeUser(0.0, 1.5);
  His3->SetYTitle("Efficiency");
  His3->SetXTitle("Momentum (MeV/c)");
  His3->GetYaxis()->SetTitleFont(62);
  His3->Draw("Esames");
  gPad->Modified();
  His4->SetLineColor(1);
  His4->SetMarkerColor(1);
  His4->SetMarkerStyle(21);
  His4->SetMarkerSize(0.9);
  His4->GetYaxis()->SetRangeUser(0.0, 1.5);
  His4->SetYTitle("Efficiency");
  His4->SetXTitle("Momentum (MeV/c)");
  His4->GetYaxis()->SetTitleFont(62);
    His4->Draw("Esames");
  gPad->Modified();
}


TGraph* GetPolAverage(TGraph * up, TGraph *down){


  // because i don't know how big the array is

  int counter=0;
  for(int i=0; i<100; ++i){

    double x=0; double y=0;

    int point=up->GetPoint(i,x,y);
    if(point != -1){counter ++; cout<<point<<" "<<x<<endl;}
    else break;
  }

  cout<<" the number of points is "<<counter<<endl;

  TGraph * retGraph = new TGraph();
  retGraph->SetMinimum(0.55);
  for(int i=0; i<counter; i++){
    double xup=0; double yup=0; double xdn=0; double ydn=0;

    int uppoint = up->GetPoint(i,xup,yup);
    int dnpoint = up->GetPoint(i,xdn,ydn);

    double xue=up->GetErrorX(i); double yue=up->GetErrorY(i);
    double xde=down->GetErrorX(i); double yde=down->GetErrorY(i);

    double centralx = (xup/xue/xue +xdn/xde/xde)/(1.0/xue/xue + 1.0/xde/xde);
    double centraly = (yup/yue/yue +ydn/yde/yde)/(1.0/yue/yue + 1.0/yde/yde);

    double xerr= sqrt(1.0/(1.0/xue/xue + 1.0/xde/xde));
    double yerr= sqrt(1.0/(1.0/yue/yue + 1.0/yde/yde));

    retGraph->SetPoint(i, centralx, centraly);


  }


  return retGraph;

}


