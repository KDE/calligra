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

#include <qstring.h>
#include <kdebug.h>
#include <kapp.h>
#include <klocale.h>


#include <ImportDialogUI.h>
#include <ImportDialog.h>

HtmlImportDialog :: HtmlImportDialog(QWidget* parent)
    : KDialogBase(parent, 0, true, QString::null, Ok|Cancel, No, true),
      m_dialog(new ImportDialogUI(this))
{

    kapp->restoreOverrideCursor();

    resize(size()); // Is this right?

    setMainWidget(m_dialog);

}

HtmlImportDialog :: ~HtmlImportDialog(void)
{
    kapp->setOverrideCursor(Qt::waitCursor);
}

void HtmlImportDialog::setHintCharset(const QString& strNew)
{
    m_dialog->lineEditCharSet->setText(strNew);
}

QString HtmlImportDialog::getHintCharset(void)
{
    QString strHint=m_dialog->lineEditCharSet->text();

    kdDebug(30503) << "Charset chosen in dialog: " << strHint << endl;

    return strHint;
}


int HtmlImportDialog::getHint(void)
{
    int result;

    if(m_dialog->radioButtonHintNone==m_dialog->buttonHint->selected())
        result=0;
    else if(m_dialog->radioButtonHintFallback==m_dialog->buttonHint->selected())
        result=1;
    else if(m_dialog->radioButtonHintForce==m_dialog->buttonHint->selected())
        result=2;
    else
    {
        kdError(30503) << "Mayday! I cannot find the hint chosen!" << endl;
        return 0;
    }

    kdDebug(30503) << "Hint chosen in dialog: " << result << endl;

    return result;
}


#include <ImportDialog.moc>
