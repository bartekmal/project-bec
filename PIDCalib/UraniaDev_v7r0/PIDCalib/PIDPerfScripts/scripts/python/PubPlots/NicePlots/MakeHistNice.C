{

  gROOT->ProcessLine(".x lhcbstyle.C");
  gROOT->ProcessLine(".L PlotDrawingFunctions.C");

    TFile KasK_20_down("PubPlots_K_as_K_Strip23Val_MagUp_PerfPlots_K_P.root");
  //  TFile KasK_20_up("PubPlots_P_as_P_Strip22_MagUp_PerfPlots_P_P.root");

    TFile PiasK_20_down("PubPlots_Pi_as_Pi_Strip23Val_MagUp_PerfPlots_Pi_P.root");
    //TFile PiasK_20_up("PubPlots_Pi_as_Pi_Strip22_MagUp_PerfPlots_Pi_P.root");

    TH1F* Pion_0_down = (TH1F*)PiasK_20_down.Get("Pi_DLLK >0.0_All");
  TH1F* Pion_5_down = (TH1F*)PiasK_20_down.Get("Pi_DLLK > 5.0_All");
  //TH1F* Pion_0_up = (TH1F*)PiasK_20_up.Get("Pi_DLLp > 0.0_All");
  //TH1F* Pion_5_up = (TH1F*)PiasK_20_up.Get("Pi_DLLp > 5.0_All");

  TH1F* Kaon_0_down = (TH1F*)KasK_20_down.Get("K_DLLK >0.0_All");
  TH1F* Kaon_5_down = (TH1F*)KasK_20_down.Get("K_DLLK > 5.0_All");
  // TH1F* Kaon_0_up = (TH1F*)KasK_20_up.Get("P_DLLp > 0.0_All");
  //TH1F* Kaon_5_up = (TH1F*)KasK_20_up.Get("P_DLLp > 5.0_All");

  //  Pion_0_down->Draw();

  //Pion_0_down->SetBit(TH1::kIsAverage);
  //Pion_5_down->SetBit(TH1::kIsAverage);
  //Pion_0_up->SetBit(TH1::kIsAverage);
  //Pion_5_up->SetBit(TH1::kIsAverage);

  //Kaon_0_down->SetBit(TH1::kIsAverage);
  //Kaon_5_down->SetBit(TH1::kIsAverage);
  //Kaon_0_up->SetBit(TH1::kIsAverage);
  //Kaon_5_up->SetBit(TH1::kIsAverage);


TH1F * Pion_0 = Pion_0_down->Clone();
TH1F * Pion_5 = Pion_5_down->Clone();
TH1F * Kaon_0 = Kaon_0_down->Clone();
TH1F * Kaon_5 = Kaon_5_down->Clone();
//Pion_0->Add(Pion_0_down);
//Pion_5->Add(Pion_5_down);
// Kaon_0->Add(Kaon_0_down);
//Kaon_5->Add(Kaon_5_down);
 TCanvas *c1 = new TCanvas("c1", "c1",14,35,700,500);
 c1->SetLeftMargin(0.1393678);
    c1->SetRightMargin(0.07614943);
    c1->SetTopMargin(0.04872881);
    c1->SetBottomMargin(0.161017);

 PlotFourTH1FwErrors(Kaon_0,Pion_0,Kaon_5, Pion_5);

 leg = new TLegend(0.62,1.4,0.89,1.6);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetShadowColor(0);
  leg->SetTextSize(0.04);
   leg->AddEntry(Pion_0,"#Delta LL(K - #pi) > 0","");
    leg->AddEntry(Pion_5,"#Delta LL(K - #pi) > 5","pe");
  
  TLegend* leg1 = new TLegend(0.55,1.4,0.89,1.6);
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
  lhcbLatex->DrawLatex(25000, 0.8, "#color[2]{K #rightarrow K}");
  lhcbLatex->DrawLatex(25000, 0.30, "#pi #rightarrow K");

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
  lhcb7TeVPaper_L->SetTextSize(0.035);
  lhcb7TeVPaper_L->AddText("#splitline{LHCb}{#scale[0.9]{#sqrt{s} = 13 TeV 25ns Validation Data}}");  
  lhcb7TeVPaper_L->Draw();
  
  c1->Print("plot.eps");

}
