/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextShapeOnPathWidget.h"
#include "ArtisticTextTool.h"
#include "ui_ArtisticTextShapeOnPathWidget.h"
#include <QAction>

ArtisticTextShapeOnPathWidget::ArtisticTextShapeOnPathWidget(ArtisticTextTool *tool, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ArtisticTextShapeOnPathWidget)
    , m_textTool(tool)
{
    ui->setupUi(this);
    ui->detachFromPath->setDefaultAction(tool->action("artistictext_detach_from_path"));
    ui->convertToPath->setDefaultAction(tool->action("artistictext_convert_to_path"));

    connect(ui->startOffset, &QAbstractSlider::valueChanged, this, &ArtisticTextShapeOnPathWidget::offsetChanged);
}

ArtisticTextShapeOnPathWidget::~ArtisticTextShapeOnPathWidget()
{
    delete ui;
}

void ArtisticTextShapeOnPathWidget::updateWidget()
{
    ArtisticTextToolSelection *selection = dynamic_cast<ArtisticTextToolSelection *>(m_textTool->selection());
    if (!selection)
        return;

    ArtisticTextShape *currentText = selection->selectedShape();
    if (!currentText)
        return;

    ui->startOffset->blockSignals(true);
    ui->startOffset->setValue(static_cast<int>(currentText->startOffset() * 100.0));
    ui->startOffset->setEnabled(currentText->isOnPath());
    ui->startOffset->blockSignals(false);
}
