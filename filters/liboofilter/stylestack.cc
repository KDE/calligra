#include "stylestack.h"

StyleStack::StyleStack()
    : m_pageMark( 0 ),
      m_objectMark( 0 )
{
}

StyleStack::~StyleStack()
{
}

void StyleStack::clear()
{
    m_pageMark = 0;
    m_objectMark = 0;
    m_stack.clear();
}

void StyleStack::clearPageMark()
{
    for ( int index = m_stack.count() - 1; index >= (int)m_pageMark; --index )
        m_stack.pop_back();
}

void StyleStack::setPageMark()
{
    m_pageMark = m_stack.count();
}

void StyleStack::clearObjectMark()
{
    for ( int index = m_stack.count() - 1; index >= (int)m_objectMark; --index )
        m_stack.pop_back();
}

void StyleStack::setObjectMark()
{
    m_objectMark = m_stack.count();
}

void StyleStack::pop()
{
    m_stack.pop_back();
}

void StyleStack::push( const QDomElement& style )
{
    m_stack.append( style );
}

bool StyleStack::hasAttribute( const QString& name )
{
    // TODO: has to be fixed for complex styles like list-styles
    QValueList<QDomElement>::Iterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return true;
    }

    return false;
}

QString StyleStack::attribute( const QString& name )
{
    // TODO: has to be fixed for complex styles like list-styles
    QValueList<QDomElement>::Iterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
    }

    return QString::null;
}

bool StyleStack::hasChildNode(const QString & name)
{
    QValueList<QDomElement>::Iterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return true;
    }

    return false;
}

QDomNode StyleStack::childNode(const QString & name)
{
    QValueList<QDomElement>::Iterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return properties.namedItem( name );
    }

    return QDomNode();          // a null node
}
