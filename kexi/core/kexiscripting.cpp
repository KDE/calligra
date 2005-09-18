/* This file is part of the KDE project
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiscripting.h"

//include <qapplication.h>
#include <qcursor.h>
#include <qfile.h>
#include <kdebug.h>

#include "keximainwindow.h"

#ifdef KEXI_KROSS_SUPPORT
# include <scriptingcore/main/manager.h>
# include <scriptingcore/main/scriptcontainer.h>
# include <scriptingcore/api/exception.h>
#endif

/****************************************************************
 * KexiScriptContainer
 */

/// \internal
class KexiScriptContainerPrivate
{
    public:
#ifdef KEXI_KROSS_SUPPORT
        /// The \a Kross::Api::Manager singelton.
        Kross::Api::Manager* manager;
        /// The wrapped \a Kross::Api::ScriptContainer instance.
        Kross::Api::ScriptContainer::Ptr scriptcontainer;
#else
        /// The name this container has.
        QString name;
        /// The name of the interpreter.
        QString interpretername;
        /// The scripting code.
        QString code;
#endif
        /// The last error.
        QString lasterr;
        /// Remembered stdout and stderr output.
        QStringList output;
};

KexiScriptContainer::KexiScriptContainer(KexiScriptManager* manager, const QString& name)
    : QObject(manager, "KexiScriptContainer")
    , d(new KexiScriptContainerPrivate())
{
#ifdef KEXI_KROSS_SUPPORT
    d->manager = Kross::Api::Manager::scriptManager();
    d->scriptcontainer = d->manager->getScriptContainer(name);
    d->scriptcontainer->setInterpreterName("python"); // set default interpreter

    // Redirect stdout and stderr.
    //FIXME remember previous stdout+stderr and restore them in the dtor?
    d->scriptcontainer->addSlot("stdout", this, SLOT(addStdOut(const QString&)));
    d->scriptcontainer->addSlot("stderr", this, SLOT(addStdErr(const QString&)));
#else
    d->name = name;
#endif
}

KexiScriptContainer::~KexiScriptContainer()
{
#ifdef KEXI_KROSS_SUPPORT
    //TODO Don't redirect stdout and stderr any longer.
    //d->scriptcontainer->removeSignal("stdout");
    //d->scriptcontainer->removeSignal("stderr");
#endif
    delete d;
}

QString KexiScriptContainer::getInterpreterName()
{
#ifdef KEXI_KROSS_SUPPORT
    return d->scriptcontainer->getInterpreterName();
#else
    return d->interpretername;
#endif
}

void KexiScriptContainer::setInterpreterName(const QString& interpretername)
{
#ifdef KEXI_KROSS_SUPPORT
    d->scriptcontainer->setInterpreterName(interpretername);
#else
    d->interpretername = interpretername;
#endif
}

QString KexiScriptContainer::getCode()
{
#ifdef KEXI_KROSS_SUPPORT
    return d->scriptcontainer->getCode();
#else
    return d->code;
#endif
}

void KexiScriptContainer::setCode(const QString& code)
{
#ifdef KEXI_KROSS_SUPPORT
    d->scriptcontainer->setCode(code);
#else
    d->code = code;
#endif
}

QString KexiScriptContainer::getLastError()
{
    return d->lasterr;
}

QStringList KexiScriptContainer::getOutput()
{
    return d->output;
}

bool KexiScriptContainer::execute()
{
    d->output.clear(); // clear previous output
    emit clearOutput();

#ifdef KEXI_KROSS_SUPPORT
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    Kross::Api::Object::Ptr result = d->scriptcontainer->execute();
    bool ok = true;
    if( d->scriptcontainer->hadException() ) {
        Kross::Api::Exception::Ptr exception = d->scriptcontainer->getException();
        d->lasterr = exception->toString();
        addStdErr(d->lasterr);
        long line = exception->getLineNo();
        if(line >= 0)
            emit lineNo(line);
        ok = false;
    }
    QApplication::restoreOverrideCursor();
    return ok;
#else
    addStdErr( "KexiScriptManager::execute() called, but Kexi is compiled without Kross scripting support." );
    return false;
#endif
}

void KexiScriptContainer::addStdOut(const QString& s)
{
    d->output.append(s);
    emit addOutput(s);
}

void KexiScriptContainer::addStdErr(const QString& s)
{
    QString t = QString("<b>%1</b>").arg(s);
    d->output.append(t);
    emit addOutput(t);
}

/****************************************************************
 * KexiScriptManager
 */

/// \internal
class KexiScriptManagerPrivate
{
    public:
        /// The \a KexiMainWindow singelton instance.
        KexiMainWindow* mainwindow;
        /// Map of \a KexiScriptContainer children this \a KexiScriptManager
        QMap<QString, KexiScriptContainer*> scriptcontainers;
};

KexiScriptManager* KexiScriptManager::self(KexiMainWindow* mainwin)
{
    static KexiScriptManager* manager = 0;
    if(! manager)
        manager = new KexiScriptManager(mainwin);
    return manager;
}

KexiScriptManager::KexiScriptManager(KexiMainWindow* mainwindow)
    : QObject(mainwindow, "KexiScriptManager")
    , d(new KexiScriptManagerPrivate())
{
    d->mainwindow = mainwindow;

#ifdef KEXI_KROSS_SUPPORT
    // Publish the KexiMainWindow singelton instance. At least the KexiApp 
    // scripting-plugin depends on this instance and loading the plugin will 
    // fail if it's not avaiable.
    Kross::Api::Manager::scriptManager()->addQObject(mainwindow, "KexiMainWindow");
#endif
}

KexiScriptManager::~KexiScriptManager()
{
    delete d;
}

bool KexiScriptManager::hasScriptContainer(const QString& name)
{
    return d->scriptcontainers.contains(name);
}

KexiScriptContainer* KexiScriptManager::getScriptContainer(const QString& name, bool create)
{
    KexiScriptContainer* sc = d->scriptcontainers[name];
    if((! sc) && create) {
        sc = new KexiScriptContainer(this, name);
        d->scriptcontainers.replace(name, sc); // remember them
    }
    return sc;
}

const QStringList KexiScriptManager::getInterpreters()
{
#ifdef KEXI_KROSS_SUPPORT
    return Kross::Api::Manager::scriptManager()->getInterpreters();
#else
    return QStringList();
#endif
}

bool KexiScriptManager::executeFile(const QString& file, QString& error)
{
    QFile f(file);
    if(! f.open(IO_ReadOnly)) {
        error = i18n("Failed to read scriptingfile %1").arg(file);
        return false;
    }
    QString code = f.readAll();
    f.close();

    KexiScriptContainer* sc = new KexiScriptContainer(this, file);
    sc->setInterpreterName("python");
    sc->setCode(code);
    bool ok = sc->execute();
    if(! ok)
        error = sc->getLastError();
    delete sc;
    return ok;
}

#include "kexiscripting.moc"

