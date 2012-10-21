/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include "GeneralStylesTab.h"
#include "ui_GeneralStylesTab.h"

GeneralStylesTab::GeneralStylesTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralStylesTab),
    m_style(0),
//    m_styleManager(0),
    m_StyleModel(0),
    m_validParentStylesModel(new ValidParentStylesProxyModel())
{
    ui->setupUi(this);

    ui->nextStyle->setStyleIsOriginal(true);
    ui->nextStyle->showEditIcon(false);

    ui->inheritStyle->setStyleIsOriginal(true);
    ui->inheritStyle->showEditIcon(false);

//widget.nextStyle->setVisible(false);
//m_characterStyleModel->setStyleThumbnailer(m_thumbnail);
//widget.nextStyle->setStylesModel(m_characterStyleModel);
//widget.label_2->setVisible(false);
//

// inherited style model
//widget.inheritStyle->showEditIcon(false);
//widget.inheritStyle->setStyleIsOriginal(true);
//for character General
}

GeneralStylesTab::~GeneralStylesTab()
{
    delete ui;
}

void GeneralStylesTab::setStylesModel(StylesModel *model)
{
    Q_ASSERT(model);
    m_StyleModel = model;
    m_validParentStylesModel->setStylesModel(m_StyleModel);
    ui->nextStyle->setStylesModel(m_StyleModel);
    ui->inheritStyle->setStylesModel(m_validParentStylesModel);
}
