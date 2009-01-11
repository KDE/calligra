/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "formscript.h"
#include "scriptmanager.h"

#include "form.h"
#include "objecttree.h"
// Kross Includes
#include "main/manager.h"
#include "main/scriptcontainer.h"
#include "api/exception.h"

#include <kdebug.h>

using namespace KFormDesigner;

FormScript::FormScript(Form *form, ScriptManager *manager, const char *name)
        : QObject(manager, name), m_manager(manager), m_form(form)
{
    m_script = manager->krossManager()->getScriptContainer(form->widget()->name());
}

FormScript::~FormScript()
{
}

QString
FormScript::getCode(const QString &)
{
    /// \todo Allow to select only one function
    return m_script->getCode();
}

void
FormScript::setCode(const QString &code)
{
    m_script->setCode(code);
}

void
FormScript::appendCode(const QString &code)
{
    QString s = m_script->getCode();
    s.append("\n");
    s.append(code);
    m_script->setCode(s);
}

bool
FormScript::execute(const QString &functionName)
{
    /// \todo  support return value and arguments
    try {
        m_script->callFunction(functionName);
    } catch (Kross::Api::Exception& e) {
        kDebug() << QString("EXCEPTION type='%1' description='%2'").arg(e.type()).arg(e.description());
        return false;
    }
    return true;
}

void
FormScript::connectEvents()
{
    if (!m_form || !m_form->objectTree())
        return;
    // first call addQObject for each widget in the Form
    ObjectTreeHash *hash = m_form->objectTree()->hash();
    foreach (ObjectTreeItem *item, *hash) {
        m_script->addQObject(item->widget());
    }
    m_script->addQObject(m_form->widget());

    // Then we connect all signals
    foreach (Event* e, m_list) {
        if (e->type() == Event::Slot) {
            connect(e->sender(), e->signal(), e->receiver(), e->slot());
        } else if (e->type() == Event::UserFunction) {
            m_script->connect(e->sender(), e->signal(), e->slot());
        } else if (e->type() == Event::Action) {
            /// \todo connect signals with actions
        }
    }
}


#include "formscript.moc"

