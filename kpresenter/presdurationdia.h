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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef presdurationdia_h
#define presdurationdia_h

#include <kdialogbase.h>

class KPresenterDoc;
class KListView;
class QLabel;

/******************************************************************
 *
 * Class: KPPresDurationDia
 *
 ******************************************************************/

class KPPresDurationDia : public KDialogBase
{
    Q_OBJECT

public:
    KPPresDurationDia( QWidget *parent, const char *name,
                       KPresenterDoc *_doc,
                       QStringList _durationListString,
		       const QString &_durationString );

protected:
    void setupSlideList( QWidget *_page );

private:
    KPresenterDoc *doc;
    KListView *slides;
    QLabel *label;

    QStringList m_durationListString;
    QString m_durationString;

protected slots:
    void slotCloseDialog() { emit presDurationDiaClosed(); }

signals:
    void presDurationDiaClosed();

};

#endif
