#ifndef __koml_feed_h__
#define __koml_feed_h__

class KOMLData
{
public:
  KOMLData( const char*, int len );
  virtual ~KOMLData();

  int length() { return m_iLen; }
  const char* data() { return m_pData; }
  
protected:
  const char *m_pData;
  int m_iLen;
};

class KOMLFeed
{
public:
  KOMLFeed();
  virtual ~KOMLFeed();
  
  virtual KOMLData* read() = 0L;
  virtual void free( KOMLData* _data ) = 0L;
};

#endif
