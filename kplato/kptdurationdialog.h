/* This file is part of the KDE project
   Copyright (C) 2003 Thomas Zander <zander@kde.org>

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

#ifndef KPTDURATIONDIALOG_H
#define KPTDURATIONDIALOG_H

#include "kptduration.h"
#include "durationpanelbase.h"
#include <kdialogbase.h>
class KPTDurationPanel;

class KPTDurationDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTDurationDialog (QWidget *parent, KPTDuration *d);

protected slots:
    // things to handle GUI changes

private:
    KPTDuration *duration;
    KPTDurationPanel *dia;
};

class KPTDurationPanel : public DurationPanelBase {
    Q_OBJECT
public:
    KPTDurationPanel (QWidget *parent, KPTDuration *d);

protected slots:
    // things to handle GUI changes
signals:
    // events we send when the object has changed.

private:
    KPTDuration *duration;
};

#endif // KPTDURATIONDIALOG_H
