#include <ktlExceptions.h>

KTL::Exception::Exception(string errMsg)
  : errMsg(errMsg)
{
}

string KTL::Exception::getErrorMessage()
{
   return errMsg;
}
