/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "exportdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextCodec>
#include <QApplication>
#include <QUrl>

ExportDialog::ExportDialog(QWidget *parent)
        : KoDialog(parent), m_mainwidget(new ExportWidget(this))
{
    setCaption(i18n("Export Sheet to HTML"));
    setButtons(Ok | Cancel);
    setDefaultButton(Cancel);
    QApplication::restoreOverrideCursor();

    connect(m_mainwidget->mCustomButton, &QAbstractButton::toggled,
            m_mainwidget->mCustomURL, &QWidget::setEnabled);
    connect(m_mainwidget->mSelectAllButton, &QAbstractButton::clicked, this, &ExportDialog::selectAll);
    connect(m_mainwidget->mDeselectAllButton, &QAbstractButton::clicked,
            m_mainwidget->mSheets, &QAbstractItemView::clearSelection);

    m_mainwidget->mEncodingBox->addItem(i18n("Recommended: UTF-8"));
    m_mainwidget->mEncodingBox->addItem(i18n("Locale (%1)", QString::fromLatin1(QTextCodec::codecForLocale()->name())));

    m_mainwidget->mCustomURL->setMode(KFile::ExistingOnly);

    setMainWidget(m_mainwidget);
}

void ExportDialog::selectAll()
{

    QListWidget *view = m_mainwidget->mSheets;
    QAbstractItemModel *model = view->model();
    QModelIndex topLeft = model->index(0, 0);
    QModelIndex bottomRight = model->index(model->rowCount() - 1, model->columnCount() - 1);
    QItemSelection selection(topLeft, bottomRight);
    view->selectionModel()->select(selection , QItemSelectionModel::QItemSelectionModel::Select);
}

ExportDialog::~ExportDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

QTextCodec *ExportDialog::encoding() const
{
    if (m_mainwidget->mEncodingBox->currentIndex() == 1)  // locale selected
        return QTextCodec::codecForLocale();

    return QTextCodec::codecForName("utf8");   // utf8 is default
}

bool ExportDialog::useBorders() const
{
    return m_mainwidget->mUseBorders->isChecked();
}

bool ExportDialog::separateFiles() const
{
    return m_mainwidget->mSeparateFiles->isChecked();
}

QUrl ExportDialog::customStyleURL() const
{
    QUrl url = m_mainwidget->mCustomURL->url();
    if (m_mainwidget->mCustomButton->isChecked() && url.isValid())
        return url;

    return QUrl();
}

void ExportDialog::setSheets(const QStringList &list)
{
    m_mainwidget->mSheets->addItems(list);
    selectAll();
}

QStringList ExportDialog::sheets() const
{
    QListWidget* view = m_mainwidget->mSheets;
    QStringList list;
    for (uint i = 0; i < (uint)view->count() ; i++) {
        QListWidgetItem* item = view->item(i);
        if (item->isSelected()) {
            list.append(item->text());
        }
    }
    return list;
}

int ExportDialog::pixelsBetweenCells() const
{
    return m_mainwidget->mPixelsBetweenCells->value();
}
