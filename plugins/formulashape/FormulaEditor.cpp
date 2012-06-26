/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

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

#include "FormulaEditor.h"
#include "BasicElement.h"
#include "RowElement.h"
#include "FixedElement.h"
#include "NumberElement.h"
#include "TableElement.h"
#include "TableDataElement.h"
#include "TableRowElement.h"
#include "ElementFactory.h"
#include "OperatorElement.h"
#include "IdentifierElement.h"
#include "ElementFactory.h"
#include "FormulaCommand.h"
#include "CollectionElement.h"
#include "FormulaRenderer.h"
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <QObject>

#include <kdebug.h>
#include <klocale.h>
#include <kundo2command.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>

FormulaEditor::FormulaEditor( FormulaCursor cursor, FormulaData* data )
{
    m_cursor=cursor;
    m_data=data;
    m_collector=NULL;
}


FormulaEditor::FormulaEditor ( FormulaData* data )
{
    m_cursor=FormulaCursor(data->formulaElement(),0);
    m_data=data;
    m_collector=NULL;
}



void FormulaEditor::paint( QPainter& painter ) const
{
    m_cursor.paint(painter);
}

FormulaCommand* FormulaEditor::insertText( const QString& text )
{
    FormulaCommand *undo = 0;
    m_inputBuffer = text;


    qDebug("before if");

    if(insideCollectionElement())
    {
        qDebug("inside if");
        if(m_collector->addCharacter(text))
        {
           // m_collector->group().chop(1);
            qDebug("after space");
             if(m_collector->group().toInt())                         //when space
             {
                 //NumberElement* number = static_cast<NumberElement*>(ElementFactory::createElement(tokenType(m_collector->group().at(0)),0));
               //  NumberElement* number = new NumberElement();

                 NumberElement* number = static_cast<NumberElement*>
                             (ElementFactory::createElement(tokenType(m_collector->group().at(0)),0));
                 qDebug("ok till here");
                 qDebug(tokenType(m_collector->group().at(0)).toAscii().data());
                 qDebug("ok till here1");

                 if(number->insertText(0,m_collector->group()))
                     qDebug("true");
                 qDebug("ok till here2");
                 undo=insertElement(number);	//fails !
                 if(undo==NULL)
                 qDebug("ok till here3");
                 if (undo) {
                     undo->setRedoCursorPosition(FormulaCursor(number,m_collector->group().length()));
                     qDebug("ok till here4");
                 }
                 qDebug("ok till here5");
                 delete(m_collector);
                 qDebug("ok till here6");




             }

             else if(tokenType(m_collector->group().at(0))=="mo")
             {
                 OperatorElement* opt = static_cast<OperatorElement*>(ElementFactory::createElement(tokenType(m_collector->group().at(0)),0));
                 opt->insertText(0,m_collector->group());
                 undo=insertElement(opt);
                 if (undo) {
                     undo->setRedoCursorPosition(FormulaCursor(opt,m_collector->group().length()));
                 }
                 delete(m_collector);
             }
             else
             {
                 IdentifierElement* id = static_cast<IdentifierElement*>(ElementFactory::createElement(tokenType(m_collector->group().at(0)),0));
                 id->insertText(0,m_collector->group());
                 undo=insertElement(id);
                 if (undo) {
                     undo->setRedoCursorPosition(FormulaCursor(id,m_collector->group().length()));
                 }
                 delete(m_collector);
             }


        }

        else
        {
            TokenElement* token=static_cast<TokenElement*>(m_cursor.currentElement());
            undo = new FormulaCommandReplaceText(token,m_cursor.position(),0,text);
       //    m_collector->insertText(0,text);
        //  undo = insertElement(m_collector);
         // if (undo) {
           //           undo->setRedoCursorPosition(FormulaCursor(m_collector,text.length()));
             //     }                                                          //display needed

        }

    }

    else{

        qDebug("in else");
        m_collector=new CollectionElement();
        m_collector->addCharacter(text);


        qDebug("Before undo");
//        undo = new FormulaCommandReplaceText(m_collector,m_cursor.position(),0,text);
        qDebug("After undo");
         undo = new FormulaCommandReplaceText(m_collector,m_cursor.position(),0,text);
        m_collector->insertText(0,text);
       undo = insertElement(m_collector);
        qDebug("after insert");
       // undo=insertElement(m_collecundo = new FormulaCommandReplaceText(m_collector,m_cursor.position(),0,text);tor);
        qDebug("not here");
   //     formulaData()->notifyDataChange(undo,0);
        qDebug("after notify");
        if (undo) {
                    undo->setRedoCursorPosition(FormulaCursor(m_collector,text.length()));
                }


    }

   /* else
    {
        m_collector=new CollectionElement();
         qDebug("here1");
         m_collector->addCharacter(text);
         TokenElement* token = static_cast<TokenElement*>
                    (ElementFactory::createElement(tokenType(text[0]),0));
         qDebug("here2");
         token->insertText(0,QString("Please"));
         undo=insertElement(token);
         qDebug("here3");
                 if (undo) {
                     undo->setRedoCursorPosition(FormulaCursor(token,text.length()));
                 }
         qDebug("Else ");

    }

*/
    if (undo) {
        undo->setText(i18nc("(qtundo-format)", "Add text"));
    }
    return undo;


}

FormulaCommand* FormulaEditor::insertMathML( const QString& data )
{
    // setup a DOM structure and start the actual loading process
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( QString(data), false, 0, 0, 0 );
    BasicElement* element=ElementFactory::createElement(tmpDocument.documentElement().tagName(),0);
    element->readMathML( tmpDocument.documentElement() );     // and load the new formula
    FormulaCommand* command=insertElement( element );
    kDebug()<<"Inserting "<< tmpDocument.documentElement().tagName();
    if (command==0) {
        delete element;
    }
    return command;
}

FormulaCommand* FormulaEditor::changeTable ( bool insert, bool rows )
{
    FormulaCommand* undo;
    TableDataElement* data=m_cursor.currentElement()->parentTableData();
    if (data) {
        TableElement* table=static_cast<TableElement*>(data->parentElement()->parentElement());
        int rowNumber=table->childElements().indexOf(data->parentElement());
        int columnNumber=data->parentElement()->childElements().indexOf(data);
        if (rows) {
            //Changing rows
            if (insert) {
                undo=new FormulaCommandReplaceRow(formulaData(),cursor(),table,rowNumber,0,1);
                if (undo) {
                    undo->setText(i18nc("(qtundo-format)", "Insert row"));
                }
            } else {
                undo=new FormulaCommandReplaceRow(formulaData(),cursor(),table,rowNumber,1,0);
                if (undo) {
                    undo->setText(i18nc("(qtundo-format)", "Remove row"));
                }
            }
        } else {
            //Changing columns
            if (insert) {
                undo=new FormulaCommandReplaceColumn(formulaData(),cursor(),table,columnNumber,0,1);
                if (undo) {
                    undo->setText(i18nc("(qtundo-format)", "Insert column"));
                }
            } else {
                undo=new FormulaCommandReplaceColumn(formulaData(),cursor(),table,columnNumber,1,0);
                if (undo) {
                    undo->setText(i18nc("(qtundo-format)", "Remove column"));
                }
            }
        }
    } else {
        return 0;
    }
    return undo;
}

FormulaCommand* FormulaEditor::insertElement( BasicElement* element )
{
    FormulaCommand *undo = 0;
    if (m_cursor.insideInferredRow()) {
        qDebug("here1");
        RowElement* tmprow=static_cast<RowElement*>(m_cursor.currentElement());
        QList<BasicElement*> list;
        list<<element;
        if (m_cursor.hasSelection()) {
            undo=new FormulaCommandReplaceElements(tmprow,m_cursor.selection().first,m_cursor.selection().second-m_cursor.selection().first,list,true);
        } else {
            qDebug("here2");
            undo=new FormulaCommandReplaceElements(tmprow,m_cursor.position(),0,list,false);
        }
    } else if (m_cursor.insideToken() && element->elementType()==Glyph) {
        //TODO: implement the insertion of glyphs
    }
    if (undo) {
        qDebug("here3");
        undo->setText(i18nc("(qtundo-format)", "Insert formula elements."));
        undo->setUndoCursorPosition(cursor());
    }
    return undo;
}

FormulaCommand* FormulaEditor::remove( bool elementBeforePosition )
{
    FormulaCommand *undo=0;
    if (m_cursor.insideInferredRow()) {
        RowElement* tmprow=static_cast<RowElement*>(m_cursor.currentElement());
        if (m_cursor.isSelecting()) {
            undo=new FormulaCommandReplaceElements(tmprow,m_cursor.selection().first,m_cursor.selection().second-m_cursor.selection().first,QList<BasicElement*>());
        } else {
            if (elementBeforePosition && !m_cursor.isHome()) {
                undo=new FormulaCommandReplaceElements(tmprow,m_cursor.position()-1,1,QList<BasicElement*>());
            } else if (!elementBeforePosition && !m_cursor.isEnd()) {
                undo=new FormulaCommandReplaceElements(tmprow,m_cursor.position(),1,QList<BasicElement*>());
            }
        }
    } else if (m_cursor.insideToken()) {
        TokenElement* tmptoken=static_cast<TokenElement*>(m_cursor.currentElement());
        if (m_cursor.hasSelection()) {
            undo=new FormulaCommandReplaceText(tmptoken,m_cursor.selection().first,m_cursor.selection().second-m_cursor.selection().first,"");
        } else {
            if (elementBeforePosition && !m_cursor.isHome()) {
                undo=new FormulaCommandReplaceText(tmptoken,m_cursor.position()-1,1,"");
            } else if (!elementBeforePosition && !m_cursor.isEnd()) {
                undo=new FormulaCommandReplaceText(tmptoken,m_cursor.position(),1,"");
            }
        }
    }
    if (undo) {
        undo->setText(i18nc("(qtundo-format)", "Remove formula elements"));
        undo->setUndoCursorPosition(cursor());
    }
    return undo;
}

void FormulaEditor::setData ( FormulaData* data )
{
    m_data=data;
}


FormulaData* FormulaEditor::formulaData() const
{
    return m_data;
}

QString FormulaEditor::inputBuffer() const
{
    return m_inputBuffer;
}

QString FormulaEditor::tokenType ( const QChar& character ) const
{
    qDebug("tokentype");
    QChar::Category chat=character.category();
    if (character.isNumber()) {
        return "mn";
    }
    else if (chat==QChar::Punctuation_Connector ||
             chat==QChar::Punctuation_Dash ||
             chat==QChar::Punctuation_Open ||
             chat==QChar::Punctuation_Close ||
             chat==QChar::Punctuation_InitialQuote ||
             chat==QChar::Punctuation_FinalQuote ||
             chat==QChar::Symbol_Math ||
             character.unicode()=='*' ||
             character.unicode()=='/')
    {
        return "mo";
    }
    else if (character.isLetter()) {
        return "mi";
    }
    return "mi";
}


FormulaCursor& FormulaEditor::cursor()
{
    return m_cursor;
}

void FormulaEditor::setCursor ( FormulaCursor& cursor )
{
    m_cursor=cursor;
}

bool FormulaEditor::insideCollectionElement()
{
    return(!m_collector==NULL);

}
