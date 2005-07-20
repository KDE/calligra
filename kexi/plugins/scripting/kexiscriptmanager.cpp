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
//#include <kparts/factory.h>
//#include <klibloader.h>
//#include <kmdimainfrm.h>
//#include <kmainwindow.h>
//#include <kpopupmenu.h>

#ifdef KEXI_KROSS_SUPPORT
# include "main/manager.h"
# include "main/scriptcontainer.h"
# include "api/exception.h"
#endif

//! @internal
class KexiScriptContainerPrivate
{
    public:
#ifdef KEXI_KROSS_SUPPORT
        Kross::Api::Manager* manager;
        KSharedPtr<Kross::Api::ScriptContainer> scriptcontainer;
#else
        QString interpretername, code;
#endif
};

/*** KexiScriptContainer ***/

KexiScriptContainer::KexiScriptContainer(KexiScriptManager* manager, const QString& name)
    : QObject(manager)
    , d(new KexiScriptContainerPrivate())
{
#ifdef KEXI_KROSS_SUPPORT
    d->manager = Kross::Api::Manager::scriptManager();
    d->scriptcontainer = d->manager->getScriptContainer(name);

    d->scriptcontainer->connect(SIGNAL(stdOut(const QString&)), this, SIGNAL(stdOut(const QString&)));
    d->scriptcontainer->connect(SIGNAL(stdErr(const QString&)), this, SIGNAL(stdErr(const QString&)));
#endif
}

KexiScriptContainer::~KexiScriptContainer()
{
#ifdef KEXI_KROSS_SUPPORT
    d->scriptcontainer->disconnect(SIGNAL(stdOut(const QString&)), this, SLOT(stdOut(const QString&)));
    d->scriptcontainer->disconnect(SIGNAL(stdErr(const QString&)), this, SLOT(stdErr(const QString&)));
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

#include "kexiscriptmanager.moc"

