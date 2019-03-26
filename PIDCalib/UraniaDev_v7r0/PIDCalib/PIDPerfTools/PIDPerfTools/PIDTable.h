/// $Id: $
#ifndef PIDPERFTOOLS_PIDTABLE_H 
#define PIDPERFTOOLS_PIDTABLE_H 1

// Include files
#include <string>
#include <ostream>
#include <iostream>
#include <list>
#include "PIDPerfTools/Exceptions.h"
#include <Rtypes.h>

// forward declarations
class TH1F;
class TH2F;
class TH3F;
class RooBinning;

/** @class PIDTable PIDTable.h PIDPerfTools/PIDTable.h
 *  
 *
 *  @author Philip John Hunt
 *  @date   2011-06-23
 */
class PIDTable {
public:
  enum Bit{ShowPercentage, SidewaysTable, ShowErrors};
  enum Separator{Top,Bottom,Left,Right,Row,Column,Header};
  
  /// Standard constructor
  PIDTable( ); 
  PIDTable( TH1F* perfHist, RooBinning* binsX );
  
  PIDTable( TH2F* perfHist, 
            RooBinning* binsX, RooBinning* binsY );
  PIDTable( TH3F* perfHist, 
            RooBinning* binsX, RooBinning* binsY,
            RooBinning* binsZ);
  
  void PrintTable(std::ostream& os=std::cout, Bool_t useLaTeXFormat=kFALSE) const;
  
  /// "Setter" functions
  void SetPerfHist(const TH1F* hist);
  void SetPerfHist(const TH2F* hist);
  void SetPerfHist(const TH3F* hist);

  void SetBinning(const RooBinning* binsX, const RooBinning* binsY, 
                  const RooBinning* binsZ=0);
  void SetBinningX(const RooBinning* bins);
  void SetBinningY(const RooBinning* bins);
  void SetBinningZ(const RooBinning* bins);

  void SetXTitle(const char* title);
  void SetYTitle(const char* title);
  void SetZTitle(const char* title);
  void SetEffTitle(const char* title);

  void SetXUnit(const char* unit);
  void SetYUnit(const char* unit);
  void SetZUnit(const char* unit);
  
  void SetXPrecision(UInt_t precision);
  void SetYPrecision(UInt_t precision);
  void SetZPrecision(UInt_t precision); 
  void SetEffPrecision(UInt_t precision);

  void SetXScale(Double_t scale);
  void SetYScale(Double_t scale);
  void SetZScale(Double_t scale);
  
  void SetXWidth(UInt_t width);
  void SetYWidth(UInt_t width);
  void SetZWidth(UInt_t width);
  void SetEffWidth(UInt_t width);
  
  void SetBit(Bit flag, Bool_t bit);
  
  void SetXAlignment(const char* alignment);
  void SetYAlignment(const char* alignment);
  void SetZAlignment(const char* alignment);
  void SetEffAlignment(const char* alignment);

  void SetLabel(const char* label);
  void SetCaption(const char* caption);

  void SetPositioning(const char* pos);
 
  void SetSeparator(Separator type, const char* sep);
  
  /// "Getter" functions
  const TH1F* GetPerfHist1D() const;
  const TH2F* GetPerfHist2D() const;
  const TH3F* GetPerfHist3D() const;

  const RooBinning* GetBinningX() const;
  const RooBinning* GetBinningY() const;
  const RooBinning* GetBinningZ() const;

  const char* GetXTitle() const;
  const char* GetYTitle() const;
  const char* GetZTitle() const;
  const char* GetEffTitle() const;

  const char* GetXUnit() const;
  const char* GetYUnit() const;
  const char* GetZUnit() const;
  
  UInt_t GetXPrecision() const;
  UInt_t GetYPrecision() const;
  UInt_t GetZPrecision() const;
  UInt_t GetEffPrecision() const;

  Double_t GetXScale() const;
  Double_t GetYScale() const;
  Double_t GetZScale() const;
  
  UInt_t GetXWidth() const;
  UInt_t GetYWidth() const;
  UInt_t GetZWidth() const;
  UInt_t GetEffWidth() const;
  
  Bool_t GetBit(Bit flag) const;
  
  const char* GetXAlignment() const;
  const char* GetYAlignment() const;
  const char* GetZAlignment() const;
  const char* GetEffAlignment() const;
  
  const char* GetLabel() const;
  const char* GetCaption() const;
  
  const char* GetPositioning() const;
  
  const char* GetSeparator(Separator type) const;
  
  virtual ~PIDTable( ); ///< Destructor

protected:

private:
  void init(); // initialises data members
  
  void PrintHeader(std::ostream& os, Bool_t useLaTeXFormat) const;
  void PrintFooter(std::ostream& os, Bool_t useLaTeXFormat) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff, Double_t err,
                Double_t xmin, Double_t xmax,
                Double_t ymin, Double_t ymax,
                Double_t zmin, Double_t zmax) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff,
                Double_t xmin, Double_t xmax,
                Double_t ymin, Double_t ymax,
                Double_t zmin, Double_t zmax) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff, Double_t err,
                Double_t xmin, Double_t xmax,
                Double_t ymin, Double_t ymax) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff,
                Double_t xmin, Double_t xmax,
                Double_t ymin, Double_t ymax) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff, Double_t err,
                Double_t xmin, Double_t xmax) const;
  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff,
                Double_t xmin, Double_t xmax) const;
  void PrintAverage(std::ostream& os, Bool_t useLaTeXFormat,
                Double_t eff, Double_t err,
                Bool_t showError,
                Int_t dimension ) const;

  const TH1F* m_perfHist1D;
  const TH2F* m_perfHist2D;
  const TH3F* m_perfHist3D;

  const RooBinning* m_xBinning; // default=NULL
  const RooBinning* m_yBinning; // default=NULL
  const RooBinning* m_zBinning; // default=NULL

  /// Table formatting
  /// if a unit is specified, then it is appended to the title
  /// if no title is specified, then the binning name is used for the x, y
  /// and z ranges, and "Efficiency" for the efficiency
  /// if m_showInPercent is true, then the efficiency errors are show as 
  /// percentages, and the titles are appended by " (%)"
  /// The precision of the x, y, and z ranges defaults to 1 (fixed field)
  /// The precision of the efficiency and its error defaults to 5
  /// The column width is only used in the non-LaTeX table, and defaults to 25
  /// for the efficiency/error, and 20 for the x,y and z ranges
  const char* m_xTitle; 
  const char* m_yTitle; 
  const char* m_zTitle;
  const char* m_effTitle;
  
  const char* m_xUnit; 
  const char* m_yUnit; 
  const char* m_zUnit;

  /// If the scale is greater than zero, then the binning values will 
  /// be rescaled by this amount when printing the x, y or z range
  Double_t m_xScale;
  Double_t m_yScale;
  Double_t m_zScale;
  
  Bool_t m_showInPercent;
  Bool_t m_showErrors;
  
  UInt_t m_xPrecision;
  UInt_t m_yPrecision;
  UInt_t m_zPrecision;
  UInt_t m_effPrecision;
  
  UInt_t m_xWidth;
  UInt_t m_yWidth;
  UInt_t m_zWidth;
  UInt_t m_effWidth;
  
  /// LaTeX table formatting
  const char* m_label; // table label (default="")
  const char* m_caption; // table caption (default="")

  // Flag to produce a sideways table (default is false)
  Bool_t m_sidewaysTable;

  /// Table alignment
  /// Default is centered ("c")
  const char* m_xAlignment;
  const char* m_yAlignment;
  const char* m_zAlignment;
  const char* m_effAlignment;
  
  /// Table positioning
  /// Defaults to htp
  const char* m_positioning;
  
  /// Table separators
  /// The default for all horizontal separators is \hline
  /// The default for all vertical separators is |
  /// For no separator, specify an empty string

  /// horizontal separators
  // The "separator" to use for the table start
  const char* m_topSeparator;
  // The "separator" to use for the table end
  const char* m_bottomSeparator;
  // The separator to use between the header and the body
  const char* m_headerSeparator;
  // the separator to use between the individual table rows
  const char* m_rowSeparator;

  /// vertical separators
  // The "separator" to use for the left edge of the table
  const char* m_leftSeparator;
  // The "separator" to use for the right edge of the table
  const char* m_rightSeparator;
  // The separator to use between the individual table columns
  const char* m_colSeparator;
};
#endif // PIDPERFTOOLS_PIDTABLE_H
