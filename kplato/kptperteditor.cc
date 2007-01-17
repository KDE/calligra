/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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
#include "kptperteditor.h"

//#include "kptcommand.h"
//#include "kptitemmodelbase.h"
//#include "kptcalendar.h"
//#include "kptduration.h"
//#include "kptfactory.h"
//#include "kptresourceappointmentsview.h"
#include "kptview.h"
//#include "kptnode.h"
//#include "kptproject.h"
//#include "kpttask.h"
//#include "kptschedule.h"
//#include "kptdatetime.h"
//#include "kptcontext.h"

//#include <kicon.h>
//#include <kglobal.h>
//#include <klocale.h>
//#include <kprinter.h>
//#include <kxmlguifactory.h>
//#include <kactioncollection.h>

#include <kdebug.h>

namespace KPlato
{

//-----------------------------------
PertEditor::PertEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
kDebug() << " ---------------- KPlato: Creating PertEditor ----------------" << endl;
    widget.setupUi(this);
    widget.assignList->setSelectedLabel(i18n("Required"));
    widget.assignList->setShowUpDownButtons(false);
    widget.assignList->layout()->setMargin(0);
}

void PertEditor::setupGui()
{

}


} // namespace KPlato

#include "kptperteditor.moc"
