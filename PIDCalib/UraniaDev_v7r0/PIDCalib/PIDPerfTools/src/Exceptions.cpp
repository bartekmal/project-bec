// $Id: $
// Include files
#include <sstream>

// local
#include "PIDPerfTools/Exceptions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GeneralException
//
// 2011-06-24 : Philip John Hunt
//-----------------------------------------------------------------------------
GeneralException::GeneralException(const std::string& func,
                                   const std::string& msg) throw() 
{
  std::stringstream ss;
  ss << func << ":  " << msg;
  m_fullMsg=ss.str();
}
GeneralException::~GeneralException() throw() {}

const char* GeneralException::what() const throw() 
{
  return m_fullMsg.c_str();
}
