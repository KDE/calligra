#include "kspread_functions.h"

#include <qdom.h>
#include <qfile.h>

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
		m_help = e.text();
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
			if ( e.tagName() == "Text" )
			    m_help = e.text();
			else if ( e.tagName() == "Syntax" )
			    m_syntax.append( e.text() );
			else if ( e.tagName() == "Example" )
			    m_examples.append( e.text() );
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

KSpreadFunctionRepository::KSpreadFunctionRepository()
{
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
		group = e.namedItem( "GroupName" ).toElement().text();
		
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
    
    return lst;
}
