/*
 * This file is part of KSpread
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ScriptingPart.h"
#include "ScriptingModule.h"
// Qt
#include <QFileInfo>
// KDE
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kcmdlineargs.h>
#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>
// KSpread
#include <part/Doc.h>
#include <part/View.h>
// Kross
#include <kross/core/manager.h>

typedef KGenericFactory< ScriptingPart > KSpreadScriptingFactory;
K_EXPORT_COMPONENT_FACTORY(krossmodulekspread, KSpreadScriptingFactory("krossmodulekspread"))

/// \internal d-pointer class.
class ScriptingPart::Private
{
public:
};

ScriptingPart::ScriptingPart(QObject* parent, const QStringList& list)
        : KoScriptingPart(new ScriptingModule(parent), list)
        , d(new Private())
{
    setComponentData(ScriptingPart::componentData());
    setXMLFile(KStandardDirs::locate("data", "kspread/kpartplugins/scripting.rc"), true);
    kDebug() << "Scripting plugin. Class:" << metaObject()->className() << ", Parent:" << parent->metaObject()->className();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args) {
        QStringList errors;
        foreach(QString ba, args->getOptionList("scriptfile")) {
            QUrl url(ba);
            QFileInfo fi(url.path());
            const QString file = fi.absoluteFilePath();
            if (! fi.exists()) {
                errors << i18n("Scriptfile \"%1\" does not exist.", file);
                continue;
            }
            if (! fi.isExecutable()) {
                errors << i18n("Scriptfile \"%1\" is not executable. Please set the executable-attribute on that file.", file);
                continue;
            }
            { // check whether file is not in some temporary directory.
                QStringList tmpDirs = KGlobal::dirs()->resourceDirs("tmp");
                tmpDirs += KGlobal::dirs()->resourceDirs("cache");
                tmpDirs.append("/tmp/");
                tmpDirs.append("/var/tmp/");
                bool inTemp = false;
                foreach(QString tmpDir, tmpDirs) {
                    if (file.startsWith(tmpDir)) {
                        inTemp = true;
                        break;
                    }
                }
                if (inTemp) {
                    errors << i18n("Scriptfile \"%1\" is in a temporary directory. Execution denied.", file);
                    continue;
                }
            }
            if (! Kross::Manager::self().executeScriptFile(url))
                errors << i18n("Failed to execute scriptfile \"%1\"", file);
        }
        if (errors.count() > 0)
            KMessageBox::errorList(module()->view(), i18n("Errors on execution of scripts."), errors);
    }
}

ScriptingPart::~ScriptingPart()
{
    //kDebug() <<"ScriptingPart::~ScriptingPart()";
    delete d;
}

#include "ScriptingPart.moc"
