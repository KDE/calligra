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

#ifndef KWPAGELAYOUT_H
#define KWPAGELAYOUT_H

// KOffice includes.
#include <KoGlobal.h>   // ??
#include <KoUnit.h>
#include <KoPageLayout.h>

// KWord includes.
#include <kword_export.h>
#include "ui_KWPageLayout.h"

class KWPagePreview;

/**
 * This class is a widget that shows the layout settings of a KoPageLayout and allows the user to change them.
 */
class KWORD_TEST_EXPORT KWPageLayout : public QWidget, Ui::KWPageLayout
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parent the parent widget.
     * @param layout the KoPageLayout that this widget should be initialized with.
     * @param unit the unit-type (mm/cm/inch) that the dialog should show.
     * @param columns the KoColumns (amount of columns) that the preview should be initialized with.
     * @param enableUnitChooser if true a combobox with the unit-type is shown for the user to change.
     * @param enableBorders if true enable the user to change the margins (aka borders) of the page.
     */
    KWPageLayout(QWidget *parent, const KoPageLayout& layout, KoUnit::Unit unit,
            const KoColumns& columns, bool enableUnitChooser, bool enableBorders);

    /**
     * @return if the dialog is in a sane state and the values can be used.
     */
    bool queryClose();
    /**
     * Update the page preview widget with the param columns.
     * @param columns the new columns
     */
    void setColumns(KoColumns &columns);

public slots:
    /**
     * Set a new unit for the widget updating the widgets.
     * @param unit the new unit
     */
    void setUnit(KoUnit::Unit unit);
    /**
     * Enable the user to edit the page border size
     * @param on if true enable the user to change the margins (aka borders) of the page
     */
    void setEnableBorders(bool on);

signals:
    /**
     * Emitted whenever the user changed something in the dialog.
     * @param layout the update layout structure with currently displayed info.
     * Note that the info may not be fully correct and physically possible (in which
     * case queryClose will return false)
     */
    void propertyChange(KoPageLayout &layout);

protected:

protected slots:
    void formatChanged(int);
    void widthChanged(double);
    void heightChanged(double);
    void leftChanged(double);
    void rightChanged(double);
    void topChanged(double);
    void bottomChanged(double);
    void orientationChanged();
    void setUnitInt(int unit);

private:
    void updatePreview();
    void setGuiValues();

    KoPageLayout m_layout;
    KoUnit::Unit m_unit;
    KWPagePreview* m_pagePreview;

    bool m_blockSignals;
    bool m_haveBorders;
};

#endif      // KWPAGELAYOUT_H
