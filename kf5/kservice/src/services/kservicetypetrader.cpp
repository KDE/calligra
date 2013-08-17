/* This file is part of the KDE libraries
   Copyright (C) 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2006 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kservicetypetrader.h"

#include "ktraderparsetree_p.h"
#include <kservicetypeprofile.h>
#include "kservicetype.h"
#include "kservicetypefactory.h"
#include "kservicefactory.h"

#include <QDebug>

using namespace KTraderParse;

// --------------------------------------------------

namespace KServiceTypeProfile {
    KServiceOfferList sortServiceTypeOffers( const KServiceOfferList& list, const QString& servicetype );
}

class KServiceTypeTraderSingleton
{
public:
    KServiceTypeTrader instance;
};

Q_GLOBAL_STATIC(KServiceTypeTraderSingleton, s_globalServiceTypeTrader)

KServiceTypeTrader* KServiceTypeTrader::self()
{
    return &s_globalServiceTypeTrader()->instance;
}

KServiceTypeTrader::KServiceTypeTrader()
    : d(0)
{
}

KServiceTypeTrader::~KServiceTypeTrader()
{
}

// shared with KMimeTypeTrader
void KServiceTypeTrader::applyConstraints( KService::List& lst,
                                const QString& constraint )
{
    if ( lst.isEmpty() || constraint.isEmpty() )
        return;

    const ParseTreeBase::Ptr constr = parseConstraints( constraint ); // for ownership
    const ParseTreeBase* pConstraintTree = constr.data(); // for speed

    if (!constr) { // parse error
        lst.clear();
    } else {
        // Find all services matching the constraint
        // and remove the other ones
        KService::List::iterator it = lst.begin();
        while( it != lst.end() )
        {
            if ( matchConstraint( pConstraintTree, (*it), lst ) != 1 )
                it = lst.erase( it );
            else
                ++it;
        }
    }
}

#if 0
static void dumpOfferList( const KServiceOfferList& offers )
{
    // qDebug() << "Sorted list:";
    OfferList::Iterator itOff = offers.begin();
    for( ; itOff != offers.end(); ++itOff )
        // qDebug() << (*itOff).service()->name() << " allow-as-default=" << (*itOff).allowAsDefault() << " preference=" << (*itOff).preference();
}
#endif

static KServiceOfferList weightedOffers( const QString& serviceType )
{
    //qDebug() << "KServiceTypeTrader::weightedOffers( " << serviceType << " )";

    KServiceType::Ptr servTypePtr = KServiceTypeFactory::self()->findServiceTypeByName( serviceType );
    if ( !servTypePtr ) {
        qWarning() << "KServiceTypeTrader: serviceType" << serviceType << "not found";
        return KServiceOfferList();
    }
    if ( servTypePtr->serviceOffersOffset() == -1 )  // no offers in ksycoca
        return KServiceOfferList();

    // First, get all offers known to ksycoca.
    const KServiceOfferList services = KServiceFactory::self()->offers( servTypePtr->offset(), servTypePtr->serviceOffersOffset() );

    const KServiceOfferList offers = KServiceTypeProfile::sortServiceTypeOffers( services, serviceType );
    //qDebug() << "Found profile: " << offers.count() << " offers";

#if 0
    dumpOfferList( offers );
#endif

    return offers;
}

KService::List KServiceTypeTrader::defaultOffers( const QString& serviceType,
                                                  const QString& constraint ) const
{
    KServiceType::Ptr servTypePtr = KServiceTypeFactory::self()->findServiceTypeByName( serviceType );
    if ( !servTypePtr ) {
        qWarning() << "KServiceTypeTrader: serviceType" << serviceType << "not found";
        return KService::List();
    }
    if ( servTypePtr->serviceOffersOffset() == -1 )
        return KService::List();

    KService::List lst =
        KServiceFactory::self()->serviceOffers( servTypePtr->offset(), servTypePtr->serviceOffersOffset() );

    applyConstraints( lst, constraint );

    //qDebug() << "query for serviceType " << serviceType << constraint
    //             << " : returning " << lst.count() << " offers" << endl;
    return lst;
}

KService::List KServiceTypeTrader::query( const QString& serviceType,
                                          const QString& constraint ) const
{
    if ( !KServiceTypeProfile::hasProfile( serviceType ) )
    {
        // Fast path: skip the profile stuff if there's none (to avoid kservice->serviceoffer->kservice)
        // The ordering according to initial preferences is done by kbuildsycoca
        return defaultOffers( serviceType, constraint );
    }

    KService::List lst;
    // Get all services of this service type.
    const KServiceOfferList offers = weightedOffers( serviceType );

    // Now extract only the services; the weighting was only used for sorting.
    KServiceOfferList::const_iterator itOff = offers.begin();
    for( ; itOff != offers.end(); ++itOff )
        lst.append( (*itOff).service() );

    applyConstraints( lst, constraint );

    //qDebug() << "query for serviceType " << serviceType << constraint
    //             << " : returning " << lst.count() << " offers" << endl;
    return lst;
}

KService::Ptr KServiceTypeTrader::preferredService( const QString & serviceType ) const
{
    const KServiceOfferList offers = weightedOffers( serviceType );

    KServiceOfferList::const_iterator itOff = offers.begin();
    // Look for the first one that is allowed as default.
    // Since the allowed-as-default are first anyway, we only have
    // to look at the first one to know.
    if( itOff != offers.end() && (*itOff).allowAsDefault() )
        return (*itOff).service();

    //qDebug() << "No offers, or none allowed as default";
    return KService::Ptr();
}
