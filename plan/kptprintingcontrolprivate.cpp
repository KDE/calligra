/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2011 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptprintingcontrolprivate.h"

#include <kptviewbase.h>

#include <QSpinBox>
#include <QPrintDialog>

namespace KPlato
{

PrintingControlPrivate::PrintingControlPrivate( PrintingDialog *job, QPrintDialog *dia )
    : QObject( dia ),
    m_job( job ),
    m_dia( dia )
{
    connect(job, SIGNAL(changed()), SLOT(slotChanged()));
}

void PrintingControlPrivate::slotChanged()
{
    if ( ! m_job || ! m_dia ) {
        return;
    }
    QSpinBox *to = m_dia->findChild<QSpinBox*>("to");
    QSpinBox *from = m_dia->findChild<QSpinBox*>("from");
    if ( to && from ) {
        from->setMinimum( m_job->documentFirstPage() );
        from->setMaximum( m_job->documentLastPage() );
        from->setValue( from->minimum() );
        to->setMinimum( from->minimum() );
        to->setMaximum( from->maximum() );
        to->setValue( to->maximum() );
    }
}

}  //KPlato namespace
