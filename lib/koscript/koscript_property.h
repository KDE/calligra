#ifndef __KSCRIPT_OBJECT_H__
#define __KSCRIPT_OBJECT_H__

#include <qshared.h>
#include <qmap.h>
#include <qlist.h>
#include <qstring.h>

#include "koscript_ptr.h"
#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"

#include <string.h>

class KSProperty : public QShared
{
public:
  KSProperty( const KSStruct::Ptr& struc, const QString& name ) { m_struct = struc; m_name = name; }
  KSProperty( const KSModule::Ptr& m, const QString& name ) { m_module = m; m_name = name; }

  virtual ~KSProperty() { }

  QString name() { return m_name; }

  virtual bool set( KSContext&, const KSValue::Ptr& v );

private:
  KSStruct::Ptr m_struct;
  KSModule::Ptr m_module;
  QString m_name;
};

#endif
