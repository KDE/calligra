/* This file is part of the KDE project
* Copyright (C) 2010 Ludovic Delfau <ludovicdelfau@gmail.com>
* Copyright (C) 2010 Julien Desgats <julien.desgats@gmail.com>
* Copyright (C) 2010-2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
* Copyright (C) 2010-2011 Benjamin Port <port.benjamin@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (  at your option ) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef KPROUTLINEEDITOR_H
#define KPROUTLINEEDITOR_H

#include <QTextEdit>
#include <KPrViewModeOutline.h>
#include <QString>

class KPrOutlineEditor : public QTextEdit {
    Q_OBJECT;
public:
    KPrOutlineEditor ( KPrViewModeOutline* out, QWidget * parent = 0 );
    KPrOutlineEditor ( KPrViewModeOutline* out, const QString & text, QWidget * parent = 0 );
    virtual ~KPrOutlineEditor();
protected:
    virtual void keyPressEvent(QKeyEvent *event);

    /**
     * Since we want to catch ALL tab key events, we completely disable
     * keyboard focus switching.
     */
    virtual bool focusNextPrevChild(bool next) { Q_UNUSED(next); return false; }
private:
    KPrViewModeOutline *m_outlineView;
};

#endif // KPROUTLINEEDITOR_H
