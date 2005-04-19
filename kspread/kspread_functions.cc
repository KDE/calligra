#include "kspread_functions.h"
#include "kspread_factory.h"

#include <qdom.h>
#include <qfile.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
#include <kinstance.h>

// these are defined in kspread_function_*.cc
void KSpreadRegisterConversionFunctions();
void KSpreadRegisterDatabaseFunctions();
void KSpreadRegisterDateTimeFunctions();
void KSpreadRegisterEngineeringFunctions();
void KSpreadRegisterFinancialFunctions();
void KSpreadRegisterInformationFunctions();
void KSpreadRegisterLogicFunctions();
void KSpreadRegisterMathFunctions();
void KSpreadRegisterReferenceFunctions();
void KSpreadRegisterStatisticalFunctions();
void KSpreadRegisterTextFunctions();
void KSpreadRegisterTrigFunctions();

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
	    return i18n("A range of floating point values (like 1.3, 0.343, 253 )" );
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
    m_type  = KSpread_Float;
    m_range = FALSE;

    QDomNode n = element.firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
	if ( n.isElement() )
	{
	    QDomElement e = n.toElement();
	    if ( e.tagName() == "Comment" )
		m_help = i18n( e.text().utf8() );
	    else if ( e.tagName() == "Type" )
	    {
		m_type = toType( e.text() );
		if ( e.hasAttribute( "range" ))
		{
		  if (e.attribute("range").lower() == "true")
		    m_range = TRUE;
		}
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
			    m_help.append ( i18n( e2.text().utf8() ) );
			else if ( e2.tagName() == "Syntax" )
			    m_syntax.append( i18n( e2.text().utf8() ) );
			else if ( e2.tagName() == "Example" )
			    m_examples.append( i18n( e2.text().utf8() ) );
			else if ( e2.tagName() == "Related" )
			    m_related.append( i18n( e2.text().utf8() ) );
		    }
		}
	    }
	}
    }
}

KSpreadFunctionDescription::KSpreadFunctionDescription( const KSpreadFunctionDescription& desc )
{
    m_examples = desc.m_examples;
    m_related = desc.m_related;
    m_syntax = desc.m_syntax;
    m_help = desc.m_help;
    m_name = desc.m_name;
    m_type = desc.m_type;
}

QString KSpreadFunctionDescription::toQML() const
{
    QString text( "<qt><h1>" );
    text += name();
    text += "</h1>";

    if( !m_help.isEmpty() )
    {
	text += i18n("<p>");
	QStringList::ConstIterator it = m_help.begin();
	for( ; it != m_help.end(); ++it )
	{
	    text += *it;
	    text += "<p>";
	}
	text += "</p>";
    }

    text += i18n("<p><b>Return type: </b>");
    text += toString( type() );
    text += "</p>";

    if ( !m_syntax.isEmpty() )
    {
	text += i18n("<h2>Syntax</h2><ul>");
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
	text += i18n("<h2>Parameters</h2><ul>");
	QValueList<KSpreadFunctionParameter>::ConstIterator it = m_params.begin();
	for( ; it != m_params.end(); ++it )
	{
	    text += i18n("<li><b>Comment:</b> ");
	    text += (*it).helpText();
	    text += i18n("<br><b>Type:</b> ");
	    text += toString( (*it).type(), (*it).hasRange() );
	}
	text += "</ul>";
    }

    if ( !m_examples.isEmpty() )
    {
	text += i18n("<h2>Examples</h2><ul>");
	QStringList::ConstIterator it = m_examples.begin();
	for( ; it != m_examples.end(); ++it )
	{
	    text += "<li>";
	    text += *it;
	}
	text += "</ul>";
    }

    if ( !m_related.isEmpty() )
    {
	text += i18n("<h2>Related Functions</h2><ul>");
	QStringList::ConstIterator it = m_related.begin();
	for( ; it != m_related.end(); ++it )
	{
	    text += "<li>";
	    text += "<a href=\"" + *it + "\">";
	    text += *it;
	    text += "</a>";
	}
	text += "</ul>";
    }

    text += "</qt>";

    return text;
}

static KStaticDeleter<KSpreadFunctionRepository> ksfr_sd;
KSpreadFunctionRepository* KSpreadFunctionRepository::s_self = 0;

KSpreadFunctionRepository* KSpreadFunctionRepository::self()
{
    if( !s_self )
    {
        ksfr_sd.setObject( s_self, new KSpreadFunctionRepository() );

        // register all built-in functions
        KSpreadRegisterConversionFunctions();
        KSpreadRegisterDatabaseFunctions();
        KSpreadRegisterDateTimeFunctions();
        KSpreadRegisterEngineeringFunctions();
        KSpreadRegisterFinancialFunctions();
        KSpreadRegisterInformationFunctions();
        KSpreadRegisterLogicFunctions();
        KSpreadRegisterMathFunctions();
        KSpreadRegisterReferenceFunctions();
        KSpreadRegisterStatisticalFunctions();
        KSpreadRegisterTextFunctions();
        KSpreadRegisterTrigFunctions();

        // find all XML description files
        QStringList files =
           KSpreadFactory::global()->dirs()->findAllResources( "extensions", "*.xml", TRUE );
        for( QStringList::Iterator it = files.begin(); it != files.end(); ++it )
	    s_self->loadFile( *it );

    }
    return s_self;
}

// class constructor
KSpreadFunctionRepository::KSpreadFunctionRepository()
{
    m_funcs.setAutoDelete( true );
    m_functions.setAutoDelete( true );
}

// loads functions description from XML file
void KSpreadFunctionRepository::loadFile( const QString& filename )
{
    QFile file( filename );
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
		group = i18n( e.namedItem( "GroupName" ).toElement().text().utf8() );
		m_groups.append( group );
		m_groups.sort();

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
                            if( m_functions.find( desc->name() ) )
			        m_funcs.insert( desc->name(), desc );
			}
		    }
		}

		group = "";
	    }
	}
    }
}

// returns description of specified function name
KSpreadFunctionDescription* KSpreadFunctionRepository::functionInfo( const QString& name )
{
    return m_funcs[ name ];
}

// returns function (KSpreadFunction) of specified function name
KSpreadFunction* KSpreadFunctionRepository::function( const QString& name )
{
    return m_functions[ name ];
}

// returns names of function in certain group
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

// returns names of all available functions
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

// returns names of all registered functions (might be more)
QStringList KSpreadFunctionRepository::regFunctionNames()
{
    QStringList lst;

    QDictIterator<KSpreadFunction> it( m_functions );
    for( ; it.current(); ++it )
    {
	lst.append( it.current()->name );
    }

    lst.sort();

    return lst;
}

// registers a new function
void KSpreadFunctionRepository::registerFunction( const QString& _name, KSpreadFunctionPtr _func )
{
    QString name = _name.upper();
    KSpreadFunction* function;

    function = new KSpreadFunction();
    function->name = name;
    function->functionPtr  = _func;

    m_functions.replace( name, function );
}
