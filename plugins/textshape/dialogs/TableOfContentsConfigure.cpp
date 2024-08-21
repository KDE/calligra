/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableOfContentsConfigure.h"
#include "KoTableOfContentsGeneratorInfo.h"
#include "KoTextDocument.h"
#include "TableOfContentsEntryDelegate.h"
#include "TableOfContentsEntryModel.h"
#include "TableOfContentsStyleConfigure.h"
#include "TableOfContentsStyleModel.h"

#include <KoParagraphStyle.h>
#include <KoTextEditor.h>

TableOfContentsConfigure::TableOfContentsConfigure(KoTextEditor *editor, QTextBlock block, QWidget *parent)
    : QDialog(parent)
    , m_textEditor(editor)
    , m_tocStyleConfigure(nullptr)
    , m_tocInfo(nullptr)
    , m_block(block)
    , m_document(nullptr)
    , m_tocEntryStyleModel(nullptr)
    , m_tocEntryConfigureDelegate(nullptr)
{
    init();

    KoTableOfContentsGeneratorInfo *info = block.blockFormat().property(KoParagraphStyle::TableOfContentsData).value<KoTableOfContentsGeneratorInfo *>();
    m_tocInfo = info->clone();

    setDisplay();
}

TableOfContentsConfigure::TableOfContentsConfigure(KoTextEditor *editor, KoTableOfContentsGeneratorInfo *info, QWidget *parent)
    : QDialog(parent)
    , m_textEditor(editor)
    , m_tocStyleConfigure(nullptr)
    , m_tocInfo(nullptr)
    , m_document(nullptr)
    , m_tocEntryStyleModel(nullptr)
    , m_tocEntryConfigureDelegate(nullptr)
{
    init();
    m_tocInfo = info->clone();
    setDisplay();
}

TableOfContentsConfigure::~TableOfContentsConfigure()
{
    if (m_tocInfo) {
        delete m_tocInfo;
    }
}

void TableOfContentsConfigure::init()
{
    ui.setupUi(this);

    setWindowTitle(i18n("Table of Contents - Configure"));

    ui.lineEditTitle->setText(i18n("Table Title"));
    ui.useOutline->setText(i18n("Use outline"));
    ui.useStyles->setText(i18n("Use styles"));
    ui.configureStyles->setText(i18n("Configure"));
    ui.tabWidget->setTabText(0, i18n("Index"));
    ui.tabWidget->setTabText(1, i18n("Styles"));
    ui.tabWidget->setCurrentIndex(0);

    ui.tocPreview->setStyleManager(KoTextDocument(m_textEditor->document()).styleManager());

    connect(this, &QDialog::accepted, this, &TableOfContentsConfigure::save);
    connect(this, &QDialog::rejected, this, &TableOfContentsConfigure::cleanUp);
    connect(ui.configureStyles, &QAbstractButton::clicked, this, &TableOfContentsConfigure::showStyleConfiguration);
    connect(ui.lineEditTitle, &QLineEdit::returnPressed, this, &TableOfContentsConfigure::updatePreview);
}

void TableOfContentsConfigure::setDisplay()
{
    setVisible(true);

    ui.lineEditTitle->setText(m_tocInfo->m_indexTitleTemplate.text);
    ui.useOutline->setCheckState(m_tocInfo->m_useOutlineLevel ? Qt::Checked : Qt::Unchecked);
    ui.useStyles->setCheckState(m_tocInfo->m_useIndexSourceStyles ? Qt::Checked : Qt::Unchecked);

    connect(ui.lineEditTitle, &QLineEdit::textChanged, this, &TableOfContentsConfigure::titleTextChanged);
    connect(ui.useOutline, &QCheckBox::stateChanged, this, &TableOfContentsConfigure::useOutline);
    connect(ui.useStyles, &QCheckBox::stateChanged, this, &TableOfContentsConfigure::useIndexSourceStyles);

    m_tocEntryStyleModel = new TableOfContentsEntryModel(KoTextDocument(m_textEditor->document()).styleManager(), m_tocInfo);
    connect(m_tocEntryStyleModel, &TableOfContentsEntryModel::tocEntryDataChanged, this, &TableOfContentsConfigure::updatePreview);

    m_tocEntryConfigureDelegate = new TableOfContentsEntryDelegate(KoTextDocument(m_textEditor->document()).styleManager());

    ui.configureToCEntryStyle->setModel(m_tocEntryStyleModel);

    ui.configureToCEntryStyle->setItemDelegateForColumn(1, m_tocEntryConfigureDelegate);

    ui.configureToCEntryStyle->setShowGrid(false);
    ui.configureToCEntryStyle->verticalHeader()->hide();
    ui.configureToCEntryStyle->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui.configureToCEntryStyle->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.configureToCEntryStyle->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui.configureToCEntryStyle->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(this, &QDialog::accepted, this, &TableOfContentsConfigure::save);
    connect(this, &QDialog::rejected, this, &TableOfContentsConfigure::cleanUp);

    updatePreview();
}

void TableOfContentsConfigure::save()
{
    m_tocInfo->m_name = ui.lineEditTitle->text();
    m_tocInfo->m_indexTitleTemplate.text = ui.lineEditTitle->text();
    m_tocInfo->m_useOutlineLevel = (ui.useOutline->checkState() == Qt::Checked);
    m_tocInfo->m_useIndexSourceStyles = (ui.useStyles->checkState() == Qt::Checked);

    if (m_tocEntryStyleModel) {
        m_tocEntryStyleModel->saveData();
    }

    if (m_block.isValid()) {
        m_textEditor->setTableOfContentsConfig(m_tocInfo, m_block);
    }
    cleanUp();
}

void TableOfContentsConfigure::showStyleConfiguration()
{
    if (!m_tocStyleConfigure) {
        m_tocStyleConfigure = new TableOfContentsStyleConfigure(KoTextDocument(m_textEditor->document()).styleManager(), this);
    }
    m_tocStyleConfigure->initializeUi(m_tocInfo);
}

void TableOfContentsConfigure::titleTextChanged(const QString &text)
{
    m_tocInfo->m_indexTitleTemplate.text = text;
    updatePreview();
}

void TableOfContentsConfigure::useOutline(int state)
{
    m_tocInfo->m_useOutlineLevel = (state == Qt::Checked);
    updatePreview();
}

void TableOfContentsConfigure::useIndexSourceStyles(int state)
{
    m_tocInfo->m_useIndexSourceStyles = (state == Qt::Checked);
    updatePreview();
}

void TableOfContentsConfigure::updatePreview()
{
    ui.tocPreview->updatePreview(m_tocInfo);
}

void TableOfContentsConfigure::cleanUp()
{
    disconnect(ui.lineEditTitle, &QLineEdit::textChanged, this, &TableOfContentsConfigure::titleTextChanged);
    disconnect(ui.useOutline, &QCheckBox::stateChanged, this, &TableOfContentsConfigure::useOutline);
    disconnect(ui.useStyles, &QCheckBox::stateChanged, this, &TableOfContentsConfigure::useIndexSourceStyles);

    disconnect(this, &QDialog::accepted, this, &TableOfContentsConfigure::save);
    disconnect(this, &QDialog::rejected, this, &TableOfContentsConfigure::cleanUp);

    if (m_tocEntryStyleModel) {
        delete m_tocEntryStyleModel;
        m_tocEntryStyleModel = nullptr;
    }

    if (m_tocEntryConfigureDelegate) {
        delete m_tocEntryConfigureDelegate;
        m_tocEntryConfigureDelegate = nullptr;
    }
}

KoTableOfContentsGeneratorInfo *TableOfContentsConfigure::currentToCData()
{
    return m_tocInfo;
}
