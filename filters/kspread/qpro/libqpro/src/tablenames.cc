#include <qpro/common.h>

#include <string.h>

#include "qpro/tablenames.h"


// ------------------------------------------------------------------

QpTableNames::QpTableNames()
{
   for( int lIdx=0; lIdx<cNameCnt; ++lIdx )
   {
      cName[lIdx] = 0;
   }
}

QpTableNames::~QpTableNames()
{
   for( int lIdx=0; lIdx<cNameCnt; ++lIdx )
   {
      delete [] cName[lIdx];
      cName[lIdx] = 0;
   }
}

 
void
QpTableNames::name(unsigned pIdx, const char* pName)
{
   if(pIdx < cNameCnt)
   {
      delete [] cName[pIdx];
      cName[pIdx] = strcpy( new char[strlen(pName)+1], pName);
   }
}

const char*
QpTableNames::name(unsigned pIdx)
{
   char* lResult = 0;

   if(pIdx < cNameCnt)
   {
      if( (lResult = cName[pIdx]) == 0 )
      {
         if( pIdx < 26 )
         {
            lResult = cName[pIdx] = new char[2];
            lResult[0] = (char)('A' + pIdx);
            lResult[1] = '\0';
         }
         else
         {
            lResult = cName[pIdx] = new char[2];
            lResult[0] = (char)('A' -1 + pIdx / 26);
            lResult[1] = (char)('A' + pIdx % 26);
            lResult[2] = '\0';
         }
      }
   }

   return lResult;
}

 
int
QpTableNames::allocated(unsigned pIdx)
{
   return (pIdx >= cNameCnt ? 0 : (cName[pIdx] != 0));
}

