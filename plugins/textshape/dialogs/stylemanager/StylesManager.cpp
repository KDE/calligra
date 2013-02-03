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

#include "StylesManager.h"
#include "ui_StylesManager.h"

#include "StylesManagerStylesModel.h"

#include <dialogs/StylesModel.h>

#include <KoParagraphStyle.h>
#include <KoStyleThumbnailer.h>

StylesManager::StylesManager(QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::StylesManager)
    , m_styleManager(0)
{
    ui->setupUi(this);
}

StylesManager::~StylesManager()
{
    delete ui;
}

void StylesManager::setStyleManager(KoStyleManager *styleManager)
{
    Q_ASSERT(styleManager);
    if (!styleManager) {
        return; //don't crash but should never happen
    }
    m_styleManager = styleManager;

    StylesModel *stylesModel = new StylesModel(m_styleManager, AbstractStylesModel::ParagraphStyle);
    KoStyleThumbnailer *thumbnailer = new KoStyleThumbnailer();
    stylesModel->setStyleThumbnailer(thumbnailer);

    StylesManagerStylesModel *paragraphStylesModel = new StylesManagerStylesModel();
    paragraphStylesModel->setStylesModel(stylesModel);
    ui->paragraphTab->setStylesModel(paragraphStylesModel);
}

void StylesManager::setParagraphStyle(KoParagraphStyle *style)
{
    ui->paragraphTab->setDisplay(style);
}
