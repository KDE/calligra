#ifndef __kofactory_h__
#define __kofactory_h__

#include "koffice.h"

class KoDocumentFactory : public KOffice::DocumentFactory_skel
{
};

#define KOFFICE_DOCUMENT_FACTORY( DOC, NAME, SKEL ) \
class NAME : virtual public KoDocumentFactory, \
             virtual public SKEL \
{ \
public: \
  NAME( const CORBA::BOA::ReferenceData &refdata ) : SKEL( refdata ) { } \
  NAME( CORBA::Object_ptr _obj ) : SKEL( _obj ) { } \
  virtual ~NAME() { } \
  virtual KOffice::Document_ptr create()  \
  { \
    return KOffice::Document::_duplicate( new DOC ); \
  } \
};

#endif
