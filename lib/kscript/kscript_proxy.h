#ifndef __KSCRIPT_PROXY_H__
#define __KSCRIPT_PROXY_H__

#include <qshared.h>
#include <qstring.h>
#include <qstringlist.h>

#include "kscript_value.h"
#include "kscript_context.h"
#include "kscript_interface.h"

class KSProxy : public QShared
{
public:
  typedef KSSharedPtr<KSProxy> Ptr;

  /**
   * @param obj is in reality a CORBA::Object pointer, but we dont
   *            want CORBA stuff in the headers to be able to compile
   *            kscript fast.
   */
  KSProxy( KSInterface* c, void* obj );
  KSProxy( KSProxy* );

  virtual ~KSProxy();

  /**
   * Implements a KScript function of the same name.
   */
  bool isA( KSContext& context );
  /**
   * Implements a KScript function of the same name.
   */
  bool inherits( KSContext& context );

  virtual KSValue::Ptr member( KSContext&, const QString& name );
  virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

  const KSInterface* interface() const { return m_interface; }
  KSInterface* interface() { return m_interface; }

  virtual KSProxy* clone() { KSProxy *s = new KSProxy( m_interface, m_object ); s->m_space = m_space; return s; }

  KSModule* module() { return m_interface->module(); }
  KSNamespace* instanceNameSpace() { return &m_space; }
  const KSNamespace* instanceNameSpace() const { return &m_space; }

  /**
   * @return a pointer to the CORBA::Object.
   */
  void* object() { return m_object; }

private:
  void init( KSInterface* iface, void* obj );

  KSInterface* m_interface;
  KSNamespace m_space;
  KSSubScope m_scope;

#ifdef WITH_CORBA
  CORBA::Object* m_object;
#else
  void* m_object;
#endif
};

#endif
