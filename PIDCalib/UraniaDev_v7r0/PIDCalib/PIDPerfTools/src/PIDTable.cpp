// $Id: $
// Include files
#include <sstream>
#include <iostream>
#include <iomanip>

#include "TMath.h"
#include "TH2F.h"
#include "TH3F.h"
#include "RooBinning.h"

// local
#include "PIDPerfTools/PIDTable.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PIDTable
//
// 2011-06-24 : Philip John Hunt
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDTable::PIDTable(  ) 
  : m_perfHist1D(NULL), m_perfHist2D(NULL), m_perfHist3D(NULL), 
    m_xBinning(NULL), m_yBinning(NULL), m_zBinning(NULL) {init();}

//=============================================================================
// Overloaded constructors, initializes variables
//=============================================================================
PIDTable::PIDTable( TH1F* perfHist, RooBinning* binsX)
  : m_perfHist1D(perfHist), m_perfHist2D(NULL), m_perfHist3D(NULL),
    m_xBinning(binsX), m_yBinning(NULL), m_zBinning(NULL) {init();}
PIDTable::PIDTable( TH2F* perfHist, 
                    RooBinning* binsX, RooBinning* binsY )
  : m_perfHist1D(NULL), m_perfHist2D(perfHist), m_perfHist3D(NULL),
    m_xBinning(binsX), m_yBinning(binsY), m_zBinning(NULL) {init();}

PIDTable::PIDTable( TH3F* perfHist, 
                    RooBinning* binsX, RooBinning* binsY,
                    RooBinning* binsZ)
  : m_perfHist1D(NULL), m_perfHist2D(NULL), m_perfHist3D(perfHist),
    m_xBinning(binsX), m_yBinning(binsY), m_zBinning(binsZ) {init();}

//=============================================================================
// Initializer for variables
//=============================================================================
void PIDTable::init() 
{
  m_xTitle="";
  m_yTitle="";
  m_zTitle="";
  m_effTitle="";
  m_xUnit="";
  m_yUnit="";
  m_zUnit="";
  m_showInPercent=kFALSE;
  m_showErrors=kTRUE;
  m_xPrecision=1;
  m_yPrecision=1;
  m_zPrecision=1;
  m_effPrecision=5;
  m_xScale=0;
  m_yScale=0;
  m_zScale=0;
  m_xWidth=20;
  m_yWidth=20;
  m_zWidth=20;
  m_effWidth=25;
  m_label="";
  m_caption="";
  m_sidewaysTable=kFALSE;
  m_xAlignment="c";
  m_yAlignment="c";
  m_zAlignment="c";
  m_effAlignment="c";
  m_positioning="htp";
  m_topSeparator="\\hline";
  m_bottomSeparator="\\hline";
  m_headerSeparator="\\hline";
  m_rowSeparator="\\hline";
  m_leftSeparator="|";
  m_rightSeparator="|";
  m_colSeparator="|";
}

void PIDTable::PrintTable(std::ostream& os, Bool_t useLaTeXFormat) const 
{
  if (!m_perfHist1D&&!m_perfHist2D&&!m_perfHist3D) {
    throw GeneralException("PIDTable::PrintTable", 
                           "No performance histogram specified");
  }
  if (m_perfHist1D&&m_perfHist2D) {
    throw GeneralException("PIDTable::PrintTable",
                           "Can only have one performance histogram");
  }
  if (m_perfHist2D&&m_perfHist3D) {
    throw GeneralException("PIDTable::PrintTable",
                           "Can only have one performance histogram");
  }
  if (m_perfHist1D&&m_perfHist3D) {
    throw GeneralException("PIDTable::PrintTable",
                           "Can only have one performance histogram");
  }
  if (!m_xBinning) {
    throw GeneralException("PIDTable::PrintTable", "No x binning specified");
  }
  if (!m_yBinning&&(m_perfHist2D||m_perfHist3D)) {
    throw GeneralException("PIDTable::PrintTable", "No y binning specified");
  }
  if (!m_zBinning&&m_perfHist3D) {
    throw GeneralException("PIDTable::PrintTable",
       "Request to print table for 3D peformance histogram, but no z binning specified");
  }
  Int_t nbins_x = m_xBinning->numBins();
  Int_t nbins_y = 0;
  Int_t nbins_z = 0;
  if (m_yBinning) {
    nbins_y = m_yBinning->numBins();
  }
  if (m_zBinning) {
    nbins_z = m_zBinning->numBins();
  }
  if (m_perfHist1D) {
    if (nbins_x!=m_perfHist1D->GetNbinsX()) {
      std::stringstream ss;
      ss << "Number of x binning bins (" << nbins_x 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist1D->GetNbinsX() << ")";
      throw GeneralException("PIDTable::PrintTable", ss.str());
    }
  }
  else if (m_perfHist2D) {
    if (nbins_x!=m_perfHist2D->GetNbinsX()) {
      std::stringstream ss;
      ss << "Number of x binning bins (" << nbins_x 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist2D->GetNbinsX() << ")";
      throw GeneralException("PIDTable::PrintTable", ss.str());
    }
    if (nbins_y!=m_perfHist2D->GetNbinsY()) {
      std::stringstream ss;
      ss << "Number of y binning bins (" << nbins_y 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist2D->GetNbinsY() << ")";
      throw GeneralException("PIDTable::PrintTable", ss.str());
    }    
  }
  else {
    if (nbins_x!=m_perfHist3D->GetNbinsX()) {
      std::stringstream ss;
      ss << "Number of x binning bins (" << nbins_x 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist2D->GetNbinsX() << ")";
      throw GeneralException("PIDTable::PrintTable", ss.str());
    }
    if (nbins_y!=m_perfHist3D->GetNbinsY()) {
      std::stringstream ss;
      ss << "Number of y binning bins (" << nbins_y 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist2D->GetNbinsY() << ")";
      throw GeneralException("PIDTable::PrintTable", ss.str());
    }    
    if (nbins_z!=m_perfHist3D->GetNbinsZ()) {
      std::stringstream ss;
      ss << "Number of z binning bins (" << nbins_z 
         << ") does not match the number of bins in the performance histogram ("
         << m_perfHist2D->GetNbinsZ() << ")";
      throw GeneralException("PIDTable::PrintTable",ss.str());
    }
  }
  
  Double_t xmin=0;
  Double_t xmax=0;
  
  Double_t ymin=0;
  Double_t ymax=0;
  
  Double_t zmin=0;
  Double_t zmax=0;
  
  Double_t eff=0;
  Double_t err=0;
  Double_t eff_average=0;
  Double_t err_average=0;
  
  PrintHeader(os, useLaTeXFormat);
  if (m_perfHist1D) {
    for (Int_t xBin=0; xBin<nbins_x; ++xBin) {
      xmin = m_xBinning->binLow(xBin);
      xmax = m_xBinning->binHigh(xBin);
      if (m_xScale>0) {
        xmin*=m_xScale;
        xmax*=m_xScale;
      }
      eff = m_perfHist1D->GetBinContent(xBin+1);
      eff_average += eff;
      if (m_showErrors) {
        err = m_perfHist1D->GetBinError(xBin+1);
        err_average += err*err;
      }
      if (m_showInPercent) {
        eff*=100;
        err*=100;
      }
      if (m_showErrors) {
        PrintRow(os, useLaTeXFormat, eff, err, xmin, xmax);
      }
      else {
        PrintRow(os, useLaTeXFormat, eff, xmin, xmax);
      }
    }
    eff_average = eff_average/((Double_t)nbins_x);
    err_average = TMath::Sqrt(err_average)/((Double_t)nbins_x);
    if (m_showInPercent) {
      eff_average=100;
      err_average=100;
    }
    PrintAverage(os, useLaTeXFormat, eff_average, err_average, m_showErrors, 1);
  } // 1d hist
  else if (m_perfHist2D) {
    for (Int_t xBin=0; xBin<nbins_x; ++xBin) {
      xmin = m_xBinning->binLow(xBin);
      xmax = m_xBinning->binHigh(xBin);
      if (m_xScale>0) {
        xmin*=m_xScale;
        xmax*=m_xScale;
      }
      for (Int_t yBin=0; yBin<nbins_y; ++yBin) {
        ymin = m_yBinning->binLow(yBin);
        ymax = m_yBinning->binHigh(yBin);
        if (m_yScale>0) {
          ymin*=m_yScale;
          ymax*=m_yScale;
        }
        eff = m_perfHist2D->GetBinContent(xBin+1, yBin+1);
        eff_average += eff;
        if (m_showErrors) {
          err = m_perfHist2D->GetBinError(xBin+1, yBin+1);
          err_average += err*err;
        }
        if (m_showInPercent) {
          eff*=100;
          err*=100;
        }
        if (m_showErrors) {
          PrintRow(os, useLaTeXFormat, eff, err, xmin, xmax, ymin, ymax);
        }
        else {
          PrintRow(os, useLaTeXFormat, eff, xmin, xmax, ymin, ymax);
        }
      } /// loop over y bins
    } // loop over x bins

    eff_average = eff_average/((Double_t)(nbins_x*nbins_y));
    err_average = TMath::Sqrt(err_average)/((Double_t)(nbins_x*nbins_y));
    if (m_showInPercent) {
      eff_average=100;
      err_average=100;
    }
    PrintAverage(os, useLaTeXFormat, eff_average, err_average, m_showErrors, 2);
  } // 2d hist
  
  else {
    for (Int_t xBin=0; xBin<nbins_x; ++xBin) {
      xmin = m_xBinning->binLow(xBin);
      xmax = m_xBinning->binHigh(xBin);
      if (m_xScale>0) {
        xmin*=m_xScale;
        xmax*=m_xScale;
      }
      for (Int_t yBin=0; yBin<nbins_y; ++yBin) {
        ymin = m_yBinning->binLow(yBin);
        ymax = m_yBinning->binHigh(yBin);
        if (m_yScale>0) {
          ymin*=m_yScale;
          ymax*=m_yScale;
        }
        for (Int_t zBin=0; zBin<nbins_z; ++zBin) {
          zmin = m_zBinning->binLow(zBin);
          zmax = m_zBinning->binHigh(zBin);
          if (m_zScale>0) {
            zmin*=m_zScale;
            zmax*=m_zScale;
          } 
          
          eff = m_perfHist3D->GetBinContent(xBin+1,yBin+1,zBin+1);
          eff_average += eff;
          if (m_showErrors) {
            err = m_perfHist3D->GetBinError(xBin+1,yBin+1,zBin+1);
            err_average += err*err;
          }
          if (m_showInPercent) {
            eff*=100;
            err*=100;
          }
          if (m_showErrors) {
            PrintRow(os, useLaTeXFormat, eff, err, 
                     xmin, xmax, ymin, ymax, zmin, zmax);
          }
          else {
            PrintRow(os, useLaTeXFormat, eff, 
                     xmin, xmax, ymin, ymax, zmin, zmax);
          }
        } // loop over z bins
      } // loop over y bins
    } // loop over x bins
    eff_average = eff_average/((Double_t)(nbins_x*nbins_y*nbins_z));
    err_average = TMath::Sqrt(err_average)/((Double_t)(nbins_x*nbins_y*nbins_z));
    if (m_showInPercent) {
      eff_average=100;
      err_average=100;
    }
    PrintAverage(os, useLaTeXFormat, eff_average, err_average, m_showErrors, 3);
  } // 3D hist
  PrintFooter(os, useLaTeXFormat);
} // end of function

void PIDTable::PrintHeader(std::ostream& os, Bool_t useLaTeXFormat) const
{
  TString xTitle = m_xTitle;
  if (xTitle.CompareTo("")==0) {
    xTitle=m_xBinning->GetName();
  }
  TString yTitle = m_yTitle;
  if ((m_yBinning!=NULL)&&(yTitle.CompareTo("")==0)) {
    yTitle=m_yBinning->GetName();
  }
  TString zTitle = m_zTitle;
  if ((m_zBinning!=NULL)&&(zTitle.CompareTo("")==0)) {
    zTitle=m_zBinning->GetName();
  }
  if (strlen(m_xUnit)>0) {
    xTitle.Form("%s (%s)", xTitle.Data(), m_xUnit);
  }
  if ((yTitle.CompareTo(""))!=0&&(strlen(m_yUnit)>0)) {
    yTitle.Form("%s (%s)", yTitle.Data(), m_yUnit);
  }
  if ((zTitle.CompareTo(""))!=0&&(strlen(m_zUnit)>0)) {
    zTitle.Form("%s (%s)", zTitle.Data(), m_zUnit);
  }
  TString effTitle=m_effTitle;
  if (effTitle.CompareTo("")==0) {
    effTitle="Efficiency";
  }
  if (m_showInPercent) {
    effTitle+=" (%)";
  }
  if (!useLaTeXFormat) {
    UInt_t tableWidth=0;
    if (m_perfHist3D) {
      tableWidth=m_xWidth+m_yWidth+m_zWidth+m_effWidth;
    }
    else if (m_perfHist2D) {
      tableWidth=m_xWidth+m_yWidth+m_effWidth;
    }
    else {
      tableWidth=m_xWidth+m_effWidth;
    }
    os << std::setfill('-') << std::setw(tableWidth) << "-" 
       << std::setfill(' ') << std::endl;
    os << std::setw(m_xWidth) << xTitle;
    if (m_perfHist2D||m_perfHist3D) {
      os << std::setw(m_yWidth) << yTitle;
    }
    if (m_perfHist3D) {
      os << std::setw(m_zWidth) << zTitle;
    }
    os << std::setw(m_effWidth) << effTitle;
    os << std::endl;
    os << std::setfill('-') << std::setw(tableWidth) << "-" 
       << std::setfill(' ') << std::endl;
  } // "Standard" format
  else {
    if (m_sidewaysTable) {
      os << "\\begin{sidewaystable}[" << m_positioning << "]" << std::endl;
    }
    else {
      os << "\\begin{table}[" << m_positioning << "]" << std::endl;
    }
    if (strlen(m_caption)>0) {
      os << "\\caption{" << m_caption << "}" << std::endl;
    }
    if (strlen(m_label)>0) {
      os << "\\label{" << m_label << "}" << std::endl;
    }
    os << "\\centering" << std::endl;

    os << "\\begin{tabular}{";
    os << m_leftSeparator;
    os << m_xAlignment << m_colSeparator;
    if (m_perfHist2D||m_perfHist3D) {
      os << m_yAlignment << m_colSeparator;
    }
    if (m_perfHist3D) {
      os << m_zAlignment << m_colSeparator;
    }
    os << m_effAlignment << m_rightSeparator;
    os << "}" << std::endl;
    if (strlen(m_topSeparator)>0) {
      os << m_topSeparator << std::endl;
    }
    os << "$" << xTitle << "$ & ";
    if (m_perfHist2D||m_perfHist3D) {
      os << "$" << yTitle << "$ & ";
    }
    if (m_perfHist3D) {
      os << "$" << zTitle << "$ &";
    }
    os << effTitle << "\\\\" << std::endl;
    if (strlen(m_headerSeparator)>0) {
      os << m_headerSeparator << std::endl;
    }
  } // LaTeX format
}
void PIDTable::PrintFooter(std::ostream& os, Bool_t useLaTeXFormat) const
{
  
  if (!useLaTeXFormat) {
    UInt_t tableWidth=0;
    if (m_perfHist3D) {
      tableWidth=m_xWidth+m_yWidth+m_zWidth+m_effWidth;
    }
    else if (m_perfHist2D) {
      tableWidth=m_xWidth+m_yWidth+m_effWidth;
    }
    else {
      tableWidth=m_xWidth+m_effWidth;
    }
    os << std::setfill('-') << std::setw(tableWidth) << "-"
       << std::setfill(' ') << std::endl;
  } // "Standard" format 
  else {
    if (strlen(m_bottomSeparator)>0) {
      os << m_bottomSeparator << std::endl;
    }
    os << "\\end{tabular}" << std::endl;
    if (m_sidewaysTable) {
      os << "\\end{sidewaystable}" << std::endl;
    }
    else {
      os << "\\end{table}" << std::endl;
    }
    os << std::endl << std::endl;
  } // LaTeX format
}

void PIDTable::PrintAverage(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff, Double_t err,
                        Bool_t showError,
                        Int_t dimension ) const {
 
  if (!useLaTeXFormat) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss << "Average";
    os << std::setw(m_xWidth) << ss.str();

    if (dimension > 1) os << std::setw(m_xWidth) << " ";
    if (dimension > 2) os << std::setw(m_yWidth) << " ";
    
    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    if (showError) {
      ss_eff << "+/-";
      ss_eff << std::setprecision(m_effPrecision) << err;
    }
    os << std::setw(m_effWidth) << ss_eff.str();

    os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "Average" << " & ";
    if (dimension > 1) os <<  " & ";
    if (dimension > 2) os <<  " & ";

    os << "$";
    os << std::setprecision(m_effPrecision) << eff;
    if (showError) {
      os << "\\pm";
      os << std::setprecision(m_effPrecision) << err;
    }
    os << "$";
    os << " \\\\" << std::endl;
  }



}


// print 3D row contents (incl. error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff, Double_t err,
                        Double_t xmin, Double_t xmax,
                        Double_t ymin, Double_t ymax,
                        Double_t zmin, Double_t zmax) const {
  if (!useLaTeXFormat) {
    
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_y;
    ss_y.setf(std::ios::fixed, std::ios::floatfield);
    ss_y << "(";
    ss_y << std::setprecision(m_yPrecision) << ymin;
    ss_y << ",";
    ss_y << std::setprecision(m_yPrecision) << ymax;
    ss_y << ")";
    os << std::setw(m_yWidth) << ss_y.str();

    std::stringstream ss_z;
    ss_z.setf(std::ios::fixed, std::ios::floatfield);
    ss_z << "(";
    ss_z << std::setprecision(m_zPrecision) << zmin;
    ss_z << ",";
    ss_z << std::setprecision(m_zPrecision) << zmax;
    ss_z << ")";
    os << std::setw(m_zWidth) << ss_z.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    ss_eff << "+/-";
    ss_eff << std::setprecision(m_effPrecision) << err;
    os << std::setw(m_effWidth) << ss_eff.str();

    os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_yPrecision) << ymin; 
    os << ",";
    os << std::setprecision(m_yPrecision) << ymax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_zPrecision) << zmin; 
    os << ",";
    os << std::setprecision(m_zPrecision) << zmax;
    os << ")$" << " & ";
 
    if (eff!=0&&eff!=0) {
      os << "$";
      os << std::setprecision(m_effPrecision) << eff;
      os << "\\pm";
      os << std::setprecision(m_effPrecision) << err;
      os << "$";
    }
    os << " \\\\" << std::endl;
  }
}


// print 3D row contents (no error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff,
                        Double_t xmin, Double_t xmax,
                        Double_t ymin, Double_t ymax,
                        Double_t zmin, Double_t zmax) const {
  if (!useLaTeXFormat) {
    
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_y;
    ss_y.setf(std::ios::fixed, std::ios::floatfield);
    ss_y << "(";
    ss_y << std::setprecision(m_yPrecision) << ymin;
    ss_y << ",";
    ss_y << std::setprecision(m_yPrecision) << ymax;
    ss_y << ")";
    os << std::setw(m_yWidth) << ss_y.str();

    std::stringstream ss_z;
    ss_z.setf(std::ios::fixed, std::ios::floatfield);
    ss_z << "(";
    ss_z << std::setprecision(m_zPrecision) << zmin;
    ss_z << ",";
    ss_z << std::setprecision(m_zPrecision) << zmax;
    ss_z << ")";
    os << std::setw(m_zWidth) << ss_z.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    os << std::setw(m_effWidth) << ss_eff.str();

    os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_yPrecision) << ymin; 
    os << ",";
    os << std::setprecision(m_yPrecision) << ymax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_zPrecision) << zmin; 
    os << ",";
    os << std::setprecision(m_zPrecision) << zmax;
    os << ")$" << " & ";
 
    if (eff!=0&&eff!=0) {
      os << "$";
      os << std::setprecision(m_effPrecision) << eff;
      os << "$";
    }
    os << " \\\\" << std::endl;
  }
}


// print 2D row contents (incl. error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff, Double_t err,
                        Double_t xmin, Double_t xmax,
                        Double_t ymin, Double_t ymax) const {
  if (!useLaTeXFormat) {
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_y;
    ss_y.setf(std::ios::fixed, std::ios::floatfield);
    ss_y << "(";
    ss_y << std::setprecision(m_yPrecision) << ymin;
    ss_y << ",";
    ss_y << std::setprecision(m_yPrecision) << ymax;
    ss_y << ")";
    os << std::setw(m_yWidth) << ss_y.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    ss_eff << "+/-";
    ss_eff << std::setprecision(m_effPrecision) << err;
    os << std::setw(m_effWidth) << ss_eff.str();

   os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_yPrecision) << ymin; 
    os << ",";
    os << std::setprecision(m_yPrecision) << ymax;
    os << ")$" << " & ";

    os << "$";
    os << std::setprecision(m_effPrecision) << eff;
    os << "\\pm";
    os << std::setprecision(m_effPrecision) << err;
    os << "$ \\\\" << std::endl;
 }
}


// print 2D row contents (no error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff,
                        Double_t xmin, Double_t xmax,
                        Double_t ymin, Double_t ymax) const {
  if (!useLaTeXFormat) {
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_y;
    ss_y.setf(std::ios::fixed, std::ios::floatfield);
    ss_y << "(";
    ss_y << std::setprecision(m_yPrecision) << ymin;
    ss_y << ",";
    ss_y << std::setprecision(m_yPrecision) << ymax;
    ss_y << ")";
    os << std::setw(m_yWidth) << ss_y.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    os << std::setw(m_effWidth) << ss_eff.str();

   os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$(";
    os << std::setprecision(m_yPrecision) << ymin; 
    os << ",";
    os << std::setprecision(m_yPrecision) << ymax;
    os << ")$" << " & ";

    os << "$";
    os << std::setprecision(m_effPrecision) << eff;
    os << "$ \\\\" << std::endl;
 }
}


// print 1D row contents (incl. error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff, Double_t err,
                        Double_t xmin, Double_t xmax) const {
  if (!useLaTeXFormat) {
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    ss_eff << "+/-";
    ss_eff << std::setprecision(m_effPrecision) << err;
    os << std::setw(m_effWidth) << ss_eff.str();

   os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$";
    os << std::setprecision(m_effPrecision) << eff;
    os << "\\pm";
    os << std::setprecision(m_effPrecision) << err;
    os << "$ \\\\" << std::endl;
 }
}


// print 1D row contents (no error)
void PIDTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                        Double_t eff,
                        Double_t xmin, Double_t xmax) const {
  if (!useLaTeXFormat) {
    std::stringstream ss_x;
    ss_x.setf(std::ios::fixed, std::ios::floatfield);
    ss_x << "(";
    ss_x << std::setprecision(m_xPrecision) << xmin;
    ss_x << ",";
    ss_x << std::setprecision(m_xPrecision) << xmax;
    ss_x << ")";
    os << std::setw(m_xWidth) << ss_x.str();

    std::stringstream ss_eff;
    ss_eff.setf(std::ios::fixed, std::ios::floatfield);
    ss_eff << std::setprecision(m_effPrecision) << eff;
    os << std::setw(m_effWidth) << ss_eff.str();

   os << std::endl;
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "$(";
    os << std::setprecision(m_xPrecision) << xmin; 
    os << ",";
    os << std::setprecision(m_xPrecision) << xmax;
    os << ")$" << " & ";

    os << "$";
    os << std::setprecision(m_effPrecision) << eff;
    os << "$ \\\\" << std::endl;
 }
}


//=============================================================================
// "Setter" function 
//=============================================================================
void PIDTable::SetPerfHist(const TH1F* hist) 
{
  if (m_perfHist1D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist1D->GetName()
              << std::endl;
    m_perfHist1D=NULL;
  }
  if (m_perfHist2D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist2D->GetName()
              << std::endl;
    m_perfHist2D=NULL;
  }
  if (m_perfHist3D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist3D->GetName()
              << std::endl;
    m_perfHist3D=NULL;
  }
  m_perfHist1D=hist;
}
void PIDTable::SetPerfHist(const TH2F* hist) 
{
  if (m_perfHist1D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist1D->GetName()
              << std::endl;
    m_perfHist1D=NULL;
  }
  if (m_perfHist2D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist2D->GetName()
              << std::endl;
    m_perfHist2D=NULL;
  }
  if (m_perfHist3D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist3D->GetName()
              << std::endl;
    m_perfHist3D=NULL;
  }
  m_perfHist2D=hist;
}
void PIDTable::SetPerfHist(const TH3F* hist) 
{
  if (m_perfHist1D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist1D->GetName()
              << std::endl;
    m_perfHist1D=NULL;
  }
  if (m_perfHist2D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist2D->GetName()
              << std::endl;
    m_perfHist2D=NULL;
  }
  if (m_perfHist3D) {
    std::cout << "WARNING: Removing old histogram " << m_perfHist3D->GetName()
              << std::endl;
    m_perfHist3D=NULL;
  }
  m_perfHist3D=hist;
}

void PIDTable::SetBinning(const RooBinning* binsX, const RooBinning* binsY, 
                          const RooBinning* binsZ) 
{
  m_xBinning=binsX;
  m_yBinning=binsY;
  m_zBinning=binsZ;
}
void PIDTable::SetBinningX(const RooBinning* bins)
{
  m_xBinning=bins;
}
void PIDTable::SetBinningY(const RooBinning* bins)
{
  m_yBinning=bins;
}
void PIDTable::SetBinningZ(const RooBinning* bins)
{
  m_zBinning=bins;
}

void PIDTable::SetXTitle(const char* title) 
{
  m_xTitle=title;
}
void PIDTable::SetYTitle(const char* title) 
{
  m_yTitle=title;
}
void PIDTable::SetZTitle(const char* title) 
{
  m_zTitle=title;
}
void PIDTable::SetEffTitle(const char* title) 
{
  m_effTitle=title;
}

void PIDTable::SetXUnit(const char* unit)
{
  m_xUnit=unit;
}
void PIDTable::SetYUnit(const char* unit) 
{
  m_yUnit=unit;
}
void PIDTable::SetZUnit(const char* unit)
{
  m_zUnit=unit;
}

void PIDTable::SetXPrecision(UInt_t precision) 
{
  m_xPrecision=precision;
}
void PIDTable::SetYPrecision(UInt_t precision)
{
  m_yPrecision=precision;
}
void PIDTable::SetZPrecision(UInt_t precision) 
{
  m_zPrecision=precision;
}
void PIDTable::SetEffPrecision(UInt_t precision)
{
  m_effPrecision=precision;
}

void PIDTable::SetXScale(Double_t scale) 
{
  m_xScale=scale;
}
void PIDTable::SetYScale(Double_t scale) 
{
  m_yScale=scale;
}
void PIDTable::SetZScale(Double_t scale)
{
  m_zScale=scale;
}

void PIDTable::SetXWidth(UInt_t width) 
{
  m_xWidth=width;
}
void PIDTable::SetYWidth(UInt_t width) 
{
  m_yWidth=width;
}
void PIDTable::SetZWidth(UInt_t width)
{
  m_zWidth=width;
}
void PIDTable::SetEffWidth(UInt_t width) 
{
  m_effWidth=width;
}

void PIDTable::SetBit(Bit flag, Bool_t bit) 
{
  switch (flag) {
  case ShowPercentage:
    m_showInPercent=bit;
    break;
  case SidewaysTable:
    m_sidewaysTable=bit;
    break;
  case ShowErrors:
    m_showErrors=bit;
    break;
  default:
    std::stringstream msg;
    msg << "Bit flag " << flag << " does not name a valid flag";
    throw GeneralException("PIDTable::SetBit", msg.str());
  }
}

void PIDTable::SetXAlignment(const char* alignment) 
{
  m_xAlignment=alignment;
}
void PIDTable::SetYAlignment(const char* alignment) 
{
  m_yAlignment=alignment;
}
void PIDTable::SetZAlignment(const char* alignment) 
{
  m_zAlignment=alignment;
}
void PIDTable::SetEffAlignment(const char* alignment) 
{
  m_effAlignment=alignment;
}

void PIDTable::SetLabel(const char* label) 
{
  m_label=label;
}
void PIDTable::SetCaption(const char* caption) 
{
  m_caption=caption;
}

void PIDTable::SetPositioning(const char* pos) 
{
  m_positioning=pos;
}

void PIDTable::SetSeparator(Separator type, const char* sep) 
{
  switch (type) {
  case Top:
    m_topSeparator=sep;
    break;
  case Bottom:
    m_bottomSeparator=sep;
    break;
  case Header:
    m_headerSeparator=sep;
    break;
  case Row:
    m_rowSeparator=sep;
    break;
  case Left:
    m_leftSeparator=sep;
    break;
  case Right:
    m_rightSeparator=sep;
    break;
  case Column:
    m_colSeparator=sep;
    break;
  default:
   std::stringstream msg;
   msg << "Separator " << type << " does not name a valid separator";
   throw GeneralException("PIDTable::SetSeparator", msg.str());
  }
}

//=============================================================================
// "Getter" functions
//=============================================================================
const TH1F* PIDTable::GetPerfHist1D() const {return m_perfHist1D;}
const TH2F* PIDTable::GetPerfHist2D() const {return m_perfHist2D;}
const TH3F* PIDTable::GetPerfHist3D() const {return m_perfHist3D;}

const RooBinning* PIDTable::GetBinningX() const {return m_xBinning;}
const RooBinning* PIDTable::GetBinningY() const {return m_yBinning;}
const RooBinning* PIDTable::GetBinningZ() const {return m_zBinning;}

const char* PIDTable::GetXTitle() const {return m_xTitle;}
const char* PIDTable::GetYTitle() const {return m_yTitle;}
const char* PIDTable::GetZTitle() const {return m_zTitle;}
const char* PIDTable::GetEffTitle() const {return m_effTitle;}

const char* PIDTable::GetXUnit() const {return m_xUnit;}
const char* PIDTable::GetYUnit() const {return m_yUnit;}
const char* PIDTable::GetZUnit() const {return m_zUnit;}

UInt_t PIDTable::GetXPrecision() const {return m_xPrecision;}
UInt_t PIDTable::GetYPrecision() const {return m_yPrecision;}
UInt_t PIDTable::GetZPrecision() const {return m_zPrecision;}
UInt_t PIDTable::GetEffPrecision() const {return m_effPrecision;}

Double_t PIDTable::GetXScale() const {return m_xScale;}
Double_t PIDTable::GetYScale() const {return m_yScale;}
Double_t PIDTable::GetZScale() const {return m_zScale;}

UInt_t PIDTable::GetXWidth() const {return m_xWidth;}
UInt_t PIDTable::GetYWidth() const {return m_yWidth;}
UInt_t PIDTable::GetZWidth() const {return m_zWidth;}
UInt_t PIDTable::GetEffWidth() const {return m_effWidth;}

Bool_t PIDTable::GetBit(Bit flag) const 
{
  switch (flag) {
  case ShowPercentage: return m_showInPercent;
  case SidewaysTable: return m_sidewaysTable;
  case ShowErrors: return m_showErrors;
  default:
    std::stringstream msg;
    msg << "Bit flag " << flag << " does not name a valid flag";
    throw GeneralException("PIDTable::GetBit", msg.str());
  }
}

const char* PIDTable::GetXAlignment() const 
{
  return m_xAlignment;
}
const char* PIDTable::GetYAlignment() const 
{
  return m_yAlignment;
}
const char* PIDTable::GetZAlignment() const 
{
  return m_zAlignment;
}
const char* PIDTable::GetEffAlignment() const 
{
  return m_effAlignment;
}

const char* PIDTable::GetLabel() const 
{
  return m_label;
}
const char* PIDTable::GetCaption() const
{
  return m_caption;
}

const char* PIDTable::GetPositioning() const 
{
  return m_positioning;
}

const char* PIDTable::GetSeparator(Separator type) const 
{
  switch (type) {
  case Top: return m_topSeparator;
  case Bottom: return m_bottomSeparator;
  case Header: return m_headerSeparator;
  case Row: return m_rowSeparator;
  case Left: return m_leftSeparator;
  case Right: return m_rightSeparator;
  case Column: return m_colSeparator;
  default:
    std::stringstream msg;
    msg << "Separator " << type << " does not name a valid separator";
    throw GeneralException("PIDTable::GetSeparator", msg.str());
  }
}

//=============================================================================
// Destructor
//=============================================================================
PIDTable::~PIDTable() {} 

//=============================================================================
