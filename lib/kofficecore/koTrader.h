#ifndef __ko_trader_h__
#define __ko_trader_h__

#include <qstring.h>

#include <ksharedptr.h>
#include <kservice.h>

/**
 */
class KTrader : public QObject
{
    Q_OBJECT
public:
    // A list of services
    typedef QValueList<KService::Ptr> OfferList;
  
    virtual ~KTrader();

    virtual OfferList query( const QString& servicetype,
			     const QString& constraint = QString::null,
			     const QString& preferences = QString::null) const;

    virtual OfferList listServices();

    virtual KService::Ptr serviceByName( const QString &name );

    static KTrader* self();
    
protected:
    KTrader();
    
private:
    static KTrader* s_self;
};

#endif
