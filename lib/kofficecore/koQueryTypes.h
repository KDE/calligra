#ifndef __ko_query_types_h__
#define __ko_query_types_h__

#include <qstring.h>
#include <qstrlist.h>
#include <qpixmap.h>
#include <vector>

#include <CORBA.h>

void koInitTrader();

class KoComponentEntry
{
public:
  QString comment;
  QString name;
  QString exec;
  QString activationMode;
  QStrList repoID;
  QPixmap miniIcon;
  QPixmap icon;
  CORBA::Object_var reference;
};

class KoDocumentEntry : public KoComponentEntry
{
public:
  KoDocumentEntry() { }
  KoDocumentEntry( const KoComponentEntry _e );
  
  QStrList mimeTypes;

  bool supportsMimeType( const char *_m ) { return ( mimeTypes.find( _m ) != -1 ); }
};

class KoFilterEntry : public KoComponentEntry
{
public:
  KoFilterEntry() { }
  KoFilterEntry( const KoComponentEntry& _e );

  QStrList import;
  QStrList export;

  bool imports( const char *_m ) { return ( import.find( _m ) != -1 ); }
  bool exports( const char *_m ) { return ( export.find( _m ) != -1 ); }
};

vector<KoDocumentEntry> koQueryDocuments( const char *_constr = "", int _count = 100 );
vector<KoFilterEntry> koQueryFilters( const char *_constr = "", int _count = 100 );

#endif
