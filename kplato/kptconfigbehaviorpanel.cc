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

#include "kptconfigbehaviorpanel.h"

#include "kptdatetime.h"
#include "kptfactory.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kcommand.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QLayout>
#include <qdatetime.h> 
#include <q3buttongroup.h> 
#include <QCheckBox> 

namespace KPlato
{

ConfigBehaviorPanel::ConfigBehaviorPanel(Behavior &behavior, QWidget *p, const char *n)
    : ConfigBehaviorPanelBase(p, n),
      m_oldvalues(behavior),
      m_behavior(behavior)
{
    setStartValues();
    
    allowOverbooking->setEnabled(false); // not yet used
}

void ConfigBehaviorPanel::setStartValues() {
    calculationGroup->setButton(m_oldvalues.calculationMode);
    allowOverbooking->setChecked(m_oldvalues.allowOverbooking);
}

bool ConfigBehaviorPanel::ok() {
    return true;
}

bool ConfigBehaviorPanel::apply() {
    m_behavior.calculationMode = calculationGroup->selectedId();
    m_behavior.allowOverbooking = allowOverbooking->isChecked();
    return true;
}


}  //KPlato namespace

#include "kptconfigbehaviorpanel.moc"
