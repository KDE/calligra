/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

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
 * Boston, MA 02110-1301, USA.
*/

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextCodec>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kurlrequester.h>

#include <exportdialog.h>

ExportDialog::ExportDialog(QWidget *parent)
        : KDialog(parent), m_mainwidget(new ExportWidget(this))
{
    setCaption(i18n("Export Sheet to HTML"));
    setButtons(Ok | Cancel);
    setDefaultButton(KDialog::Cancel);
    kapp->restoreOverrideCursor();

    connect(m_mainwidget->mCustomButton, SIGNAL(toggled(bool)),
            m_mainwidget->mCustomURL, SLOT(setEnabled(bool)));
    connect(m_mainwidget->mSelectAllButton, SIGNAL(clicked()), SLOT(selectAll()));
    connect(m_mainwidget->mDeselectAllButton, SIGNAL(clicked()),
            m_mainwidget->mSheets, SLOT(clearSelection()));

    m_mainwidget->mEncodingBox->addItem(i18n("Recommended: UTF-8"));
    m_mainwidget->mEncodingBox->addItem(i18n("Locale (%1)", QString::fromLatin1(KGlobal::locale()->codecForEncoding()->name())));

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
    kapp->setOverrideCursor(Qt::WaitCursor);
}

QTextCodec *ExportDialog::encoding() const
{
    if (m_mainwidget->mEncodingBox->currentIndex() == 1)  // locale selected
        return KGlobal::locale()->codecForEncoding();

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

KUrl ExportDialog::customStyleURL() const
{
    KUrl url = m_mainwidget->mCustomURL->url();
    if (m_mainwidget->mCustomButton->isChecked() && url.isValid())
        return url;

    return KUrl();
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
    for (uint i = 0; i < view->count() ; i++) {
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

#include <exportdialog.moc>
