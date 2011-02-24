/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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

#include <QTextCodec>

#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kapplication.h>

#include <ImportDialogUI.h>
#include <ImportDialog.h>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QCheckBox>

MSWriteImportDialog :: MSWriteImportDialog(QWidget* parent)
        : KDialog(parent),
        m_dialog(new ImportDialogUI(this))
{
    setCaption(i18n("KWord's MS Write Import Filter"));
    setButtons(Ok | Cancel);
    kapp->restoreOverrideCursor();

    m_dialog->comboBoxEncoding->addItems(KGlobal::charsets()->availableEncodingNames());
    //m_dialog->comboBoxEncoding->addItems(KGlobal::charsets()->descriptiveEncodingNames());

    resize(size()); // Is this right?

    setMainWidget(m_dialog);

    connect(m_dialog->comboBoxEncoding, SIGNAL(activated(int)), this,
            SLOT(comboBoxEncodingActivated(int)));
}

MSWriteImportDialog :: ~MSWriteImportDialog(void)
{
    kapp->setOverrideCursor(Qt::WaitCursor);
}

QTextCodec* MSWriteImportDialog::getCodec(void) const
{
    QTextCodec* codec = NULL;

    if (m_dialog->radioEncodingDefault == m_dialog->buttonGroupEncoding->selected()) {
        kDebug(30509) << "Encoding: CP 1252";
        codec = QTextCodec::codecForName("CP 1252");
    }
    /*else if (m_dialog->radioEncodingLocal==m_dialog->buttonGroupEncoding->selected())
    {
        kDebug(30503) <<"Encoding: Locale";
        codec=QTextCodec::codecForLocale();
    }*/
    else if (m_dialog->radioEncodingOther == m_dialog->buttonGroupEncoding->selected()) {
        QString strCodec = m_dialog->comboBoxEncoding->currentText();
        kDebug(30509) << "Encoding:" << strCodec;
        if (strCodec.isEmpty()) {
            codec = QTextCodec::codecForLocale();
        } else {
            // We do not use QTextCodec::codecForName here
            //   because we fear subtle problems
            codec = KGlobal::charsets()->codecForName(strCodec);
        }
    }

    if (!codec) {
        // Default: UTF-8
        kWarning(30509) << "No codec set, assuming UTF-8";
        codec = QTextCodec::codecForName("UTF-8");
    }

    return codec;
}

bool MSWriteImportDialog::getSimulateLinespacing(void) const
{
    return (m_dialog->checkBoxLinespacing->isChecked());
}

bool MSWriteImportDialog::getSimulateImageOffset(void) const
{
    return (m_dialog->checkBoxImageOffset->isChecked());
}

void MSWriteImportDialog::comboBoxEncodingActivated(int)
{
    m_dialog->buttonGroupEncoding->setButton(1); // Select the "Other Encoding" button
}


#include <ImportDialog.moc>
