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

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_FONT
#define CALLIGRA_SHEETS_LAYOUT_PAGE_FONT

#include "ui_FontWidget.h"
#include <QWidget>

namespace Calligra
{
namespace Sheets
{

class Style;

/**
 * \ingroup UI
 * Dialog page to choose the used font.
 */
class LayoutPageFont : public QWidget, public Ui::FontWidget
{
    Q_OBJECT
public:
    LayoutPageFont(QWidget *parent);

    void apply(Style *style, bool partial);
    void loadFrom(const Style &style, bool partial);

private Q_SLOTS:
    void display_example();
    void slotSetTextColor(const QColor &_color);

private:
    QString m_family;
    int m_size;
    QColor m_color;
    bool m_colorChanged;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_FONT
