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

#include <krsectiondata.h>
#include "krscriptsection.h"

KRScriptHandler::KRScriptHandler(KexiDB::Connection* c, QScriptEngine *e)
{
	_conn = c;
	_engine = e;
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
	_functions = new KRScriptFunctions(_conn);
	QScriptValue funcs = _engine->newQObject(_functions, QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	_engine->globalObject().setProperty("functions",funcs);
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

	QScriptValue sec = _engine->newQObject(new Scripting::Section(section), QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
	
	_engine->globalObject().setProperty(section->name(),sec);
	
	kDebug() << (_engine->evaluate(section->eventOnRender())).toString() << endl;
}