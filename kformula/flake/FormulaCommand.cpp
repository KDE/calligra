/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "FormulaCommand.h"
#include "FormulaCursor.h"
#include <klocale.h> 
#include "TokenElement.h"
#include "BasicElement.h"
#include <kdebug.h>

FormulaCommand::FormulaCommand(QUndoCommand* parent)
              : QUndoCommand(parent)
{
    m_done=false;
}

void FormulaCommand::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
    if (undo) {
        cursor->moveTo(m_undoCursorPosition);
    } else {
        cursor->moveTo(m_redoCursorPosition);
    }
    if (!cursor->isAccepted()) {
        cursor->move(MoveRight);
    }
    cursor->moveToEmpty();
}

void FormulaCommand::setUndoCursorPosition ( const FormulaCursorPosition& position )
{
    m_undoCursorPosition=position;
}

void FormulaCommand::setRedoCursorPosition ( const FormulaCursorPosition& position )
{
    m_redoCursorPosition=position;
}

FormulaCommandReplaceText::FormulaCommandReplaceText( TokenElement* owner, int position, int length, const QString& added , QUndoCommand* parent)
                  : FormulaCommand(parent)
{
    m_ownerElement = owner;
    m_position = position;
    m_added = added;
    m_length = length;
    m_removedGlyphs=m_ownerElement->glyphList(position,length);
    m_removed=m_ownerElement->text().mid(position,length);
    setText( i18n( "Add text to formula" ) );
    setUndoCursorPosition(FormulaCursorPosition(m_ownerElement, m_position+m_removed.length()));
    setRedoCursorPosition(FormulaCursorPosition(m_ownerElement, m_position+m_added.length()));
}

FormulaCommandReplaceText::~FormulaCommandReplaceText()
{
}

void FormulaCommandReplaceText::redo()
{
    m_done=true;
    if (m_length>0) {
        m_glyphpos=m_ownerElement->removeText(m_position,m_length);
    }
    m_ownerElement->insertText(m_position, m_added);
}

void FormulaCommandReplaceText::undo()
{
    m_done=false;
    m_ownerElement->removeText(m_position,m_added.length());
    m_ownerElement->insertText(m_position, m_removed);
    m_ownerElement->insertGlyphs(m_glyphpos,m_removedGlyphs);
}

FormulaCommandReplaceElements::FormulaCommandReplaceElements ( RowElement* owner, int position, int length, QList< BasicElement* > elements, bool wrap,QUndoCommand* parent )
                            : FormulaCommand(parent)
{
    m_ownerElement=owner;
    m_position=position;
    m_added=elements;
    m_length=length;
    m_wrap=wrap;
    m_removed=m_ownerElement->childElements().mid(m_position,m_length);
    //we have to remember to which descendant of m_added the elements got moved
    if (m_wrap) {
        foreach (BasicElement* tmp, m_added) {
            if (m_oldPlaceholder=tmp->emptyDescendant()) {
                break;
            }
        }
        if (m_oldPlaceholder) {
            m_newPlaceholder=new RowElement(m_oldPlaceholder->parentElement());
        }
    } else {
        m_oldPlaceholder=0;
        m_newPlaceholder=0;
    }
    setUndoCursorPosition(FormulaCursorPosition(m_ownerElement,m_position+m_removed.length()));
    setRedoCursorPosition(FormulaCursorPosition(m_ownerElement,m_position+m_added.length()));
}

FormulaCommandReplaceElements::~FormulaCommandReplaceElements()
{
    if (m_done) {
        if (m_oldPlaceholder!=0) {
            delete m_oldPlaceholder;
        } else {
            foreach (BasicElement* tmp, m_removed) {
                delete tmp;
            }
        }
    } else {
        if (m_oldPlaceholder!=0) {
            delete m_newPlaceholder;
        }
        foreach (BasicElement* tmp, m_added) {
            delete tmp;
        }
    }
}

void FormulaCommandReplaceElements::redo()
{
    m_done=true;
    for (int i=0; i<m_length; ++i) {
        m_ownerElement->removeChild(m_removed[i]);
    }
    if (m_oldPlaceholder!=0) {
            foreach (BasicElement *tmp, m_removed) {
                m_newPlaceholder->insertChild(m_newPlaceholder->length(),tmp);
            }
            m_oldPlaceholder->parentElement()->replaceChild(m_oldPlaceholder,m_newPlaceholder);
    }
    for (int i=0; i<m_added.length(); ++i) {
        m_ownerElement->insertChild(m_position+i,m_added[i]);
    }
}

void FormulaCommandReplaceElements::undo()
{
    m_done=false;
    for (int i=0; i<m_added.length(); ++i) {
        m_ownerElement->removeChild(m_added[i]);
    }
    if (m_oldPlaceholder!=0) {
        foreach (BasicElement *tmp, m_removed) {
            m_newPlaceholder->removeChild(tmp);
        }
        m_oldPlaceholder->parentElement()->replaceChild(m_oldPlaceholder,m_newPlaceholder);
    }
    for (int i=0; i<m_length; ++i) {
        m_ownerElement->insertChild(m_position+i,m_removed[i]);
    }
}

FormulaCommandReplaceSingleElement::FormulaCommandReplaceSingleElement ( BasicElement* owner, BasicElement* oldel, BasicElement* newel, QUndoCommand* parent )
                            : FormulaCommand ( parent )
{
    m_ownerElement=owner;
    m_newel=newel;
    m_oldel=oldel;
    setUndoCursorPosition(FormulaCursorPosition(m_ownerElement,m_ownerElement->positionOfChild(m_oldel)));
    setRedoCursorPosition(FormulaCursorPosition(m_ownerElement,m_ownerElement->positionOfChild(m_oldel)));
}


FormulaCommandReplaceSingleElement::~FormulaCommandReplaceSingleElement()
{
    if (m_done) {
    } else {
    }
}

void FormulaCommandReplaceSingleElement::redo()
{
    m_done=true;
    m_ownerElement->replaceChild(m_oldel,m_newel);
    m_oldel->setParentElement(0);
    m_newel->setParentElement(m_ownerElement);
}

void FormulaCommandReplaceSingleElement::undo()
{
    m_done=false;
    m_ownerElement->replaceChild(m_newel,m_oldel);
    m_newel->setParentElement(0);
    m_oldel->setParentElement(m_ownerElement);
}

FormulaCommandWrapSingleElement::FormulaCommandWrapSingleElement ( BasicElement* owner, BasicElement* oldel, BasicElement* newel, BasicElement* oldpar, QUndoCommand* parent )
                            : FormulaCommand ( parent )
{
    m_ownerElement=owner;
    m_newel=newel;
    m_oldel=oldel;
    m_oldpar=oldpar;
    setUndoCursorPosition(FormulaCursorPosition(m_ownerElement,m_ownerElement->positionOfChild(m_oldel)));
    setRedoCursorPosition(FormulaCursorPosition(m_ownerElement,m_ownerElement->positionOfChild(m_oldel)));
}

FormulaCommandWrapSingleElement::~FormulaCommandWrapSingleElement()
{
    if (m_done) {
    } else {
    }
}


void FormulaCommandWrapSingleElement::redo()
{
    m_done=true;
    m_ownerElement->replaceChild(m_oldel,m_newel);
    m_oldel->setParentElement(m_oldpar);
    m_newel->setParentElement(m_ownerElement);
}

void FormulaCommandWrapSingleElement::undo()
{
    m_done=false;
    m_ownerElement->replaceChild(m_newel,m_oldel);
    m_newel->setParentElement(0);
    m_oldel->setParentElement(m_ownerElement);
}

FormulaCommandLoad::FormulaCommandLoad ( FormulaData* data, FormulaElement* newelement, QUndoCommand* parent )
                   : FormulaCommand ( parent)
{
    m_data=data;
    m_newel=newelement;
    m_oldel=data->formulaElement();
    setUndoCursorPosition(FormulaCursorPosition(m_oldel,0));
    setRedoCursorPosition(FormulaCursorPosition(m_newel,0));
}

FormulaCommandLoad::~FormulaCommandLoad()
{
    if (m_done) {
    } else {
    }
}

void FormulaCommandLoad::redo()
{
    m_done=true;
    m_data->setFormulaElement(m_newel);
}

void FormulaCommandLoad::undo()
{
    m_done=false;
    m_data->setFormulaElement(m_oldel);
}

// FormulaCommandAttribute::FormulaCommandAttribute( FormulaCursor* cursor,
//                                                   QHash<QString,QString> attributes )
//                        : QUndoCommand()
// {
//     m_ownerElement = cursor->ownerElement();
//     m_attributes = attributes;
//     m_oldAttributes = m_ownerElement->attributes();
//     QHashIterator<QString, QString> i( m_oldAttributes );
//     while( i.hasNext() )
//     {
//         i.next();
// 	if( !m_attributes.contains( i.key() ) )
//             m_attributes.insert( i.key(), i.value() );
//     }
// 
//     setText( i18n( "Attribute Changed" ) );
// }
// 
// void FormulaCommandAttribute::redo()
// {
//     m_ownerElement->setAttributes( m_attributes );
// }
// 
// void FormulaCommandAttribute::undo()
// {
//     m_ownerElement->setAttributes( m_oldAttributes );
// }
