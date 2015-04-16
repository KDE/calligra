/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include <ui_KWDocumentColumns.h>
#include <KoPageLayout.h>
#include <KoColumns.h>

#include <QWidget>

class KoPagePreviewWidget;

/**
 * This widget visually shows and alters the KoColumns data structure.
 */
class KWDocumentColumns : public QWidget
{
    Q_OBJECT
public:
    KWDocumentColumns(QWidget *parent, const KoColumns &columns);

    KoColumns columns() const { return m_columns; }

    void setShowPreview(bool on);
    void setUnit(const KoUnit &unit);

Q_SIGNALS:
    void columnsChanged(const KoColumns &columns);

public Q_SLOTS:
    void setTextAreaAvailable(bool available);
    void setColumns(const KoColumns &columns);

private Q_SLOTS:
    void optionsChanged();

private:
    Ui::KWDocumentColumns widget;
    KoColumns m_columns;
    KoPagePreviewWidget *m_preview;
};

#endif
