/// $Id: $
#ifndef PIDPERFTOOLS_PIDCROSSTABLE_H 
#define PIDPERFTOOLS_PIDCROSSTABLE_H 1

// Include files
#include <string>
#include <ostream>
#include <list>
#include "PIDPerfTools/Exceptions.h"

// forward declarations
class TH2F;
class RooBinning;

/** @class PIDCrossTable PIDCrossTable.h PIDPerfTools/PIDCrossTable.h
 *  
 *
 *  @author Philip John Hunt
 *  @date   2011-06-23
 */
class PIDCrossTable {
public:
  enum Bit{ShowPercentage, SidewaysTable, PrintTotals, ShowTotalsAsAverages,
           ShowErrors};
  enum Separator{Top,Bottom,Left,Right,Row,Column,ColHeader,RowHeader};
  
  /// Standard constructor
  PIDCrossTable( ); 
  PIDCrossTable( TH2F* perfHist, 
                 RooBinning* rowBins, RooBinning* colBins);
  
  void PrintTable(std::ostream& os, Bool_t useLaTeXFormat=kFALSE) const;
  
  /// "Setter" functions
  void SetPerfHist(const TH2F* hist);

  void SetBinning(const RooBinning* rowBins, const RooBinning* colBins);
  void SetRowBinning(const RooBinning* bins);
  void SetColBinning(const RooBinning* bins);

  void SetRowTitle(const char* title);
  void SetColTitle(const char* title);

  void SetRowUnit(const char* unit);
  void SetColUnit(const char* unit);
  
  void SetRowPrecision(UInt_t precision);
  void SetColPrecision(UInt_t precision);
  void SetEffPrecision(UInt_t precision);

  void SetRowScale(Double_t scale);
  void SetColScale(Double_t scale);
  
  void SetRowWidth(UInt_t width);
  void SetColWidth(UInt_t width);
  
  void SetBit(Bit flag, Bool_t bit);
  
  void SetRowAlignment(const char* alignment);
  void SetColAlignment(const char* alignment);

  void SetLabel(const char* label);
  void SetCaption(const char* caption);

  void SetPositioning(const char* pos);
 
  void SetSeparator(Separator type, const char* sep);
  
  /// "Getter" functions
  const TH2F* GetPerfHist() const;

  const RooBinning* GetRowBinning() const;
  const RooBinning* GetColBinning() const;

  const char* GetRowTitle() const;
  const char* GetColTitle() const;

  const char* GetRowUnit() const;
  const char* GetColUnit() const;
  
  UInt_t GetRowPrecision() const;
  UInt_t GetColPrecision() const;
  UInt_t GetEffPrecision() const;

  Double_t GetRowScale() const;
  Double_t GetColScale() const;
  
  UInt_t GetRowWidth() const;
  UInt_t GetColWidth() const;
  
  Bool_t GetBit(Bit flag) const;
  
  const char* GetRowAlignment() const;
  const char* GetColAlignment() const;
  
  const char* GetLabel() const;
  const char* GetCaption() const;
  
  const char* GetPositioning() const;
  
  const char* GetSeparator(Separator type) const;
  
  virtual ~PIDCrossTable( ); ///< Destructor

protected:

private:
  void init(); // initialises data members

  void PrintColHeader(std::ostream& os, Bool_t useLaTeXFormat) const;
  
  void PrintFooter(std::ostream& os, Bool_t useLaTeXFormat) const;

  void PrintRowHeader(std::ostream& os, Bool_t useLaTeXFormat,
                      Int_t row) const;

  void PrintColMarginals(std::ostream& os, Bool_t useLaTeXFormat) const;

  void PrintEfficiency(std::ostream& os, Bool_t useLaTeXFormat,
                       Double_t eff, Double_t err) const;
  void PrintEfficiency(std::ostream& os, Bool_t useLaTeXFormat,
                       Double_t eff) const;

  void PrintRow(std::ostream& os, Bool_t useLaTeXFormat,
                Int_t row) const;

  void PrintColMarginal(std::ostream& os, Bool_t useLaTeXFormat,
                        Int_t col) const;

  const TH2F* m_perfHist;

  const RooBinning* m_rowBinning; // default=NULL
  const RooBinning* m_colBinning; // default=NULL

  /// Table formatting
  /// if a unit is specified, then it is appended to the title
  /// if no title is specified, then the binning name is used for the 
  /// row and column ranges
  /// if m_showInPercent is true, then the efficiency errors are show as 
  /// percentages, and the titles are appended by " (%)"
  /// The precision of the column and row ranges defaults to 1 (fixed field)
  /// The precision of the efficiency and its error defaults to 5
  /// The column width is only used in the non-LaTeX table, and defaults to 25
  /// for the column width, and 20 for the "row width" (1st column) 
  const char* m_rowTitle; 
  const char* m_colTitle;
  
  const char* m_rowUnit; 
  const char* m_colUnit;

  /// If the scale is greater than zero, then the binning values will 
  /// be rescaled by this amount when printing the row or column ranges
  Double_t m_rowScale;
  Double_t m_colScale;
  
  Bool_t m_showInPercent;
  Bool_t m_showErrors;
  
  // if true, print the row and column marginals
  Bool_t m_printTotals;

  // if true, show marginal averages instead of totals
  Bool_t m_showTotalsAsAverages;

  UInt_t m_rowPrecision;
  UInt_t m_colPrecision;
  UInt_t m_effPrecision;
  
  UInt_t m_rowWidth;
  UInt_t m_colWidth;
  
  /// LaTeX table formatting
  const char* m_label; // table label (default="")
  const char* m_caption; // table caption (default="")

  // Flag to produce a sideways table (default is false)
  Bool_t m_sidewaysTable;

  /// Table alignment
  /// Default is centered ("c")
  const char* m_rowAlignment;
  const char* m_colAlignment;
  
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
  // The separator to use between the "column" header and the body
  const char* m_colHeaderSeparator;
  // the separator to use between the individual table rows
  const char* m_rowSeparator;

  /// vertical separators
  // The "separator" to use for the left edge of the table
  const char* m_leftSeparator;
  // The "separator" to use for the right edge of the table
  const char* m_rightSeparator;
  // The separator to use between the "row" header and the body
  const char* m_rowHeaderSeparator;
  // The separator to use between the individual table columns
  const char* m_colSeparator;
};
#endif // PIDPERFTOOLS_PIDCROSSTABLE_H
