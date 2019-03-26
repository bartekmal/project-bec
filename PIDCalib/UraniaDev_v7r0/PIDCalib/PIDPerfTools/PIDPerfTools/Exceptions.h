// $Id: $
#ifndef PIDPERFTOOLS_EXCEPTIONS_H
#define PIDPERFTOOLS_EXCEPTIONS_H 1

// Include files
#include <string>
#include <exception>

/** @class Exceptions Exceptions.h PIDPerfTools/Exceptions.h
 *
 *
 *  @author Philip Hunt
 *  @date   2011-07-03
 */
class GeneralException: public std::exception
{
public:
  explicit GeneralException(const std::string& func,
                            const std::string& msg) throw();
  virtual ~GeneralException() throw();
  const char* what() const throw() override;
private:
  std::string m_fullMsg;
};

#endif // PIDPERFTOOLS_EXCEPTIONS_H
