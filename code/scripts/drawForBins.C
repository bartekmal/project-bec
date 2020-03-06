Double_t funcLevy(Double_t *x, Double_t *par)  {
  return par[0] * (1 + par[1] * exp(-(x[0] * par[2])))
    * (1 + x[0] * par[3]);
}

TFitResultPtr drawFit( TH1D* h, TF1* f, const TString fitOpts = "S0R" ){

  const auto fitResult = h->Fit( f->GetName(),fitOpts );

  f->SetLineColor( kRed );
  f->SetLineWidth( f->GetLineWidth()*0.5 );
  f->Draw( "SAME" );

  return fitResult;

}

void printDescription( const TString dataType, const int currentMultBin, const int currentKtBin ){

  Double_t commPosX = 0.55;
  Double_t commPosY = 0.35;
  TLatex comments;
  comments.SetNDC();
  comments.SetTextSize(0.035);
  comments.DrawLatex( commPosX, commPosY, dataType );
  if( currentMultBin != 0 ){
    comments.DrawLatex( commPosX, commPosY-0.05, TString("Multiplicity bin: ") += currentMultBin );
  }
  if( currentKtBin != 0 ){
    comments.DrawLatex( commPosX, commPosY-0.10, TString("k_{T} bin: ") += currentKtBin );
  }

}

void drawPull( TH1D* hFit, TF1* fFit, const TString drawOpts ="BSAME" ){

  TH1D* hPull = new TH1D( TString( hFit->GetName() ) + "_pull" , "pull;Q [GeV]; pull", hFit->GetNbinsX(), hFit->GetMinimum(), hFit->GetMaximum() );

  //make pull
  for( int i = 1; i < hFit->GetNbinsX() + 1 ; ++i ){

    const Double_t expected = fFit->Eval( hFit->GetBinCenter( i ) );
    const Double_t observed = hFit->GetBinContent( i );
    const Double_t sigma = hFit->GetBinError( i );

    const Double_t pullValue = ( sigma != 0. ) ? ( observed - expected ) / sigma : 0.;

    hPull->SetBinContent( i, pullValue );

  }

  //draw
  hPull->SetLabelSize(0.06,"x");    // X numbers label size ()
  hPull->SetLabelSize(0.06,"y");    // Y numbers label size ()
  hPull->SetTitleSize(0.07,"x");    // X title size ()
  hPull->SetTitleSize(0.07,"y");    // Y title size ()
  hPull->GetYaxis()->SetTitleOffset(0.5);
  //hPull->SetBarWidth( 0.1 );
  hPull->SetFillColor( kBlue );
  hPull->GetYaxis()->SetRangeUser( -6,6 );
  hPull->Draw( drawOpts );
  
  //draw lines
  TLine* lineUp = new TLine( 0, 3, 2, 3 );
  lineUp->SetLineColor( kRed );
  lineUp->Draw( "SAME" );
  TLine* lineDown = new TLine( 0, -3, 2, -3 );
  lineDown->SetLineColor( kRed );
  lineDown->Draw( "SAME" );
  
}

void setStyle(){
  //style
  gROOT->Reset();                    // Reset options
  gStyle->SetPadGridX(kFALSE);       // No grid in x (0)
  gStyle->SetPadGridY(kFALSE);       // No grid in y (0)
  gStyle->SetOptTitle(kFALSE);       // No title
  gStyle->SetStatBorderSize(0);      // Stats box shadow size (2)
  gStyle->SetStatColor(18);          // Stats box fill color (0)
  gStyle->SetStatFont(62);           // Stats font style (62)
  gStyle->SetStatH(0.1);             // Stats box height
  gStyle->SetStatW(0.15);            // Stats box width
  gStyle->SetStatX(0.91);            // Stats box x coordinate
  gStyle->SetStatY(0.91);            // Stats box y coordinate
  gStyle->SetStatStyle(1001);        // Stat box fill style
  gStyle->SetStatTextColor(1);       // Stat text color (1)
  gStyle->SetOptStat(0);    // No statistics (0) (1000001110)
  gStyle->SetOptFit(111);            // No fit box (0) (111)
  gStyle->SetFrameBorderMode(0);     // No red box
  gStyle->SetHistFillColor(0);       // Histogram fill color (0) (18)
  gStyle->SetHistFillStyle(1001);    // Histogram fill style (1001) (0)
  gStyle->SetHistLineColor(kBlue);       // Histogram line color (1)
  gStyle->SetHistLineStyle(0);       // Histogram line style (0)
  gStyle->SetHistLineWidth(1);       // Histogram line width (1.0)
  gStyle->SetMarkerStyle(21);        // Marker style (0)
  gStyle->SetMarkerColor(kBlack);         // Marker color (1)
  gStyle->SetMarkerSize(1.2);       // Marker size ()
  gStyle->SetLineColor(kBlack);           // Line color (1)
  gStyle->SetLineWidth(1.0);           // Line width (1.0)
  gStyle->SetTextSize(0.07);         // Text size (1.0)
  gStyle->SetLabelSize(0.03,"x");    // X numbers label size ()
  gStyle->SetLabelSize(0.03,"y");    // Y numbers label size ()
  gStyle->SetTitleSize(0.04,"x");    // X title size ()
  gStyle->SetTitleSize(0.04,"y");    // Y title size ()
  gStyle->SetErrorX(0);              // No errors along x
  gROOT -> ForceStyle();
}

void drawHistogram( TH1D* h, const int color = kBlue, const TString drawOpts = "ESAME", const Double_t yMin = 0.5, const Double_t yMax = 1.6 ){

    //draw histogram
    h->GetYaxis()->SetRangeUser( yMin,yMax );
    h->SetMarkerColor( color );
    h->SetLineColor( color ); 
    h->GetYaxis()->SetTitleOffset( 1.2 );

    h->Draw( drawOpts ); 
   
}

void drawForBins( const TString inputFile, const TString hLikeNameBase, const TString hUnlikeNameBase, const TString dataType = "", const int nrBinsMult = 6, const int nrBinsKt = 0, TString hCommonEndName = "", const bool doFit = true, const bool doUnlike = true ) {
  
  setStyle();
  
  const bool isMultBinsOnly = ( nrBinsKt == 0 ) ? true : false;
  const int padSize = 1200;
  const int canvasSizeHeight = doFit ? nrBinsMult * ( padSize * 1.5 ) : nrBinsMult * padSize;
  const int canvasSizeWidth = isMultBinsOnly ? padSize : nrBinsKt * padSize;

  TString title = isMultBinsOnly ? TString( "mult") : TString( "kT");

  TCanvas* tc = new TCanvas( title, title, canvasSizeWidth, canvasSizeHeight );
  if ( isMultBinsOnly ) {
    tc->Divide( 1, nrBinsMult );
  } else {
    tc->Divide( nrBinsKt, nrBinsMult );
  }
  
  //get histogram to draw
  TFile* fIn = new TFile( inputFile, "READ" );

  for( int i = 0; i < nrBinsMult; ++i ){

    if ( !isMultBinsOnly ) {

        for( int j = 0; j < nrBinsKt; ++j ){

          tc->cd( ( j + nrBinsKt * i ) + 1 );

          TString hLikeName = ( TString( ( TString( hLikeNameBase+"_" ) += ( i+1 ) ) + "_" ) += ( j+1 ) )  +  hCommonEndName;
          TH1D* hLike = (TH1D*)fIn->Get( hLikeName );     
          
          TString hUnlikeName = ( TString( ( TString( hUnlikeNameBase+"_" ) += ( i+1 ) ) + "_" ) += ( j+1 ) )  +  hCommonEndName;     
          TH1D* hUnlike = (TH1D*)fIn->Get( hUnlikeName );
          
          TLegend* tl = new TLegend( 0.55,0.55,0.85,0.70 );

          if( doFit ){

            TPad* p1 = new TPad( "p1", "p1", 0., 0.33, 1., 1. );
            TPad* p2 = new TPad( "p2", "p2", 0., 0., 1., 0.33 );
            p2->SetBottomMargin( 0.15 );
            p1->Draw();
            p2->Draw();

            p1->cd();
            drawHistogram( hLike, kBlue );
            tl->AddEntry( hLike, "LIKE" ,"pe" );

            TF1* funcLike = new TF1( "funcLike", funcLevy, 0.05, 2.0, 4 );
            funcLike->SetParNames( "N", "#lambda", "R", "#delta" );
            funcLike->SetParameters( 1.0, 0.5, 7., 0. );
            const auto fitResultLike = drawFit( hLike, funcLike, "S0R" );
            tl->AddEntry( funcLike, "LIKE (fit - Levy)" ,"l" );

            if( doUnlike ){
              drawHistogram( hUnlike, kGreen );
              tl->AddEntry( hUnlike, "UNLIKE" ,"pe" );
            }
            
            printDescription( dataType, i+1, j+1 );
            tl->Draw("SAME");

            p2->cd();
            drawPull( hLike, funcLike );
                       
          } else {

            drawHistogram( hLike, kBlue );
            tl->AddEntry( hLike, "LIKE" ,"pe" );

            if( doUnlike ){
              drawHistogram( hUnlike, kGreen );
              tl->AddEntry( hUnlike, "UNLIKE" ,"pe" );
            }

            printDescription( dataType, i+1, j+1 );
            tl->Draw("SAME");

          }
          
        }

    } else {
        
        tc->cd( i + 1 );
                
        TString hLikeName = ( TString( ( TString( hLikeNameBase+"_" ) += ( i+1 ) ) + "_0" ) )  +  hCommonEndName;
        TH1D* hLike = (TH1D*)fIn->Get( hLikeName );   

        TString hUnlikeName = ( TString( ( TString( hUnlikeNameBase+"_" ) += ( i+1 ) ) + "_0" ) )  +  hCommonEndName; 
        TH1D* hUnlike = (TH1D*)fIn->Get( hUnlikeName ); 
                
        TLegend* tl = new TLegend( 0.55,0.55,0.85,0.70 );

        if( doFit ){

          TPad* p1 = new TPad( "p1", "p1", 0., 0.33, 1., 1. );
          TPad* p2 = new TPad( "p2", "p2", 0., 0., 1., 0.33 );
          p2->SetBottomMargin( 0.15 );
          p1->Draw();
          p2->Draw();
          
          p1->cd();
          drawHistogram( hLike, kBlue );
          tl->AddEntry( hLike, "LIKE" ,"pe" );

          TF1* funcLike = new TF1( "funcLike", funcLevy, 0.05, 2.0, 4 );
          funcLike->SetParNames( "N", "#lambda", "R", "#delta" );
          funcLike->SetParameters( 1.0, 0.5, 7., 0. );
          const auto fitResultLike = drawFit( hLike, funcLike, "S0R" );
          tl->AddEntry( funcLike, "LIKE (fit - Levy)" ,"l" );

          if( doUnlike ){
              drawHistogram( hUnlike, kGreen );
              tl->AddEntry( hUnlike, "UNLIKE" ,"pe" );
          }
          
          printDescription( dataType, i+1, 0 );
          tl->Draw("SAME");

          p2->cd();
          drawPull( hLike, funcLike );

        } else {

          drawHistogram( hLike, kBlue );
          tl->AddEntry( hLike, "LIKE" ,"pe" );

          if( doUnlike ){
              drawHistogram( hUnlike, kGreen );
              tl->AddEntry( hUnlike, "UNLIKE" ,"pe" );
          }

          printDescription( dataType, i+1, 0 );
          tl->Draw("SAME");

        }

    }

  }
    
  tc->SaveAs(".pdf");

  fIn->Close();
  delete fIn;

}
