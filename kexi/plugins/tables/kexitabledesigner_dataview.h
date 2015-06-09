/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITABLEDESIGNERDATAVIEW_H
#define KEXITABLEDESIGNERDATAVIEW_H

#include <widget/tableview/KexiDataTableView.h>
#include "kexitablepart.h"

class KexiTableDesigner_DataView : public KexiDataTableView
{
    Q_OBJECT

public:
    explicit KexiTableDesigner_DataView(QWidget *parent);

    virtual ~KexiTableDesigner_DataView();

    KexiTablePart::TempData* tempData() const;

protected:
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool *dontStore);
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

};

#endif
