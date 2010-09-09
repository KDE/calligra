/* This file is part of the KDE project

   Copyright 1999-2006 The KSpread Team <koffice-devel@kde.org>

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


#ifndef KSPREAD_LOCATION_COMBOBOX
#define KSPREAD_LOCATION_COMBOBOX

#include <kcombobox.h>

#include "kspread_export.h"

class QKeyEvent;

namespace KSpread
{
class CellToolBase;

/**
 * \class LocationComboBox
 * \ingroup UI
 * The combobox, that shows the address string of the current cell selection.
 *
 * Depending on the sheet settings the address is displayed in normal form
 * (e.g. A1 or B1:C3) or in LC (Line/Column) form (e.g. L1xC1 or 3Lx2C).
 */
class LocationComboBox : public KComboBox
{
    Q_OBJECT
public:
    explicit LocationComboBox(CellToolBase *cellTool, QWidget *parent = 0);

    void addCompletionItem(const QString &_item);
    void removeCompletionItem(const QString &_item);

    /**
     * Updates the address string according to the current cell selection
     * and the current address mode (normal or LC mode).
     */
    void updateAddress();

public slots:
    void slotAddAreaName(const QString &);
    void slotRemoveAreaName(const QString &);

protected: // reimplementations
    virtual void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    void slotActivateItem();

private:
    bool activateItem();

private:
    CellToolBase *m_cellTool;
    KCompletion completionList;
};

} // namespace KSpread

#endif // KSPREAD_LOCATION_COMBOBOX
