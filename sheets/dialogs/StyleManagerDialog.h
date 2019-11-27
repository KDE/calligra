/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Laurent Montel <montel@kde.org>
   Copyright 2003 Norbert Andres <nandres@web.de>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_STYLE_MANAGER_DIALOG
#define CALLIGRA_SHEETS_STYLE_MANAGER_DIALOG

#include <KoDialog.h>

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class KComboBox;

namespace Calligra
{
namespace Sheets
{
class Selection;
class StyleManager;

/**
 * \ingroup UI
 * Dialog to manage names cell styles.
 */
class StyleManagerDialog : public KoDialog
{
    Q_OBJECT

public:
    StyleManagerDialog(QWidget* parent, Selection* selection, StyleManager* manager);
    ~StyleManagerDialog() override;

protected Q_SLOTS:
    void slotOk();
    void slotNew();
    void slotEdit();
    void slotRemove();
    void slotDisplayMode(int mode);
    void selectionChanged(QTreeWidgetItem*);

private:
    void fillComboBox();

private:
    Selection*      m_selection;
    StyleManager*   m_styleManager;
    QTreeWidget*    m_styleList;
    KComboBox*      m_displayBox;
    QPushButton*    m_newButton;
    QPushButton*    m_modifyButton;
    QPushButton*    m_deleteButton;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STYLE_MANAGER_DIALOG
