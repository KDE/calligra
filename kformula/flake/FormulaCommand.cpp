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
}


void FormulaCommand::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
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
}


void FormulaCommandReplaceText::redo()
{
    if (m_length>0) {
        m_glyphpos=m_ownerElement->removeText(m_position,m_length);
    }
    m_ownerElement->insertText(m_position, m_added);
}


void FormulaCommandReplaceText::undo()
{
    m_ownerElement->removeText(m_position,m_added.length());
    m_ownerElement->insertText(m_position, m_removed);
    m_ownerElement->insertGlyphs(m_glyphpos,m_removedGlyphs);
}

void FormulaCommandReplaceText::changeCursor ( FormulaCursor* cursor , bool undo) const
{
    cursor->setCurrentElement(m_ownerElement);
    cursor->setSelecting(false);
    if (!undo) {
        cursor->setPosition(m_position+m_added.length());
    } else {
        cursor->setPosition(m_position+m_removed.length());
    }
    cursor->moveToEmpty();
}

FormulaCommandReplaceElements::FormulaCommandReplaceElements ( RowElement* owner, int position, int length, QList< BasicElement* > elements, QUndoCommand* parent )
                            : FormulaCommand(parent)
{
    m_ownerElement=owner;
    m_position=position;
    m_added=elements;
    m_length=length;
    m_removed=m_ownerElement->childElements().mid(m_position,m_length);
}

void FormulaCommandReplaceElements::redo()
{
    for (int i=0; i<m_length; ++i) {
        m_ownerElement->removeChild(m_removed[i]);
        m_removed[i]->setParentElement(0);
    }
    for (int i=0; i<m_added.length(); ++i) {
        m_ownerElement->insertChild(m_position+i,m_added[i]);
        m_added[i]->setParentElement(m_ownerElement);
    }
}

void FormulaCommandReplaceElements::undo()
{

    for (int i=0; i<m_added.length(); ++i) {
        m_ownerElement->removeChild(m_added[i]);
        m_added[i]->setParentElement(0);
    }
    for (int i=0; i<m_length; ++i) {
        m_ownerElement->insertChild(m_position+i,m_removed[i]);
        m_removed[i]->setParentElement(m_ownerElement);
    }
}

void FormulaCommandReplaceElements::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
    cursor->setCurrentElement(m_ownerElement);
    cursor->setSelecting(false);
    if (!undo) {
        cursor->setPosition(m_position+m_added.length());
    } else {
        cursor->setPosition(m_position+m_removed.length());
    }
    cursor->moveToEmpty();

}

FormulaCommandReplaceSingleElement::FormulaCommandReplaceSingleElement ( BasicElement* owner, BasicElement* oldel, BasicElement* newel, QUndoCommand* parent )
                            : FormulaCommand ( parent )
{
    m_ownerElement=owner;
    m_newel=newel;
    m_oldel=oldel;
}

void FormulaCommandReplaceSingleElement::redo()
{
    m_ownerElement->replaceChild(m_oldel,m_newel);
    m_oldel->setParentElement(0);
    m_newel->setParentElement(m_ownerElement);
}

void FormulaCommandReplaceSingleElement::undo()
{
    m_ownerElement->replaceChild(m_newel,m_oldel);
    m_newel->setParentElement(0);
    m_oldel->setParentElement(m_ownerElement);
}

void FormulaCommandReplaceSingleElement::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
    cursor->setCurrentElement(m_ownerElement);
    cursor->setSelecting(false);
    if (undo) {
        cursor->setPosition(m_ownerElement->positionOfChild(m_oldel));
    } else {
        cursor->setPosition(m_ownerElement->positionOfChild(m_newel));
    }
    cursor->moveToEmpty();
}

FormulaCommandWrapSingleElement::FormulaCommandWrapSingleElement ( BasicElement* owner, BasicElement* oldel, BasicElement* newel, BasicElement* oldpar, QUndoCommand* parent )
                            : FormulaCommand ( parent )
{
    m_ownerElement=owner;
    m_newel=newel;
    m_oldel=oldel;
    m_oldpar=oldpar;
}

void FormulaCommandWrapSingleElement::redo()
{
    m_ownerElement->replaceChild(m_oldel,m_newel);
    m_oldel->setParentElement(m_oldpar);
    m_newel->setParentElement(m_ownerElement);
}

void FormulaCommandWrapSingleElement::undo()
{
    m_ownerElement->replaceChild(m_newel,m_oldel);
    m_newel->setParentElement(0);
    m_oldel->setParentElement(m_ownerElement);
}

void FormulaCommandWrapSingleElement::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
    cursor->setCurrentElement(m_ownerElement);
    cursor->setSelecting(false);
    if (undo) {
        cursor->setPosition(m_ownerElement->positionOfChild(m_oldel));
    } else {
        cursor->setPosition(m_ownerElement->positionOfChild(m_newel));
    }
    cursor->moveToEmpty();
}

FormulaCommandLoad::FormulaCommandLoad ( FormulaData* data, FormulaElement* newelement, QUndoCommand* parent )
                   : FormulaCommand ( parent)
{
    m_data=data;
    m_newel=newelement;
    m_oldel=data->formulaElement();
}

void FormulaCommandLoad::redo()
{
    m_data->setFormulaElement(m_newel);
}

void FormulaCommandLoad::undo()
{
    m_data->setFormulaElement(m_oldel);
}


void FormulaCommandLoad::changeCursor ( FormulaCursor* cursor, bool undo ) const
{
    cursor->setCurrentElement(m_data->formulaElement());
    cursor->setSelecting(false);
    cursor->setPosition(0);
    if (!cursor->isAccepted()) {
        cursor->move(MoveRight);
    }
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
