#ifndef CLONECHECKERTOOL_H 
#define CLONECHECKERTOOL_H 1

// Include files
// from Gaudi
#include "Event/Particle.h"
#include "Event/Track.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "MCInterfaces/IMCParticleSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "GaudiAlg/GaudiTool.h"

using namespace LHCb;

static const InterfaceID IID_CloneCheckerTool ( "CloneCheckerTool", 1, 0 );

/** @class CloneCheckerTool CloneCheckerTool.h
 *   
 *
 *  @author Mariusz Witek
 *  @date   2014-02-19
 */
class CloneCheckerTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_CloneCheckerTool; }

  /// Standard constructor
  CloneCheckerTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~CloneCheckerTool( ); ///< Destructor

  bool hasClone(const LHCb::Particle* part, 
                std::vector<const LHCb::Particle*> &vparts);
  
  double addtwo(double x) 
  {
    return x+2;
    
  }
  

protected:

private:

  const Rich::Rec::MC::IMCTruthTool * truthTool() const {
    if(!m_truthTool) {
      m_truthTool = 
        tool<Rich::Rec::MC::IMCTruthTool>("Rich::Rec::MC::MCTruthTool",
                                          "RichRecMCTruth");
    }
    return m_truthTool;
  }

  bool acceptParticle(const LHCb::Particle* part);

  double m_minAssWeightCh;
  mutable const Rich::Rec::MC::IMCTruthTool* m_truthTool;

};
#endif // CLONECHECKERTOOL_H
