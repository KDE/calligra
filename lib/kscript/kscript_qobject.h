#ifndef __KSCRIPT_QOBJECT_H__
#define __KSCRIPT_QOBJECT_H__

#include <qshared.h>
#include <qguardedptr.h>
#include <qobject.h>

#include "kscript_value.h"

class KSContext;
class QObject;
class QVariant;

class KSQObject : public QShared
{
public:
    typedef KSSharedPtr<KSQObject> Ptr;

    /**
     */
    KSQObject( QObject* obj );

    virtual ~KSQObject();

    virtual KSValue::Ptr member( KSContext&, const QString& name );
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );
    
    virtual const char* className() const;
 
protected:
    bool pack( KSContext& context, QVariant& var, const KSValue::Ptr& v );
    KSValue::Ptr unpack( KSContext& context, QVariant& var );
    
private:
    QGuardedPtr<QObject> m_ptr;
};

#endif
