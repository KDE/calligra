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

#ifndef AlignmentDialog_h
#define AlignmentDialog_h

#include <kdialogbase.h>

#include <AlignCmd.h>
#include <DistributeCmd.h>

class QRadioButton;
class QCheckBox;

class GDocument;
class CommandHistory;

enum AlignmentMode { AMode_Align, AMode_Distribute };

class AlignmentDialog : public KDialogBase {

    Q_OBJECT

public:
    AlignmentDialog (QWidget* parent = 0L, const char* name = 0L);

    HorizAlignment getHorizAlignment ();
    VertAlignment getVertAlignment ();
    AlignmentMode getMode ();
    bool centerToPage ();
    bool snapToGrid ();

    HorizDistribution getHorizDistribution ();
    VertDistribution getVertDistribution ();
    DistributionMode getDistributionMode ();

    static void alignSelection (GDocument* doc, CommandHistory *history);

protected:
    void createAlignmentWidget (QWidget* parent);
    void createDistributionWidget (QWidget* parent);

private:
    QPushButton* halignButton[3];
    QPushButton* valignButton[3];
    QPushButton* hdistButton[4];
    QPushButton* vdistButton[4];
    QCheckBox *gbutton, *cbutton;
    QRadioButton *sbutton, *pbutton;
};

#endif
