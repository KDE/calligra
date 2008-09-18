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

#include "scriptmanager.h"
#include "formscript.h"

#include "form.h"
#include "formmanager.h"
// Kross includes
#include "main/manager.h"

using KFormDesigner::Form;

ScriptManager::ScriptManager(QObject *parent, const char *name)
        : QObject(parent, name)
{
    m_manager = Kross::Api::Manager::scriptManager();
    m_dict.setAutoDelete(true);
}

ScriptManager::~ScriptManager()
{
}

FormScript*
ScriptManager::newFormScript(Form *form)
{
    FormScript *script = new FormScript(form, this);
    m_dict.insert(form, script);
    return script;
}

FormScript*
ScriptManager::scriptForForm(Form *form)
{
    return m_dict[form];
}

void
ScriptManager::setFormManager(FormManager *manager)
{
    m_formManager = manager;
    connect(m_formManager, SIGNAL(aboutToDeleteForm(KFormDesigner::Form*)), this, SLOT(slotFormDeleted(KFormDesigner::Form*)));
    connect(m_formManager, SIGNAL(formCreated(KFormDesigner::Form*)), this, SLOT(newFormScript(KFormDesigner::Form*)));
}

void
ScriptManager::slotFormDeleted(KFormDesigner::Form *form)
{
    m_dict.remove(form);
}

#include "scriptmanager.moc"

