/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2005 Thomas Zander <zander@kde.org>

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

#ifndef resizetabledia_h
#define resizetabledia_h

#include <kdialog.h>

class KWTableFrameSet;
class KWDocument;
class QWidget;
class QSpinBox;
class KoUnitDoubleSpinBox;

/**
 * A dialog that can be used to resize columns of a table using numeric input.
 */
class KWResizeTableDia : public KDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent parent widget for dialog
     * @param table the table to do the resizes on
     * @param doc the document this table belongs to, for undo information
     * @param resizeColumn the initial column that the user can resize.
     */
    KWResizeTableDia( QWidget *parent, KWTableFrameSet *table, KWDocument *doc, int resizeColumn);

protected:
    void setupTab1( int resizeColumn);
    bool doResize();
    QSpinBox *m_value;
    KWTableFrameSet *m_table;
    KWDocument *m_doc;
    KoUnitDoubleSpinBox *m_position;
    double m_resetValue;
protected slots:
    virtual void slotOk();
    virtual void slotApply();
    virtual void slotUser1();
    void slotValueChanged( int pos);
};

#endif


