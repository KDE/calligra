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

#ifndef CSVFILTERDIA_H
#define CSVFILTERDIA_H

#ifndef USE_QFD

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>

#include <klocale.h>
#include <koFilterDialog.h>

class CSVFilterDia : public KoFilterDialog {

    Q_OBJECT

public:
    CSVFilterDia(QWidget *parent=0L, QString name=QString::null);
    virtual ~CSVFilterDia() {}

    virtual const QString state();

private:
    QVButtonGroup *box;
    QRadioButton *r1, *r2;
};
#endif
#endif // CSVFILTERDIA_H
