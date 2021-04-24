/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_NAMED_AREA_DIALOG
#define CALLIGRA_SHEETS_NAMED_AREA_DIALOG

#include <KoDialog.h>

class KComboBox;
class QLabel;
class KLineEdit;
class QPushButton;
class QListWidget;

namespace Calligra
{
namespace Sheets
{
class Region;
class Selection;

/**
 * \ingroup UI
 * Dialog to manage named areas.
 */
class NamedAreaDialog : public KoDialog
{
    Q_OBJECT

public:
    NamedAreaDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void slotClose();
    void slotNew();
    void slotEdit();
    void slotRemove();
    void displayAreaValues(const QString& name);

private:
    Selection*      m_selection;
    QListWidget*    m_list;
    QLabel*         m_rangeName;
    QPushButton*    m_newButton;
    QPushButton*    m_editButton;
    QPushButton*    m_removeButton;
};


/**
 * \ingroup UI
 * Dialog to edit a named area.
 */
class EditNamedAreaDialog : public KoDialog
{
    Q_OBJECT

public:
    EditNamedAreaDialog(QWidget* parent, Selection* selection);
    ~EditNamedAreaDialog() override;

    QString areaName() const;
    void setAreaName(const QString& areaname);
    void setRegion(const Region& region);

public Q_SLOTS:
    void slotOk();
    void slotAreaNameModified(const QString&);

private:
    Selection*  m_selection;
    KLineEdit*  m_areaNameEdit;
    KComboBox*  m_sheets;
    KLineEdit*  m_cellRange;
    QString     m_initialAreaName;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_NAMED_AREA_DIALOG
