#include "stylestack.h"
#include <koUnit.h>
#include <kdebug.h>

//#define DEBUG_STYLESTACK

StyleStack::StyleStack()
    : m_marks(3)
{
    clear();
}

StyleStack::~StyleStack()
{
}

void StyleStack::clear()
{
    m_marks.fill( -1 );
    m_stack.clear();
}

void StyleStack::popToMark( int mark )
{
    int toIndex = m_marks[ mark ];
#ifdef DEBUG_STYLESTACK
    kdDebug() << "popToMark " << mark << " -> to index " << toIndex << endl;
#endif
    Q_ASSERT( toIndex != -1 ); // If this happens, you didn't call setMark()...
    Q_ASSERT( toIndex <= (int)m_stack.count() ); // If equal, nothing to remove. If greater, bug.
    for ( int index = (int)m_stack.count() - 1; index >= toIndex; --index )
        m_stack.pop_back();
}

void StyleStack::setMark( int mark )
{
    if ( (int)m_marks.size() <= mark )
        m_marks.resize( mark + 1 );
    m_marks[ mark ] = m_stack.count();
#ifdef DEBUG_STYLESTACK
    kdDebug() << "setMark " << mark << " -> index " << m_stack.count() << endl;
#endif
}

void StyleStack::pop()
{
    m_stack.pop_back();
#ifdef DEBUG_STYLESTACK
    kdDebug() << "pop -> count=" << m_stack.count() << endl;
#endif
}

void StyleStack::push( const QDomElement& style )
{
    m_stack.append( style );
#ifdef DEBUG_STYLESTACK
    kdDebug() << "pushed " << style.attribute("style:name") << " -> count=" << m_stack.count() << endl;
#endif
}

bool StyleStack::hasAttribute( const QString& name ) const
{
    // TODO: has to be fixed for complex styles like list-styles
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return true;
    }

    return false;
}

QString StyleStack::attribute( const QString& name ) const
{
    // TODO: has to be fixed for complex styles like list-styles
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) )
            return properties.attribute( name );
    }

    return QString::null;
}

// Font size is a bit special. "115%" applies to "the fontsize of the parent style".
// This can be generalized though (hasAttributeThatCanBePercentOfParent() ? :)
// Although, if we also add support for fo:font-size-rel here then it's not general anymore.
double StyleStack::fontSize() const
{
    QString name = "fo:font-size";
    double percent = 1;
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( properties.hasAttribute( name ) ) {
            QString value = properties.attribute( name );
            if ( value.endsWith( "%" ) )
                percent *= value.toDouble() / 100.0;
            else
                return percent * KoUnit::parseValue( value ); // e.g. 12pt
        }
    }
    return 0;
}

bool StyleStack::hasChildNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return true;
    }

    return false;
}

QDomNode StyleStack::childNode(const QString & name) const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        QDomElement properties = (*it).namedItem( "style:properties" ).toElement();
        if ( !properties.namedItem( name ).isNull() )
            return properties.namedItem( name );
    }

    return QDomNode();          // a null node
}

static bool isUserStyle( const QDomElement& e )
{
    QDomElement parent = e.parentNode().toElement();
    //kdDebug() << k_funcinfo << "tagName=" << e.tagName() << " parent-tagName=" << parent.tagName() << endl;
    return parent.tagName() == "office:styles";
}

QString StyleStack::userStyleName() const
{
    QValueList<QDomElement>::ConstIterator it = m_stack.end();
    while ( it != m_stack.begin() )
    {
        --it;
        //kdDebug() << k_funcinfo << (*it).attribute("style:name") << endl;
        if ( isUserStyle( *it ) )
            return (*it).attribute("style:name");
    }
    // Can this ever happen?
    return "Standard";
}
