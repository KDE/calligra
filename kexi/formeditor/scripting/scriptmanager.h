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

#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <qobject.h>
#include <q3ptrdict.h>

class FormScript;

namespace Kross
{
namespace Api  {
class Manager;
}
}

namespace KFormDesigner
{
class FormManager;
class Form;
}

using namespace KFormDesigner;

class ScriptManager : public QObject
{
    Q_OBJECT

public:
    ScriptManager(QObject *parent = 0, const char *name = 0);
    ~ScriptManager();

    /*! \return The FormScript object associated to this Form. */
    FormScript*  scriptForForm(Form *form);

    void  setFormManager(FormManager *manager);
    FormManager*  formManager() {
        return m_formManager;
    }
    Kross::Api::Manager*  krossManager() {
        return m_manager;
    }

public slots:
    /*! Called when a form is deleted. It is removed from the dict. */
    void slotFormDeleted(KFormDesigner::Form *form);
    /*! \return A new FormScript object associated to the Form \a form.  */
    FormScript*  newFormScript(KFormDesigner::Form *form);

private:
    Kross::Api::Manager  *m_manager;
    KFormDesigner::FormManager *m_formManager;
    Q3PtrDict<FormScript>  m_dict;
};

#endif

