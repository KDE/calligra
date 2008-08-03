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

#ifndef FORMSCRIPT_H
#define FORMSCRIPT_H

#include "kexievents.h"

#include <qobject.h>
#include <qstring.h>
#include <ksharedptr.h>

class ScriptManager;

namespace KFormDesigner
{
class Form;
}

namespace Kross
{
namespace Api  {
class ScriptContainer;
}
}

using namespace KFormDesigner;

//! A class that stores the code and events related to a single form
class FormScript : public QObject
{
    Q_OBJECT

public:
    FormScript(Form *form, ScriptManager *manager, const char *name = 0);
    ~FormScript();

    EventList*  eventList()  {
        return  &m_list;
    }
    Kross::Api::ScriptContainer*   scriptContainer()  {
        return m_script;
    }

    /*! \return The code of funtionName. If parameter is empty, it returns the full code of this form.*/
    QString  getCode(const QString &functionName = QString());
    /*! Replaces the actual form code with the string \a code.
     Called eg by (future) script editor. */
    void  setCode(const QString &code);
    /*! Adds the string \a code at the end of current code. Used to add a function in the script. */
    void  appendCode(const QString &code);

    /*! Executes the \a functionName.
     \todo how do we give parameters? */
    bool  execute(const QString &functionName);
    /*! Really connects all events in the list.
     Also calls Kross;;Api::Manager::addObject for each widget in the form to allow the user to
     use these widgets in the script.  */
    void  connectEvents();

private:
    ScriptManager  *m_manager;
    Form  *m_form;
    KSharedPtr<Kross::Api::ScriptContainer> m_script;
    EventList  m_list;
};

#endif
