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

#include <BlendDialog.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qhgroupbox.h>

#include <klocale.h>

BlendDialog::BlendDialog (QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n ("Blend"),
                KDialogBase::Ok | KDialogBase::Cancel) {
    createWidget(makeMainWidget());
}

void BlendDialog::createWidget (QWidget* parent) {

    QVBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint());
    QGroupBox *box = new QHGroupBox(i18n("Blend Objects"), parent);
    layout->addWidget(box);

    (void) new QLabel (i18n("Steps"), box);

    spinbox = new QSpinBox(box);
    spinbox->setValue (10);
    spinbox->setRange (0, 1000);
}

int BlendDialog::getNumOfSteps () {

    BlendDialog dialog (0L, "Blend");

    if (dialog.exec () == Accepted)
        return dialog.spinbox->value ();
    else
        return 0;
}

#include <BlendDialog.moc>
