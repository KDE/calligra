#ifndef __datasource_h__
#define __datasource_h__

#include <qstring.h>
#include <qmap.h>
#include <kom.h>
#include <komComponent.h>
#include <komAggregate.h>

#include <kdb.h>

#include <kdbDBProperties.h>

class PGSQLDataSource : virtual public KDBDBProperties,
			virtual public KOMAggregate,
			virtual public KOMComponent,
			virtual public ::KDB::IDataSource_skel
{
 public:
  PGSQLDataSource();
  virtual ~PGSQLDataSource();

  virtual void Initialize();
  virtual void UnInitialize();
  virtual KDB::ISession_ptr CreateSession();
  
 protected:
  bool m_bInitialized;
};


#endif


