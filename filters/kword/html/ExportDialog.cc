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

#include <kapp.h>
#include <klocale.h>

#include <qstring.h>

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

QString HtmlExportDialog::getState(void)
{
    QString result;

    if(m_dialog->radioDocType1==m_dialog->buttonGroup1->selected())
        result += "HTML";
    else if(m_dialog->radioDocType2==m_dialog->buttonGroup1->selected())
        result += "XHTML";
    else
        result += "XHTML";

    result += '-';

    if(m_dialog->radioMode1==m_dialog->buttonGroup2->selected())
        result += "SPARTAN";
    else if(m_dialog->radioMode2==m_dialog->buttonGroup2->selected())
        result += "TRANSITIONAL";
    else if(m_dialog->radioMode3==m_dialog->buttonGroup2->selected())
        result += "STYLE";
    else
        result += "STYLE";

    return result;
}

#include <ExportDialog.moc>