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
    box=new QVButtonGroup(i18n("Document Type"),this);
    ml->addWidget(box);
    QBoxLayout *bl=new QVBoxLayout(box);
    r1=new QRadioButton(i18n("HTML 4.01"), box);
    bl->addWidget(r1);
    r2=new QRadioButton(i18n("XHTML 1.0"), box);
    bl->addWidget(r2);
    box->setExclusive(true);
    box->setButton(0);
    bl->activate();
    ml->addStretch(5);
    ml->activate();
}

QString HTMLExportDia::state() {

    if(r1==box->selected())
        return "HTML";
    else if(r2==box->selected())
        return "XHTML";
    else
        return "HTML";
}
#endif
