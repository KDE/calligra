/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krscripthandler.h"
#include <kdebug.h>
#include <QScriptEngine>
#include "krscriptfunctions.h"

#include <parsexmlutils.h>
#include <krsectiondata.h>
#include "krscriptsection.h"
#include "krscriptdebug.h"

KRScriptHandler::KRScriptHandler(KexiDB::Connection* c, QScriptEngine *e, const ORReportData* d)
{
	_conn = c;
	_engine = e;
	_data = d;
	
	
}
 
KRScriptHandler::~KRScriptHandler()
{
	
}

void KRScriptHandler::setSource(const QString &s)
{
	_source = s;
	_functions->setSource(s);	
}

void KRScriptHandler::slotInit()
{
	//QScriptValue sumFunc = _scriptEngine->newFunction(functions.sum);
	//_scriptEngine->globalObject().setProperty("sum", sumFunc);
	
	//Add math functions to the script
	_functions = new KRScriptFunctions(_conn);
	QScriptValue funcs = _engine->newQObject(_functions, QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("functions",funcs);
	
	//A simple debug function to allow printing from functions
	_debug = new KRScriptDebug();
	QScriptValue deb = _engine->newQObject(_debug, QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("debug",deb);
	
	//Add the detail section
	QScriptValue detail = _engine->newQObject(new Scripting::Section(_data->detailsection->detail), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("detail",detail);
	
	//Add the pghead_any section
	QScriptValue pghead_any = _engine->newQObject(new Scripting::Section(_data->pghead_any), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagehead_any",pghead_any);
	
	//Add the pghead_even section
	QScriptValue pghead_even = _engine->newQObject(new Scripting::Section(_data->pghead_even), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagehead_even",pghead_even);
	
	//Add the pghead_odd section
	QScriptValue pghead_odd = _engine->newQObject(new Scripting::Section(_data->pghead_odd), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagehead_odd",pghead_odd);
	
	//Add the pghead_first section
	QScriptValue pghead_first = _engine->newQObject(new Scripting::Section(_data->pghead_first), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagehead_first",pghead_first);
	
	//Add the pghead_last section
	QScriptValue pghead_last = _engine->newQObject(new Scripting::Section(_data->pghead_last), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagehead_last",pghead_last);
	
	//Add the pgfoot_any section
	QScriptValue pgfoot_any = _engine->newQObject(new Scripting::Section(_data->pgfoot_any), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagefoot_any",pgfoot_any);
	
	//Add the pgfoot_even section
	QScriptValue pgfoot_even = _engine->newQObject(new Scripting::Section(_data->pgfoot_even), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagefoot_even",pgfoot_even);
	
	//Add the pgfoot_odd section
	QScriptValue pgfoot_odd = _engine->newQObject(new Scripting::Section(_data->pgfoot_odd), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagefoot_odd",pgfoot_odd);
	
	//Add the pgfoot_first section
	QScriptValue pgfoot_first = _engine->newQObject(new Scripting::Section(_data->pgfoot_first), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagefoot_first",pgfoot_first);
	
	//Add the pgfoot_last section
	QScriptValue pgfoot_last = _engine->newQObject(new Scripting::Section(_data->pgfoot_last), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("pagefoot_last",pgfoot_last);
	
	//Engine constants for line styles, see http://doc.trolltech.com/4.3/qt.html#PenStyle-enum
	_engine->globalObject().setProperty("QtNoPen", QScriptValue(_engine, 0));
	_engine->globalObject().setProperty("QtSolidLine", QScriptValue(_engine, 1));
	_engine->globalObject().setProperty("QtDashLine", QScriptValue(_engine, 2));
	_engine->globalObject().setProperty("QtDotLine", QScriptValue(_engine, 3));
	_engine->globalObject().setProperty("QtDashDotLine", QScriptValue(_engine, 4));
	_engine->globalObject().setProperty("QtDashDotDotLine", QScriptValue(_engine, 5));
	
	//Evaluate the script now, we'll call the functions in it later;
	_engine->evaluate(_data->script);
	
}

 void KRScriptHandler::slotEnteredGroup(const QString &key, const QVariant &value)
{
	kDebug() << key << value << endl;
	_where = "(" + key + " = " + value.toString() + ")";
	_functions->setWhere(_where);
}
void KRScriptHandler::slotExitedGroup(const QString &key, const QVariant &value)
{
	kDebug() << key << value << endl;
	_where = "(" + key + " = " + value.toString() + ")";
	_functions->setWhere(_where);
}

void KRScriptHandler::slotEnteredSection(KRSectionData *section)
{
	kDebug() << endl;
	
	
	QScriptValue sec = _engine->globalObject().property(section->name() + "_onrender");
	
	if (sec.isFunction())
	{
		kDebug() << sec.call().toString() << endl;;
	}
	else
	{
		kDebug() << "No such function " << section->name() + "_onrender" << endl;
	}
	
	//The old way
	//kDebug() << (_engine->evaluate(section->eventOnRender())).toString() << endl;
}