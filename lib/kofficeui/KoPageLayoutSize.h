/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
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

#ifndef kopagelayoutsize_h
#define kopagelayoutsize_h

#include <qgroupbox.h>
#include <koGlobal.h>
#include <koUnit.h>
#include <kdialogbase.h>
#include <koPageLayout.h>

class QComboBox;
class KoUnitDoubleSpinBox;
class KoPageLayoutColumns;

class KoPageLayoutSize : public QWidget {
    Q_OBJECT

public:
    KoPageLayoutSize(QWidget *parent, const KoPageLayout& layout, KoUnit::Unit unit,
            const KoColumns& columns, bool unitChooser, bool enableBorders);

    bool queryClose();
    void setColumns(KoColumns &columns);

public slots:
    void setUnit( KoUnit::Unit unit );

signals:
    void propertyChange(KoPageLayout &layout);

protected:
    QComboBox *cpgUnit;
    QComboBox *cpgFormat;
    KoUnitDoubleSpinBox *epgWidth;
    KoUnitDoubleSpinBox *epgHeight;
    KoUnitDoubleSpinBox *ebrLeft;
    KoUnitDoubleSpinBox *ebrRight;
    KoUnitDoubleSpinBox *ebrTop;
    KoUnitDoubleSpinBox *ebrBottom;
    KoPagePreview *pgPreview;
    QButtonGroup *m_orientGroup;

protected slots:
    void formatChanged( int );
    void widthChanged( double );
    void heightChanged( double );
    void leftChanged( double );
    void rightChanged( double );
    void topChanged( double );
    void bottomChanged( double );
    void orientationChanged( int );

private:
    void updatePreview();
    void setValues();

    KoUnit::Unit m_unit;
    KoColumns m_columns;
    KoPageLayout m_layout;
};

#endif
