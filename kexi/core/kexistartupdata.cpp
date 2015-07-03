/* This file is part of the KDE project
   Copyright (C) 2004-2015 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexistartupdata.h"
#include "kexi.h"
#include "KexiCommandLineOptions.h"
#include "config-kexi.h"

#include <KDbDriver>
#include <KDbDriverManager>

#include <KAboutData>

#include <QCommandLineParser>

class KexiStartupData::Private
{
public:
    Private();
    ~Private();

    QCommandLineParser parser;
    KexiCommandLineOptions options;
    KexiProjectData *projectData;
    Action action;
    KexiStartupData::Import importActionData;
    bool forcedUserMode;
    bool forcedDesignMode;
    bool isProjectNavigatorVisible;
    bool isMainMenuVisible;
    bool createDB;
    bool dropDB;
    bool alsoOpenDB;
    bool forcedFullScreen;
};

KexiStartupData::Private::Private()
    : options(0/*&parser*/)
    , projectData(0), action(KexiStartupData::DoNothing), forcedUserMode(false)
    , forcedDesignMode(false), isProjectNavigatorVisible(false), forcedFullScreen(false)
{
}

KexiStartupData::Private::~Private()
{
    delete projectData;
}

KexiStartupData::KexiStartupData() : d(new Private)
{
}

KexiStartupData::~KexiStartupData()
{
    delete d;
}

KexiProjectData *KexiStartupData::projectData()
{
    return d->projectData;
}

void KexiStartupData::setProjectData(KexiProjectData *data)
{
    if (data != d->projectData) {
        delete d->projectData;
    }
    d->projectData = data;
}

KexiStartupData::Action KexiStartupData::action() const
{
    return d->action;
}

void KexiStartupData::setAction(KexiStartupData::Action action)
{
    d->action = action;
}

bool KexiStartupData::forcedUserMode() const
{
    return d->forcedUserMode;
}

void KexiStartupData::setForcedUserMode(bool set)
{
    d->forcedUserMode = set;
}

bool KexiStartupData::forcedDesignMode() const
{
    return d->forcedDesignMode;
}

void KexiStartupData::setForcedDesignMode(bool set)
{
    d->forcedDesignMode = set;
}

bool KexiStartupData::isProjectNavigatorVisible() const
{
    if (d->forcedUserMode && !d->forcedDesignMode)
        return d->isProjectNavigatorVisible;
    return true;
}

void KexiStartupData::setProjectNavigatorVisible(bool set)
{
    d->isProjectNavigatorVisible = set;
}

bool KexiStartupData::isMainMenuVisible() const
{
    return d->isMainMenuVisible;
}

void KexiStartupData::setMainMenuVisible(bool set)
{
    d->isMainMenuVisible = set;
}

KexiStartupData::Import KexiStartupData::importActionData() const
{
    return d->importActionData;
}

void KexiStartupData::setImportActionData(KexiStartupData::Import import)
{
    d->importActionData = import;
}

KexiStartupData::Import::Import()
{
}

KexiStartupData::Import::operator bool() const
{
    return !fileName.isEmpty() && !mimeType.isEmpty();
}

bool KexiStartupData::forcedFullScreen() const
{
    return d->forcedFullScreen;
}

void KexiStartupData::setForcedFullScreen(bool set)
{
    d->forcedFullScreen = set;
}

//! Command line options
KexiCommandLineOptions KexiStartupData::options() const
{
    return d->options;
}

tristate KexiStartupData::parseOptions()
{
    //KAboutData::applicationData().setupCommandLine(&d->parser);
    //d->parser.addHelpOption();
    //d->parser.addVersionOption();
    d->parser.setApplicationDescription(KAboutData::applicationData().shortDescription());

#define ADD_OPTION(o) \
    if (!d->parser.addOption(d->options.o)) { \
        qWarning() << "Could not add option" << d->options.o.names(); \
        return false; \
    } else { qDebug() << "OK" << d->options.o.names(); }
    ADD_OPTION(createDb)
    ADD_OPTION(createAndOpenDb)
    ADD_OPTION(dropDb)
    ADD_OPTION(dbDriver)
    ADD_OPTION(fileType)
    ADD_OPTION(connectionShortcut)
    ADD_OPTION(readOnly)
    ADD_OPTION(userMode)
    ADD_OPTION(designMode)
    ADD_OPTION(showNavigator)
    ADD_OPTION(hideMenu)
    ADD_OPTION(open)
    ADD_OPTION(design)
    ADD_OPTION(editText)
    ADD_OPTION(execute)
    ADD_OPTION(newObject)
#ifndef KEXI_NO_QUICK_PRINTING
    ADD_OPTION(print)
    ADD_OPTION(printPreview)
#endif
    ADD_OPTION(user)
    ADD_OPTION(host)
    ADD_OPTION(port)
    ADD_OPTION(localSocket)
    ADD_OPTION(skipConnDialog)
    ADD_OPTION(fullScreen)
    ADD_OPTION(listPlugins)
#undef ADD_OPTION

    d->parser.addPositionalArgument("file",
        xi18nc("<file> argument description for the command line",
               "Kexi database project filename, Kexi shortcut filename, or name of a Kexi "
               "database project on a server to open."));

    qDebug() << QCoreApplication::instance()->arguments();
    if (!d->parser.parse(QCoreApplication::instance()->arguments())) {
        return false;
    }
    KAboutData::applicationData().processCommandLine(&d->parser);
    return true;
}

bool KexiStartupData::isSet(const QCommandLineOption & option) const
{
    return d->parser.isSet(option);
}

QString	KexiStartupData::value(const QCommandLineOption & option) const
{
    return d->parser.value(option);
}

QStringList KexiStartupData::positionalArguments() const
{
    return d->parser.positionalArguments();
}

QString KexiStartupData::helpText() const
{
    return d->parser.helpText();
}
