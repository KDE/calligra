#include "koTrader.h"
#include "koTraderParseTree.h"

#include <qtl.h>
#include <qbuffer.h>

#include <kuserprofile.h>
#include <kregfactories.h>
#include <kregistry.h>
#include <kstddirs.h>

class KoTraderSorter
{
public:
  KoTraderSorter() { m_pService = 0; };
  KoTraderSorter( const KoTraderSorter& s ) : m_userPreference( s.m_userPreference ),
    m_bAllowAsDefault( s.m_bAllowAsDefault ),
    m_traderPreference( s.m_traderPreference ), m_pService( s.m_pService ) { }
  KoTraderSorter( const KService* _service, double _pref1, int _pref2, bool _default )
  { m_pService = _service;
    m_userPreference = _pref2;
    m_traderPreference = _pref1;
    m_bAllowAsDefault = _default;
  }

  const KService* service() const { return m_pService; }

  bool operator< ( const KoTraderSorter& ) const;

private:
  /**
   * The bigger this number is, the better is this service in
   * the users opinion.
   */
  int m_userPreference;
  /**
   * Is it allowed to use this service for default actions.
   */
  bool m_bAllowAsDefault;

  /**
   * The bigger this number is, the better is this service with
   * respect to the querys preferences expression.
   */
  double m_traderPreference;

  const KService* m_pService;
};

bool KoTraderSorter::operator< ( const KoTraderSorter& _o ) const
{
  if ( _o.m_bAllowAsDefault && !m_bAllowAsDefault )
    return true;
  if ( _o.m_userPreference > m_userPreference )
    return true;
  if ( _o.m_userPreference < m_userPreference )
    return true;
  if ( _o.m_traderPreference > m_traderPreference )
    return true;
  return false;
}

// --------------------------------------------------

KoTrader* KoTrader::s_self = 0;

KoTrader* KoTrader::self()
{
    if ( !s_self )
	s_self = new KoTrader;
    
    return s_self;
}

KoTrader::KoTrader()
{
}

KoTrader::~KoTrader()
{
}

KoTrader::OfferList KoTrader::query( const QString& _servicetype, const QString& _constraint,
					const QString& _preferences ) const
{
  // TODO: catch errors here
  ParseTreeBase::Ptr constr;
  ParseTreeBase::Ptr prefs;

  if ( _constraint != QString::null )
    constr = parseConstraints( _constraint );

  if ( _preferences != QString::null )
    prefs = parsePreferences( _preferences );

  KServiceTypeProfile::OfferList lst;
  KoTrader::OfferList ret;

  // Get all services of this service type.
  lst = KServiceTypeProfile::offers( _servicetype );
  if ( lst.count() == 0 )
    return ret;

  if ( !!constr )
  {
    // Find all services matching the constraint
    // and remove the other ones
    KServiceTypeProfile::OfferList::Iterator it = lst.begin();
    while( it != lst.end() )
    {
      if ( matchConstraint( constr, (*it).service(), lst ) != 1 )
	it = lst.remove( it );
      else
	++it;
    }
  }

  if ( !!prefs )
  {
    QValueList<KoTraderSorter> sorter;
    KServiceTypeProfile::OfferList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
      PreferencesReturn p = matchPreferences( prefs, (*it).service(), lst );
      if ( p.type == PreferencesReturn::PRT_DOUBLE )
	sorter.append( KoTraderSorter( (*it).service(), p.f, (*it).preference(), (*it).allowAsDefault() ) );
    }
    qBubbleSort( sorter );

    QValueList<KoTraderSorter>::Iterator it2 = sorter.begin();
    for( ; it2 != sorter.end(); ++it2 )
    {
      KService *s = (KService*)(*it2).service();
      ret.append( s );
    }
  }
  else
  {
    KServiceTypeProfile::OfferList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
      KService *s = (KService*)(*it).service();
      ret.append( s );
    }
  }

  return ret;
}

KoTrader::OfferList KoTrader::listServices()
{
  KoTrader::OfferList res;

  const QList<KService> &serviceList = KService::services();

  QListIterator<KService> it( serviceList );
  for (; it.current(); ++it )
  {
    res.append( it.current() );
  }

  return res;
}

KService::Ptr KoTrader::serviceByName( const QString &name )
{
  KService *s = KService::service( name );

  return KService::Ptr( s );
}

/* void KoTrader::loadRegistry()
{
  QString regDB = locate("config", "kregistry");

  // Load from binary registry + check dirs
  KRegistry::self()->load( regDB );

  // If dirs had anything new : save in binary registry
  if ( KRegistry::self()->isModified() )
     KRegistry::self()->save( regDB );
}
*/

#include "koTrader.moc"
