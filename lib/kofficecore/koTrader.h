#ifndef __ko_trader_h__
#define __ko_trader_h__

#include <qstring.h>

#include <ksharedptr.h>
#include <kservices.h>

/**
 */
class KoTrader : public QObject
{
    Q_OBJECT
public:
    // A list of services
    typedef QValueList<KService::Ptr> OfferList;
  
    virtual ~KoTrader();

    virtual OfferList query( const QString& servicetype,
			     const QString& constraint = QString::null,
			     const QString& preferences = QString::null) const;

    virtual OfferList listServices();

    virtual KService::Ptr serviceByName( const QString &name );

    static KoTrader* self();
    
protected:
    KoTrader();
    
private:
    static KoTrader* s_self;
};

#endif
