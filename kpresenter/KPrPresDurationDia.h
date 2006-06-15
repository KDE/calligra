// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef presdurationdia_h
#define presdurationdia_h

#include <kdialog.h>
//Added by qt3to4:
#include <QLabel>

class KPrDocument;
class K3ListView;
class QLabel;

class KPrPresDurationDia : public KDialog
{
    Q_OBJECT

public:
    KPrPresDurationDia( QWidget *parent, const char *name,
                       KPrDocument *_doc,
                       QStringList _durationListString,
                       const QString &_durationString );

protected:
    void setupSlideList( QWidget *_page );

private:
    KPrDocument *doc;
    K3ListView *slides;
    QLabel *label;

    QStringList m_durationListString;
    QString m_durationString;

protected slots:
    void slotCloseDialog() { emit presDurationDiaClosed(); }

signals:
    void presDurationDiaClosed();

};

#endif
