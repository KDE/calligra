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

#include <AlignmentDialog.h>

#include <qpushbutton.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <GDocument.h>
#include <GObject.h>
#include <CommandHistory.h>


AlignmentDialog::AlignmentDialog (QWidget* parent, const char* name) :
    KDialogBase(KDialogBase::Tabbed, i18n("Alignment"),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                parent, name, true) {
        createAlignmentWidget(addPage(i18n("Align")));
        createDistributionWidget(addPage(i18n("Distribute")));
}

void AlignmentDialog::createAlignmentWidget (QWidget* parent) {

    QGridLayout *layout = new QGridLayout(parent, 2, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QButtonGroup *group = new QVButtonGroup(i18n ("Vertically"), parent, "Vertically");
    group->setExclusive(true);
    layout->addMultiCellWidget (group, 0, 1, 0, 0);

    valignButton[0] = new QPushButton (group);
    valignButton[0]->setToggleButton (true);
    valignButton[0]->setPixmap (SmallIcon ("atop"));

    valignButton[1] = new QPushButton (group);
    valignButton[1]->setToggleButton (true);
    valignButton[1]->setPixmap (SmallIcon ("avcenter"));

    valignButton[2] = new QPushButton (group);
    valignButton[2]->setToggleButton (true);
    valignButton[2]->setPixmap (SmallIcon ("abottom"));

    group = new QHButtonGroup (i18n ("Horizontally"), parent, "Horizontally");
    group->setExclusive (true);
    layout->addWidget (group, 0, 1);

    halignButton[0] = new QPushButton (group);
    halignButton[0]->setToggleButton (true);
    halignButton[0]->setPixmap (SmallIcon ("aleft"));

    halignButton[1] = new QPushButton (group);
    halignButton[1]->setToggleButton (true);
    halignButton[1]->setPixmap (SmallIcon ("ahcenter"));

    halignButton[2] = new QPushButton (group);
    halignButton[2]->setToggleButton (true);
    halignButton[2]->setPixmap (SmallIcon ("aright"));

    QGroupBox *box = new QVGroupBox (parent);
    layout->addWidget(box, 1, 1);

    gbutton = new QCheckBox(i18n ("To Grid"), box);
    cbutton = new QCheckBox (i18n ("Align To Center of Page"), box);
}

void AlignmentDialog::createDistributionWidget (QWidget* parent) {

    QGridLayout *layout = new QGridLayout (parent, 2, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QButtonGroup *group = new QVButtonGroup(i18n ("Vertically"), parent, "Vertically");
    group->setExclusive(true);
    layout->addMultiCellWidget(group, 0, 1, 0, 0);

    vdistButton[0] = new QPushButton (group);
    vdistButton[0]->setToggleButton (true);
    vdistButton[0]->setPixmap (SmallIcon ("dtop"));

    vdistButton[1] = new QPushButton (group);
    vdistButton[1]->setToggleButton (true);
    vdistButton[1]->setPixmap (SmallIcon ("dvcenter"));

    vdistButton[2] = new QPushButton (group);
    vdistButton[2]->setToggleButton (true);
    vdistButton[2]->setPixmap (SmallIcon ("dvdist"));

    vdistButton[3] = new QPushButton (group);
    vdistButton[3]->setToggleButton (true);
    vdistButton[3]->setPixmap (SmallIcon ("dbottom"));

    group = new QHButtonGroup (i18n ("Horizontally"), parent, "Horizontally");
    group->setExclusive (true);
    layout->addWidget(group, 0, 1);

    hdistButton[0] = new QPushButton (group);
    hdistButton[0]->setToggleButton (true);
    hdistButton[0]->setPixmap (SmallIcon ("dleft"));

    hdistButton[1] = new QPushButton (group);
    hdistButton[1]->setToggleButton (true);
    hdistButton[1]->setPixmap (SmallIcon ("dhcenter"));

    hdistButton[2] = new QPushButton (group);
    hdistButton[2]->setToggleButton (true);
    hdistButton[2]->setPixmap (SmallIcon ("dhdist"));

    hdistButton[3] = new QPushButton (group);
    hdistButton[3]->setToggleButton (true);
    hdistButton[3]->setPixmap (SmallIcon ("dright"));

    group = new QVButtonGroup(i18n ("Distribute at"), parent);
    layout->addWidget(group, 1, 1);

    sbutton = new QRadioButton (i18n ("Selection"), group);
    sbutton->setChecked (true);
    pbutton = new QRadioButton (i18n ("Page"), group);
}

HorizAlignment AlignmentDialog::getHorizAlignment () {

    HorizAlignment result = HAlign_None;

    if (halignButton[0]->isOn ())
        result = HAlign_Left;
    else if (halignButton[1]->isOn ())
        result = HAlign_Center;
    else if (halignButton[2]->isOn ())
        result = HAlign_Right;

    return result;
}

VertAlignment AlignmentDialog::getVertAlignment () {

    VertAlignment result = VAlign_None;

    if (valignButton[0]->isOn ())
        result = VAlign_Top;
    else if (valignButton[1]->isOn ())
        result = VAlign_Center;
    else if (valignButton[2]->isOn ())
        result = VAlign_Bottom;

    return result;
}

bool AlignmentDialog::centerToPage () {
    return cbutton->isOn ();
}

bool AlignmentDialog::snapToGrid () {
    return gbutton->isOn ();
}

HorizDistribution AlignmentDialog::getHorizDistribution () {

    HorizDistribution result = HDistrib_None;

    if (hdistButton[0]->isOn ())
        result = HDistrib_Left;
    if (hdistButton[1]->isOn ())
        result = HDistrib_Center;
    if (hdistButton[2]->isOn ())
        result = HDistrib_Distance;
    if (hdistButton[3]->isOn ())
        result = HDistrib_Right;

    return result;
}

VertDistribution AlignmentDialog::getVertDistribution () {

    VertDistribution result = VDistrib_None;

    if (vdistButton[0]->isOn ())
        result = VDistrib_Top;
    if (vdistButton[1]->isOn ())
        result = VDistrib_Center;
    if (vdistButton[2]->isOn ())
        result = VDistrib_Distance;
    if (vdistButton[3]->isOn ())
        result = VDistrib_Bottom;

    return result;
}

DistributionMode AlignmentDialog::getDistributionMode () {
    return (sbutton->isOn () ? DMode_AtSelection : DMode_AtPage);
}

AlignmentMode AlignmentDialog::getMode () {
    return (activePageIndex()==0 ? AMode_Align : AMode_Distribute);
}

void AlignmentDialog::alignSelection (GDocument* doc,
                                      CommandHistory* history) {
    if (! doc->selectionIsEmpty ()) {
        AlignmentDialog dialog (0L, "Alignment");

        if (dialog.exec() == Accepted) {
            if (dialog.getMode () == AMode_Align) {
                AlignCmd *cmd = new AlignCmd (doc, dialog.getHorizAlignment (),
                                              dialog.getVertAlignment (),
                                              dialog.centerToPage (),
                                              dialog.snapToGrid ());
                history->addCommand (cmd, true);
            }
            else {
                DistributeCmd *cmd =
                    new DistributeCmd (doc, dialog.getHorizDistribution (),
                                       dialog.getVertDistribution (),
                                       dialog.getDistributionMode ());
                history->addCommand (cmd, true);
            }
        }
    }
}

#include <AlignmentDialog.moc>
