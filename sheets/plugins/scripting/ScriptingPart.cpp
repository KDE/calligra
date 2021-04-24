// This file is part of KSpread
// SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
// SPDX-FileCopyrightText: 2006 Isaac Clerencia <isaac@warp.es>
// SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ScriptingPart.h"

#include "ScriptingModule.h"
#include "ScriptingDebug.h"
// Qt
#include <QFileInfo>
#include <QStandardPaths>
// KF5
#include <kpluginfactory.h>
#include <kstandarddirs.h>
#include <kcmdlineargs.h>
#include <KLocalizedString>
#include <kmessagebox.h>
#include <kglobal.h>
// KSpread
#include <part/Doc.h>
#include <part/View.h>
// Kross
#include <kross/core/manager.h>


K_PLUGIN_FACTORY_WITH_JSON(KSpreadScriptingFactory, "sheetsscripting.json",
                           registerPlugin<ScriptingPart>();)


ScriptingPart::ScriptingPart(QObject* parent, const QVariantList& argList)
    : KoScriptingPart(new ScriptingModule(parent))
{
    Q_UNUSED(argList);
    //setComponentData(ScriptingPart::componentData());
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrasheets/viewplugins/scripting.rc"), true);
    debugSheetsScripting << "Scripting plugin. Class:" << metaObject()->className() << ", Parent:" << parent->metaObject()->className();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args) {
        QStringList errors;
        foreach(const QString &ba, args->getOptionList("scriptfile")) {
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
                foreach(const QString &tmpDir, tmpDirs) {
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
    //debugSheetsScripting <<"ScriptingPart::~ScriptingPart()";
}

#include "ScriptingPart.moc"
