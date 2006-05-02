/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <iostream>

#include <QPainter>
//Added by qt3to4:
#include <QMouseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPaintEvent>

#include <kapplication.h>
#include <kdebug.h>

#include <basicelement.h>
#include <formulacursor.h>
#include <formulaelement.h>
#include <kformulacontainer.h>
#include <kformuladocument.h>
#include "kformulawidget.h"


KFormulaWidget::KFormulaWidget(KFormula::Container* doc,
                               QWidget* parent, const char* /*name*/, Qt::WFlags f)
    : QWidget(parent, f | Qt::WNoAutoErase | Qt::WResizeNoErase),
      formulaView(doc)
{
    connect(doc, SIGNAL(formulaChanged(int, int)),
            this, SLOT(slotFormulaChanged(int, int)));
    connect(&formulaView, SIGNAL(cursorChanged(bool, bool)),
            this, SLOT(slotCursorChanged(bool, bool)));

    setFocusPolicy(Qt::StrongFocus);
    setBackgroundRole( QPalette::NoRole );

    QRect rect = doc->boundingRect();
    slotFormulaChanged(rect.width(), rect.height());
}

KFormulaWidget::~KFormulaWidget()
{
}


QPoint KFormulaWidget::getCursorPoint() const
{
    return formulaView.getCursorPoint();
}

void KFormulaWidget::setReadOnly(bool ro)
{
    formulaView.setReadOnly(ro);
}


void KFormulaWidget::paintEvent(QPaintEvent* event)
{
    // Always repaint the buffer. This is not so much more work
    // than it seems to be as each cursor movement requires a repaint.
    QPainter p( &buffer );
    //p.translate( -fr.x(), -fr.y() );
    formulaView.draw( p, event->rect(), palette() );

    QPainter painter;
    painter.begin(this);
    painter.drawPixmap( event->rect().x(), event->rect().y(),
                        buffer, event->rect().x(), event->rect().y(), event->rect().width(), event->rect().height() );
    painter.end();
}

void KFormulaWidget::keyPressEvent(QKeyEvent* event)
{
    formulaView.keyPressEvent(event);
}


void KFormulaWidget::focusInEvent(QFocusEvent* event)
{
    formulaView.focusInEvent(event);
}

void KFormulaWidget::focusOutEvent(QFocusEvent* event)
{
    formulaView.focusOutEvent(event);
}

void KFormulaWidget::mousePressEvent(QMouseEvent* event)
{
    formulaView.mousePressEvent(event);
}

void KFormulaWidget::mouseReleaseEvent(QMouseEvent* event)
{
    formulaView.mouseReleaseEvent(event);
}

void KFormulaWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    formulaView.mouseDoubleClickEvent(event);
}

void KFormulaWidget::mouseMoveEvent(QMouseEvent* event)
{
    formulaView.mouseMoveEvent(event);
}

void KFormulaWidget::slotFormulaChanged(int width, int height)
{
    // Magic numbers just to see the cursor.
    resize(width + 5, height + 5);
    buffer = QPixmap( width + 5, height + 5 );
    update();
    //kDebug( 40000 ) << "KFormulaWidget::slotFormulaChanged" << endl;
}

/**
 * The document we show.
 */
KFormula::Container* KFormulaWidget::getDocument()
{
    return formulaView.getDocument();
}

/**
 * Our cursor.
 */
KFormula::FormulaCursor* KFormulaWidget::getCursor()
{
    return formulaView.getCursor();
}


void KFormulaWidget::slotSelectAll()
{
    formulaView.slotSelectAll();
}

void KFormulaWidget::slotCursorChanged(bool visible, bool selecting)
{
    emit cursorChanged(visible, selecting);
    update();
}

#include "kformulawidget.moc"
