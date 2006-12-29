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
#include <KoPageLayout.h>

class KoPageLayoutSize;
class KoPageLayoutColumns;
class KWDocument;

/**
 * The 'Custom Document' widget in the KWord startup widget.
 * This class embeds the page size/columns tabs to allow the user to select the document
 * markup for his new empty document.
 */
class KWStartupWidget : public KWStartupWidgetBase {
    Q_OBJECT
public:
    /**
     * Constructor. Please note that this class is being used/created by KWDocument.
     * @param parent the parent widget
     * @param doc the document that wants to be altered
     * @param columns the suggested columns to be shown initially
     */
    KWStartupWidget(QWidget *parent, KWDocument *doc, const KoColumns &columns);

private slots:
    void sizeUpdated(KoPageLayout &layout);
    void columnsUpdated(KoColumns &columns);
    void buttonClicked();

signals:
    /// this signal is emitted (as defined by KoDocument) the moment the document is 'ready'
    void documentSelected();

private:
    KoPageLayoutSize *m_sizeWidget;
    KoPageLayoutColumns *m_columnsWidget;

    KoPageLayout m_layout;
    KoColumns m_columns;
    KWDocument *m_doc;
};

#endif
