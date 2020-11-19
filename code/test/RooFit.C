#include <RooFit.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooDataSet.h>
#include <RooPlot.h>
#include <RooGenericPdf.h>
#include <RooFormulaVar.h>

#include <TFitResult.h>
#include <TF1.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TMath.h>

using namespace RooFit;

int test()
{
    RooRealVar x("x", "x", 0, 1);
    RooRealVar N("N", "N", 1, 0, 10000);

    auto f = new TF1("f", "x", 0, 1);

    RooAbsReal *funcAbs = bindFunction(f, x);
    RooAbsPdf *funcPdf = bindPdf(f, x);

    RooAbsReal *funcAbsNorm = new RooProduct("f", "f", RooArgSet(N, funcAbs));

    auto hist = funcPdf->generate(x, 1000)->binnedClone();

    //plot
    auto frame = x.frame();
    hist->plotOn(frame);

    // fit
    funcAbsNorm->chi2FitTo(*hist);

    // show
    funcAbsNorm->plotOn(frame, LineColor(kRed)); //Normalization(0.002)
    funcAbsNorm->Print("t");

    //save canvas
    TCanvas tc("c", "c", 800, 800);
    tc.Draw();
    frame->Draw();
    tc.SaveAs(".pdf");

    return 0;
}