/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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

#include "kptmilestone.h"
#include "kptmilestonedialog.h"

#include <kdebug.h>

KPTMilestone::KPTMilestone() : KPTNode() {
}

KPTMilestone::~KPTMilestone() {
}

bool KPTMilestone::load(QDomElement &element) {
    return false;
}

void KPTMilestone::save(QDomElement &element) const {
    return;
}

KPTDuration *KPTMilestone::getStartTime() {
    return 0L;
}

KPTDuration *KPTMilestone::getFloat() {
    return 0L;
}


bool KPTMilestone::openDialog() {
    kdDebug()<<k_funcinfo<<endl;
    KPTMilestoneDialog *dialog = new KPTMilestoneDialog(*this);
    bool ret = dialog->exec();
    delete dialog;
    return ret;
}
