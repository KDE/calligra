#include "kdbDBProperties.h"

#include <kded_utils.h>

KDBDBProperties::KDBDBProperties( const QMap<QString,KDB::PROPINFO> &_infos )
{
  m_infos = _infos;
}


KDBDBProperties::~KDBDBProperties()
{
}


bool KDBDBProperties::SetProperty( KDB::PROP& _prop )
{
  // We store the name of the property in an automatic qstring
  // for convenience

  QString qstrName = _prop.wstrName;

  // We first find out whether the property is documented
  // (if it has a corresponding PropertyInfo)
  // we use a _var_type for automatic memory management

  KDB::PROPINFO_var varInfo = new KDB::PROPINFO;

  varInfo->wstrName = _prop.wstrName;

  if ( GetPropertyInfo( varInfo ) ) {

    // yes, it is documented.
    // now we see if this property is not writable
    // if it is readonly we throw an exception

    if ( varInfo->mode == KDB::READ ) {

      // yes, it is readonly, setting the property is not allowed
      
      _prop.status = KDB::ACCESS_DENIED;
      
      return FALSE;
      
    } else {
      
      // it is writable
      // remark: the Any can just be assigned, AFAIK CORBA makes
      // a deep copy
      
      // name is a QString and is properly dealt with by Qt (shallow
      // copy and deep copy when first change is done)
      
      m_props.insert( qstrName, _prop.aValue );
      
      varInfo->status = KDB::OK;
      
      return TRUE;
      
    }
  }
  
  // apparently, the user wants to set an undocumented
  // property. We consider this a feature, not a bug
  // remark: this behavior _could_ change in future versions
  
  m_props.insert( qstrName, _prop.aValue );
  
  varInfo->status = KDB::OK;
  
  return TRUE;
}


bool KDBDBProperties::SetPropertySeq( KDB::PROPSEQ& _prop_seq )
{
  // we implement this method using SetProperty
  // on the first exception, we return with a new exception
  // so, every property is set up to but not including the first
  // readonly property
  
  bool result = true;
  
  for( unsigned int i = 0;  i != _prop_seq.count(); i++ ) {
    
    result = result && SetProperty( _prop_seq[i] );
    
  }
  
  return result;
}


bool KDBDBProperties::GetProperty( KDB::PROP& _prop )
{
  // We store the name of the property in an automatic qstring
  // for convenience
  
  QString qstrName = _prop.wstrName;
  
  // since this is the only bullet proof way for a user to know whether
  // a property (value) exists, we do not throw an exception when
  // the property does not exist
  // we do not consider that to be an 'exceptional' status

  // we first check if the user has read access to this property
  
  QMap<QString,KDB::PROPINFO>::Iterator itInfo = m_infos.find( qstrName );
  
  if ( itInfo == m_infos.end() )
    {
      
      // officially this property doesn't exist (undocumented property)
      // if we have a value for it we return it
      
      QMap<QString,CORBA::Any>::Iterator it = m_props.find( qstrName );
      
      // it is end() when the property does not exist
      
      if ( it != m_props.end() ) {
	
	// the property exists
	// we fill in the PROP struct
	
	_prop.aValue = it.data();
	_prop.status = KDB::OK;
	
	return TRUE;
	
      } else {
	
	// it was end(), the property has no value
	// we tell the user this be setting the status field
	// we return false
	
	_prop.status = KDB::UNKNOWN;
	
	return FALSE;
	
      }
    } else {
      
      // the property officially exists
      
      if ( itInfo.data().mode == KDB::WRITE ) {
	
	// the property is not readable
	
	_prop.status = KDB::ACCESS_DENIED;
	
	return FALSE;
	
      } else {
	
	// the property is documented and is readable
	
	QMap<QString,CORBA::Any>::Iterator it = m_props.find( qstrName );
	
	// it is end() when the property does not exist
	
	if ( it != m_props.end() ) {
	  
	  // the property exists
	  // we allocate because _prop is an 'out' parameter
	  
	  _prop.aValue = it.data();
	  _prop.status = KDB::OK;
	  
	  return TRUE;
	  
	} else {
	  
	  // it was end(), the property has no value
	  // we tell the user this be setting the status field
	  // we return false
	  
	  _prop.status = KDB::UNKNOWN;
	  
	  return FALSE;
	}
      }
    }
}


bool KDBDBProperties::GetPropertySeq( KDB::PROPSEQ& _prop_seq )
{
  // we implement this method using GetProperty
  
  bool result = true;
  
  for( unsigned int i = 0;  i != _prop_seq.count(); i++ ) {
    
    result = result && GetProperty( _prop_seq[i] );
    
  }
  
  return result;
}


void KDBDBProperties::GetAllProperties( KDB::PROPSEQ*& _prop_seq )
{
  // what do we return ? -> every set property that is not write only
  // undocumented properties included
  
  KDB::PROPSEQ *seqResult = new KDB::PROPSEQ;
  
  unsigned int iIdx = 0;
  unsigned int iLength = 0;
  
  // we iterate through the set properties
  
  QMap<QString,CORBA::Any>::Iterator itProps;
  QMap<QString,KDB::PROPINFO>::Iterator itInfos;
  
  for( itProps = m_props.begin(); itProps != m_props.end(); itProps++ ) {
    
    itInfos = m_infos.find( itProps.key() );
    
    if ( itInfos != m_infos.end() ) {

      // itProps points to a documented property

      if ( itInfos.data().mode != KDB::WRITE ) {

	// this property can be read

        KDB::PROP data;

        data.wstrName = itProps.key();
        data.aValue = itProps.data();

        seqResult->append( data );

	iIdx++;
	
      } // if ( itInfos.data().mode != KDB::WRITE )
      
    } else {
      
      // this property is undocumented so we consider it readable

      KDB::PROP data;
 
      data.wstrName = itProps.key();
      data.aValue = itProps.data();
 
      seqResult->append( data );
      
      iIdx++;
      
    } // if ( itInfos != m_infos.end() )
    
  } // for( itProps = m_props.begin(); itProps != m_props.end(); itProps++ )
  
  _prop_seq = seqResult;
  
  return;
  
}


bool KDBDBProperties::GetPropertyInfo( KDB::PROPINFO& _prop_info )
{
  // We store the name of the property in an automatic qstring
  // for convenience
  
  QString qstrName = _prop_info.wstrName;

  // now we look for PropertyInfo record in m_infos

  QMap<QString,KDB::PROPINFO>::Iterator it;

  it = m_infos.find( qstrName );

  if ( it == m_infos.end() ) {

    // for that property name was no property info available

    _prop_info.status = KDB::UNKNOWN;

    return FALSE;

  }

  // we fill in a new (allocated) PropertyInfo with this

  _prop_info.strType = it.data().strType;
  _prop_info.wstrDescr = it.data().wstrDescr;
  _prop_info.mode = it.data().mode;
  _prop_info.status = KDB::OK;
  
  // now that _prop_info is properly filled in, we can safely return
  
  return TRUE;
}


bool KDBDBProperties::GetPropertyInfoSeq( KDB::PROPINFOSEQ& _prop_info_seq )
{
  // we implement this method using GetPropertyInfo
  
  bool result = TRUE;
  
  for( unsigned int i = 0; i != _prop_info_seq.count(); i++ ) {
    
    result = result && GetPropertyInfo( _prop_info_seq[i] );
    
  }
  
  return result;
}


void KDBDBProperties::GetAllPropertyInfos( KDB::PROPINFOSEQ*& _prop_info_seq )
{
  // we query the qmap to know the number of properties that are set

  unsigned long nrInfos = m_infos.count();

  // make a sequence with the desired number of items

  KDB::PROPINFOSEQ *ptrInfoSeq = new KDB::PROPINFOSEQ;

  ptrInfoSeq->clear();

  // we iterate through the map

  QMap<QString,KDB::PROPINFO>::Iterator it;

  unsigned int index = 0; // this holds the item we are currently setting

  for( it = m_infos.begin(); it != m_infos.end(); it++ ) {

    KDB::PROPINFO propinfo;

    propinfo.wstrName = it.data().wstrName;
    propinfo.strType = it.data().strType;
    propinfo.wstrDescr = it.data().wstrDescr;
    propinfo.mode = it.data().mode;
    propinfo.status = KDB::OK;

    ptrInfoSeq->append( propinfo );

    index++;
  }
  
  _prop_info_seq = ptrInfoSeq;
  
  return;
}


bool KDBDBProperties::SetPropertyMode( QString _qstrName, KDB::PROPMODE _mode )
{
  QMap<QString,KDB::PROPINFO>::Iterator it;
  
  it = m_infos.find( _qstrName );
  
  if ( it == m_infos.end() ) {
    
    return FALSE;
    
  } else {
    
    it.data().mode = _mode;
    
    return TRUE;
    
  }
}


bool KDBDBProperties::SetPropertyQStringValue( QString _qstrName, QString _qstrValue )
{
  KDB::PROP prop;

  prop.wstrName = _qstrName;
  prop.aValue <<= _qstrValue;

  return SetProperty( prop );
}

bool KDBDBProperties::GetPropertyQStringValue( QString _qstrName, QString &_qstrValue )
{
  KDB::PROP prop;

  prop.wstrName = _qstrName;

  bool bResult;
  
  bResult = GetProperty( prop );
  
  if ( bResult == FALSE ) {
    
    return FALSE;

  }

  QString  wstrValue;

  if ( prop.aValue >>= wstrValue ) {

    _qstrValue = wstrValue;

    return TRUE;
    
  } else {

    return FALSE;

  }
}



