/* This file is part of the KDE project
   Copyright (C) 2002 koffice project koffice@kde.org

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

#ifndef KPTMILESTONEDIALOG_H
#define KPTMILESTONEDIALOG_H

#include <kdialogbase.h>

class KPTMilestone;
class KLineEdit;
class QTextEdit;


class KPTMilestoneDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTMilestoneDialog(KPTMilestone &ms, QWidget *parent=0, const char *name=0);

protected slots:
    void slotOk();

private:
    KPTMilestone &m_ms;
    KLineEdit *m_namefield;
    QTextEdit *m_descriptionfield;
};


#endif // KPTMILESTONEDIALOG_H
