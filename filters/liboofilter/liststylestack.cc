#include "liststylestack.h"
#include "ooutils.h" // for ooNS
#include <kodom.h>

ListStyleStack::ListStyleStack()
    : m_initialLevel( 0 )
{
}

ListStyleStack::~ListStyleStack()
{
}

void ListStyleStack::pop()
{
    m_stack.pop();
}

void ListStyleStack::push( const QDomElement& style )
{
    m_stack.push( style );
}

void ListStyleStack::setInitialLevel( int initialLevel )
{
    Q_ASSERT( m_stack.isEmpty() );
    m_initialLevel = initialLevel;
}

QDomElement ListStyleStack::currentListStyle() const
{
    Q_ASSERT( !m_stack.isEmpty() );
    return m_stack.top();
}

QDomElement ListStyleStack::currentListStyleProperties() const
{
    QDomElement style = currentListStyle();
    return KoDom::namedItemNS( style, ooNS::style, "properties" );
}
