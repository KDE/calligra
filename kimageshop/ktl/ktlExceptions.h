#ifndef _KTLEXCEPTIONS_H_INCLUDED
#define _KTLEXCEPTIONS_H_INCLUDED


#include <string>

namespace KTL
{

class Exception
{
 public:
   Exception(string errMsg);
   
   string getErrorMessage();
   
 private:
   string errMsg;
};

};

#define DECLARE_EXCEPTION(NAME) \
class NAME : public Exception \
{ \
public: \
   ## NAME (string errMsg) \
     : Exception(errMsg) {} \
};

namespace KTL
{

DECLARE_EXCEPTION(NullPointerException)

}

#endif // _EXCEPTIONS_H_INCLUDED
