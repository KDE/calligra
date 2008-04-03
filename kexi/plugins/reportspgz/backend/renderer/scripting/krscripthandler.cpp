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

#include <kexidb/cursor.h>
#include "krscriptfunctions.h"

#include <parsexmlutils.h>
#include <krsectiondata.h>
#include "krscriptsection.h"
#include "krscriptdebug.h"
#include <krobjectdata.h>
#include <krfielddata.h>
#include <kmessagebox.h>
#include "krscriptconstants.h"
#include <krreportdata.h>
#include <krdetailsectiondata.h>
#include "krscriptreport.h"

KRScriptHandler::KRScriptHandler(const KexiDB::Cursor* cu, KRReportData* d)
{
	_conn = cu->connection();
	_data = d;
	_curs = cu;
	
	// Create the Kross::Action instance .
	_action = new Kross::Action(this, "ReportScript");
	
	_action->setInterpreter("javascript");
	
}
 
KRScriptHandler::~KRScriptHandler()
{
	
}

void KRScriptHandler::setSource(const QString &s)
{
	_source = s;	
}

void KRScriptHandler::slotInit()
{
	//Add math functions to the script
	_functions = new KRScriptFunctions(_curs);
	_action->addObject(_functions, "math");
	
	_constants = new KRScriptConstants();
	_action->addObject(_constants, "constants");
	
	//A simple debug function to allow printing from functions
	_debug = new KRScriptDebug();
	_action->addObject(_debug, "debug");
	
	//Add a general report object
	_action->addObject(new Scripting::Report(_data), "report");
	
	//Add the detail section
	_action->addObject(new Scripting::Section(_data->detailsection->detail), "detail");
	
	//Add the pghead_any section
	_action->addObject(new Scripting::Section(_data->pghead_any), "pagehead_any");
	
	//Add the pghead_even section
	_action->addObject(new Scripting::Section(_data->pghead_even), "pagehead_even");
	
	//Add the pghead_odd section
	_action->addObject(new Scripting::Section(_data->pghead_odd), "pagehead_odd");
	
	//Add the pghead_first section
	_action->addObject(new Scripting::Section(_data->pghead_first), "pagehead_first");
	
	//Add the pghead_last section
	_action->addObject(new Scripting::Section(_data->pghead_last), "pagehead_last");
	
	//Add the pgfoot_any section
	_action->addObject(new Scripting::Section(_data->pgfoot_any), "pagefoot_any");
	
	//Add the pgfoot_even section
	_action->addObject(new Scripting::Section(_data->pgfoot_even), "pagefoot_even");
	
	//Add the pgfoot_odd section
	_action->addObject(new Scripting::Section(_data->pgfoot_odd), "pagefoot_odd");
	
	//Add the pgfoot_first section
	_action->addObject(new Scripting::Section(_data->pgfoot_first), "pagefoot_first");
	
	//Add the pgfoot_last section
	_action->addObject(new Scripting::Section(_data->pgfoot_last), "pagefoot_last");
	
	//Engine constants for line styles, see http://doc.trolltech.com/4.3/qt.html#PenStyle-enum
	//_engine->globalObject().setProperty("QtNoPen", QScriptValue(_engine, 0));
	//_engine->globalObject().setProperty("QtSolidLine", QScriptValue(_engine, 1));
	//_engine->globalObject().setProperty("QtDashLine", QScriptValue(_engine, 2));
	//_engine->globalObject().setProperty("QtDotLine", QScriptValue(_engine, 3));
	//_engine->globalObject().setProperty("QtDashDotLine", QScriptValue(_engine, 4));
	//_engine->globalObject().setProperty("QtDashDotDotLine", QScriptValue(_engine, 5));
	
	//Evaluate the script now, we'll call the functions in it later;
	//_engine->evaluate(_data->script);
	
	_action->setCode((_data->script + "\n" + fieldFunctions()).toLocal8Bit());
	
	kDebug() << _action->code() << endl;
	
	_action->trigger();
	
	kDebug() << "Function Names:" << _action->functionNames()<< endl;
	
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
	kDebug() << section->name() << endl;
	
	if (_action->functionNames().contains(section->name() + "_onrender"))
	{
		QVariant result = _action->callFunction(section->name() + "_onrender");
		displayErrors();
	}
}

void KRScriptHandler::populateEngineParameters(KexiDB::Cursor *q)
{
	
}

QString KRScriptHandler::fieldFunctions()
{
	QString funcs;
	QString func;

	QList<KRObjectData *>obs = _data->objects();
	foreach (KRObjectData* o, obs)
	{
		if (o->type() == KRObjectData::EntityField)
		{
			KRFieldData* fld = o->toField();
			if (fld->column()[0] == '=')
			{
				//The field contains an expression
				func = QString("function %1_onrender_(){return %2;}").arg(fld->entityName().toLower()).arg(fld->column().mid(1));
				
				funcs += func + "\n";
			}
			
		}
	}
	
	return funcs;
}

QVariant KRScriptHandler::evaluate(const QString &field)
{	
	QString func = field.toLower() + "_onrender_";
	
	if (_action->functionNames().contains(func))
	{
		kDebug() << "*** Evaluating field" << endl;
		return _action->callFunction(func);
	}
	else
	{
		return QVariant();
	}
}

void KRScriptHandler::displayErrors()
{
	if (_action->hadError())
	{
		KMessageBox::error(0, _action->errorMessage());
	}
}