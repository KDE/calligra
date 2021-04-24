/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
