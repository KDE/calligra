/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableOfContentsStyleConfigure.h"
#include "ui_TableOfContentsStyleConfigure.h"

#include "KoParagraphStyle.h"
#include "KoStyleManager.h"

#include <QHeaderView>
#include <QTableView>

TableOfContentsStyleConfigure::TableOfContentsStyleConfigure(KoStyleManager *manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TableOfContentsStyleConfigure)
    , m_stylesTree(nullptr)
    , m_styleManager(manager)
    , m_tocInfo(nullptr)
    , m_stylesModel(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(i18n("Table of Contents - Configure Styles"));

    Q_ASSERT(manager);

    ui->stylesAvailableLabel->setText(i18n("Styles available"));
    connect(this, &QDialog::accepted, this, &TableOfContentsStyleConfigure::save);
}

TableOfContentsStyleConfigure::~TableOfContentsStyleConfigure()
{
    delete ui;
}

void TableOfContentsStyleConfigure::initializeUi(KoTableOfContentsGeneratorInfo *info)
{
    Q_ASSERT(info);

    m_tocInfo = info;

    connect(this, &QDialog::accepted, this, &TableOfContentsStyleConfigure::save);
    connect(this, &QDialog::rejected, this, &TableOfContentsStyleConfigure::discardChanges);

    m_stylesModel = new TableOfContentsStyleModel(m_styleManager, m_tocInfo);
    ui->tableView->setModel(m_stylesModel);

    ui->tableView->setItemDelegateForColumn(1, &m_delegate);

    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->resizeSection(1, 100);

    this->setVisible(true);
}

void TableOfContentsStyleConfigure::save()
{
    if (m_stylesModel) {
        m_stylesModel->saveData();
        delete m_stylesModel;
        m_stylesModel = nullptr;
    }

    disconnect(this, &QDialog::accepted, this, &TableOfContentsStyleConfigure::save);
    disconnect(this, &QDialog::rejected, this, &TableOfContentsStyleConfigure::discardChanges);
}

void TableOfContentsStyleConfigure::discardChanges()
{
    if (m_stylesModel) {
        delete m_stylesModel;
        m_stylesModel = nullptr;
    }

    disconnect(this, &QDialog::accepted, this, &TableOfContentsStyleConfigure::save);
    disconnect(this, &QDialog::rejected, this, &TableOfContentsStyleConfigure::discardChanges);
}
