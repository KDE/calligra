/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "AlignmentDockWidget.h"
#include "ui_AlignmentDockWidget.h"

// Qt

// KDE
#include <KAction>

// Calligra

// Calligra Tables
#include "CellToolBase.h"

using namespace Calligra::Tables;

AlignmentDockWidget::AlignmentDockWidget(CellToolBase* cellTool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlignmentDockWidget)
{
    ui->setupUi(this);
    ui->alignLeftButton->setDefaultAction(cellTool->action("alignLeft"));
    ui->alignCenterButton->setDefaultAction(cellTool->action("alignCenter"));
    ui->alignRightButton->setDefaultAction(cellTool->action("alignRight"));
    ui->alignTopButton->setDefaultAction(cellTool->action("alignTop"));
    ui->alignMiddleButton->setDefaultAction(cellTool->action("alignMiddle"));
    ui->alignBottomButton->setDefaultAction(cellTool->action("alignBottom"));
    ui->wrapButton->setDefaultAction(cellTool->action("wrapText"));
    ui->angleButton->setDefaultAction(cellTool->action("changeAngle"));
    ui->verticalButton->setDefaultAction(cellTool->action("verticalText"));
    ui->decreaseIndentButton->setDefaultAction(cellTool->action("decreaseIndentation"));
    ui->increaseIndentButton->setDefaultAction(cellTool->action("increaseIndentation"));
}

AlignmentDockWidget::~AlignmentDockWidget()
{
    delete ui;
}
