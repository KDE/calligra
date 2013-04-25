/*
 *  Copyright (C) 2012 C. Boemann <cbo@boemann.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#include "SimplePageStyleWidget.h"
#include "ui_SimplePageStyleWidget.h"
#include "KWView.h"
#include "KWPageTool.h"
#include "KWDocument.h"
#include <QTimer>

SimplePageStyleWidget::SimplePageStyleWidget(KWView* view, KWPageTool* pageTool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimplePageStyleWidget)
{
    ui->setupUi(this);
    ui->list->updatesEnabled();
    m_view = view;
    m_pageTool = pageTool;
    connect(m_view->kwdocument(), SIGNAL(pageSetupChanged()), this, SLOT(refreshInformations()));
    refreshInformations();
}

void SimplePageStyleWidget::refreshInformations() {
    ui->spinFrom->setMinimum(1);
    ui->spinTo->setMinimum(1);
    ui->spinFrom->setMaximum(m_view->kwdocument()->pageCount());
    ui->spinTo->setMaximum(m_view->kwdocument()->pageCount());
    ui->spinFrom->update();
    ui->spinTo->update();

    QHash<QString, KWPageStyle> styles = m_view->kwdocument()->pageManager()->pageStyles();
    ui->list->clear();
    for(int i = 0; i < styles.keys().size(); i++) {
        ui->list->addItem(styles.keys().at(i));
    }
}

SimplePageStyleWidget::~SimplePageStyleWidget()
{
    delete ui;
}

void SimplePageStyleWidget::on_spinTo_valueChanged(int arg1)
{
    ui->spinFrom->setValue(std::min(arg1,ui->spinFrom->value()));
}

void SimplePageStyleWidget::on_spinFrom_valueChanged(int arg1)
{
    ui->spinTo->setValue(std::max(arg1,ui->spinTo->value()));
}

void SimplePageStyleWidget::on_buttonApply_clicked()
{
    if(!ui->list->selectedItems().isEmpty()) {
        QString style = ui->list->selectedItems().at(0)->text();
        for(int i = ui->spinFrom->value(); i <= ui->spinTo->value(); i++) {
            m_pageTool->applyStyle(i,style);
        }
    }
}

void SimplePageStyleWidget::on_list_clicked(const int &index) {
    refreshInformations();
}
