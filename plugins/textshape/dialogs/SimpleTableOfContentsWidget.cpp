/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleTableOfContentsWidget.h"
#include "ReferencesTool.h"
#include "TableOfContentsConfigure.h"
#include "TableOfContentsPreview.h"
#include "TableOfContentsTemplate.h"

#include <KoIcon.h>
#include <KoTableOfContentsGeneratorInfo.h>
#include <KoTextEditor.h>

#include <QAction>
#include <QDebug>

#include <QMenu>
#include <QWidget>

SimpleTableOfContentsWidget::SimpleTableOfContentsWidget(ReferencesTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_blockSignals(false)
    , m_referenceTool(tool)
    , m_templateGenerator(std::make_unique<TableOfContentsTemplate>(KoTextDocument(m_referenceTool->editor()->document()).styleManager()))
{
    widget.setupUi(this);
    Q_ASSERT(tool);

    widget.addToC->setIcon(koIcon("insert-table-of-contents"));
    connect(widget.addToC, &QAbstractButton::clicked, this, &SimpleTableOfContentsWidget::doneWithFocus);
    connect(widget.addToC, &FormattingButton::aboutToShowMenu, this, &SimpleTableOfContentsWidget::prepareTemplateMenu);
    connect(widget.addToC, &FormattingButton::itemTriggered, this, &SimpleTableOfContentsWidget::applyTemplate);
}

SimpleTableOfContentsWidget::~SimpleTableOfContentsWidget() = default;

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
    for (KoTableOfContentsGeneratorInfo *info : std::as_const(m_templateList)) {
        TableOfContentsPreview *preview = new TableOfContentsPreview();
        preview->setStyleManager(KoTextDocument(m_referenceTool->editor()->document()).styleManager());
        preview->setPreviewSize(QSize(200, 120));
        preview->updatePreview(info);
        connect(preview, &TableOfContentsPreview::pixmapGenerated, this, [this, index](const QPixmap &pixmap) {
            pixmapReady(index, pixmap);
        });
        m_previewGenerator.append(preview);
        ++index;

        // put dummy pixmaps until the actual pixmap previews are generated and added in pixmapReady()
        if (!widget.addToC->hasItemId(index)) {
            QPixmap pmm(QSize(200, 120));
            pmm.fill(Qt::white);
            widget.addToC->addItem(m_chooser, pmm, index);
        }
    }
    if (widget.addToC->isFirstTimeMenuShown()) {
        widget.addToC->addSeparator();
        widget.addToC->addAction(m_referenceTool->action("insert_configure_tableofcontents"));
        connect(m_referenceTool->action("insert_configure_tableofcontents"),
                &QAction::triggered,
                this,
                &SimpleTableOfContentsWidget::insertCustomToC,
                Qt::UniqueConnection);
        widget.addToC->addAction(m_referenceTool->action("format_tableofcontents"));
    }
}

void SimpleTableOfContentsWidget::pixmapReady(int templateId, const QPixmap &pixmap)
{
    // +1 to the templateId is because formattingButton does not allow id = 0
    widget.addToC->addItem(m_chooser, pixmap, templateId + 1);
    auto previewGenerator = m_previewGenerator.at(templateId);
    if (previewGenerator) {
        disconnect(previewGenerator, &TableOfContentsPreview::pixmapGenerated, this, nullptr);
        m_previewGenerator.at(templateId)->deleteLater();
    }
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
