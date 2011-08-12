/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptrecalculatedialog.h"

#include <kdebug.h>

namespace KPlato
{

RecalculateDialogImpl::RecalculateDialogImpl (QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    QDateTime ct = QDateTime::currentDateTime();
    ct.setTime( QTime( ct.time().hour(), ct.time().minute() ) ); // clear secs/msecs
    dateTimeEdit->setDateTime( ct );
    btnCurrent->setChecked( true );
    dateTimeEdit->setEnabled( false );
    connect( btnFrom, SIGNAL( toggled( bool ) ), dateTimeEdit, SLOT( setEnabled( bool ) ) );
}



//////////////////  ResourceDialog  ////////////////////////

RecalculateDialog::RecalculateDialog( QWidget *parent )
    : KDialog(parent)
{
    setCaption( i18n("Re-calculate Schedule") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    dia = new RecalculateDialogImpl(this);
    setMainWidget(dia);
}

QDateTime RecalculateDialog::dateTime() const {
    return dia->btnFrom->isChecked() ? dia->dateTimeEdit->dateTime() : QDateTime::currentDateTime();
}


}  //KPlato namespace

#include "kptrecalculatedialog.moc"
