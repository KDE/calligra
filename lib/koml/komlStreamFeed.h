#ifndef __koml_streamfeed_h__
#define __koml_streamfeed_h__

#include <iostream.h>

#include "komlFeed.h"

class KOMLStreamFeed : public KOMLFeed
{
public:
  KOMLStreamFeed( istream& ); 
  virtual ~KOMLStreamFeed();

  virtual KOMLData* read();
  virtual void free( KOMLData* _data );
  
protected:
  istream &m_in;
};

#endif
