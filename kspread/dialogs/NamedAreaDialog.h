/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2002 Laurent Montel <montel@kde.org>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_NAMED_AREA_DIALOG
#define KSPREAD_NAMED_AREA_DIALOG

#include <kdialog.h>

class KComboBox;
class QLabel;
class KLineEdit;

class KListWidget;

namespace KSpread
{
class Region;
class Selection;

/**
 * \ingroup UI
 * Dialog to manage named areas.
 */
class NamedAreaDialog : public KDialog
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
    KListWidget*    m_list;
    QLabel*         m_rangeName;
};


/**
 * \ingroup UI
 * Dialog to edit a named area.
 */
class EditNamedAreaDialog : public KDialog
{
    Q_OBJECT

public:
    EditNamedAreaDialog(QWidget* parent, Selection* selection);
    ~EditNamedAreaDialog();

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

} // namespace KSpread

#endif // KSPREAD_NAMED_AREA_DIALOG
