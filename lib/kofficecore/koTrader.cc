#include "koTrader.h"
#include "koTraderParseTree.h"

#include <qtl.h>
#include <qbuffer.h>

#include <kuserprofile.h>
#include <kstddirs.h>

class KTraderSorter
{
public:
  KTraderSorter() { m_pService = 0; };
  KTraderSorter( const KTraderSorter& s ) : m_userPreference( s.m_userPreference ),
    m_bAllowAsDefault( s.m_bAllowAsDefault ),
    m_traderPreference( s.m_traderPreference ), m_pService( s.m_pService ) { }
  KTraderSorter( const KService* _service, double _pref1, int _pref2, bool _default )
  { m_pService = _service;
    m_userPreference = _pref2;
    m_traderPreference = _pref1;
    m_bAllowAsDefault = _default;
  }

  const KService* service() const { return m_pService; }

  bool operator< ( const KTraderSorter& ) const;

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

bool KTraderSorter::operator< ( const KTraderSorter& _o ) const
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

KTrader* KTrader::s_self = 0;

KTrader* KTrader::self()
{
    if ( !s_self )
	s_self = new KTrader;

    return s_self;
}

KTrader::KTrader()
{
}

KTrader::~KTrader()
{
}

KTrader::OfferList KTrader::query( const QString& _servicetype, const QString& _constraint,
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
  KTrader::OfferList ret;

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
    QValueList<KTraderSorter> sorter;
    KServiceTypeProfile::OfferList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
      PreferencesReturn p = matchPreferences( prefs, (*it).service(), lst );
      if ( p.type == PreferencesReturn::PRT_DOUBLE )
	sorter.append( KTraderSorter( (*it).service(), p.f, (*it).preference(), (*it).allowAsDefault() ) );
    }
    qBubbleSort( sorter );

    QValueList<KTraderSorter>::Iterator it2 = sorter.begin();
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

KTrader::OfferList KTrader::listServices()
{
  KTrader::OfferList res;

  const KService::List serviceList = KService::allServices();

  KService::List::ConstIterator it = serviceList.begin();
  for (; it != serviceList.end(); ++it )
  {
    res.append( *it );
  }

  return res;
}

KService::Ptr KTrader::serviceByName( const QString &name )
{
  KService *s = KService::service( name );

  return KService::Ptr( s );
}

/* void KTrader::loadRegistry()
{
  QString regDB = locate("config", "kregistry");

  // Load from binary registry + check dirs

  // If dirs had anything new : save in binary registry
}
*/

#include "koTrader.moc"
