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
#include <KexiMainWindowIface.h>
#include <kexiproject.h>
#include <kexipart.h>
#include <kexiutils/tristate.h>

#include "../../../scripting/kexiscripting/kexiscriptadaptor.h"

#include "krscriptfunctions.h"
#include <parsexmlutils.h>
#include <krsectiondata.h>
#include "krscriptsection.h"
#include "krscriptdebug.h"
#include <krobjectdata.h>
#include <krfielddata.h>
#include <krcheckdata.h>
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

    _action = 0;
    _functions = 0;
    _constants = 0;
    _debug = 0;
    _draw = 0;

    // Create the Kross::Action instance .
    _action = new Kross::Action(this, "ReportScript");

    _action->setInterpreter(d->interpreter());

    //Add a kexi object to provide kexidb and extra functionality
    _kexi = new KexiScriptAdaptor();
    _action->addObject( _kexi, "Kexi" );
    
    //Add math functions to the script
    _functions = new KRScriptFunctions(_curs);
    _action->addObject(_functions, "field");

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
    _report = new Scripting::Report(_data);

    //Add the sections
    QList<KRSectionData*> secs = _data->sections();
    foreach(KRSectionData *sec, secs) {
        _sectionMap[sec] = new Scripting::Section(sec);
	_sectionMap[sec]->setParent( _report );
	_sectionMap[sec]->setObjectName(sec->name());
    }
    
    _action->addObject(_report, _data->name());
    kDebug() << "Report name is" << _data->name();
    
    _action->setCode( fieldFunctions().toLocal8Bit() + "\n" + scriptCode().toLocal8Bit());

    kDebug() << _action->code();

    _action->trigger();

    if (_action->hadError()) {
        KMessageBox::error(0, _action->errorMessage());
    } else {
        kDebug() << "Function Names:" << _action->functionNames();
    }

    _report->eventOnOpen();
}

KRScriptHandler::~KRScriptHandler()
{
    delete _report;
    delete _functions;
    delete _constants;
    delete _debug;
    delete _draw;
    delete _action;
}

void KRScriptHandler::setSource(const QString &s)
{
    _source = s;
    _functions->setSource(_source);
}

void KRScriptHandler::slotEnteredGroup(const QString &key, const QVariant &value)
{
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
    if (cp)
        _draw->setPage(cp);
    _draw->setOffset(off);

    Scripting::Section *ss = _sectionMap[section];
    if (ss)
    {
	ss->eventOnRender();
    }
    
    return;
    
    if (!_action->hadError() && _action->functionNames().contains(section->name() + "_onrender")) {
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
    foreach(KRObjectData* o, obs) {
        //The field or check contains an expression
        //TODO this is a horrible hack, need to get a similar feature into kross
        if (o->type() == KRObjectData::EntityField) {
            KRFieldData* fld = o->toField();
            if (fld->controlSource()[0] == '=') {
                func = QString("function %1_onrender_(){return %2;}").arg(fld->entityName().toLower()).arg(fld->controlSource().mid(1));

                funcs += func + "\n";
            }
        }
        if (o->type() == KRObjectData::EntityCheck) {
        KRCheckData* chk = o->toCheck();
            if (chk->controlSource()[0] == '=') {
                func = QString("function %1_onrender_(){return %2;}").arg(chk->entityName().toLower()).arg(chk->controlSource().mid(1));

                funcs += func + "\n";
            }

        }
    }


    return funcs;
}

QVariant KRScriptHandler::evaluate(const QString &field)
{
    QString func = field.toLower() + "_onrender_";

    if (!_action->hadError() && _action->functionNames().contains(func)) {
        return _action->callFunction(func);
    } else {
        return QVariant();
    }
}

void KRScriptHandler::displayErrors()
{
    if (_action->hadError()) {
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
    kDebug() << w;
    return w;
}

QString KRScriptHandler::scriptCode()
{
    QList<int> scriptids = KexiMainWindowIface::global()->project()->dbConnection()->objectIds(KexiPart::ScriptObjectType);
    QStringList scriptnames = KexiMainWindowIface::global()->project()->dbConnection()->objectNames(KexiPart::ScriptObjectType);
    QString scripts;
    
    int id;
    int i = 0;
    QString script;
   
    foreach (id, scriptids) {
        kDebug() << "ID:" << id;
        tristate res;
        res = KexiMainWindowIface::global()->project()->dbConnection()->loadDataBlock(id, script, QString());
        if (res == true){
            QDomDocument domdoc;
            bool parsed = domdoc.setContent(script, false);

            if (! parsed) {
            kDebug() << "XML parsing error";
            return false;
            }

            QDomElement scriptelem = domdoc.namedItem("script").toElement();
            if (scriptelem.isNull()) {
            kDebug() << "script domelement is null";
            return false;
            }

            QString interpretername = scriptelem.attribute("language");
            kDebug() << interpretername;
            kDebug() << scriptelem.attribute("scripttype");
            
            if (_data->interpreter() == interpretername && (scriptelem.attribute("scripttype") == "module" || _data->script() == scriptnames[i] )) {
                scripts += '\n' + scriptelem.text().toUtf8();
            }
            ++i;
        }
        else{
            kDebug() << "Unable to loadDataBlock";
        }
    }
    
    return scripts;
}
