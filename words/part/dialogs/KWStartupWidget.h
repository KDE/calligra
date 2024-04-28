/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KW_STARTUP_WIDGET_H
#define KW_STARTUP_WIDGET_H

#include <ui_KWStartupWidget.h>

#include <KoColumns.h>
#include <KoPageLayout.h>
#include <KoUnit.h>

#include <QWidget>

class KWDocument;
class KoPageLayoutWidget;
class KWDocumentColumns;

/**
 * The 'Custom Document' widget in the Words startup widget.
 * This class embeds the page size/columns tabs to allow the user to select the document
 * markup for his new empty document.
 */
class KWStartupWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor. Please note that this class is being used/created by KWDocument.
     * @param parent the parent widget
     * @param doc the document that wants to be altered
     * @param columns the suggested columns to be shown initially
     */
    KWStartupWidget(QWidget *parent, KWDocument *doc, const KoColumns &columns);

private Q_SLOTS:
    void sizeUpdated(const KoPageLayout &layout);
    void columnsUpdated(const KoColumns &columns);
    void buttonClicked();
    void unitChanged(const KoUnit &unit);

Q_SIGNALS:
    /// this signal is emitted (as defined by KoDocument) the moment the document is 'ready'
    void documentSelected();

private:
    KoPageLayoutWidget *m_sizeWidget;
    KWDocumentColumns *m_columnsWidget;

    KoPageLayout m_layout;
    KoColumns m_columns;
    KWDocument *m_doc;
    Ui::KWStartupWidget widget;
    KoUnit m_unit;
};

#endif
