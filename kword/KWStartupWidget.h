/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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
#ifndef kw_startup_widget_h
#define kw_startup_widget_h

#include <KWStartupWidgetBase.h>
#include <koPageLayout.h>

class KoPageLayoutSize;
class KoPageLayoutColumns;

class KWStartupWidget : public KWStartupWidgetBase {
    Q_OBJECT;
public:
    KWStartupWidget(QWidget *parent, const KoColumns &columns);

private slots:
    void sizeUpdated(KoPageLayout &layout);
    void columnsUpdated(KoColumns &columns);

private:
    KoPageLayoutSize *m_sizeWidget;
    KoPageLayoutColumns *m_columnsWidget;

    KoPageLayout m_layout;
    KoColumns m_columns;
};

#endif
