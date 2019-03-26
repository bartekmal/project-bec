{

  gROOT->ProcessLine(".x lhcbstyle.C");
  gROOT->ProcessLine(".L PlotDrawingFunctions.C");



  TFile Scan_1_u("PerfCurve_KVsPi_Strip20_MagUp_DLLK>.root");
 TFile Scan_2_u("PerfCurve_KVsPi_Strip22_MagUp_DLLK>.root");
 // TFile Scan_3_d("PerfCurve_KVsPi_Strip20_MagDown_DLLK>K_hasRich>0 && nTracks>200 && nTracks<300.root");
 //TFile Scan_4_d("PerfCurve_KVsPi_Strip20_MagDown_DLLK>K_hasRich>0 && nTracks>300 && nTracks<400.root");

 //  TGraph *Kpi1_d_Scan = (TGraph*)Scan_1_d.Get("KVsPi");
 // TGraph *Kpi2_d_Scan = (TGraph*)Scan_2_d.Get("KVsPi");
  //TGraph *Kpi3_d_Scan = (TGraph*)Scan_3_d.Get("KVsPi");
  //TGraph *Kpi4_d_Scan = (TGraph*)Scan_4_d.Get("KVsPi");
 
  //TFile Scan_1_u("PerfCurve_KVsPi_Strip20_MagUp_DLLK>K_hasRich>0 && nTracks>0 && nTracks<100.root");
  // TFile Scan_2_u("PerfCurve_KVsPi_Strip20_MagUp_DLLK>K_hasRich>0 && nTracks>100 && nTracks<200.root");
  //TFile Scan_3_u("PerfCurve_KVsPi_Strip20_MagUp_DLLK>K_hasRich>0 && nTracks>200 && nTracks<300.root");
  //TFile Scan_4_u("PerfCurve_KVsPi_Strip20_MagUp_DLLK>K_hasRich>0 && nTracks>300 && nTracks<400.root");

    TGraph *Kpi1_u_Scan = (TGraph*)Scan_1_u.Get("KVsPi");
  TGraph *Kpi2_u_Scan = (TGraph*)Scan_2_u.Get("KVsPi");
  //TGraph *Kpi3_u_Scan = (TGraph*)Scan_3_u.Get("KVsPi");
  //TGraph *Kpi4_u_Scan = (TGraph*)Scan_4_u.Get("KVsPi");

  //TGraph *Kpi1_Scan = GetPolAverage(Kpi1_u_Scan, Kpi1_d_Scan);
  //TGraph *Kpi2_Scan = GetPolAverage(Kpi2_u_Scan, Kpi2_d_Scan);
  //TGraph *Kpi3_Scan = GetPolAverage(Kpi3_u_Scan, Kpi3_d_Scan);
  //TGraph *Kpi4_Scan = GetPolAverage(Kpi4_u_Scan, Kpi4_d_Scan);

 Kpi1_u_Scan->SetLineColor(kBlue);
  Kpi1_u_Scan->SetMarkerColor(kBlue);
  Kpi1_u_Scan->SetMarkerSize(1.1);
  Kpi1_u_Scan->SetMarkerStyle(20);
  Kpi2_u_Scan->SetLineColor(kRed);
  Kpi2_u_Scan->SetMarkerColor(kRed);
  Kpi2_u_Scan->SetMarkerSize(1.1);
  Kpi2_u_Scan->SetMarkerStyle(21);
  // Kpi3_Scan->SetLineColor(kBlue);
  //Kpi3_Scan->SetMarkerColor(kBlue);
  //Kpi3_Scan->SetMarkerSize(1.1);
  //Kpi3_Scan->SetMarkerStyle(22);
  //Kpi4_Scan->SetLineColor(kGreen+1);
  //Kpi4_Scan->SetMarkerColor(kGreen+1);
  //Kpi4_Scan->SetMarkerSize(1.1);
  //Kpi4_Scan->SetMarkerStyle(23);
  
 string GraphTitle;
  string PlotTitle;

  GraphTitle = "Kaon_vs_Pion";
  PlotTitle = "title;Kaon ID Efficiency; Pion Mis-ID Efficiency";


 TMultiGraph *mg = new TMultiGraph();
 
  mg->SetTitle(PlotTitle.c_str()); 
  mg->Add(Kpi1_u_Scan);
  mg->Add(Kpi2_u_Scan);
  //  mg->Add(Kpi3_Scan);
  // mg->Add(Kpi4_Scan); 

 leg = new TLegend(0.17,0.52,0.44,0.92);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetShadowColor(0);
  leg->SetTextSize(0.04);
  leg->SetHeader("Initial performance");
  leg->AddEntry(Kpi1_u_Scan,"Early 2012 data (S20)","lp");
  leg->AddEntry(Kpi2_u_Scan,"Validation data 2015","lp");
  //  leg->AddEntry(Kpi3_Scan,"[200,300]","lp");
  //leg->AddEntry(Kpi4_Scan,"[300,400]","lp");


 TCanvas* myCanvas = new TCanvas("myCanvas", "", 800. 600.);
  gPad->Modified();
  
  TPaveText *lhcb8TeVPaper = new TPaveText(0.7,0.2,0.9,0.4,     "BRNDC");
  lhcb8TeVPaper->SetFillColor(0);
  lhcb8TeVPaper->SetTextAlign(12);
  lhcb8TeVPaper->SetBorderSize(0);
  //lhcb7TeVPaper_L->SetTextSize(0.07);                                                                        
  lhcb8TeVPaper->AddText("#splitline{LHCb}{#scale[0.7]{#sqrt{s} = 8 TeV Data}}");
 



  mg->Draw("APL");
  //lhcb8TeVPaper->Draw();
  leg->Draw();
  
  mg->SetMinimum(0.004);
  mg->SetMaximum(0.6);
  myCanvas->SetLogy(1);







  /*
  TFile KasK_20_down("PubPlots_K_as_K_Strip20_MagDown_PerfPlots_KPi_P.root");
  TFile KasK_20_up("PubPlots_K_as_K_Strip20_MagUp_PerfPlots_KPi_P.root");

  TFile PiasK_20_down("PubPlots_Pi_as_K_Strip20_MagDown_PerfPlots_KPi_P.root");
  TFile PiasK_20_up("PubPlots_Pi_as_K_Strip20_MagUp_PerfPlots_KPi_P.root");

  TH1F* Pion_0_down = (TH1F*)PiasK_20_down.Get("Pi_DLLK >0.0_All");
  TH1F* Pion_5_down = (TH1F*)PiasK_20_down.Get("Pi_DLLK > 5.0_All");
  TH1F* Pion_0_up = (TH1F*)PiasK_20_up.Get("Pi_DLLK >0.0_All");
  TH1F* Pion_5_up = (TH1F*)PiasK_20_up.Get("Pi_DLLK > 5.0_All");

  TH1F* Kaon_0_down = (TH1F*)KasK_20_down.Get("K_DLLK >0.0_All");
  TH1F* Kaon_5_down = (TH1F*)KasK_20_down.Get("K_DLLK > 5.0_All");
  TH1F* Kaon_0_up = (TH1F*)KasK_20_up.Get("K_DLLK >0.0_All");
  TH1F* Kaon_5_up = (TH1F*)KasK_20_up.Get("K_DLLK > 5.0_All");

  Pion_0_down->Draw();

  Pion_0_down->SetBit(TH1::kIsAverage);
  Pion_5_down->SetBit(TH1::kIsAverage);
  Pion_0_up->SetBit(TH1::kIsAverage);
  Pion_5_up->SetBit(TH1::kIsAverage);

  Kaon_0_down->SetBit(TH1::kIsAverage);
  Kaon_5_down->SetBit(TH1::kIsAverage);
  Kaon_0_up->SetBit(TH1::kIsAverage);
  Kaon_5_up->SetBit(TH1::kIsAverage);


TH1F * Pion_0 = Pion_0_down->Clone();
TH1F * Pion_5 = Pion_5_down->Clone();
TH1F * Kaon_0 = Kaon_0_down->Clone();
TH1F * Kaon_5 = Kaon_5_down->Clone();
Pion_0->Add(Pion_0_up);
Pion_5->Add(Pion_5_up);
 Kaon_0->Add(Kaon_0_up);
Kaon_5->Add(Kaon_5_up);
 TCanvas *c1 = new TCanvas("c1", "c1",14,35,700,500);
 c1->SetLeftMargin(0.1393678);
    c1->SetRightMargin(0.07614943);
    c1->SetTopMargin(0.04872881);
    c1->SetBottomMargin(0.161017);

 PlotFourTH1FwErrors(Kaon_0,Pion_0,Kaon_5, Pion_5);

 leg = new TLegend(0.62,0.75,0.89,0.92);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetShadowColor(0);
  leg->SetTextSize(0.04);
  leg->AddEntry(Pion_0,"#Delta LL(K - #pi) > 0","");
  leg->AddEntry(Pion_5,"#Delta LL(K - #pi) > 5","pe");
  
  TLegend* leg1 = new TLegend(0.55,0.75,0.89,0.92);
  leg1->SetFillColor(0);
  leg1->SetLineColor(0);
  leg1->SetShadowColor(0);
  leg1->SetTextSize(0.04);
  leg1->AddEntry(Kaon_0,"","");
  leg1->AddEntry(Kaon_5,"","pe");

  lhcbLatex->SetTextSize(0.04);
  lhcbLatex->DrawLatex(70000, 1.270, "#Delta LL(K - #pi) > 0");
  lhcbLatex->DrawLatex(70000, 1.120, "#Delta LL(K - #pi) > 5");

  lhcbLatex->SetTextSize(0.05);
  lhcbLatex->DrawLatex(25000, 0.75, "#color[2]{K #rightarrow K}");
  lhcbLatex->DrawLatex(25000, 0.20, "#pi #rightarrow K");

  TMarker* Marker_Pion_0 = new TMarker(66000, 1.275, 25);
  Marker_Pion_0->Draw();
  TMarker* Marker_Kaon_0 = new TMarker(58500, 1.275, 24);
  Marker_Kaon_0->SetMarkerColor(2);
  Marker_Kaon_0->Draw();
  TMarker* Marker_Pion_5 = new TMarker(66000, 1.125, 21);
  Marker_Pion_5->Draw();
  TMarker* Marker_Kaon_5 = new TMarker(58500, 1.125, 20);
  Marker_Kaon_5->SetMarkerColor(2);
  Marker_Kaon_5->Draw();

  gPad->RedrawAxis();
  TPaveText *lhcb7TeVPaper_L = new TPaveText(lhcbStyle->GetPadLeftMargin() + 0.05,
                                             0.76 - lhcbStyle->SetPadTopMargin(0.05),
                                             lhcbStyle->GetPadLeftMargin() + 0.30,
                                             0.91 - lhcbStyle->SetPadTopMargin(0.05),
                                             "BRNDC");
  lhcb7TeVPaper_L->SetFillColor(0);
  lhcb7TeVPaper_L->SetTextAlign(12);
  lhcb7TeVPaper_L->SetBorderSize(0);
  //lhcb7TeVPaper_L->SetTextSize(0.07);
  lhcb7TeVPaper_L->AddText("#splitline{LHCb}{#scale[0.7]{#sqrt{s} = 8 TeV Data}}");  
  lhcb7TeVPaper_L->Draw();
  
  c1->Print("plot.eps");
  */
}
