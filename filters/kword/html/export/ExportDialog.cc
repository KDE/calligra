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

#include <kapp.h>
#include <klocale.h>

#include <ExportDialogUI.h>
#include <ExportDialog.h>

HtmlExportDialog :: HtmlExportDialog(QWidget* parent)
    : KDialogBase(parent, 0, true, QString::null, Ok|Cancel, No, true),
      m_dialog(new ExportDialogUI(this))
{

    kapp->restoreOverrideCursor();

    resize(size()); // Is this right?

    setMainWidget(m_dialog);

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
    QTextCodec* codec;

    if(m_dialog->radioEncodingUTF8==m_dialog->buttonGroupEncoding->selected())
        codec=QTextCodec::codecForName("UTF-8");
    else if(m_dialog->radioEncodingLocal==m_dialog->buttonGroupEncoding->selected())
        codec=QTextCodec::codecForLocale();
    else
        // Default: UTF-8
        codec=QTextCodec::codecForName("UTF-8");

    return codec;
}

#include <ExportDialog.moc>
