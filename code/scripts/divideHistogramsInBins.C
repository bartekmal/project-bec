TH1D* divideHistograms( const TH1D* h1, const TH1D* h2, const TString hOutName = "", const bool isDR = false ){
  
  //create a histogram with identical binning
  TH1D* hOut = (TH1D*)h1->Clone( hOutName );
  hOut->Sumw2();
  
  //set title
  hOut->SetTitle( isDR ? TString( TString( hOut->GetName() ) + "; Q [GeV]; DR(Q)" ) : TString( TString( hOut->GetName() ) + "; Q [GeV]; C(Q)" ) );

  //divide
  if( !isDR ) hOut->Divide( h1, h2, h2->Integral(), h1->Integral() );
  else hOut->Divide( h1, h2 );

  return hOut;
     
}

void divideHistogramsInBins( const TString file1, const TString h1BaseName, const TString file2, const TString h2BaseName, const TString fileOut, const TString hOutBaseName, const int isDR = 0, const int nrBinsMult = 6, const int nrBinsKt = 0, TString hCommonEndName = "" ){
  
  //prepare input/output files
  auto f1 = new TFile( TString( file1 ),"READ" );
  auto f2 = new TFile( TString( file2 ),"READ" );
  auto fOut = new TFile( TString( fileOut ),"UPDATE" );

  for( int i = 0; i < nrBinsMult; ++i ){

    if ( nrBinsKt != 0 ) {

      for( int j = 0; j < nrBinsKt; ++j ){

        TString h1Name = ( TString( ( TString( h1BaseName+"_" ) += ( i+1 ) ) + "_" ) += ( j+1 ) )  +  hCommonEndName;
        TString h2Name = ( TString( ( TString( h2BaseName+"_" ) += ( i+1 ) ) + "_" ) += ( j+1 ) ) +  hCommonEndName;
        TString hOutName = ( TString( ( TString( hOutBaseName+"_" ) += ( i+1 ) ) + "_" ) += ( j+1 ) ) +  hCommonEndName;

        TH1D* h1 = (TH1D*)f1->Get(h1Name);
        TH1D* h2 = (TH1D*)f2->Get(h2Name);

        auto hOut = divideHistograms( h1, h2, hOutName, isDR ); 
        hOut->Write();

        delete h1;
        delete h2;
        delete hOut;

      }

    } else {

      TString h1Name = ( TString( ( TString( h1BaseName+"_" ) += ( i+1 ) ) + "_0" ) )  +  hCommonEndName;
      TString h2Name = ( TString( ( TString( h2BaseName+"_" ) += ( i+1 ) ) + "_0" ) ) +  hCommonEndName;
      TString hOutName = ( TString( ( TString( hOutBaseName+"_" ) += ( i+1 ) ) + "_0" ) ) +  hCommonEndName;

      TH1D* h1 = (TH1D*)f1->Get(h1Name);
      TH1D* h2 = (TH1D*)f2->Get(h2Name);

      auto hOut = divideHistograms( h1, h2, hOutName, isDR ); 
      hOut->Write();

      delete h1;
      delete h2;
      delete hOut;

    }

  }
  
  //cleanup
  f1->Close();
  f2->Close();
  fOut->Close();

  delete f1;
  delete f2;
  delete fOut;

}