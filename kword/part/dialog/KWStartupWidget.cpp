/* This file is part of the KOffice project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
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

#include "KWStartupWidget.h"
#include <KWDocument.h>
#include "KWPageLayout.h"
#include "KWDocumentColumns.h"
//
//   #include <kdebug.h>
//   #include <QPushButton>
//   #include <QCheckBox>
//   #include <QLayout>
//   //Added by qt3to4:
//   #include <Q3VBoxLayout>

KWStartupWidget::KWStartupWidget(QWidget *parent, KWDocument *doc, const KoColumns &columns)
    : QWidget(parent)
{
    widget.setupUi(this);
    m_columns = columns;
    m_layout = KoPageLayout::standardLayout();
    m_doc = doc;

    setFocusProxy(widget.createButton);

    QVBoxLayout *lay = new QVBoxLayout(widget.sizeTab);
    m_sizeWidget = new KWPageLayout(widget.sizeTab, m_layout, m_columns);
    lay->addWidget(m_sizeWidget);
    lay->setMargin(0);

    lay = new QVBoxLayout(widget.columnsTab);
    m_columnsWidget = new KWDocumentColumns(); // widget.columnsTab, m_columns, KoUnit::Millimeter, m_layout);
    lay->addWidget(m_columnsWidget);
    lay->setMargin(0);

    connect (m_columnsWidget, SIGNAL( propertyChange(KoColumns&)),
            this, SLOT (columnsUpdated( KoColumns&)));

    connect (m_sizeWidget, SIGNAL( layoutChanged(const KoPageLayout&)),
            this, SLOT (sizeUpdated( const KoPageLayout&)));

    connect (widget.createButton, SIGNAL( clicked() ), this, SLOT (buttonClicked()) );

    connect (widget.mainText, SIGNAL(toggled(bool)), m_sizeWidget, SLOT(setTextAreaAvailable(bool)));
    connect (widget.mainText, SIGNAL(toggled(bool)), m_columnsWidget, SLOT(setEnableColumns(bool)));
}

void KWStartupWidget::sizeUpdated(const KoPageLayout &layout) {
    m_layout = layout;
    //m_columnsWidget->setLayout(layout);
}

void KWStartupWidget::columnsUpdated(KoColumns &columns) {
    m_columns.columns = columns.columns;
    m_columns.columnSpacing = columns.columnSpacing;
    //m_sizeWidget->setColumns(columns);
}

void KWStartupWidget::buttonClicked() {
/*
    if(widget.mainText->isChecked())
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

*/
    emit documentSelected();
}

#include <KWStartupWidget.moc>
