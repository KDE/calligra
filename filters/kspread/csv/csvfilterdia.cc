/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#include <csvfilterdia.h>
#ifndef USE_QFD
#include <csvfilterdia.moc>

CSVFilterDia::CSVFilterDia(QWidget *parent, QString name) :
                           KoFilterDialog(parent, name) {

    QBoxLayout *ml=new QVBoxLayout(this, 10);
    ml->addStretch(5);
    box=new QVButtonGroup(i18n("CSV Delimiter"),this);
    ml->addWidget(box, 3);
    QBoxLayout *bl=new QVBoxLayout(box, 5);
    r1=new QRadioButton(i18n("semi-colon (;)"), box);
    bl->addStretch(5);
    bl->addWidget(r1, 3);
    bl->addStretch(5);
    r2=new QRadioButton(i18n("comma (,)"), box);
    bl->addWidget(r2, 3);
    bl->addStretch(5);
    r3=new QRadioButton(i18n("tabulator"), box);
    bl->addWidget(r3, 3);
    bl->addStretch(5);
    box->setExclusive(true);
    box->setButton(0);
    bl->activate();
    ml->addStretch(5);
    ml->activate();
}

const QString CSVFilterDia::state() {

    if(r1==box->selected())
        return QString(QChar(';'));
    else if(r2==box->selected())
        return QString(QChar(','));
    else
        return QString(QChar('\t'));  // Is that the only possibility? (Werner)
}
#endif
