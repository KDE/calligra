#ifndef __kofactory_h__
#define __kofactory_h__

#include "koffice.h"

#define KOFFICE_DOCUMENT_FACTORY( DOC, NAME ) \
class NAME : virtual public KOffice::DocumentFactory_skel \
{ \
public: \
  NAME( const CORBA::BOA::ReferenceData &refdata ) : KOffice::DocumentFactory_skel( refdata ) { } \
  NAME( CORBA::Object_ptr _obj ) : KOffice::DocumentFactory_skel( _obj ) { } \
  virtual ~NAME() { } \
  virtual KOffice::Document_ptr create()  \
  { \
    return KOffice::Document::_duplicate( new DOC ); \
  } \
};

#endif
