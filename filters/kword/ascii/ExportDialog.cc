// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qtextcodec.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kapplication.h>

#include <ExportDialogUI.h>
#include <ExportDialog.h>

AsciiExportDialog :: AsciiExportDialog(QWidget* parent)
    : KDialogBase(parent, 0, true, i18n("KWord's Plain Text Export Filter"), Ok|Cancel, No, true),
      m_dialog(new ExportDialogUI(this))
{

    kapp->restoreOverrideCursor();


    m_dialog->comboBoxEncoding->insertStringList(KGlobal::charsets()->availableEncodingNames());
    //m_dialog->comboBoxEncoding->insertStringList(KGlobal::charsets()->descriptiveEncodingNames());

    resize(size()); // Is this right?

    setMainWidget(m_dialog);

    connect(m_dialog->comboBoxEncoding, SIGNAL(activated(int)), this,
        SLOT(comboBoxEncodingActivated(int)));
}

AsciiExportDialog :: ~AsciiExportDialog(void)
{
    kapp->setOverrideCursor(Qt::waitCursor);
}

QTextCodec* AsciiExportDialog::getCodec(void) const
{
    QTextCodec* codec=NULL;

    if (m_dialog->radioEncodingUTF8==m_dialog->buttonGroupEncoding->selected())
    {
        kdDebug(30502) << "Encoding: UTF-8" << endl;
        codec=QTextCodec::codecForName("UTF-8");
    }
    else if (m_dialog->radioEncodingLocal==m_dialog->buttonGroupEncoding->selected())
    {
        kdDebug(30502) << "Encoding: Locale" << endl;
        codec=QTextCodec::codecForLocale();
    }
    else if (m_dialog->radioEncodingOther==m_dialog->buttonGroupEncoding->selected())
    {
        QString strCodec=m_dialog->comboBoxEncoding->currentText();
        kdDebug(30502) << "Encoding: " << strCodec << endl;
        if (strCodec.isEmpty())
        {
            codec=QTextCodec::codecForLocale();
        }
        else
        {
            // We do not use QTextCodec::codecForName here
            //   because we fear subtle problems
            codec=KGlobal::charsets()->codecForName(strCodec);
        }
    }

    if (!codec)
    {
        // Default: UTF-8
        kdWarning(30502) << "No codec set, assuming UTF-8" << endl;
        codec=QTextCodec::codecForName("UTF-8");
    }

    return codec;
}

QString AsciiExportDialog::getEndOfLine(void) const
{
    QString strReturn;
    if (m_dialog->radioEndOfLineLF==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\n";
    else if (m_dialog->radioEndOfLineCRLF==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\r\n";
    else if (m_dialog->radioEndOfLineCR==m_dialog->buttonGroupEndOfLine->selected())
        strReturn="\r";
    else
        strReturn="\n";

    return strReturn;
}

void AsciiExportDialog::comboBoxEncodingActivated(int)
{
    m_dialog->buttonGroupEncoding->setButton(2); // Select the "other" button
}

#include <ExportDialog.moc>
