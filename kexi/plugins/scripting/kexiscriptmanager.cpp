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

#include "kexiscriptmanager.h"
#include "kexiscriptpart.h"

#include <kdebug.h>

#ifdef KEXI_KROSS_SUPPORT
# include "main/manager.h"
# include "main/scriptcontainer.h"
# include "api/exception.h"
#endif

/*** KexiScriptContainer ***/

/// @internal
class KexiScriptContainerPrivate
{
    public:
#ifdef KEXI_KROSS_SUPPORT
        Kross::Api::Manager* manager;
        Kross::Api::ScriptContainer::Ptr scriptcontainer;
#else
        QString interpretername, code;
#endif
        QString output; // remember stdout and stderr scripting output.
};

KexiScriptContainer::KexiScriptContainer(KexiScriptManager* manager, const QString& name)
    : QObject(manager)
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

QString KexiScriptContainer::getOutput()
{
    return d->output;
}

bool KexiScriptContainer::execute()
{
    bool ret = false;
#ifdef KEXI_KROSS_SUPPORT
    try {
        ret = d->scriptcontainer->execute();
    }
    catch(Kross::Api::Exception& e) {
        kdDebug() << QString("EXCEPTION type='%1' description='%2'").arg(e.type()).arg(e.description()) << endl;
    }
#else
    kdWarning() << "KexiScriptManager::execute() called, but Kexi is compiled without Kross scripting support." << endl;
#endif
    return ret;
}

void KexiScriptContainer::addStdOut(const QString& s)
{
    d->output += s;
    emit stdOut(s);
}

void KexiScriptContainer::addStdErr(const QString& s)
{
    d->output += QString("<b>%1</b>").arg(s);
    emit stdErr(s);
}

/*** KexiScriptManager ***/

KexiScriptManager::KexiScriptManager(KexiScriptPart* part)
    : QObject(part)
{
}

KexiScriptManager::~KexiScriptManager()
{
}

KexiScriptContainer* KexiScriptManager::getScriptContainer(const QString& name)
{
    if(m_scriptcontainers.contains(name))
        return m_scriptcontainers[name];
    KexiScriptContainer* sc = new KexiScriptContainer(this, name);
    m_scriptcontainers.replace(name, sc);
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

#include "kexiscriptmanager.moc"

