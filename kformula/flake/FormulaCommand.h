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

#ifndef FORMULACOMMAND_H
#define FORMULACOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QHash>
#include <QMetaType>

class BasicElement;
class TokenElement;
class FormulaData;
class FormulaCursor;
class GlyphElement;
class RowElement;

class FormulaCommand :  public QUndoCommand {
public:
    FormulaCommand(QUndoCommand* parent=0);
    
    virtual void changeCursor(FormulaCursor* cursor, bool undo) const = 0; 
};

Q_DECLARE_METATYPE(FormulaCommand*)


class FormulaCommandReplaceText : public FormulaCommand {
public:
    FormulaCommandReplaceText( TokenElement* owner, int position,int length, const QString& added , QUndoCommand *parent=0);

    /// Execute the command
    void redo();

    /// Revert the actions done in redo()
    void undo();

    virtual void changeCursor ( FormulaCursor* cursor, bool undo ) const;

private:
    /// The BasicElement that owns the newly added Text
    TokenElement* m_ownerElement;


    /// The position inside m_ownerElement
    int m_position;

    int m_length;

    int m_glyphpos;
    
    /// The list of added elements
    QString m_added;

    QString m_removed;

    QList<GlyphElement*> m_removedGlyphs;
};

class FormulaCommandReplaceElements : public FormulaCommand {
public:
    FormulaCommandReplaceElements( RowElement* owner, int position, int length, QList<BasicElement*> elements , QUndoCommand *parent=0);

    /// Execute the command
    void redo();

    /// Revert the actions done in redo()
    void undo();

    virtual void changeCursor ( FormulaCursor* cursor, bool undo ) const;

private:
    /// The BasicElement that owns the newly added Text
    RowElement* m_ownerElement;

    /// The position inside m_ownerElement
    int m_position;

    int m_length;

    /// The list of added elements
    QList<BasicElement*> m_added;

    /// The list of removed elements
    QList<BasicElement*> m_removed;

};

class FormulaCommandReplaceSingleElement : public FormulaCommand {
public:
    FormulaCommandReplaceSingleElement( BasicElement* owner, BasicElement* oldel, BasicElement* newel, QUndoCommand *parent=0);

    /// Execute the command
    void redo();

    /// Revert the actions done in redo()
    void undo();

    virtual void changeCursor ( FormulaCursor* cursor, bool undo ) const;

private:
    /// The BasicElement that owns the newly added Text
    BasicElement* m_ownerElement;
    BasicElement* m_newel;
    BasicElement* m_oldel;
};

 
// /**
//  * @short The command for changes of an element's attributes
//  * 
//  * Whenever the user changes the attributes assigned to an element an instance of this
//  * class is created to make it possible to revert the changes. The former attributes
//  * are stored in m_oldAttributes.
//  */
// class FormulaCommandAttribute : public QUndoCommand {
// public:
//     /**
//      * The constructor
//      * @param cursor The FormulaCursor where the elements will be replaced 
//      * @param attributes The list of the old attributes
//      */
//     FormulaCommandAttribute( FormulaCursor* cursor, QHash<QString,QString> attributes );
// 
//     /// Execute the command
//     void redo();
// 
//     /// Revert the actions done in redo()
//     void undo();
//     
// private:
//     /// The BasicElement whose attributes have been changed
//     BasicElement* m_ownerElement;
//     
//     /// All attributes that are set newly
//     QHash<QString,QString> m_attributes;
//     
//     /// All attributes the element had before
//     QHash<QString,QString> m_oldAttributes;
// };

#endif // FORMULACOMMAND_H
