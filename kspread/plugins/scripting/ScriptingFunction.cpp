/*
 * This file is part of KSpread
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ScriptingFunction.h"
//#include "ScriptingPart.h"
//#include "ScriptingModule.h"

//#include <QApplication>
#include <QDomDocument>

#include <kaction.h>
#include <kactioncollection.h>
//#include <kstandarddirs.h>
#include <klocale.h>

//#include <Doc.h>
//#include <View.h>
#include <Value.h>
#include <Functions.h>

//#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <core/manager.h>
//#include <core/guiclient.h>
//#include <main/wdgscriptsmanager.h>

KSpread::Value scripting_function_call(KSpread::valVector args, KSpread::ValueCalc *calc, KSpread::FuncExtra*)
{
    QString scriptname = args[0].isString() ? args[0].asString() : QString();
    QString functionname = args[1].isString() ? args[1].asString() : QString();

    KActionCollection* collection = Kross::Manager::self().actionCollection();
    if( ! collection ) {
        KSpread::Value err = KSpread::Value::errorNA();
        err.setError( "#" + i18n("Scripfunctions are disabled.") );
        return err;
    }

    KAction* action = scriptname.isNull() ? 0 : collection->action(scriptname);
    if( ! action ) {
        KSpread::Value err = KSpread::Value::errorVALUE(); //errorNAME();
        err.setError( "#" + i18n("No such script.") );
        return err;
    }

    //TODO
    return KSpread::Value("This is a string");
}

class ScriptingFunctionImpl
{
	public:
		ScriptingFunctionImpl(const QString& name, int minparam, int maxparam, const QString& comment, const QString& syntax)
		{
			KSpread::FunctionRepository* repo = KSpread::FunctionRepository::self();

			KSpread::Function* f = new KSpread::Function(name, scripting_function_call);
			f->setParamCount(minparam, maxparam);
			f->setAcceptArray();
			f->setNeedsExtra(true);
			repo->add(f);

			m_funcelem = m_domdoc.createElement("Function");

			QDomElement nameelem = m_domdoc.createElement("Name");
			nameelem.appendChild( m_domdoc.createTextNode(name) );
			m_funcelem.appendChild(nameelem);

			QDomElement typeelem = m_domdoc.createElement("Type");
			typeelem.appendChild( m_domdoc.createTextNode("String") );
			m_funcelem.appendChild(typeelem);

			m_helpElem = m_domdoc.createElement("Help");
			QDomElement helpTextElem = m_domdoc.createElement("Text");
			helpTextElem.appendChild( m_domdoc.createTextNode(comment) );
			m_helpElem.appendChild(helpTextElem);

			QDomElement helpSyntaxElem = m_domdoc.createElement("Syntax");
			helpSyntaxElem.appendChild( m_domdoc.createTextNode(syntax) );
			m_helpElem.appendChild(helpSyntaxElem);

			m_funcelem.appendChild(m_helpElem);
		}
		~ScriptingFunctionImpl()
		{
		}
		void addExample(const QString& example)
		{
			QDomElement helpExampleElem = m_domdoc.createElement("Example");
			helpExampleElem.appendChild( m_domdoc.createTextNode(example) );
			m_helpElem.appendChild(helpExampleElem);

		}
		void addParameter(const QString& typeName, const QString& comment)
		{
			QDomElement paramElem = m_domdoc.createElement("Parameter");
			QDomElement paramCommentElem = m_domdoc.createElement("Comment");
			paramCommentElem.appendChild( m_domdoc.createTextNode(comment) );
			paramElem.appendChild(paramCommentElem);
			QDomElement paramTypeElem = m_domdoc.createElement("Type");
			paramTypeElem.appendChild( m_domdoc.createTextNode(typeName) );
			paramElem.appendChild(paramTypeElem);
			m_funcelem.appendChild(paramElem);
		}
		void registerFunction()
		{
			KSpread::FunctionRepository* repo = KSpread::FunctionRepository::self();
			KSpread::FunctionDescription* desc = new KSpread::FunctionDescription(m_funcelem);
			desc->setGroup("Scripts");
			repo->add(desc);
		}
	private:
		QDomDocument m_domdoc;
		QDomElement m_funcelem;
		QDomElement m_helpElem;
};

class ScriptingFunction::Private
{
	public:
		ScriptingFunctionImpl* script_function;
};

ScriptingFunction::ScriptingFunction(QObject* parent)
    : QObject(parent)
    , d(new Private())
{
    KSpread::FunctionRepository* repo = KSpread::FunctionRepository::self();
    if( ! repo->groups().contains("Scripts") )
        repo->addGroup("Scripts");

    d->script_function = new ScriptingFunctionImpl(
        "SCRIPT", // name
        2, // minimum arguments
        -1, // maximum arguments
        i18n("The SCRIPT() function calls a function in an external scriptfile. "
             "The script file references a file relative to your KSpread scripts "
             "folder while the function name should be an existing function "
             "within that script file."), // comment
        "SCRIPT(scriptfile;functionname;...)" // syntax
    );
    d->script_function->addParameter("String", i18n("The name of the script file"));
    d->script_function->addParameter("String", i18n("The name of the function"));
    d->script_function->addExample("SCRIPT(\"myscript1.py\";\"myfunction1\")");
    d->script_function->addExample("SCRIPT(\"myscript2.rb\";\"myfunction2\";\"optional argument\")");
    d->script_function->addExample("SCRIPT(\"myscript3.js\";\"myfunction3\";17;20.0;\"string\")");
    d->script_function->addExample("SCRIPT(\"subfolder/myscript4.py\";\"myfunction4\"");
    d->script_function->registerFunction();

    //d->registerFunction("SCRIPT_GET");
    //d->registerFunction("SCRIPT_SET");
    //d->registerFunction("SCRIPT_CALL");
    //d->registerFunction("SCRIPT_EVAL");
    //d->registerFunction("SCRIPT_DEBUG");
    //d->registerFunction("SCRIPT_TEST");
}

ScriptingFunction::~ScriptingFunction()
{
    delete d->script_function;
    delete d;
}

#include "ScriptingFunction.moc"
