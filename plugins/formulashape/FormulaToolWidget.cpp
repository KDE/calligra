/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>
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

#include "FormulaToolWidget.h"
#include "KoFormulaTool.h"
#include "KoFormulaShape.h"
#include "ElementFactory.h"
#include "BasicElement.h"
#include <QWidgetAction>
#include <QTableWidget>
#include <QTextEdit>
#include <KAction>
#include <QMouseEvent>
#include <KMessageBox>
#include <QMenu>
#include <QLabel>
#include <kdebug.h>
#include <QMessageBox>
#include <KFontComboBox>
#include "FormulaCursor.h"
#include "IdentifierElement.h"
#include "SubSupElement.h"

FormulaToolWidget::FormulaToolWidget( KoFormulaTool* tool, QWidget* parent )
                  : QTabWidget( parent )
{
    m_tool = tool;
    setupUi( this );

  //  qDebug()<<fontComboBox->currentFont().toString();

    connect(fontComboBox,SIGNAL(currentFontChanged(const QFont)),this,SLOT(print(QFont)));
    // setup the element insert menus
    m_fractionMenu.addAction( m_tool->action( "insert_fraction" ) );
    m_fractionMenu.addAction( m_tool->action( "insert_bevelled_fraction" ) );
    
    
    m_fenceMenu.addAction( m_tool->action( "insert_fence" ) );
    m_fenceMenu.addAction( m_tool->action( "insert_enclosed" ) );
    m_fenceMenu.addAction(m_tool->action("insert_parantheses"));
    
    m_tableMenu.addAction( m_tool->action( "insert_22table" ) );
    m_tableMenu.addAction( m_tool->action( "insert_33table" ) );
    m_tableMenu.addAction( m_tool->action( "insert_44table" ) );
    m_tableMenu.addAction( m_tool->action( "insert_21table" ) );

    m_rootMenu.addAction( m_tool->action( "insert_root" ) );
    m_rootMenu.addAction( m_tool->action( "insert_sqrt" ) );

    m_scriptsMenu.addAction( m_tool->action( "insert_subscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_supscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_subsupscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_underscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_overscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_underoverscript" ) );

    m_alterTableMenu.addAction( m_tool->action( "insert_row") );
    m_alterTableMenu.addAction( m_tool->action( "insert_column") );
    m_alterTableMenu.addAction( m_tool->action( "remove_row") );
    m_alterTableMenu.addAction( m_tool->action( "remove_column") );

    // assign menus to toolbuttons
    buttonFence->setMenu( &m_fenceMenu );
    buttonFence->setDefaultAction( m_tool->action( "insert_fence" ) );
    buttonRoot->setMenu( &m_rootMenu );
    buttonRoot->setDefaultAction( m_tool->action( "insert_sqrt" ) );
    buttonFraction->setMenu( &m_fractionMenu );
    buttonFraction->setDefaultAction(m_tool->action("insert_fraction"));
    buttonTable->setMenu( &m_tableMenu );
    buttonTable->setDefaultAction(m_tool->action( "insert_33table"));
    buttonScript->setMenu( &m_scriptsMenu );
    buttonScript->setDefaultAction(m_tool->action( "insert_subscript"));

    buttonAlterTable->setMenu(&m_alterTableMenu);
    buttonAlterTable->setDefaultAction(m_tool->action("insert_row"));
    // setup the buttons for symbol insertion
    buttonArrows->setText(QChar(0x2190));
    setupButton(buttonArrows,m_arrowMenu,i18n("Arrows"), symbolsInRange(0x2190,0x21FF));
    buttonGreek->setText(QChar(0x03B2));
    setupButton(buttonGreek,m_greekMenu,i18n("Greek"), symbolsInRange(0x0391,0x03A1)
                                                     <<symbolsInRange(0x03A3,0x03A9)
                                                     <<symbolsInRange(0x03B1,0x03C9));
    buttonRelation->setText(QChar(0x2265));
    setupButton(buttonRelation,m_relationMenu,i18n("Relations"), symbolsInRange(0x223C,0x2292)
                                                               <<symbolsInRange(0x2AAE,0x2ABA));
    buttonOperators->setText(QChar(0x2211));
    setupButton(buttonOperators,m_operatorMenu,i18n("Operators"), symbolsInRange(0x220F,0x2219)
                                                               <<symbolsInRange(0x2227,0x2233)
                                                               <<symbolsInRange(0x2207,0x2208));
    buttonMisc->setText(QChar(0x211A));
    setupButton(buttonMisc,m_miscMenu,i18n("Miscellaneous"), symbolsInRange(0x2200,0x2205)
                                                                   <<symbolsInRange(0x221F,0x2222));

  //  buttonRow->hide();
    buttonFormula->setText("Formulae");
    buttonFormula->setMenu(&m_formulaMenu);
    buttonFormula->setToolTip("Insert common formulae");


    QList<QString> formulalist;
    formulalist<<QString("<pre>A = &Pi;r<sup>2</sup></pre>");
    formulalist<<QString("<pre>(x+a)<sup>n</sup> = <sup>n</sup><strong>&Sigma;</strong><sub>k=0</sub> <sup>n</sup>C<sub>k</sub> x<sup>k</sup> a<sup>n-k</sup> </pre>");
    formulalist<<QString("<pre>(1+x)<sup>n</sup> = 1 + nx/1! + n(n-1)x<sup>2</sup>/2!+...</pre>");
    formulalist<<QString("<pre>x = -b &plusmn; &radic;(b<sup>2</sup> - 4ac) / 2a</pre>");
    formulalist<<QString("<pre><strong>e<sup>n</sup></strong> = 1 + n/1! + n<sup>2</sup>/2! + n<sup>3</sup>/3! + ... </pre>");
    formulalist<<QString("<pre><strong>e<sup>x</sup></strong> = 1 + x/1! + x<sup>2</sup>/2! + x<sup>3</sup>/3! + ... </pre>");
    formulalist<<QString("<pre><strong>sin</strong>&alpha; &plusmn; <strong>sin</strong>&beta; = 2<strong>sin</strong>(&alpha; &plusmn; &beta;)/2 <strong>cos</strong>(&alpha; &plusmn; &beta;)/2 </pre>");
    formulalist<<QString("<pre><strong>cos</strong>&alpha; + <strong>cos</strong>&beta; = 2 <strong>cos</strong>(&alpha; + &beta;)/2 <strong>cos</strong>(&alpha; - &beta;)/2</pre>");
    formulalist<<QString("<pre><strong>cos</strong>&alpha; - <strong>cos</strong>&beta; = 2 <strong>sin</strong>(&alpha; + &beta;)/2 <strong>sin</strong>(&beta; - &alpha;)/2</pre>");

    m_printFormula<<QString("A = ")+QString(0x03C0)+QString("r")+QString(0x00B2)
                <<QString("(x+a)")+QString(0x207F)+QString(" = 1 + nx/1! + n(n-1)x/2! + ...")
                <<QString("(1+x)")+QString(0x207F)+QString(" = 1 + nx/1! + n(n-1)x")+QString(0x00B2)+QString("/2! + n(n-1)(n-2)x")+QString(0x00B3)+QString("/3! + ...")
                <<QString("x = -b ")+QString(0x00B1)+QString(" ")+QString(0x221A)+QString("(b")+QString(0x00B2)+QString(" - 4ac) / 2a")
                <<QString("e")+QString(0x207F)+QString(" = 1 + n/1! + n")+QString(0x00B2)+QString("/2! + n")+QString(0x00B3)+QString("/3! + ...")
                <<QString("sin")+QString(0x03B1)+QString(" ")+QString(0x00B1)+QString(" sin")+QString(0x03B2)+QString(" = ")+QString("2sin(")+QString(0x03B1)+QString(0x00B1)+QString(0x03B2)+QString(")/2 ")+QString("cos(")+QString(0x03B1)+QString(0x00B1)+QString(0x03B2)+QString(")/2 ")
                <<QString("cos")+QString(0x03B1)+QString(" + cos")+QString(0x03B2)+QString(" = 2cos(")+QString(0x03B1)+QString("+")+QString(0x03B2)+QString(")/2 ")+QString("cos(")+QString(0x03B1)+QString("-")+QString(0x03B2)+QString(")/2 ")
                <<QString("cos")+QString(0x03B1)+QString(" - cos")+QString(0x03B2)+QString(" = 2sin(")+QString(0x03B1)+QString("+")+QString(0x03B2)+QString(")/2 ")+QString("sin(")+QString(0x03B2)+QString("-")+QString(0x03B1)+QString(")/2 ");

    setupformulaButton(formulalist);

    connect( buttonLoad, SIGNAL( clicked() ), m_tool, SLOT( loadFormula() ) );
    connect( buttonSave, SIGNAL( clicked() ), m_tool, SLOT( saveFormula() ) );
    connect( buttonAlterTable, SIGNAL( triggered( QAction* ) ), m_tool, SLOT( changeTable(QAction*)));
}

FormulaToolWidget::~FormulaToolWidget()
{}


void FormulaToolWidget::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}


void FormulaToolWidget::insertSymbol ( QTableWidgetItem* item )
{
    m_tool->insertSymbol(item->text());
}


void FormulaToolWidget::insertFormula (int a,int b)
{
    Q_UNUSED(b);
    m_tool->insertSymbol(m_printFormula.at(a));
    m_formulaMenu.hide();
}

void FormulaToolWidget::insertSymbol (QLabel* label )
{
    m_tool->insertSymbol(label->text());

}


void FormulaToolWidget::setupButton ( QToolButton* button, QMenu& menu, const QString& text, QList<QString> list, int length)
{
    QWidgetAction *widgetaction=new QWidgetAction(button);
    QTableWidget* table= new QTableWidget(list.length()/length,length,button);
    for (int i=0; i<list.length();i++) {
        QTableWidgetItem *newItem = new QTableWidgetItem(list[i]);
        newItem->setFlags(Qt::ItemIsEnabled);
        table->setItem(i/length,i%length, newItem);
    }
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->setShowGrid(true);
    table->setFixedSize(table->horizontalHeader()->length(), table->verticalHeader()->length());
    button->setToolTip(text);
    //TODO: that is a little bit hackish
//     connect( table,SIGNAL( itemActivated(QTableWidgetItem*)),
//              table, SIGNAL( itemClicked(QTableWidgetItem*)));
    connect( table,SIGNAL( itemClicked(QTableWidgetItem*)),
             this, SLOT( insertSymbol(QTableWidgetItem*)));
    connect( table,SIGNAL( itemClicked(QTableWidgetItem*)),
             &menu, SLOT(hide()));
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(&menu);
    
    widgetaction->setDefaultWidget(table);
    menu.addAction(widgetaction);
}

QList< QString > FormulaToolWidget::symbolsInRange ( int first, int last )
{
    QList<QString> list;
    for (int i=first;i<=last;++i) {
        list.append(QChar(i));
    }
    return list;
}

void FormulaToolWidget::setupformulaButton(QList<QString>list)
{
    QWidgetAction *widgetaction=new QWidgetAction(buttonFormula);
    QTableWidget* table= new QTableWidget(list.length(),1,buttonFormula);


    for(int i=0;i<list.length();i++)
     {
       //  txtedit->setText(list[i]);
       //  txtedit->insertHtml(list[i]);
        //txtedit->setReadOnly(true);
       //  txtedit->setContextMenuPolicy(Qt::NoContextMenu);

         table->setCellWidget(i,0,new QLabel(list[i]));

       //  txtedit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


     }
    //table->setItem(0,0, newItem);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->resizeColumnsToContents();
   // table->setFixedSize(table->horizontalHeader()->length(), table->verticalHeader()->length());


    connect( table,SIGNAL(cellPressed(int,int)),
             this, SLOT( insertFormula(int,int)));

    connect( table,SIGNAL(cellClicked(int,int)),
             &m_formulaMenu, SLOT(hide()));


    buttonFormula->setPopupMode(QToolButton::InstantPopup);
    widgetaction->setDefaultWidget(table);
    m_formulaMenu.addAction(widgetaction);

}


KoFormulaTool* FormulaToolWidget:: formulatool()
{
    return m_tool;
}

#include "FormulaToolWidget.moc"
