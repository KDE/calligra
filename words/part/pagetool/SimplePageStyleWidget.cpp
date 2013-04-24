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
    qDebug() << "Rafraichissement des informations";
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
    QString style = ui->list->selectedItems().at(0)->text();
    if(!ui->list->selectedItems().isEmpty()) {
        for(int i = ui->spinFrom->value(); i <= ui->spinTo->value(); i++) {
               m_pageTool->applyStyle(i,style);
        }
    }
}

void SimplePageStyleWidget::on_list_clicked(const int &index) {
    refreshInformations();
}
