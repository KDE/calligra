/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptconfigdialog.h"

#include "kpttaskdefaultpanel.h"
//#include "kptconfigbehaviorpanel.h"

#include "kptconfig.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kcommand.h>
#include <kinstance.h>

//Added by qt3to4:
#include <QPixmap>

#include <kdebug.h>
#include <kvbox.h>

namespace KPlato
{

// little helper stolen from kmail/kword
static inline QPixmap loadIcon( const char * name ) {
  return KGlobal::instance()->iconLoader()
    ->loadIcon( QString::fromLatin1(name), K3Icon::NoGroup, K3Icon::SizeMedium );
}


ConfigDialog::ConfigDialog(Config &config, Project &project, QWidget *parent, const char *)
    : KPageDialog(parent),
      m_config(config)
{
    setCaption( i18n("Configure KPlato") );
    setButtons( KDialog::Ok | KDialog::Apply | KDialog::Cancel| KDialog::Default );
    setDefaultButton( Ok );
    setTypeFace( KPageDialog::List );

/*    QVBox *page = addVBoxPage(i18n("Behavior"), i18n("Behavior"), loadIcon("misc"));
    m_behaviorPage = new ConfigBehaviorPanel(config.behavior(), page);*/

    KVBox *page = new KVBox();
    addPage( page, i18n("Task Defaults") );
    //addVBoxPage(i18n("Task Defaults"), i18n("Task Defaults"), loadIcon("misc"));
    m_taskDefaultPage = new TaskDefaultPanel(config.taskDefaults(), project.standardWorktime(), page);

    enableButtonOK(false);
    enableButtonApply(false);

//    connect(m_behaviorPage, SIGNAL(changed()), SLOT(slotChanged()));
    connect(m_taskDefaultPage, SIGNAL(changed()), SLOT(slotChanged()));
}


void ConfigDialog::slotApply() {
    if (!m_taskDefaultPage->ok())
        return;
/*    if (!m_behaviorPage->ok())
        return;*/
    KCommand *cmd = m_taskDefaultPage->buildCommand(0);
    if (cmd)
        cmd->execute();

//    m_behaviorPage->apply();
}

void ConfigDialog::slotOk() {
    slotApply();
    accept();
}

void ConfigDialog::slotDefault() {
    kDebug()<<k_funcinfo<<endl;
    m_taskDefaultPage->setStartValues(m_config.taskDefaults());
//    m_behaviorPage->setStartValues();

    enableButtonOK(false);
    enableButtonApply(false);
}

void ConfigDialog::slotChanged() {
    enableButtonOK(true);
    enableButtonApply(true);
}

}  //KPlato namespace

#include "kptconfigdialog.moc"
