/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kptconfigdialog.h"

#include "kpttaskdefaultpanel.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kcommand.h>

#include <qvbox.h>

#include <kdebug.h>

namespace KPlato
{

// little helper stolen from kmail/kword
static inline QPixmap loadIcon( const char * name ) {
  return KGlobal::instance()->iconLoader()
    ->loadIcon( QString::fromLatin1(name), KIcon::NoGroup, KIcon::SizeMedium );
}


KPTConfigDialog::KPTConfigDialog(KPTTask &task, QWidget *parent, const char *n)
    : KDialogBase(KDialogBase::IconList, i18n("Configure KPlato"),
                  KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel| KDialogBase::Default,
                  KDialogBase::Ok, parent)
{

    QVBox *page = addVBoxPage(i18n("Task defaults"), i18n("Task defaults"), loadIcon("misc"));
    m_taskDefaultPage = new KPTTaskDefaultPanel(task, 0, page);
    
    enableButtonOK(false);
    enableButtonApply(false);
    
    connect(m_taskDefaultPage, SIGNAL(changed()), SLOT(slotChanged()));
}


void KPTConfigDialog::slotApply() {
    if (!m_taskDefaultPage->ok())
        return;
    KCommand *cmd = m_taskDefaultPage->buildCommand(0);
    if (cmd)
        cmd->execute();    
}

void KPTConfigDialog::slotOk() {
    slotApply();
    accept();
}

void KPTConfigDialog::slotDefault() {
    kdDebug()<<k_funcinfo<<endl;
    KPTTask task;
    m_taskDefaultPage->setStartValues(task);
    //enableButtonOk(false);
}

void KPTConfigDialog::slotChanged() {
    enableButtonOK(true);
    enableButtonApply(true);
}

}  //KPlato namespace

#include "kptconfigdialog.moc"
