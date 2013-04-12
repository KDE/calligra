/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

// Own
#include "FormulaTool.h"

// Qt
#include <QToolButton>
#include <QGridLayout>
#include <QTextEdit>

// KDE
#include <KLocale>
#include <KUrl>
#include <KFileDialog>
#include <KIO/Job>

// Calligra
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoIcon.h>

// This shape
#include "FormulaShape.h"
#include "FormulaCursor.h"
//#include "ChangeFormulaCommand.h"


FormulaTool::FormulaTool( KoCanvasBase* canvas )
    : KoToolBase( canvas ),
      m_formulaShape(0)
    , m_textEdit(0)
    , m_cursor(0)
{

}

void FormulaTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_formulaShape = dynamic_cast<FormulaShape*>( shape );
        if (m_formulaShape)
            break;
    }

    if (!m_formulaShape) {
        emit done();
        return;
    }

    delete m_cursor;
    m_cursor = new FormulaCursor(m_formulaShape->document());
    m_formulaShape->update();
}

void FormulaTool::deactivate()
{
    m_formulaShape = 0;
    delete m_cursor;
    m_cursor = 0;
}

QWidget * FormulaTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(optionWidget);

    QToolButton *button = 0;

    m_textEdit = new QTextEdit(optionWidget);
    m_textEdit->setText(m_formulaShape->MML());
    button = new QToolButton(optionWidget);
    //button->setIcon(koIcon("document-open"));
    button->setText(i18n("Insert"));
    button->setToolTip(i18n( "Insert"));
    layout->addWidget(button, 1, 0);
    layout->addWidget(m_textEdit, 0, 0);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(insertPressed()));


    return optionWidget;
}


// ----------------------------------------------------------------
//                         Private slots


void FormulaTool::insertPressed()
{
    if (m_formulaShape == 0)
        return;

    m_formulaShape->setMML(m_textEdit->toPlainText());
}

// ----------------------------------------------------------------
//                         Event handling


void FormulaTool::mousePressEvent(KoPointerEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->ignore();
    }
}

void FormulaTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
}

void FormulaTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_formulaShape == 0) {
        return;
    }
    
    m_formulaShape->update();
}

void FormulaTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace) {
        
    } else if (event->key() == Qt::Key_Delete) {
        if(m_cursor) {
            m_cursor->deleteNode();
        }
    } else if ((event->key() == Qt::Key_Left) && (event->modifiers() & Qt::ControlModifier) == 0) {
        if(m_cursor) {
            m_cursor->previousNode();
        }
    } else if ((event->key() == Qt::Key_Right) && (event->modifiers() & Qt::ControlModifier) == 0) {
        if(m_cursor) {
            m_cursor->nextNode();
	    }
    } else if ((event->key() == Qt::Key_Up) && (event->modifiers() & Qt::ControlModifier) == 0) {
        
    } else if ((event->key() == Qt::Key_Down) && (event->modifiers() & Qt::ControlModifier) == 0) {
        
    } else {
      
    }
 }

#include <FormulaTool.moc>
