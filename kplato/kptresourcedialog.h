/* This file is part of the KDE project
   Copyright (C) 2002 The koffice team <koffice@kde.org>

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

#ifndef KPTRESOURCE_H
#define KPTRESOURCE_H

#include <kdialogbase.h>

class KPTResourceGroup;
class KPTResource;
class KLineEdit;
class QTextEdit;
class QTimeEdit;


class KPTResourceDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTResourceDialog(QPtrList<KPTResourceGroup> &rgList, QWidget *parent=0, const char *name=0);

protected slots:
    void slotOk();

private:
    QPtrList<KPTResourceGroup> &m_rgList;
    KPTResource &m_resource;
    KPTResource &m_resourceGroup;    
    KLineEdit *m_namefield;
    QTimeEdit *m_availableFrom;
    QTimeEdit *m_availableUntil;
};


#endif // KPTRESOURCE_H
