#ifndef UTILS_H
#define UTILS_H

#include "Units.hpp"

#include <TString.h>

namespace HBT
{

    namespace Utils
    {
        // get bin index for the given X
        inline int getBinIndex(const HBT::Units::FloatType &x, const HBT::Units::FloatType &xMin, const HBT::Units::FloatType &binWidth)
        {
            //follow the ROOT histograms bin numbering
            return static_cast<int>(floor((x - xMin) / binWidth)) + 1;
        }

        // get a full histogram name in the given bin
        inline TString getHistogramName(const TString &baseNameBegin, const TString &baseNameEnd, const bool &flagMultBins, const bool &flagMultAndKtBins, const int &binNrMult = 0, const int &binNrKt = 0)
        {
            return !flagMultBins ? baseNameBegin + "_0_0" + baseNameEnd : !flagMultAndKtBins ? TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_0") + baseNameEnd
                                                                                             : TString(TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_") += (binNrKt + 1)) + baseNameEnd;
        }

        // get the fit result name
        inline TString getFitResultName(const TString &hName, const TString &fName)
        {
            return hName + TString("_") + fName;
        }

        // unified method to compare floats
        inline bool compareFloats(const float &a, const float &b, const float &epsilon = 0.001f)
        {
            return (fabs(a - b) < epsilon);
        }

        // set the ROOT style
        inline void setStyle();

    } // namespace Utils

} // namespace HBT

inline void HBT::Utils::setStyle()
{
    gROOT->Reset();              // Reset options
    gStyle->SetPadGridX(kFALSE); // No grid in x (0)
    gStyle->SetPadGridY(kFALSE); // No grid in y (0)
    gStyle->SetOptTitle(kFALSE); // No title
    // gStyle->SetStatBorderSize(0);    // Stats box shadow size (2)
    // gStyle->SetStatColor(18);        // Stats box fill color (0)
    // gStyle->SetStatFont(62);         // Stats font style (62)
    // gStyle->SetStatH(0.1);           // Stats box height
    // gStyle->SetStatW(0.15);          // Stats box width
    // gStyle->SetStatX(0.91);          // Stats box x coordinate
    // gStyle->SetStatY(0.91);          // Stats box y coordinate
    // gStyle->SetStatStyle(1001);      // Stat box fill style
    // gStyle->SetStatTextColor(1);     // Stat text color (1)
    gStyle->SetOptStat(0);           // No statistics (0) (1000001110)
    gStyle->SetOptFit(11);           // No fit box (pcev) (set v=2 to show fixed params)
    gStyle->SetFrameBorderMode(0);   // No red box
    gStyle->SetHistFillColor(0);     // Histogram fill color (0) (18)
    gStyle->SetHistFillStyle(1001);  // Histogram fill style (1001) (0)
    gStyle->SetHistLineColor(kBlue); // Histogram line color (1)
    gStyle->SetHistLineStyle(0);     // Histogram line style (0)
    gStyle->SetHistLineWidth(1);     // Histogram line width (1.0)
    gStyle->SetMarkerStyle(21);      // Marker style (0)
    gStyle->SetMarkerColor(kBlack);  // Marker color (1)
    gStyle->SetMarkerSize(0.8);      // Marker size ()
    gStyle->SetLineColor(kBlack);    // Line color (1)
    gStyle->SetLineWidth(1.0);       // Line width (1.0)
    gStyle->SetTextSize(0.07);       // Text size (1.0)
    gStyle->SetLabelSize(0.03, "x"); // X numbers label size ()
    gStyle->SetLabelSize(0.03, "y"); // Y numbers label size ()
    gStyle->SetTitleSize(0.04, "x"); // X title size ()
    gStyle->SetTitleSize(0.04, "y"); // Y title size ()
    gStyle->SetErrorX(0);            // No errors along x
    gStyle->SetPalette(kRainBow);    // kColorPrintableOnGrey
    gROOT->ForceStyle();
}

#endif /* !UTILS_H */