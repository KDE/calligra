#include "kspread_functions.h"

#include <qdom.h>
#include <qfile.h>

#include <klocale.h>

static KSpreadParameterType toType( const QString& type )
{
    if ( type == "Boolean" )
	return KSpread_Boolean;
    if ( type == "Int" )
	return KSpread_Int;
    if ( type == "String" )
	return KSpread_String;
    if ( type == "Any" )
	return KSpread_Any;
    
    return KSpread_Float;
}

static QString toString( KSpreadParameterType type, bool range = FALSE )
{
    if ( !range )
    {
	switch( type )
	{
	case KSpread_String:
	    return i18n("Text");
	case KSpread_Int:
	    return i18n("Whole number (like 1, 132, 2344)");
	case KSpread_Boolean:
	    return i18n("A truth value (TRUE or FALSE)" );
	case KSpread_Float:
	    return i18n("A floating point value (like 1.3, 0.343, 253 )" );
	case KSpread_Any:
	    return i18n("Any kind of value");
	}
    }
    else
    {
	switch( type )
	{
	case KSpread_String:
	    return i18n("A range of strings");
	case KSpread_Int:
	    return i18n("A range of whole numbers (like 1, 132, 2344)");
	case KSpread_Boolean:
	    return i18n("A range of truth values (TRUE or FALSE)" );
	case KSpread_Float:
	    return i18n("A range of A floating point values (like 1.3, 0.343, 253 )" );
	case KSpread_Any:
	    return i18n("A range of any kind of values");
	}
    }
    
    return QString::null;
}

KSpreadFunctionParameter::KSpreadFunctionParameter()
{
    m_type = KSpread_Float;
    m_range = FALSE;
}

KSpreadFunctionParameter::KSpreadFunctionParameter( const KSpreadFunctionParameter& param )
{
    m_help = param.m_help;
    m_type = param.m_type;
    m_range = param.m_range;
}

KSpreadFunctionParameter::KSpreadFunctionParameter( const QDomElement& element )
{
    QDomNode n = element.firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
	if ( n.isElement() )
	{ 
	    QDomElement e = n.toElement();
	    if ( e.tagName() == "Comment" )
		m_help = i18n( e.text().latin1() );
	    else if ( e.tagName() == "Type" )
	    {
		m_type = toType( e.text() );
		if ( e.hasAttribute( "range" ) && e.attribute("range") == "true" )
		    m_range = TRUE;
	    }
	}
    }
}

KSpreadFunctionDescription::KSpreadFunctionDescription()
{
    m_type = KSpread_Float;
}

KSpreadFunctionDescription::KSpreadFunctionDescription( const QDomElement& element )
{
    QDomNode n = element.firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
	if ( n.isElement() )
	{ 
	    QDomElement e = n.toElement();
	    if ( e.tagName() == "Name" )
		m_name = e.text();
	    else if ( e.tagName() == "Type" )
	    {
		m_type = toType( e.text() );
	    }
	    else if ( e.tagName() == "Parameter" )
	    {
		m_params.append( KSpreadFunctionParameter( e ) );
	    }
	    else if ( e.tagName() == "Help" )
	    {
		QDomNode n2 = e.firstChild();
		for( ; !n2.isNull(); n2 = n2.nextSibling() )
		{
		    if ( n2.isElement() )
		    { 
			QDomElement e2 = n2.toElement();
			if ( e2.tagName() == "Text" )
			    m_help = i18n( e2.text().latin1() );
			else if ( e2.tagName() == "Syntax" )
			    m_syntax.append( i18n( e2.text().latin1() ) );
			else if ( e2.tagName() == "Example" )
			    m_examples.append( i18n( e2.text().latin1() ) );
		    }
		}
	    }
	}
    }
}

KSpreadFunctionDescription::KSpreadFunctionDescription( const KSpreadFunctionDescription& desc )
{
    m_examples = desc.m_examples;
    m_syntax = desc.m_syntax;
    m_help = desc.m_help;
    m_name = desc.m_name;
    m_type = desc.m_type;
}

QString KSpreadFunctionDescription::toQML() const
{
    QString text( "<qt><h1>" );
    text += name();
    text += "</h1><p>";
    text += helpText();
    text += "</p><p><b>Return type: </b>";
    text += toString( type() );
    text += "</p>";

    if ( !m_examples.isEmpty() )
    {
	text += "<h2>Syntax</h2><ul>";
	QStringList::ConstIterator it = m_syntax.begin();
	for( ; it != m_syntax.end(); ++it )
	{
	    text += "<li>";
	    text += *it;
	}
	text += "</ul>";
    }

    if ( !m_params.isEmpty() )
    {
	text += "<h2>Parameters</h2><ul>";
	QValueList<KSpreadFunctionParameter>::ConstIterator it = m_params.begin();
	for( ; it != m_params.end(); ++it )
	{
	    text += "<li><b>Comment:</b> ";
	    text += (*it).helpText();
	    text += "<br><b>Type:</b> ";
	    text += toString( (*it).type(), (*it).hasRange );
	}
	text += "</ul>";
    }
    
    if ( !m_examples.isEmpty() )
    {
	text += "<h2>Example</h2><ul>";
	QStringList::ConstIterator it = m_examples.begin();
	for( ; it != m_examples.end(); ++it )
	{
	    text += "<li>";
	    text += *it;
	}
	text += "</ul>";
    }
    
    text += "</qt>";
    
    return text;
}


KSpreadFunctionRepository::KSpreadFunctionRepository()
{
    m_funcs.setAutoDelete( TRUE );
    

    QFile file( "kspread_functions.xml" );
    if ( !file.open( IO_ReadOnly ) )
	return;
	
    QDomDocument doc;
    doc.setContent( &file );
    file.close();
    
    QString group = "";
    
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
	if ( n.isElement() )
	{ 
	    QDomElement e = n.toElement();
	    if ( e.tagName() == "Group" )
	    {
		group = i18n( e.namedItem( "GroupName" ).toElement().text().latin1() );
		m_groups.append( group );
		
		QDomNode n2 = e.firstChild();
		for( ; !n2.isNull(); n2 = n2.nextSibling() )
		{
		    if ( n2.isElement() )
		    { 
			QDomElement e2 = n2.toElement();
			if ( e2.tagName() == "Function" )
			{
			    KSpreadFunctionDescription* desc = new KSpreadFunctionDescription( e2 );
			    desc->setGroup( group );
			    m_funcs.insert( desc->name(), desc );
			}
		    }
		}
		
		group = "";
	    }
	}
    }
}
    
KSpreadFunctionDescription* KSpreadFunctionRepository::function( const QString& name )
{
    return m_funcs[ name ];
}
    
QStringList KSpreadFunctionRepository::functionNames( const QString& group )
{
    QStringList lst;
    
    QDictIterator<KSpreadFunctionDescription> it( m_funcs );
    for( ; it.current(); ++it )
    {
	if ( it.current()->group() == group )
	    lst.append( it.current()->name() );
    }

    lst.sort();
    
    return lst;
}

QStringList KSpreadFunctionRepository::functionNames()
{
    QStringList lst;
    
    QDictIterator<KSpreadFunctionDescription> it( m_funcs );
    for( ; it.current(); ++it )
    {
	lst.append( it.current()->name() );
    }
    
    lst.sort();
    
    return lst;
}
