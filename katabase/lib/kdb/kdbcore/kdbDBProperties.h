#ifndef __prop_impl_h__
#define __prop_impl_h__

#include <qstring.h>
#include <qmap.h>

#include <kdb.h>

class KDBDBProperties : virtual public ::KDB::IDBProperties_skel
{
 public:
  KDBDBProperties( const QMap<QString,KDB::PROPINFO> &infos );
  virtual ~KDBDBProperties();
  
  // IDL specified funtions

  virtual bool SetProperty( KDB::PROP& prop );
  virtual bool SetPropertySeq( KDB::PROPSEQ& prop_seq );
  virtual bool GetProperty( KDB::PROP& prop );
  virtual bool GetPropertySeq( KDB::PROPSEQ& prop_seq );
  virtual void GetAllProperties( KDB::PROPSEQ*& prop_seq );
  virtual bool GetPropertyInfo( KDB::PROPINFO& prop_info );
  virtual bool GetPropertyInfoSeq( KDB::PROPINFOSEQ& prop_info_seq );
  virtual void GetAllPropertyInfos( KDB::PROPINFOSEQ*& prop_info_seq );
  
  virtual bool SetPropertyMode( QString _qstrName, KDB::PROPMODE _mode);
  
  // local helper functions

  virtual bool GetPropertyQStringValue( QString _qstrName, QString &_qstrValue );
  virtual bool SetPropertyQStringValue( QString _qstrName, QString _qstrValue );

 protected:
  QMap<QString,CORBA::Any> m_props;
  QMap<QString,KDB::PROPINFO> m_infos;

  KDBDBProperties() {};
};

#endif
















