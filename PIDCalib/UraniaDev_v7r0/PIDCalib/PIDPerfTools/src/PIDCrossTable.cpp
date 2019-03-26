// $Id: $
// Include files
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>

#include "TH2F.h"
#include "RooBinning.h"

// local
#include "PIDPerfTools/PIDCrossTable.h"
#include "PIDPerfTools/Exceptions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PIDCrossTable
//
// 2011-07-03 : Philip Hunt
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDCrossTable::PIDCrossTable(  ) 
  : m_perfHist(NULL), m_rowBinning(NULL), m_colBinning(NULL) {init();}

//=============================================================================
// Overloaded constructors, initializes variables
//=============================================================================
PIDCrossTable::PIDCrossTable( TH2F* perfHist, 
                              RooBinning* rowBins, RooBinning* colBins ) 
  : m_perfHist(perfHist), m_rowBinning(rowBins), m_colBinning(colBins) 
{
  init();
}


//=============================================================================
// Initializer for variables
//=============================================================================
void PIDCrossTable::init() 
{
  m_rowTitle="";
  m_colTitle="";
  m_rowUnit="";
  m_colUnit="";
  m_showInPercent=kFALSE;
  m_printTotals=kFALSE;
  m_showTotalsAsAverages=kTRUE;
  m_showErrors=kTRUE;
  m_rowPrecision=1;
  m_colPrecision=1;
  m_effPrecision=5;
  m_rowScale=0;
  m_colScale=0;
  m_rowWidth=15;
  m_colWidth=20;
  m_label="";
  m_caption="";
  m_sidewaysTable=kFALSE;
  m_rowAlignment="c";
  m_colAlignment="c";
  m_positioning="htp";
  m_topSeparator="\\hline";
  m_bottomSeparator="\\hline";
  m_colHeaderSeparator="\\hline";
  m_rowSeparator="\\hline";
  m_leftSeparator="|";
  m_rightSeparator="|";
  m_rowHeaderSeparator="|";
  m_colSeparator="|";
}

void PIDCrossTable::PrintTable(std::ostream& os, Bool_t useLaTeXFormat) const 
{
  if (!m_perfHist) {
    throw GeneralException("PIDCrossTable::PrintTable", 
                           "No performance histogram specified");
  }
  if (!m_rowBinning) {
    throw GeneralException("PIDCrossTable::PrintTable",
                           "No row binning specified");
  }
  if (!m_colBinning) {
    throw GeneralException("PIDCrossTable::PrintTable",
                           "No column binning specified");
  }
  Int_t nbins_row = m_rowBinning->numBins();
  Int_t nbins_col = m_colBinning->numBins();
  
  if (nbins_row!=m_perfHist->GetNbinsX()) {
    std::stringstream ss;
    ss << "Number of row binning bins (" << nbins_row
       << ") does not match the number of bins in the performance histogram ("
         << m_perfHist->GetNbinsX() << ")";
    throw GeneralException("PIDCrossTable::PrintTable", ss.str());
  }
  if (nbins_col!=m_perfHist->GetNbinsY()) {
    std::stringstream ss;
    ss << "Number of column binning bins (" << nbins_col
       << ") does not match the number of bins in the performance histogram ("
         << m_perfHist->GetNbinsY() << ")";
    throw GeneralException("PIDCrossTable::PrintTable", ss.str());
  }
  
  PrintColHeader(os, useLaTeXFormat);
  
  for (Int_t rowBin=0; rowBin<nbins_row; ++rowBin) {
    PrintRow(os, useLaTeXFormat, rowBin);
  }
  if (m_printTotals) {
    PrintColMarginals(os, useLaTeXFormat);
  }
  PrintFooter(os, useLaTeXFormat);
}

void PIDCrossTable::PrintColHeader(std::ostream& os, Bool_t useLaTeXFormat) 
  const 
{
  TString rowTitle = m_rowTitle;
  if (rowTitle.CompareTo("")==0) {
    rowTitle=m_rowBinning->GetName();
  }
  TString colTitle = m_colTitle;
  if (colTitle.CompareTo("")==0) {
    colTitle=m_colBinning->GetName();
  }
//  if (strlen(m_rowUnit)>0) {
//    rowTitle.Form("%s (%s)", rowTitle.Data(), m_rowUnit);
//  }
  if (strlen(m_colUnit)>0) {
    colTitle.Form("%s (%s)", colTitle.Data(), m_colUnit);
  }
  TString totTitle="Total";
  if (m_showTotalsAsAverages) {
    totTitle="Average";
  }
  Int_t nbins_col = m_colBinning->numBins();
  if (!useLaTeXFormat) {
    UInt_t tableWidth = m_rowWidth+(nbins_col*m_colWidth);
    if (m_printTotals) {
      tableWidth+=m_colWidth;
    }
    
    os << std::setfill('-') << std::setw(tableWidth) << "-"
       << std::setfill(' ') << std::endl;
    os << std::setw(m_rowWidth) << " ";
    os << std::setw((0.5*nbins_col*m_colWidth))
       << colTitle << std::endl;
    
    os << std::setw(m_rowWidth) << rowTitle;

    for (Int_t colBin=0; colBin<nbins_col; ++colBin) {
      std::stringstream ss;
      Double_t min = m_colBinning->binLow(colBin);
      Double_t max = m_colBinning->binHigh(colBin);
      if (m_colScale>0) {
        min*=m_colScale;
        max*=m_colScale;
      }
      ss.setf(std::ios::fixed, std::ios::floatfield);
      ss << "(";
      ss << std::setprecision(m_colPrecision) << min;
      ss << ",";
      ss << std::setprecision(m_colPrecision) << max;
      ss << ")";
      os << std::setw(m_colWidth) << ss.str();
    }
    if (m_printTotals) {
      os << std::setw(m_colWidth) << totTitle;
    }
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
    os << m_rowAlignment << m_rowHeaderSeparator;
    
    for (Int_t colBin=0; colBin<nbins_col; ++colBin) {
      os << m_colAlignment;
      if (colBin!=nbins_col-1) os << m_colSeparator;
    }
    if (m_printTotals) {
      os << m_colSeparator << m_colAlignment;
    }
    os << m_rightSeparator << "}" << std::endl;
    if (strlen(m_topSeparator)>0) {
      os << m_topSeparator << std::endl;
    }
    os << "$" << rowTitle << "$" << " & \\multicolumn{" << nbins_col << "}{" 
       << m_colAlignment << m_colSeparator << "}{$" << colTitle << "$}";
    if (m_printTotals) os << " & ";
    os << "\\\\" << std::endl;
//     os << m_colHeaderSeparator << std::endl;
    os << "$(" << m_rowUnit << ")$";
    for (Int_t colBin=0; colBin<nbins_col; ++colBin) {
      Double_t min = m_colBinning->binLow(colBin);
      Double_t max = m_colBinning->binHigh(colBin);
      if (m_colScale>0) {
        min*=m_colScale;
        max*=m_colScale;
      }
      os.setf(std::ios::fixed, std::ios::floatfield);
      os << " & $(" << std::setprecision(m_colPrecision) << min;
      os << ",";
      os << std::setprecision(m_colPrecision) << max;
      os << ")$";
    }
    if (m_printTotals) {
      os << " & " << totTitle;
    }
    os << "\\\\" << std::endl;
    if (strlen(m_colHeaderSeparator)>0) {
      os << m_colHeaderSeparator << std::endl;
    }
  } // LaTeX format
}

void PIDCrossTable::PrintFooter(std::ostream& os, Bool_t useLaTeXFormat) const 
{
  if (!useLaTeXFormat) {
    Int_t nbins_col = m_colBinning->numBins();
    UInt_t tableWidth=m_rowWidth+(nbins_col*m_colWidth);
    if (m_printTotals) {
      tableWidth+=m_colWidth;
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

void PIDCrossTable::PrintRowHeader(std::ostream& os, Bool_t useLaTeXFormat,
                                   Int_t row) const
{
  Double_t min = m_rowBinning->binLow(row);
  Double_t max = m_rowBinning->binHigh(row);
  if (m_rowScale>0) {
    min*=m_rowScale;
    max*=m_rowScale;
  }
  if (!useLaTeXFormat) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss << "(";
    ss << std::setprecision(m_rowPrecision) << min;
    ss << ",";
    ss << std::setprecision(m_rowPrecision) << max;
    ss << ")";
    os << std::setw(m_rowWidth) << ss.str();
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << "$(" << std::setprecision(m_rowPrecision) << min;
    os << ",";
    os << std::setprecision(m_rowPrecision) << max;
    os << ")$";
  }
}

void PIDCrossTable::PrintEfficiency(std::ostream& os, Bool_t useLaTeXFormat,
                                    Double_t eff) const
{
  if (!useLaTeXFormat) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    if (m_showInPercent) {
      eff*=100;
      ss << "(";
    }
    ss << std::setprecision(m_effPrecision) << eff;
    if (m_showInPercent) {
      ss << ")%";
    }
    os << std::setw(m_colWidth) << ss.str();
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << " & ";
    if (eff==0) return; 
    os << "$";  
    if (m_showInPercent) {
      eff*=100;
      os << "(";
    }
    os << std::setprecision(m_effPrecision) << eff;
    if (m_showInPercent) {
      os << ")%";
    }
    os << "$";
  }
}


void PIDCrossTable::PrintEfficiency(std::ostream& os, Bool_t useLaTeXFormat,
                                    Double_t eff, Double_t err) const
{
  if (!useLaTeXFormat) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    if (m_showInPercent) {
      eff*=100;
      err*=100;
      ss << "(";
    }
    ss << std::setprecision(m_effPrecision) << eff;
    ss << "+/-";
    ss << std::setprecision(m_effPrecision) << err;
    if (m_showInPercent) {
      ss << ")%";
    }
    os << std::setw(m_colWidth) << ss.str();
  }
  else {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << " & ";
    if (eff==0&&err==0) return; 
    os << "$";  
    if (m_showInPercent) {
      eff*=100;
      err*=100;
      os << "(";
    }
    os << std::setprecision(m_effPrecision) << eff;
    os << "\\pm";
    os << std::setprecision(m_effPrecision) << err;
    if (m_showInPercent) {
      os << ")%";
    }
    os << "$";
  }
}

void PIDCrossTable::PrintColMarginals(std::ostream& os, 
                                      Bool_t useLaTeXFormat) const
{
  if (useLaTeXFormat) {
    os << m_rowSeparator << std::endl;
  }
  TString totTitle="Total";
  if (m_showTotalsAsAverages) {
    totTitle="Average";
  }
  if (!useLaTeXFormat) {
    os << std::setw(m_rowWidth) << totTitle;
  }
  else {
    os << totTitle;
  }
  Int_t ncols = m_colBinning->numBins();
  for (Int_t col=0; col<ncols; ++col) {
    PrintColMarginal(os, useLaTeXFormat, col);
  }
  if (useLaTeXFormat) {
    os << " & \\\\";
  }
  os << std::endl;
}

void PIDCrossTable::PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                             Int_t row) const 
{
  Double_t effSum=0;
  Double_t errSum=0;
  Int_t ncols = m_colBinning->numBins();
  if (useLaTeXFormat&&row!=0) {
    os << m_rowSeparator << std::endl;
  }
  PrintRowHeader(os, useLaTeXFormat, row);
  for (Int_t col=0; col<ncols; ++col) {
    Double_t eff = m_perfHist->GetBinContent(row+1, col+1);
    Double_t err=0;
    if (m_showErrors) {
     err = m_perfHist->GetBinError(row+1, col+1);
     PrintEfficiency(os, useLaTeXFormat, eff, err);
    }
    else {
      PrintEfficiency(os, useLaTeXFormat, eff);
    }
    effSum+=eff;
    if (m_showErrors) {
      errSum+=err;
    }
  }
  if (m_printTotals) {
    if (m_showTotalsAsAverages) {
      effSum/=ncols;
      if (m_showErrors) {
        errSum/=ncols;
      }
    }
    if (m_showErrors) {
      PrintEfficiency(os, useLaTeXFormat, effSum, errSum);
    }
    else {
      PrintEfficiency(os, useLaTeXFormat, effSum);
    } 
  }
  if (useLaTeXFormat) {
    os << "\\\\";
  }
  os << std::endl;
}

void PIDCrossTable::PrintColMarginal(std::ostream& os, Bool_t useLaTeXFormat,
                                     Int_t col) const 
{
  Double_t effSum=0;
  Double_t errSum=0;
  Int_t nrows = m_rowBinning->numBins();
  for (Int_t row=0; row<nrows; ++row) {
    Double_t eff = m_perfHist->GetBinContent(row+1, col+1);
    Double_t err = 0;
    if (m_showErrors) {
      err = m_perfHist->GetBinError(row+1, col+1);
    }
    effSum+=eff;
    if (m_showErrors) {
      errSum+=err;
    }
  }
  if (m_showTotalsAsAverages) {
    effSum/=nrows;
    if (m_showErrors) {
      errSum/=nrows;
    }
  }
  if (m_showErrors) {
    PrintEfficiency(os, useLaTeXFormat, effSum, errSum);
  }
  else {
    PrintEfficiency(os, useLaTeXFormat, effSum);
  }
}

//=============================================================================
// "Setter" functions
//=============================================================================
void PIDCrossTable::SetPerfHist(const TH2F* hist) {m_perfHist=hist;}
void PIDCrossTable::SetBinning(const RooBinning* rowBins,
                               const RooBinning* colBins) 
{
  m_rowBinning=rowBins;
  m_colBinning=colBins;
}
void PIDCrossTable::SetRowBinning(const RooBinning* bins) 
{
  m_rowBinning=bins;
}
void PIDCrossTable::SetColBinning(const RooBinning* bins) 
{
  m_colBinning=bins;
}
void PIDCrossTable::SetRowTitle(const char* title) {m_rowTitle=title;}
void PIDCrossTable::SetColTitle(const char* title) {m_colTitle=title;}
void PIDCrossTable::SetRowUnit(const char* unit) {m_rowUnit=unit;}
void PIDCrossTable::SetColUnit(const char* unit) {m_colUnit=unit;}
void PIDCrossTable::SetRowPrecision(UInt_t precision) 
{
  m_rowPrecision=precision;
}
void PIDCrossTable::SetColPrecision(UInt_t precision) 
{
  m_colPrecision=precision;
}
void PIDCrossTable::SetEffPrecision(UInt_t precision) 
{
  m_effPrecision=precision;
}
void PIDCrossTable::SetRowScale(Double_t scale) {m_rowScale=scale;}
void PIDCrossTable::SetColScale(Double_t scale) {m_colScale=scale;}
void PIDCrossTable::SetRowWidth(UInt_t width) {m_rowWidth=width;}
void PIDCrossTable::SetColWidth(UInt_t width) {m_colWidth=width;}
void PIDCrossTable::SetBit(Bit flag, Bool_t bit) 
{
  switch (flag) {
  case ShowPercentage:
    m_showInPercent=bit;
    break;
  case SidewaysTable:
    m_sidewaysTable=bit;
    break;
  case PrintTotals:
    m_printTotals=bit;
    break;
  case ShowTotalsAsAverages:
    m_showTotalsAsAverages=bit;
    break;
  case ShowErrors:
    m_showErrors=bit;
    break;
  default:
    std::stringstream msg;
    msg << "Bit flag " << flag << " does not name a valid flag";
    throw GeneralException("PIDCrossTable::SetBit", msg.str());
  }
}
void PIDCrossTable::SetRowAlignment(const char* alignment) 
{
  m_rowAlignment=alignment;
}
void PIDCrossTable::SetColAlignment(const char* alignment) 
{
  m_colAlignment=alignment;
}
void PIDCrossTable::SetLabel(const char* label) {m_label=label;}
void PIDCrossTable::SetCaption(const char* caption) {m_caption=caption;}
void PIDCrossTable::SetPositioning(const char* pos) {m_positioning=pos;}
void PIDCrossTable::SetSeparator(Separator type, const char* sep) 
{
  switch (type) {
  case Top:
    m_topSeparator=sep;
    break;
  case Bottom:
    m_bottomSeparator=sep;
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
  case ColHeader:
    m_colHeaderSeparator=sep;
    break;
  case RowHeader:
    m_rowHeaderSeparator=sep;
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
const TH2F* PIDCrossTable::GetPerfHist() const {return m_perfHist;}
const RooBinning* PIDCrossTable::GetRowBinning() const {return m_rowBinning;}
const RooBinning* PIDCrossTable::GetColBinning() const {return m_colBinning;}
const char* PIDCrossTable::GetRowTitle() const {return m_rowTitle;}
const char* PIDCrossTable::GetColTitle() const {return m_colTitle;}
const char* PIDCrossTable::GetRowUnit() const {return m_rowUnit;}
const char* PIDCrossTable::GetColUnit() const {return m_colUnit;}
UInt_t PIDCrossTable::GetRowPrecision() const {return m_rowPrecision;}
UInt_t PIDCrossTable::GetColPrecision() const {return m_colPrecision;}
UInt_t PIDCrossTable::GetEffPrecision() const {return m_effPrecision;}
Double_t PIDCrossTable::GetRowScale() const {return m_rowScale;}
Double_t PIDCrossTable::GetColScale() const {return m_colScale;}
UInt_t PIDCrossTable::GetRowWidth() const {return m_rowWidth;}
UInt_t PIDCrossTable::GetColWidth() const {return m_colWidth;}
Bool_t PIDCrossTable::GetBit(Bit flag) const {
  switch (flag) {
  case ShowPercentage: return m_showInPercent;
  case SidewaysTable: return m_sidewaysTable;
  case PrintTotals: return m_printTotals;
  case ShowTotalsAsAverages: return m_showTotalsAsAverages;
  case ShowErrors: return m_showErrors;
  default:
    std::stringstream msg;
    msg << "Bit flag " << flag << " does not name a valid flag";
    throw GeneralException("PIDCrossTable::GetBit", msg.str());
  }
}
const char* PIDCrossTable::GetRowAlignment() const {return m_rowAlignment;}
const char* PIDCrossTable::GetColAlignment() const {return m_colAlignment;}
const char* PIDCrossTable::GetLabel() const {return m_label;}
const char* PIDCrossTable::GetCaption() const {return m_caption;}
const char* PIDCrossTable::GetPositioning() const {return m_positioning;}
const char* PIDCrossTable::GetSeparator(Separator type) const 
{
  switch (type) {
  case Top: return m_topSeparator;
  case Bottom: return m_bottomSeparator;
  case Row: return m_rowSeparator;
  case Left: return m_leftSeparator;
  case Right: return m_rightSeparator;
  case Column: return m_colSeparator;
  case RowHeader: return m_rowHeaderSeparator;
  case ColHeader: return m_colHeaderSeparator;    
  default:
    std::stringstream msg;
    msg << "Separator " << type << " does not name a valid separator";
    throw GeneralException("PIDTable::GetSeparator", msg.str());
  }
}

//=============================================================================
// Destructor
//=============================================================================
PIDCrossTable::~PIDCrossTable() {} 

//=============================================================================
