#ifndef PIDPERFTOOLS_TRKPIDPARAMS_H 
#define PIDPERFTOOLS_TRKPIDPARAMS_H 1

/** @class TrkPIDParams TrkPIDParams.h PIDPerfTools/TrkPIDParams.h
 *  
 *  Simple container class
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2013-02-03
 */
class TrkPIDParams {
public: 

  TrkPIDParams(Float_t a = 0,
             Float_t b = 0,
             Int_t c = 0)
    : TrkPIDEff    (a),
      TrkPIDErr    (b),
      TrkBinNumber (c)
  {}

  virtual ~TrkPIDParams( ) {};

  Float_t TrkPIDEff;
  Float_t TrkPIDErr;
  Float_t TrkPIDEffWeight;
  Float_t TrkPIDErrWeight;
  Int_t   TrkBinNumber;

protected:

private:

};
#endif // PIDPERFTOOLS_TRKPIDPARAMS_H
