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

#include <KWStartupWidget.h>
#include <KWStartupWidgetBase.h>
#include <KWDocument.h>
#include <KoPageLayoutSize.h>
#include <KoPageLayoutColumns.h>

#include <kdebug.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>

KWStartupWidget::KWStartupWidget(QWidget *parent, KWDocument *doc, const KoColumns &columns)
    : KWStartupWidgetBase(parent) {
    m_columns = columns;
    m_layout = KoPageLayout::standardLayout();
    m_doc = doc;

    setFocusProxy(m_createButton);

    QVBoxLayout *lay = new QVBoxLayout(m_sizeTab, KDialog::marginHint());
    m_sizeWidget = new KoPageLayoutSize(m_sizeTab, m_layout, m_doc->unit(), m_columns , true, true);
    lay->addWidget(m_sizeWidget);

    lay = new QVBoxLayout(m_columnsTab, KDialog::marginHint());
    m_columnsWidget = new KoPageLayoutColumns(m_columnsTab, m_columns, m_doc->unit(), m_layout);
    lay->addWidget(m_columnsWidget);

    connect (m_columnsWidget, SIGNAL( propertyChange(KoColumns&)),
            this, SLOT (columnsUpdated( KoColumns&)));

    connect (m_sizeWidget, SIGNAL( propertyChange(KoPageLayout&)),
            this, SLOT (sizeUpdated( KoPageLayout&)));

    connect (m_createButton, SIGNAL( clicked() ), this, SLOT (buttonClicked()) );

    connect (m_WpStyleCheckbox, SIGNAL(toggled(bool)), m_sizeWidget, SLOT(setEnableBorders(bool)));
    connect (m_WpStyleCheckbox, SIGNAL(toggled(bool)), m_columnsWidget, SLOT(setEnableColumns(bool)));
}

void KWStartupWidget::sizeUpdated(KoPageLayout &layout) {
    m_layout = layout;
    m_columnsWidget->setLayout(layout);
}

void KWStartupWidget::columnsUpdated(KoColumns &columns) {
    m_columns.columns = columns.columns;
    m_columns.ptColumnSpacing = columns.ptColumnSpacing;
    m_sizeWidget->setColumns(columns);
}

void KWStartupWidget::buttonClicked() {
    if(m_WpStyleCheckbox->isChecked())
        m_doc->initEmpty();
    else {
        m_doc->m_processingType = KWDocument::DTP;
        m_doc->clear();
    }
    KoKWHeaderFooter hf;
    hf.header = HF_SAME;
    hf.footer = HF_SAME;
    hf.ptHeaderBodySpacing = 10.0;
    hf.ptFooterBodySpacing = 10.0;
    hf.ptFootNoteBodySpacing = 10.0;
    m_doc->setPageLayout( m_layout, m_columns, hf, false );
    m_doc->delayedRecalcFrames(1);
    m_doc->setUnit(m_sizeWidget->unit());

    emit documentSelected();
}

#include "KWStartupWidget.moc"
