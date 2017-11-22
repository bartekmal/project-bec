// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CloneCheckerTool.h"

using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : CloneCheckerTool
//
// 2014-02-19 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CloneCheckerTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CloneCheckerTool::CloneCheckerTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<CloneCheckerTool>(this);
  declareProperty("MinChargedProtoPMCAssWeight",  m_minAssWeightCh   = 0.0);
 
}
//=============================================================================
// Destructor
//=============================================================================
CloneCheckerTool::~CloneCheckerTool() {} 


bool CloneCheckerTool::hasClone(const LHCb::Particle* part, 
                               std::vector<const LHCb::Particle*> &vparts)
{

  m_truthTool = NULL;

  if ( ! acceptParticle(part) ) return false;

  const LHCb::MCParticle * mcP(NULL);
  mcP = truthTool()->mcParticle(part->proto()->track(), m_minAssWeightCh);    

  bool isClone = false;

  for(std::vector<const LHCb::Particle*>::const_iterator ip = vparts.begin(); 
      ip != vparts.end(); ip++) {

    if ( part == *ip ) continue;
    if ( part->proto() == (*ip)->proto() ) continue;
    if ( ! acceptParticle(part) ) continue;

    const LHCb::MCParticle * vmcP(NULL);        
    vmcP = truthTool()->mcParticle((*ip)->proto()->track(), m_minAssWeightCh);    

    if ( mcP == vmcP ) 
    {
      isClone = true;
      break;
    }
  }
  
  return isClone;

} 

bool CloneCheckerTool::acceptParticle(const LHCb::Particle* part) 
{
  
  bool accept = false;
  if(part->proto() != NULL  && 
     part->proto()->track() != NULL && 
     part->proto()->track()->type() == Track::Long) 
  {
    accept = true;
  }
  return accept;

}

//=============================================================================
