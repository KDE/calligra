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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTCONFIGBEHAVIORPANEL_H
#define KPTCONFIGBEHAVIORPANEL_H

#include "kptconfigbehaviorpanelbase.h"
#include "kptconfig.h"

class KMacroCommand;

namespace KPlato
{

class KPTConfigBehaviourPanelBase;
class KPTPart;

class KPTConfigBehaviorPanel : public KPTConfigBehaviorPanelBase {
    Q_OBJECT
public:
    KPTConfigBehaviorPanel(KPTBehavior &behavior, QWidget *parent=0, const char *name=0);

    void setStartValues();
    bool ok();
    bool apply();
    
private:
    KPTBehavior m_oldvalues;
    KPTBehavior &m_behavior;

};

} //KPlato namespace

#endif // KPTCONFIGBEHAVIORPANEL_H
