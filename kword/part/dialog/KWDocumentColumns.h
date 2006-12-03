/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Gary Cramblitt <garycramblitt@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWDOCUMENTCOLUMNS_H
#define KWDOCUMENTCOLUMNS_H

// KOffice includes.
#include <KoGlobal.h>   // ??
#include <KoUnit.h>
#include <KoPageLayout.h>

// KWord includes.
#include <kword_export.h>
#include "ui_KWDocumentColumns.h"

class KWPagePreview;

/**
 * This class is a widget that shows the column settings of a KoColumns and allows the user to change them.
 */
class KWORD_TEST_EXPORT KWDocumentColumns : public QWidget, Ui::KWDocumentColumns
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parent the parent widget.
     * @param columns the KoColumns this this widget should be initialized with.
     * @param unit the unit-type (mm/cm/inch) that the dialog should show.
     * @param enableUnitChooser if true a combobox with the unit-type is shown for the user to change.
     * @param layout the KoPageLayout used to display the page preview.
     */
    KWDocumentColumns(QWidget *parent, const KoColumns& columns, KoUnit unit,
        bool enableUnitChooser, const KoPageLayout& layout);

    /**
     * @return if the dialog is in a sane state and the values can be used.
     */
    bool queryClose();

    /**
     * Update the page preview widget with the layout.
     * @param layout the new layout
     */
    void setLayout(const KoPageLayout &layout);

public slots:
    /**
     * Set a new unit for the widget updating the widgets.
     * @param unit the new unit
     */
    void setUnit(KoUnit unit);

signals:
    /**
     * Emitted whenever the user changed something in the dialog.
     * @param columns the update columns structure with currently displayed info.
     * Note that the info may not be fully correct and physically possible (in which
     * case queryClose will return false)
     */
    void propertyChange(KoColumns &columns);

protected:

protected slots:
    void setUnitInt(int unit);
    void columnsChanged(int columns);
    void spacingChanged(double spacing);

private:
    void updatePreview();
    void setGuiValues();

    KoColumns m_columns;
    KoUnit m_unit;
    KoPageLayout m_layout;
    KWPagePreview* m_pagePreview;
};

#endif      // KWDOCUMENTCOLUMNS_H
