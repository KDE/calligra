/* This file is part of the KDE project
   Copyright (C) 2002 The koffice team <koffice@kde.org>

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

#include "kptresourcedialog.h"
#include "kptresource.h"

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>

#include <qtextedit.h>
#include <qdatetimeedit.h>


KPTResourceDialog::KPTResourceDialog(QWidget *p, const char *n)
    : KDialogBase(Plain, i18n("New Resource"), Ok|Cancel, Ok, p, n, true, true),
      m_resource(0)
{
    init();
}

KPTResourceDialog::KPTResourceDialog(KPTResource *resource, QWidget *p, const char *n)
    : KDialogBase(Plain, i18n("Edit Resource"), Ok|Cancel, Ok, p, n, true, true),
      m_resource(resource)
{
    init();
}

void KPTResourceDialog::init()
{
    QWidget *page = plainPage();
    QGridLayout *layout = new QGridLayout(page, 4, 2, marginHint(), spacingHint());

    // Name of the milestone
    layout->addWidget(new QLabel(i18n("Resource name:"), page), 0, 0);
    layout->addMultiCellWidget(m_namefield=new KLineEdit("", page), 0, 0, 1, 3);
    if (m_resource)
        m_namefield->setText(m_resource->name());

    layout->addWidget(new QLabel(i18n("Available:"), page), 1,0);
    
    m_availableFrom = new QTimeEdit(QTime(8,0),page);
    m_availableFrom->setDisplay(QTimeEdit::Hours|QTimeEdit::Minutes);
    layout->addWidget(m_availableFrom, 1, 2);

    m_availableFrom = new QTimeEdit(QTime(16,0),page);
    m_availableFrom->setDisplay(QTimeEdit::Hours|QTimeEdit::Minutes);
    layout->addWidget(m_availableFrom, 1, 3);
}


void KPTResourceDialog::slotOk() {
    if (m_namefield->text().isEmpty() ) {
	    KMessageBox::sorry(this, i18n("You have to set a name for this resource"));
	    return;
    }
    
    accept();
}


#include "kptresourcedialog.moc"
