// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kspread/csv/csvfilterdia.cc

   The old file was copyrighted by
    Copyright (C) 1999 David Faure <faure@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <htmlexportdia.h>
#ifndef USE_QFD
#include <htmlexportdia.moc>

HTMLExportDia::HTMLExportDia(QWidget *parent, const char *name) :
                           KoFilterDialog(parent, name) {

    QBoxLayout *ml=new QVBoxLayout(this);
    box1=new QVButtonGroup(i18n("Document Type"),this);
    ml->addWidget(box1);
    QBoxLayout *bl1=new QVBoxLayout(box1);
    r1=new QRadioButton(i18n("HTML 4.01"), box1);
    bl1->addWidget(r1);
    r2=new QRadioButton(i18n("XHTML 1.0"), box1);
    bl1->addWidget(r2);
    box1->setExclusive(true);
    box1->setButton(0);
    bl1->activate();
//    ml->addStretch(5);
    box2=new QVButtonGroup(i18n("Flavor"),this);
    ml->addWidget(box2);
    QBoxLayout *bl2=new QVBoxLayout(box2);
    ra1=new QRadioButton(i18n("Spartan (Only document structure, no formatting!)"), box2);
    bl2->addWidget(ra1);
    ra2=new QRadioButton(i18n("Direct (Formatting coded directly, no style sheets)"), box2);
    bl2->addWidget(ra2);
    box2->setExclusive(true);
    box2->setButton(1);
    bl2->activate();
    ml->addStretch(5);
    ml->activate();
}

QString HTMLExportDia::state()
{
    QString result;

    if(r1==box1->selected())
        result += "HTML";
    else if(r2==box1->selected())
        result += "XHTML";
    else
        result += "HTML";

    result += '-';

    if(ra1==box2->selected())
        result += "SPARTAN";
    else if(ra2==box2->selected())
        result += "TRANSITIONAL";
    else
        result += "TRANSITIONAL"; // TODO: best is CSS2 in fact!

    return result;
}
#endif
