/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextShapeConfigWidget.h"
#include "ArtisticTextShape.h"
#include "ArtisticTextTool.h"
#include "ArtisticTextToolSelection.h"

#include <QAction>

ArtisticTextShapeConfigWidget::ArtisticTextShapeConfigWidget(ArtisticTextTool *textTool)
    : m_textTool(textTool)
{
    Q_ASSERT(m_textTool);

    widget.setupUi(this);

    widget.bold->setDefaultAction(textTool->action("artistictext_font_bold"));
    widget.italic->setDefaultAction(textTool->action("artistictext_font_italic"));
    widget.superScript->setDefaultAction(textTool->action("artistictext_superscript"));
    widget.subScript->setDefaultAction(textTool->action("artistictext_subscript"));
    widget.anchorStart->setDefaultAction(textTool->action("artistictext_anchor_start"));
    widget.anchorMiddle->setDefaultAction(textTool->action("artistictext_anchor_middle"));
    widget.anchorEnd->setDefaultAction(textTool->action("artistictext_anchor_end"));
    widget.fontSize->setRange(2, 1000);

    connect(widget.fontFamily, &QFontComboBox::currentFontChanged, this, &ArtisticTextShapeConfigWidget::fontFamilyChanged);
    connect(widget.fontSize, &QSpinBox::valueChanged, this, &ArtisticTextShapeConfigWidget::fontSizeChanged);
}

void ArtisticTextShapeConfigWidget::blockChildSignals(bool block)
{
    widget.fontFamily->blockSignals(block);
    widget.fontSize->blockSignals(block);
}

void ArtisticTextShapeConfigWidget::updateWidget()
{
    ArtisticTextToolSelection *selection = dynamic_cast<ArtisticTextToolSelection *>(m_textTool->selection());
    if (!selection)
        return;

    ArtisticTextShape *currentText = selection->selectedShape();
    if (!currentText)
        return;

    blockChildSignals(true);

    QFont font = currentText->fontAt(m_textTool->textCursor());

    widget.fontSize->setValue(font.pointSize());
    font.setPointSize(8);
    widget.fontFamily->setCurrentFont(font);

    blockChildSignals(false);
}
