#include "kstartparams.h"

KStartParams::KStartParams( int& argc, const char** argv )
{
  int i;

  for( i = 1; i <= argc; i++ )
  {
    m_paramList.append( QString( argv[ i ] ) );
  }
}

KStartParams::~KStartParams()
{
  // Liste leeren
  // => wird automatisch gemacht
}

int KStartParams::countParams()
{
  return m_paramList.count();
}

QString KStartParams::getParam( uint _index )
{
  return *m_paramList.at( _index );
}

void KStartParams::deleteParam( uint _index )
{
  if( _index < m_paramList.count() )
  {
    m_paramList.remove( m_paramList.at( _index ) );
  }
}

bool KStartParams::paramIsPresent( const QString& _param )
{
  QStringList::Iterator it;

  for( it = m_paramList.begin(); it != m_paramList.end(); ++it )
  {
    if( _param == *it )
    {
      return true;
    }
  }
  return false;
}

QStringList m_paramList;
