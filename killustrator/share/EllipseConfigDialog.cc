/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <EllipseConfigDialog.h>

#include <klocale.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>

#include <OvalTool.h>

EllipseConfigDialog::EllipseConfigDialog (QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n("Setup Ellipse Tool"),
                KDialogBase::Ok | KDialogBase::Cancel) {
    createWidget (makeMainWidget());
}

void EllipseConfigDialog::createWidget (QWidget* parent) {

    QVBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint());
    QButtonGroup *group=new QVButtonGroup(i18n("Draw ellipse"), parent);
    layout->addWidget(group);

    radiusButton=new QRadioButton(i18n("between points"), group);
    diameterButton=new QRadioButton(i18n("around fixed center"), group);
}

void EllipseConfigDialog::setupTool (OvalTool* tool) {

    EllipseConfigDialog dialog;

    if (tool->aroundFixedCenter ())
        dialog.diameterButton->setChecked (true);
    else
        dialog.radiusButton->setChecked (true);

    if (dialog.exec() == Accepted)
        tool->aroundFixedCenter(dialog.diameterButton->isChecked());
}

#include <EllipseConfigDialog.moc>
