/* This file is part of the KDE project
 * Copyright (C) 2010 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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
#include "SimpleTableOfContentsWidget.h"
#include "ReferencesTool.h"
#include "TableOfContentsConfigure.h"
#include "TableOfContentsTemplate.h"
#include "TableOfContentsPreview.h"

#include <KoTextEditor.h>
#include <KoTableOfContentsGeneratorInfo.h>
#include <KoIcon.h>

#include <QAction>
#include <QDebug>

#include <QWidget>
#include <QMenu>

SimpleTableOfContentsWidget::SimpleTableOfContentsWidget(ReferencesTool *tool, QWidget *parent)
        : QWidget(parent),
        m_blockSignals(false),
        m_referenceTool(tool)
{
    widget.setupUi(this);
    Q_ASSERT(tool);

    m_templateGenerator = new TableOfContentsTemplate(KoTextDocument(m_referenceTool->editor()->document()).styleManager());

    widget.addToC->setIcon(koIcon("insert-table-of-contents"));
    connect(widget.addToC, SIGNAL(clicked(bool)), this, SIGNAL(doneWithFocus()));
    connect(widget.addToC, SIGNAL(aboutToShowMenu()), this, SLOT(prepareTemplateMenu()));
    connect(widget.addToC, SIGNAL(itemTriggered(int)), this, SLOT(applyTemplate(int)));
}

SimpleTableOfContentsWidget::~SimpleTableOfContentsWidget()
{
    delete m_templateGenerator;
}

void SimpleTableOfContentsWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
}

void SimpleTableOfContentsWidget::prepareTemplateMenu()
{
    m_previewGenerator.clear();
    qDeleteAll(m_templateList.begin(), m_templateList.end());
    m_templateList.clear();

    m_templateList = m_templateGenerator->templates();

    m_chooser = widget.addToC->addItemChooser(1);

    int index = 0;
    foreach (KoTableOfContentsGeneratorInfo *info, m_templateList) {
        TableOfContentsPreview *preview = new TableOfContentsPreview();
        preview->setStyleManager(KoTextDocument(m_referenceTool->editor()->document()).styleManager());
        preview->setPreviewSize(QSize(200,120));
        preview->updatePreview(info);
        connect(preview, &TableOfContentsPreview::pixmapGenerated, this, [this, index] { pixmapReady(index); });
        m_previewGenerator.append(preview);
        ++index;

        //put dummy pixmaps until the actual pixmap previews are generated and added in pixmapReady()
        if (! widget.addToC->hasItemId(index)) {
            QPixmap pmm(QSize(200,120));
            pmm.fill(Qt::white);
            widget.addToC->addItem(m_chooser, pmm, index);
        }
    }
    if (widget.addToC->isFirstTimeMenuShown()) {
        widget.addToC->addSeparator();
        widget.addToC->addAction(m_referenceTool->action("insert_configure_tableofcontents"));
        connect(m_referenceTool->action("insert_configure_tableofcontents"), SIGNAL(triggered()), this, SLOT(insertCustomToC()), Qt::UniqueConnection);
        widget.addToC->addAction(m_referenceTool->action("format_tableofcontents"));
    }
}

void SimpleTableOfContentsWidget::pixmapReady(int templateId)
{
    // +1 to the templateId is because formattingButton does not allow id = 0
    widget.addToC->addItem(m_chooser, m_previewGenerator.at(templateId)->previewPixmap(), templateId + 1);
    disconnect(m_previewGenerator.at(templateId), &TableOfContentsPreview::pixmapGenerated, this, nullptr);
    m_previewGenerator.at(templateId)->deleteLater();
}

void SimpleTableOfContentsWidget::applyTemplate(int templateId)
{
    m_templateGenerator->moveTemplateToUsed(m_templateList.at(templateId - 1));
    m_referenceTool->editor()->insertTableOfContents(m_templateList.at(templateId - 1));
}

void SimpleTableOfContentsWidget::insertCustomToC()
{
    m_templateGenerator->moveTemplateToUsed(m_templateList.at(0));
    m_referenceTool->insertCustomToC(m_templateList.at(0));
}
