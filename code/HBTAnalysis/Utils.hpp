#ifndef UTILS_H
#define UTILS_H

#include "Units.hpp"

#include <TString.h>
#include <TObject.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TPaveText.h>

#include <string>
#include <iostream>

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

        // get name of bkg scaling function
        inline TString getBkgScalingFuncName(const TString &fName, const bool &isMultBinsOnly, const unsigned int &binNrKt = 0)
        {
            return fName + "_0_" + (isMultBinsOnly ? 0 : binNrKt + 1);
        }

        // unified method to compare floats
        inline bool compareFloats(const float &a, const float &b, const float &epsilon = 0.001f)
        {
            return (fabs(a - b) < epsilon);
        }

        // get a fresh copy (clone) of the given object in a ROOT file
        inline TObject *getObjCopy(TFile *&inputFile, const std::string &objName)
        {
            if (!inputFile)
            {
                return nullptr;
            }
            const auto objTmp = inputFile->Get(objName.c_str());
            if (!objTmp)
            {
                return nullptr;
            }
            else
            {
                // make a unique name
                return objTmp->Clone((inputFile->GetName() + std::string("_") + objName).c_str());
            }
        }

        // check if number is in range (inclusive)
        inline bool numberInRange(const float &number, const float &min, const float &max)
        {
            return (number >= min && number <= max);
        }

        // get a valid (existing and non-empty) histogram from a file
        inline TH1 *getValidHist(TFile *&inputFile, const std::string &objName)
        {
            const auto tmpHist = (TH1 *)(getObjCopy(inputFile, objName));

            if (!tmpHist)
            {
                std::cout << "Histogram not found: " << objName.c_str() << std::endl;
                return nullptr;
            }
            else if (tmpHist->GetEntries() == 0)
            {
                std::cout << "Histogram is empty: " << objName.c_str() << std::endl;
                return nullptr;
            }
            else
            {
                return tmpHist;
            }
        }

        // split string and add to TPaveText as lines
        inline void addMultilineText(const std::string &textToAdd, TPaveText *&paveText, const char delimiter = ';')
        {
            std::istringstream tmpStringStream(textToAdd.c_str());
            std::string tmpString;
            while (std::getline(tmpStringStream, tmpString, delimiter))
            {
                paveText->AddText(tmpString.c_str());
            }
        }

        // get data type in readable format (string for labels etc)
        inline std::string dataTypeAsString(const TString &dataType)
        {
            if (!dataType.CompareTo("RD_pPb"))
            {
                return std::string("#font[12]{p}Pb data");
            }
            else if (!dataType.CompareTo("RD_Pbp"))
            {
                return std::string("Pb#font[12]{p} data");
            }
            else if (!dataType.CompareTo("MC_pPb"))
            {
                return std::string("#font[12]{p}Pb sim");
            }
            else if (!dataType.CompareTo("MC_Pbp"))
            {
                return std::string("Pb#font[12]{p} sim");
            }
            else
            {
                return std::string("unknown data type");
            }
        }

        // get histogram type in readable format (string for labels etc)
        inline std::string histTypeAsString(const TString &histType)
        {
            if (!histType.CompareTo("LIKE"))
            {
                return std::string("SS");
            }
            else if (!histType.CompareTo("UNLIKE"))
            {
                return std::string("OS");
            }
            else
            {
                return std::string("unknown hist type");
            }
        }
    } // namespace Utils

} // namespace HBT

#endif /* !UTILS_H */