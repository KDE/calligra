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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>

#include "formulacursor.h"
#include "kformulacontainer.h"
#include "kformulawidget.h"


KFormulaWidget::KFormulaWidget(KFormulaContainer* doc)
    : QWidget(), document(doc)
{
    cursor = document->createCursor();

    connect(document, SIGNAL(formulaChanged()), this, SLOT(formulaChanged()));
}

KFormulaWidget::~KFormulaWidget()
{
    document->destroyCursor(cursor);
}


void KFormulaWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    document->draw(painter);
    cursor->draw(painter);
    painter.end();
}

void KFormulaWidget::keyPressEvent(QKeyEvent* event)
{
    QPainter painter;
    painter.begin(this);
    cursor->draw(painter);
    document->keyPressEvent(cursor, event);
    cursor->draw(painter);
    painter.end();
}


void KFormulaWidget::formulaChanged()
{
    update();
}
