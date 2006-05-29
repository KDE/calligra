/* This file is part of the KDE project
   Copyright (C)  2001,2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include "KWEditPersonnalExpression.h"

#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include <QFile>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <Q3GroupBox>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QDomDocument>

KWEditPersonnalExpression::KWEditPersonnalExpression( QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, i18n("Edit Personal Expression"), Ok|Cancel, Ok, true )
{
    resize( 412, 339 );
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    Q3GridLayout* form1Layout = new Q3GridLayout( page, 1, 1, 0, KDialog::spacingHint(), "form1Layout");

    Q3HBoxLayout *horlayout1 = new Q3HBoxLayout( 0, 0, KDialog::spacingHint(), "horlayout1");

    QLabel *groupnameLabel = new QLabel(i18n("Group name:"), page, "groupnameLabel" );
    horlayout1->addWidget( groupnameLabel );

    m_groupLineEdit = new QLineEdit( page, "groupLineEdit" );
    horlayout1->addWidget( m_groupLineEdit );

    form1Layout->addLayout( horlayout1, 0, 1 );

    Q3GroupBox *expressionGroupBox = new Q3GroupBox( page, "expressionGroupBox" );
    expressionGroupBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 1, 0, expressionGroupBox->sizePolicy().hasHeightForWidth() ) );
    expressionGroupBox->setTitle( i18n( "Expressions" ) );
    expressionGroupBox->setColumnLayout(0, Qt::Vertical );
    expressionGroupBox->layout()->setSpacing( KDialog::spacingHint() );
    expressionGroupBox->layout()->setMargin( KDialog::marginHint() );
    Q3VBoxLayout *expressionGroupBoxLayout = new Q3VBoxLayout( expressionGroupBox->layout() );
    expressionGroupBoxLayout->setAlignment( Qt::AlignTop );

    m_ExpressionsList = new Q3ListBox( expressionGroupBox, "listOfExpressions" );
    m_ExpressionsList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 1, 0, m_ExpressionsList->sizePolicy().hasHeightForWidth() ) );
    expressionGroupBoxLayout->addWidget( m_ExpressionsList );

    m_expressionLineEdit = new QLineEdit( expressionGroupBox, "expressionLineEdit" );
    expressionGroupBoxLayout->addWidget( m_expressionLineEdit );

    Q3HBoxLayout *horlayout2 = new Q3HBoxLayout( 0, 0, KDialog::spacingHint(), "horlayout2");
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    horlayout2->addItem( spacer );

    m_addExpression = new QPushButton( i18n("&New"), expressionGroupBox, "m_addExpression" );
    horlayout2->addWidget( m_addExpression );

    m_delExpression = new QPushButton( i18n("&Delete"), expressionGroupBox, "m_delExpression" );
    horlayout2->addWidget( m_delExpression );
    expressionGroupBoxLayout->addLayout( horlayout2 );

    form1Layout->addWidget( expressionGroupBox, 1, 1 );

    Q3VBoxLayout *vertlayout = new Q3VBoxLayout( 0, 0, KDialog::spacingHint(), "vertlayout");

    m_groupList = new Q3ListBox( page, "groupList" );
    m_groupList->insertItem( "group1"  );
    m_groupList->setMaximumSize( QSize( 200, 32767 ) );
    vertlayout->addWidget( m_groupList );

    Q3HBoxLayout *horlayout3 = new Q3HBoxLayout( 0, 0, KDialog::spacingHint(), "horlayout3");

    m_addGroup = new QPushButton( i18n("New"), page, "m_addGroup" );
    horlayout3->addWidget( m_addGroup );

    m_delGroup = new QPushButton(i18n("Delete"), page, "m_delGroup" );
    horlayout3->addWidget( m_delGroup );
    vertlayout->addLayout( horlayout3 );

    form1Layout->addMultiCellLayout( vertlayout, 0, 1, 0, 0 );

    loadFile();
    if(listExpression.count() == 0)
        slotAddGroup();
    else
        initGroupList();

    connect(m_groupList,SIGNAL(selectionChanged()), this,SLOT(slotGroupSelected()));
    connect(m_ExpressionsList,SIGNAL(selectionChanged()), this,SLOT(slotExpressionSelected()));
    if(m_groupList->count() > 0)  m_groupList->setSelected(0, true);

    connect(m_groupLineEdit,SIGNAL(textChanged(const QString&)), this,SLOT(slotUpdateGroupName(const QString&)));
    connect(m_expressionLineEdit,SIGNAL(textChanged(const QString&)), this,SLOT(slotUpdateExpression(const QString&)));
    connect(m_addExpression, SIGNAL( clicked() ), this,SLOT( slotAddExpression() ));
    connect(m_delExpression, SIGNAL( clicked() ), this,SLOT( slotRemoveExpression() ));
    connect(m_addGroup, SIGNAL( clicked() ), this,SLOT( slotAddGroup()));
    connect(m_delGroup, SIGNAL( clicked() ), this,SLOT( slotRemoveGroup() ));
    updateWidget();

    setFocus();
}

void KWEditPersonnalExpression::slotExpressionSelected()
{
    m_delExpression->setEnabled(m_ExpressionsList->currentItem()!=-1);
    m_expressionLineEdit->setText(m_ExpressionsList->currentText());
}

void KWEditPersonnalExpression::slotGroupSelected()
{
    m_groupLineEdit->blockSignals(true);
    m_delExpression->setEnabled(m_groupList->currentItem()!=-1);
    m_groupLineEdit->setText(m_groupList->currentText());
    list::Iterator it= listExpression.find(m_groupList->currentText());
    QStringList lst(it.data());
    m_ExpressionsList->blockSignals(true);
    m_ExpressionsList->clear();
    m_ExpressionsList->insertStringList(lst);
    m_ExpressionsList->blockSignals(false);
    m_delExpression->setEnabled(lst.count()>0);
    if(m_ExpressionsList->count() > 0)
        m_ExpressionsList->setSelected(0, true);
    m_groupLineEdit->blockSignals(false);
}

void KWEditPersonnalExpression::slotUpdateGroupName(const QString &newGroupName)
{
    if ( m_groupList->currentItem()==-1 )
        return;

    m_addGroup->setEnabled( true );
    list::Iterator it= listExpression.find(m_groupList->currentText());
    QStringList lst(it.data());
    listExpression.insert(newGroupName,lst);
    listExpression.remove(m_groupList->currentText());
    m_groupList->blockSignals(true);
    m_groupList->changeItem(newGroupName, m_groupList->currentItem());
    m_groupList->blockSignals(false);
    m_bChanged=true;
}

void KWEditPersonnalExpression::slotUpdateExpression(const QString &newExpression)
{
    if(newExpression.isEmpty() || m_groupList->currentItem()==-1 || m_ExpressionsList->currentText().isEmpty())
        return;

    list::Iterator it= listExpression.find(m_groupList->currentText());
    QStringList lst = it.data();
    QStringList::Iterator it2=lst.find(m_ExpressionsList->currentText());
    lst.insert(it2,QString(newExpression));
    lst.remove(it2);
    listExpression.replace(m_groupList->currentText(),lst);

    lst.remove(m_ExpressionsList->currentText());
    m_ExpressionsList->blockSignals(true);
    m_ExpressionsList->changeItem(newExpression, m_ExpressionsList->currentItem());
    m_ExpressionsList->blockSignals(false);
    m_bChanged=true;
}

void KWEditPersonnalExpression::loadFile()
{
    QString file=locateLocal("data","kword/expression/perso.xml");
    init( file );
}

void KWEditPersonnalExpression::initGroupList()
{
    QStringList lst;
    m_groupList->clear();
    list::Iterator it;
    for( it = listExpression.begin(); it != listExpression.end(); ++it )
        lst<<it.key();
    m_groupList->insertStringList(lst);
}

void KWEditPersonnalExpression::init(const QString& filename )
{
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;

    kDebug() << "KWEditPersonnalExpression::init parsing " << filename << endl;
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    QString group = "";
    QStringList list;
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == "Type" )
            {
                list.clear();
                group = i18n( e.namedItem( "TypeName" ).toElement().text().toUtf8() );

                QDomNode n2 = e.firstChild();
                for( ; !n2.isNull(); n2 = n2.nextSibling() )
                {

                    if ( n2.isElement() )
                    {
                        QDomElement e2 = n2.toElement();
                        if ( e2.tagName() == "Expression" )
                        {
                            QString text = i18n( e2.namedItem( "Text" ).toElement().text().toUtf8() );
                            list<<text;
                        }
                    }
                }
                listExpression.insert(group,list);

                group = "";
            }
        }
    }
}

void KWEditPersonnalExpression::slotOk()
{
    if( m_bChanged)
        saveFile();
    KDialogBase::slotOk();
}

void KWEditPersonnalExpression::slotAddExpression() {
    list::Iterator it= listExpression.find(m_groupList->currentText());
    QStringList lst(it.data());
    QString newWord = i18n("empty");
    if ( lst.contains( newWord ))
        return;
    lst<< newWord;
    listExpression.replace(m_groupList->currentText(),lst);

    m_ExpressionsList->blockSignals(true);
    m_ExpressionsList->insertItem(newWord);
    m_ExpressionsList->clearSelection();
    m_ExpressionsList->setBottomItem (m_ExpressionsList->count() -1);
    m_ExpressionsList->blockSignals(false);
    m_ExpressionsList->setSelected(m_ExpressionsList->count() -1, true);
    m_expressionLineEdit->selectAll();
    m_expressionLineEdit->setFocus();

    updateExpression();

    m_bChanged=true;
}

void KWEditPersonnalExpression::slotRemoveExpression() {
    QString text=m_ExpressionsList->currentText ();
    if(!text.isEmpty())
    {
        list::Iterator it= listExpression.find(m_groupList->currentText());
        QStringList lst(it.data());
        lst.remove(text);
        listExpression.replace(m_groupList->currentText(),lst);

        m_ExpressionsList->blockSignals(true);
        m_ExpressionsList->clear();
        m_ExpressionsList->insertStringList(lst);
        m_delExpression->setEnabled(lst.count()>0);
        m_ExpressionsList->blockSignals(false);
        m_expressionLineEdit->blockSignals(true);
        m_expressionLineEdit->clear();
        m_expressionLineEdit->blockSignals(false);
        m_bChanged=true;
    }
}

void KWEditPersonnalExpression::slotAddGroup() {
    listExpression.insert(i18n("new group"),QStringList());
    initGroupList();
    m_groupList->setCurrentItem(m_groupList->count() -1);
    updateWidget();
    m_bChanged=true;
}

void KWEditPersonnalExpression::slotRemoveGroup() {
    QString group=m_groupList->currentText();
    if(group.isEmpty())
        return;
    listExpression.remove( group );
    m_groupList->removeItem(m_groupList->currentItem());
    updateWidget();
    m_bChanged=true;
}

void KWEditPersonnalExpression::updateWidget()
{
    bool state = listExpression.count()> 0;
    m_delGroup->setEnabled( state );
    m_addExpression->setEnabled( state );
    m_groupLineEdit->setEnabled( state );

    state =state && m_ExpressionsList->count()>0;
    m_expressionLineEdit->setEnabled( state );
    m_delExpression->setEnabled( state);

}

void KWEditPersonnalExpression::updateExpression()
{
    bool state = m_ExpressionsList->count()>0;
    m_expressionLineEdit->setEnabled( state );
    m_delExpression->setEnabled( state);
}

void KWEditPersonnalExpression::saveFile()
{
    QDomDocument doc( "KWordExpression" );
    QDomElement begin = doc.createElement( "KWordExpression" );
    doc.appendChild( begin );
    QStringList lst;
    list::Iterator it;
    for( it = listExpression.begin(); it != listExpression.end(); ++it )
    {
        lst=it.data();
        if ( !lst.isEmpty() )
        {
            QDomElement type = doc.createElement( "Type" );
            begin.appendChild( type );
            QDomElement typeName = doc.createElement( "TypeName" );
            type.appendChild( typeName );
            typeName.appendChild( doc.createTextNode(it.key()  ) );

            for( uint i=0;i<lst.count();i++ )
            {
                QDomElement expr = doc.createElement( "Expression" );
                type.appendChild( expr );
                QDomElement text = doc.createElement( "Text" );
                expr.appendChild( text );
                text.appendChild( doc.createTextNode(lst[i] ) );
            }
        }
    }
    QByteArray s = doc.toByteArray();

    QFile file( locateLocal("data","kword/expression/perso.xml") );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        kDebug()<<"Error \n";
        return;
    }
    file.write(s,s.length());
    file.close();
}

#include "KWEditPersonnalExpression.moc"
