#if 0

#ifndef __KSCRIPT_CORBA_FUNC_H__
#define __KSCRIPT_CORBA_FUNC_H__

#include <qvaluelist.h>
#include <qstring.h>

#include "kscript_typecode.h"
#include "kscript_func.h"
#include "kscript_context.h"

class KSCorbaFunc : public KSFunction
{
public:
  enum ParameterMode { T_IN, T_OUT, T_INOUT };

  typedef KSSharedPtr<KSCorbaFunction> Ptr;

  KSCorbaFunc( KSModule* m, KSParseNode* );
  virtual ~KSCorbaFunc() { }

  virtual bool call( KSContext& context );
  virtual bool isSignal() const { return false; }
  virtual QString name() const { return m_name; }

  void addParameter( ParameterMode, const QString&, const KSTypeCode::Ptr & );
  void addException( const KSValue::Ptr& );
  void setReturnTypeCode( const KSTypeCode::Ptr& );

private:
  bool init( KSContext& context );

  struct Parameter
  {
    ParameterMode mode;
    QString name;
    KSTypeCode::Ptr typecode;
  };

  QValueList<KSValue::Ptr> m_exceptions;
  QValueList<Parameter> m_parameters;
  KSTypeCode::Ptr m_returnTypeCode;
  QString m_name;
  KSParseNode* m_node;
};

#endif

#endif
