#ifndef __KSCRIPT_OBJECT_H__
#define __KSCRIPT_OBJECT_H__

#include <qmap.h>
#include <qlist.h>
#include <qstring.h>

#include <ksharedptr.h>

#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"

#include <string.h>

class KSProperty : public KShared
{
public:
  KSProperty( const KSStruct::Ptr& struc, const QString& name ) : KShared() { m_struct = struc; m_name = name; }
  KSProperty( const KSModule::Ptr& m, const QString& name ) : KShared() { m_module = m; m_name = name; }

  virtual ~KSProperty() { }

  QString name() { return m_name; }

  virtual bool set( KSContext&, const KSValue::Ptr& v );

private:
  KSStruct::Ptr m_struct;
  KSModule::Ptr m_module;
  QString m_name;
};

#endif
