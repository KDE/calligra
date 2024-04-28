/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_POSITION
#define CALLIGRA_SHEETS_LAYOUT_PAGE_POSITION

#include "ui_PositionWidget.h"
#include <KoUnit.h>
#include <QWidget>

class KoUnitDoubleSpinBox;

namespace Calligra
{
namespace Sheets
{

class Style;

/**
 * \ingroup UI
 * Dialog page to set up the cell dimension, value alignment, etc.
 */
class LayoutPagePosition : public QWidget, public Ui::PositionWidget
{
    Q_OBJECT
public:
    LayoutPagePosition(QWidget *parent, KoUnit unit);

    void apply(Style *style, bool partial);
    void loadFrom(const Style &style, bool partial);

public Q_SLOTS:
    void updateElements();
    void slotStateChanged(int);

protected:
    KoUnitDoubleSpinBox *m_indent;

    Style::HAlign m_halign;
    Style::VAlign m_valign;
    int m_angle;
    double m_indentation;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_POSITION
