// $Header$

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qtextcodec.h>

#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qcombobox.h>

#include <ExportDialogUI.h>
#include <ExportDialog.h>

HtmlExportDialog :: HtmlExportDialog(QWidget* parent)
    : KDialogBase(parent, 0, true, i18n("KWord's HTML Export Filter"), Ok|Cancel, No, true),
      m_dialog(new ExportDialogUI(this))
{

    kapp->restoreOverrideCursor();

    QStringList encodingList;

    encodingList += i18n( "Descriptive encoding name", "Recommended ( %1 )" ).arg( "UTF-8" );
    encodingList += i18n( "Descriptive encoding name", "Locale ( %1 )" ).arg( QTextCodec::codecForLocale()->name() );
    encodingList += KGlobal::charsets()->descriptiveEncodingNames();
    
    m_dialog->comboBoxEncoding->insertStringList( encodingList );


    setMainWidget(m_dialog);

    connect(m_dialog->comboBoxEncoding, SIGNAL(activated(int)), this,
        SLOT(comboBoxEncodingActivated(int)));
}

HtmlExportDialog :: ~HtmlExportDialog(void)
{
    kapp->setOverrideCursor(Qt::waitCursor);
}

bool HtmlExportDialog::isXHtml(void) const
{
    if(m_dialog->radioDocType1==m_dialog->buttonGroup1->selected())
        return false;
    else if(m_dialog->radioDocType2==m_dialog->buttonGroup1->selected())
        return true;
    return true;
}

QTextCodec* HtmlExportDialog::getCodec(void) const
{
    QTextCodec* codec=0;

    if (m_dialog->radioEncodingUTF8==m_dialog->buttonGroupEncoding->selected())
    {
        kdDebug(30503) << "Encoding: UTF-8" << endl;
        codec=QTextCodec::codecForName("UTF-8");
    }
    else if (m_dialog->radioEncodingLocal==m_dialog->buttonGroupEncoding->selected())
    {
        kdDebug(30503) << "Encoding: Locale" << endl;
        codec=QTextCodec::codecForLocale();
    }
    else if (m_dialog->radioEncodingOther==m_dialog->buttonGroupEncoding->selected())
    {
        const QString strCodec( KGlobal::charsets()->encodingForName( m_dialog->comboBoxEncoding->currentText() ) );
        kdDebug(30503) << "Encoding: " << strCodec << endl;
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
        kdWarning(30503) << "No codec set, assuming UTF-8" << endl;
        codec=QTextCodec::codecForName("UTF-8");
    }

    return codec;
}

void HtmlExportDialog::comboBoxEncodingActivated(int)
{
    m_dialog->buttonGroupEncoding->setButton(2); // Select the "other" button
}

HtmlExportDialog::Mode HtmlExportDialog::getMode(void) const
{
    if (m_dialog->radioModeEnhanced==m_dialog->buttonGroupMode->selected())
    {
        return CSS;
    }
    else if (m_dialog->radioModeBasic==m_dialog->buttonGroupMode->selected())
    {
        return Basic;
    }
    else if (m_dialog->radioModeLight==m_dialog->buttonGroupMode->selected())
    {
        return Light;
    }

    return CSS;//Our default
}


#include <ExportDialog.moc>
