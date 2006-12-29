/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander <zander@kde.org>

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

#ifndef splitcelldia_h
#define splitcelldia_h

#include <kdialog.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3GridLayout>

#include "defs.h"

class Q3GridLayout;
class QLabel;
class QCheckBox;
class QSpinBox;
class KWTablePreview;

class KWSplitCellDia : public KDialog {
    Q_OBJECT

public:
    KWSplitCellDia( QWidget *parent, const char *name, unsigned int numcols, unsigned int numrows);

    unsigned int rows()const { return m_rows; }
    unsigned int columns()const { return m_cols; }

protected:
    QSpinBox *nRows, *nCols;
    KWTablePreview *preview;

    unsigned int m_rows, m_cols;

protected slots:
    void rowsChanged( int );
    void colsChanged( int );
};

#endif


