#ifndef __KSCRIPT_PROXY_H__
#define __KSCRIPT_PROXY_H__

#include <qshared.h>
#include <qcstring.h>

#include "kscript_value.h"

class QDataStream;
class QString;

class KSContext;

class KSProxy : public QShared
{
public:
    typedef KSSharedPtr<KSProxy> Ptr;

    /**
     */
    KSProxy( const QCString& app, const QCString& obj );

    virtual ~KSProxy();

    virtual KSValue::Ptr member( KSContext&, const QString& name );
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    QCString appId() const;
    QCString objId() const;

protected:
    bool call( KSContext& context, const QString& name );
    QString pack( KSContext&, QDataStream& str, KSValue::Ptr& v );
    KSValue::Ptr unpack( KSContext&, QDataStream& str, const QCString& type );

private:
    QCString m_app;
    QCString m_obj;
    bool m_propertyProxyCheckDone;
    bool m_supportsPropertyProxy;
};

#endif
