/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Adam Pigg <adam@piggz.co.uk>

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

#ifndef KEXIACTIONSELECTIONDIALOG_H
#define KEXIACTIONSELECTIONDIALOG_H

#include <kdialog.h>
#include "kexiformeventhandler.h"

class QTreeWidgetItem;

namespace KexiPart
{
class Item;
}

//! @short A dialog for selecting an action to be executed for a form's button
/*! Available actions are:
 - application's global actions like "edit->copy" (KAction-based)
 - opening/printing/executing of selected object (table/query/form/script/macrto, etc.)
*/
class KFORMEDITOR_EXPORT KexiActionSelectionDialog : public KDialog
{
    Q_OBJECT
public:
    KexiActionSelectionDialog(QWidget *parent,
                              const KexiFormEventAction::ActionData& action, const QString& actionWidgetName);
    ~KexiActionSelectionDialog();

    /*! \return selected action data or empty action if dialog has been rejected
     or "No action" has been selected. */
    KexiFormEventAction::ActionData currentAction() const;

protected slots:
    void slotActionCategorySelected(QTreeWidgetItem* item);
    void slotKActionItemExecuted(QTreeWidgetItem*);
    void slotKActionItemSelected(QTreeWidgetItem*);
    void slotActionToExecuteItemExecuted(QTreeWidgetItem* item);
    void slotActionToExecuteItemSelected(QTreeWidgetItem*);
    void slotCurrentFormActionItemExecuted(QTreeWidgetItem*);
    void slotCurrentFormActionItemSelected(QTreeWidgetItem*);
    void slotItemForOpeningOrExecutingSelected(KexiPart::Item* item);

protected:
    void updateOKButtonStatus();

    class Private;
    Private* const d;
};

#endif
