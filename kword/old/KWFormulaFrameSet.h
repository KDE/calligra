/* This file is part of the KDE project
   Copyright (C) 2003 Ulrich Kuettler <ulrich.kuettler@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWFORMULAFRAME_H
#define KWFORMULAFRAME_H

#include "KWFrameSet.h"
#include "KWFrameSetEdit.h"
//Added by qt3to4:
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>

namespace KFormula {
    class FormulaCursor;
    class Container;
    class View;
}

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

// needed for signals & slots ;(
using KFormula::Container;
using KFormula::FormulaCursor;
using KFormula::View;

class KWFormulaFrameSetEdit;

class KWFormulaFrameSet : public KWFrameSet {
    Q_OBJECT
public:
    KWFormulaFrameSet( KWDocument *doc, const QString & name ) : KWFrameSet(doc) {};
    virtual ~KWFormulaFrameSet();

    QDomElement save( QDomElement &parentElem, bool saveFrames = true ) {};
    void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const {};
    void setProtectContent ( bool protect ) {};
    bool protectContent() const {};
};


class KWFormulaFrameSetEdit : public QObject, public KWFrameSetEdit
{
    Q_OBJECT
public:
    KWFormulaFrameSetEdit(KWFormulaFrameSet* fs, KWCanvas* canvas);
    virtual ~KWFormulaFrameSetEdit();

};

#endif
