/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIFORMEVENTHANDLER_H
#define KEXIFORMEVENTHANDLER_H

#include <QWidget>
#include <kaction.h>
#include <kexi_export.h>

namespace KexiPart
{
class Info;
}

//! The KexiFormEventHandler class handles events defined within Kexi Forms
/*! For now only "onClickAction" property of Push Button widget is handled:
 It's possible to connect this event to predefined global action.

 Note: This interface will be extended in the future!

 @see KexiFormPart::slotAssignAction()
 */
class KFORMEDITOR_EXPORT KexiFormEventHandler
{
public:
    KexiFormEventHandler();
    virtual ~KexiFormEventHandler();

    /*! Sets \a mainWidget to be a main widget for this handler.
     Also find widgets having action assigned and connects them
     to appropriate actions.
     For now, all of them must be KexiPushButton). */
    void setMainWidgetForEventHandling(QWidget* mainWidget);

private:
    class Private;

    Private* const d;
};

//! @internal form-level action for handling "on click" actions
class KFORMEDITOR_EXPORT KexiFormEventAction : public KAction
{
    Q_OBJECT
public:
    //! A structure used in currentActionName()
    class KFORMEDITOR_EXPORT ActionData
    {
    public:
        ActionData();

        /*! Decodes action string into action type/action argument parts.
         Action string has to be in a form of "actiontype:actionarg"
         - Action type is passed to \a actionType on success. Action type can be "kaction"
          or any of the part names (see KexiPart::Info::objectName()), e.g. "table", "query", etc.
         - Action argument can be an action name in case of "kaction" type or object name
          in case of action of type "table", "query", etc.
         \a ok is set to true on success and to false on failure. On failure no other
         values are passed.
         \return part info if action type is "table", "query", etc., or 0 for "kaction" type. */
        KexiPart::Info* decodeString(QString& actionType, QString& actionArg, bool& ok) const;

        //! \return true if the action is empty
        bool isEmpty() const;

        QString string; //!< action string with prefix, like "kaction:edit_copy" or "table:<tableName>"

        QString option; //!< option used when name is "table/query/etc.:\<objectName\>" is set;
        //!< can be set to "open", "design", "editText", etc.
        //!< @see ActionToExecuteListView::showActionsForPartClass()
    };

    KexiFormEventAction(QObject* parent, const QString& actionName,
                        const QString& objectName, const QString& actionOption);
    virtual ~KexiFormEventAction();

public slots:
    //! Activates the action. If the object supports executing (macro, script),
    //! it is executed; otherwise (table, query, form,...) it is opened in its data view.
    void slotTrigger();

private:
    class Private;

    Private* const d;
};

#endif
