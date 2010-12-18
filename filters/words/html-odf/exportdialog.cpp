/* This file is part of the KDE project
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>

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
    m_mainwidget->mEncodingBox->addItem(i18n("Recommended: UTF-8"));
    m_mainwidget->mEncodingBox->addItem(i18n("Locale (%1)", QString::fromLatin1(KGlobal::locale()->codecForEncoding()->name())));
    m_mainwidget->mCustomURL->setMode(KFile::ExistingOnly);
    setMainWidget(m_mainwidget);
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


KUrl ExportDialog::customStyleURL() const
{
    KUrl url = m_mainwidget->mCustomURL->url();
    if (m_mainwidget->mCustomButton->isChecked() && url.isValid())
        return url;

    return KUrl();
}

#include <exportdialog.moc>
