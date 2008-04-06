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
#include <renderobjects.h>
#include "krscriptdraw.h"

KRScriptHandler::KRScriptHandler(const KexiDB::Cursor* cu, KRReportData* d)
{
	_conn = cu->connection();
	_data = d;
	_curs = cu;
	
	// Create the Kross::Action instance .
	_action = new Kross::Action(this, "ReportScript");
	
	_action->setInterpreter("javascript");
	
	//Add math functions to the script
	_functions = new KRScriptFunctions(_curs);
	_action->addObject(_functions, "math");
	
	//Add constants object
	_constants = new KRScriptConstants();
	_action->addObject(_constants, "constants");
	
	//A simple debug function to allow printing from functions
	_debug = new KRScriptDebug();
	_action->addObject(_debug, "debug");
	
	//A simple drawing object
	_draw = new KRScriptDraw();
	_action->addObject(_draw, "draw");
	
	//Add a general report object
	_action->addObject(new Scripting::Report(_data), "report");
	

	//Add the detail section
	//_action->addObject(new Scripting::Section(_data->detailsection->detail), "detail");
	
	
	QList<KRSectionData*> secs = _data->sections();
	foreach (KRSectionData *sec, secs)
	{
		_action->addObject(new Scripting::Section(sec), sec->name());
	}
	
#if 0
	for (int i = 0; i <12 ; ++i)
	{
		sec = _data->section((KRReportData::Section)(i+1));
		if (sec)
		{
			_action->addObject(new Scripting::Section(sec), sec->name());
		}
	}
#endif
	_action->setCode((_data->script + "\n" + fieldFunctions()).toLocal8Bit());
	
	kDebug() << _action->code() << endl;
	
	_action->trigger();
	
	if (_action->hadError())
	{
		KMessageBox::error(0, _action->errorMessage());
	}
	else
	{
		kDebug() << "Function Names:" << _action->functionNames()<< endl;
	}
}
 
KRScriptHandler::~KRScriptHandler()
{
 delete _action;
 delete _functions;
 delete _constants;
 delete _debug;
}

void KRScriptHandler::setSource(const QString &s)
{
	_source = s;
	_functions->setSource(_source);
}

void KRScriptHandler::slotEnteredGroup(const QString &key, const QVariant &value)
{
	kDebug() << key << value << endl;
	_groups[key] = value;

	_functions->setWhere(where());
}
void KRScriptHandler::slotExitedGroup(const QString &key, const QVariant &value)
{
	_groups.remove(key);
	_functions->setWhere(where());
}

void KRScriptHandler::slotEnteredSection(KRSectionData *section, OROPage* cp, QPointF off)
{
	kDebug() << section->name() << endl;
	
	_draw->setPage(cp);
	_draw->setOffset(off);
	
	if (!_action->hadError() && _action->functionNames().contains(section->name() + "_onrender"))
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
	
	if (!_action->hadError() && _action->functionNames().contains(func) )
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

QString KRScriptHandler::where()
{
	QString w;
	QMap<QString, QVariant>::const_iterator i = _groups.constBegin();
	while (i != _groups.constEnd()) {
		w += "(" + i.key() + " = " + i.value().toString() + ") AND ";
		++i;
	}
	w = w.mid(0, w.length() - 4);
	kDebug() << w << endl;
	return w;
}