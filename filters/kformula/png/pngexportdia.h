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

#ifndef PNGEXPORTDIA_H
#define PNGEXPORTDIA_H

class QCheckBox;
class QString;
class KIntNumInput;
class KDoubleNumInput;

namespace KFormula {
    class Container;
    class DocumentWrapper;
}

#include <qdom.h>

#include <kdialog.h>


class PNGExportDia : public KDialog
{
    Q_OBJECT

public:
    PNGExportDia( const QDomDocument &dom, const QString &outFile, QWidget *parent=0L );
    ~PNGExportDia();

public slots:
    void slotOk();

protected slots:

    void widthChanged( int  );
    void heightChanged( int );
    void percentWidthChanged( double );
    void percentHeightChanged( double );

    void proportionalClicked();

private:

    void connectAll();
    void disconnectAll();

    void setupGUI();

    int realWidth;
    int realHeight;

    QString _fileOut;
    QByteArray _arrayOut;

    KFormula::Container* formula;
    KFormula::DocumentWrapper* wrapper;

    QCheckBox* proportional;
    KIntNumInput* widthEdit;
    KIntNumInput* heightEdit;
    KDoubleNumInput* percWidthEdit;
    KDoubleNumInput* percHeightEdit;
};

#endif // PNGEXPORTDIA_H
