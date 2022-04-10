// This file is part of KSpread
// SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
// SPDX-FileCopyrightText: 2006 Isaac Clerencia <isaac@warp.es>
// SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ScriptingModule.h"

#include <part/Part.h>
#include <part/View.h>
#include <part/MapAdaptor.h>
#include <part/SheetAdaptor.h>
#include <part/ViewAdaptor.h>

#include "ScriptingFunction.h"
#include "ScriptingWidgets.h"
#include "ScriptingReader.h"
#include "ScriptingWriter.h"
#include "ScriptingListener.h"
#include "ScriptingDebug.h"
#include "sheetsscripting_export.h"

#include <QPointer>
#include <QLayout>


extern "C" {
    SHEETSSCRIPTING_EXPORT QObject* krossmodule() {
        return new ScriptingModule();
    }
}

/// \internal d-pointer class.
class ScriptingModule::Private
{
public:
    QPointer<Calligra::Sheets::Doc> doc;
    QHash< QString, ScriptingFunction* > functions;
    QStringList functionnames;
};

ScriptingModule::ScriptingModule(QObject* parent)
        : KoScriptingModule(parent, "KSpread")
        , d(new Private())
{
    d->doc = 0;
}

ScriptingModule::~ScriptingModule()
{
    debugSheetsScripting << "ScriptingModule::~ScriptingModule()";
    delete d;
}

Calligra::Sheets::View* ScriptingModule::kspreadView()
{
    return dynamic_cast< Calligra::Sheets::View* >(KoScriptingModule::view());
}

Calligra::Sheets::Doc* ScriptingModule::kspreadDoc()
{
    if (! d->doc) {
        if (Calligra::Sheets::View* v = kspreadView())
            d->doc = v->doc();
        if (! d->doc) {
            Calligra::Sheets::Part *part = new Calligra::Sheets::Part(this);
            d->doc = new Calligra::Sheets::Doc(part);
            part->setDocument(d->doc);
        }
    }
    return d->doc;
}

KoDocument* ScriptingModule::doc()
{
    return kspreadDoc();
}

QObject* ScriptingModule::map()
{
    return kspreadDoc()->map()->findChild< Calligra::Sheets::MapAdaptor* >();
}

QObject* ScriptingModule::view()
{
    Calligra::Sheets::View* v = kspreadView();
    return v ? v->findChild< Calligra::Sheets::ViewAdaptor* >() : 0;
}

QObject* ScriptingModule::currentSheet()
{
    Calligra::Sheets::View* v = kspreadView();
    Calligra::Sheets::Sheet* s = v ? v->activeSheet() : 0;
    return s ? s->findChild< Calligra::Sheets::SheetAdaptor* >() : 0;
}

QObject* ScriptingModule::sheetByName(const QString& name)
{
    if (kspreadDoc()->map())
        for(Calligra::Sheets::SheetBase* sheet : kspreadDoc()->map()->sheetList()) {
        if (sheet->sheetName() == name) {
            Calligra::Sheets::Sheet *fullSheet = dynamic_cast<Calligra::Sheets::Sheet *>(sheet);
            return fullSheet->findChild< Calligra::Sheets::SheetAdaptor* >(); {
            }
        }
    }
    return 0;
}

QStringList ScriptingModule::sheetNames()
{
    QStringList names;
    for(Calligra::Sheets::SheetBase* sheet : kspreadDoc()->map()->sheetList()) {
        names.append(sheet->sheetName());
    }
    return names;
}

bool ScriptingModule::hasFunction(const QString& name)
{
    return d->functions.contains(name);
}

QObject* ScriptingModule::function(const QString& name)
{
    if (d->functions.contains(name))
        return d->functions[name];
    ScriptingFunction* function = new ScriptingFunction(this);
    function->setName(name);
    d->functions.insert(name, function);
    d->functionnames.append(name);
    return function;
}

QObject* ScriptingModule::createListener(const QString& sheetname, const QString& range)
{
    Calligra::Sheets::Sheet* sheet = dynamic_cast<Calligra::Sheets::Sheet *>(kspreadDoc()->map()->findSheet(sheetname));
    if (! sheet) return 0;
    Calligra::Sheets::Region region = kspreadDoc()->map()->regionFromName(range, sheet);
    QRect r = region.firstRange();
    return new Calligra::Sheets::ScriptingCellListener(sheet, r.isNull() ? sheet->usedArea() : r);
}

bool ScriptingModule::openUrl(const QString& url)
{
    return kspreadDoc()->openUrl(QUrl(url)); // QT5TODO: check QUrl usage here
}

bool ScriptingModule::saveUrl(const QString& url)
{
    return kspreadDoc()->saveAs(QUrl(url)); // QT5TODO: check QUrl usage here
}

bool ScriptingModule::importUrl(const QString& url)
{
    return kspreadDoc()->importDocument(QUrl(url)); // QT5TODO: check QUrl usage here
}

bool ScriptingModule::exportUrl(const QString& url)
{
    return kspreadDoc()->exportDocument(QUrl(url)); // QT5TODO: check QUrl usage here
}

QObject* ScriptingModule::reader()
{
    return new ScriptingReader(this);
}

QObject* ScriptingModule::writer()
{
    return new ScriptingWriter(this);
}

QWidget* ScriptingModule::createSheetsListView(QWidget* parent)
{
    ScriptingSheetsListView* listview = new ScriptingSheetsListView(this, parent);
    if (parent && parent->layout())
        parent->layout()->addWidget(listview);
    return listview;
}
