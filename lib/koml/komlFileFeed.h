#ifndef __filefeed_h__
#define __filefeed_h__

#include <stdio.h>

#include "komlFeed.h"

class KOMLFileFeed : public KOMLFeed
{
public:
  KOMLFileFeed( const char* _filename );
  virtual ~KOMLFileFeed();

  virtual KOMLData* read();
  virtual void free( KOMLData* _data );
  
protected:
  FILE *m_fh;  
};

#endif
