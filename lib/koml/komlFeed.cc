#include "komlFeed.h"

KOMLData::KOMLData( const char* _data, int _len )
{
  m_pData = _data;
  m_iLen = _len;
}

KOMLData::~KOMLData()
{
  if ( m_pData )
    delete []m_pData;
}

KOMLFeed::KOMLFeed()
{
}

KOMLFeed::~KOMLFeed()
{
}

