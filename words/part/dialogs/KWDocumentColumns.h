/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWDOCUMENTCOLUMNS_H
#define KWDOCUMENTCOLUMNS_H

#include <KoColumns.h>
#include <KoPageLayout.h>
#include <ui_KWDocumentColumns.h>

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

    KoColumns columns() const
    {
        return m_columns;
    }

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
