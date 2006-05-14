/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef EXPORTSIZEDIA_H
#define EXPORTSIZEDIA_H


class QCheckBox;
class QString;
class KIntNumInput;
class KDoubleNumInput;


#include <kdialog.h>


class ExportSizeDia : public KDialog
{
    Q_OBJECT

public:
    ExportSizeDia( int width, int height, 
		  QWidget *parent=0L);
    ~ExportSizeDia();

    int  width() const;
    int  height() const;

#if 0
public slots:
    void slotOk();
#endif

protected slots:

    void widthChanged( int  );
    void heightChanged( int );
    void percentWidthChanged( double );
    void percentHeightChanged( double );

    void proportionalClicked();

private:

    void setupGUI();
    void connectAll();
    void disconnectAll();

    int  m_realWidth;
    int  m_realHeight;

    QCheckBox        *m_proportional;
    KIntNumInput     *m_widthEdit;
    KIntNumInput     *m_heightEdit;
    KDoubleNumInput  *m_percWidthEdit;
    KDoubleNumInput  *m_percHeightEdit;
};


#endif // EXPORTSIZEDIA_H

