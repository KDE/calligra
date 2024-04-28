/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SimpleCitationBibliographyWidget.h"
#include "BibliographyPreview.h"
#include "BibliographyTemplate.h"
#include "ReferencesTool.h"
#include <KoBibliographyInfo.h>
#include <KoTextEditor.h>

#include <QAction>
#include <QDebug>

#include <QWidget>

SimpleCitationBibliographyWidget::SimpleCitationBibliographyWidget(ReferencesTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_blockSignals(false)
    , m_referenceTool(tool)
{
    widget.setupUi(this);
    Q_ASSERT(tool);

    m_templateGenerator = new BibliographyTemplate(KoTextDocument(m_referenceTool->editor()->document()).styleManager());

    widget.addCitation->setDefaultAction(tool->action("insert_citation"));
    connect(widget.addCitation, &QAbstractButton::clicked, this, &SimpleCitationBibliographyWidget::doneWithFocus);

    widget.addBibliography->setDefaultAction(tool->action("insert_bibliography"));
    connect(widget.addBibliography, &QAbstractButton::clicked, this, &SimpleCitationBibliographyWidget::doneWithFocus);
    connect(widget.addBibliography, &FormattingButton::aboutToShowMenu, this, &SimpleCitationBibliographyWidget::prepareTemplateMenu);
    connect(widget.addBibliography, &FormattingButton::itemTriggered, this, &SimpleCitationBibliographyWidget::applyTemplate);

    widget.configureBibliography->setDefaultAction(tool->action("configure_bibliography"));
    connect(widget.configureBibliography, &QAbstractButton::clicked, this, &SimpleCitationBibliographyWidget::doneWithFocus);
}

SimpleCitationBibliographyWidget::~SimpleCitationBibliographyWidget()
{
    delete m_templateGenerator;
}

void SimpleCitationBibliographyWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
}

void SimpleCitationBibliographyWidget::prepareTemplateMenu()
{
    m_previewGenerator.clear();
    qDeleteAll(m_templateList.begin(), m_templateList.end());
    m_templateList.clear();

    m_templateList = m_templateGenerator->templates();

    m_chooser = widget.addBibliography->addItemChooser(1);

    int index = 0;
    foreach (KoBibliographyInfo *info, m_templateList) {
        BibliographyPreview *preview = new BibliographyPreview();
        preview->setStyleManager(KoTextDocument(m_referenceTool->editor()->document()).styleManager());
        preview->setPreviewSize(QSize(200, 120));
        preview->updatePreview(info);
        connect(preview, &BibliographyPreview::pixmapGenerated, [this, index] {
            pixmapReady(index);
        });
        m_previewGenerator.append(preview);
        ++index;

        // put dummy pixmaps until the actual pixmap previews are generated and added in pixmapReady()
        if (!widget.addBibliography->hasItemId(index)) {
            QPixmap pmm(QSize(200, 120));
            pmm.fill(Qt::white);
            widget.addBibliography->addItem(m_chooser, pmm, index);
        }
    }
    if (widget.addBibliography->isFirstTimeMenuShown()) {
        widget.addBibliography->addSeparator();
        widget.addBibliography->addAction(m_referenceTool->action("insert_custom_bibliography"));
        connect(m_referenceTool->action("insert_custom_bibliography"),
                &QAction::triggered,
                this,
                &SimpleCitationBibliographyWidget::insertCustomBibliography,
                Qt::UniqueConnection);
    }
}

void SimpleCitationBibliographyWidget::insertCustomBibliography()
{
    m_templateGenerator->moveTemplateToUsed(m_templateList.at(0));
    m_referenceTool->insertCustomBibliography(m_templateList.at(0));
}

void SimpleCitationBibliographyWidget::pixmapReady(int templateId)
{
    // +1 to the templateId is because formattingButton does not allow id = 0
    widget.addBibliography->addItem(m_chooser, m_previewGenerator.at(templateId)->previewPixmap(), templateId + 1);
    disconnect(m_previewGenerator.at(templateId), &BibliographyPreview::pixmapGenerated, this, nullptr);
    m_previewGenerator.at(templateId)->deleteLater();
}

void SimpleCitationBibliographyWidget::applyTemplate(int templateId)
{
    m_templateGenerator->moveTemplateToUsed(m_templateList.at(templateId - 1));
    m_referenceTool->editor()->insertBibliography(m_templateList.at(templateId - 1));
}
