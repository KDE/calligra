/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kwdoc.h"
#include "kwstyle.h"
#include "stylist.h"
#include "stylist.moc"
#include "defs.h"
#include "fontdia.h"

#include <qwidget.h>
#include <qtabwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstrlist.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolordlg.h>
#include <klocale.h>

#include <kdebug.h>

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

/*================================================================*/
KWStyleManager::KWStyleManager( QWidget *_parent, KWDocument *_doc, QStringList _fontList )
    : QDialog(_parent, "Stylist", true), m_changedStyles(10007, false), m_deletedStyles(10007, false) {
    setWFlags(getWFlags() || WDestructiveClose);
    m_fontList = _fontList;
    m_doc = _doc;
    m_currentStyle =0L;
    noSignals=true;

    setupWidget(); // build the widget with the buttons and the list selector.
    addGeneralTab();

    // basicTab *newTab = new blaat();
    // m_tabsList.add(newTab);
    // tabs->insertTab( newTab, newTab->getName() );

    m_stylesList->setCurrentItem( 0 );
    noSignals=false;
    switchStyle();
}

void KWStyleManager::setupWidget() {
    resize( 625, 495 );
    QVBoxLayout *Form1Layout = new QVBoxLayout( this );
    Form1Layout->setSpacing( 6 );
    Form1Layout->setMargin( 11 );
 
    QFrame *Frame1 = new QFrame( this);
    Frame1->setFrameShape( QFrame::StyledPanel );
    Frame1->setFrameShadow( QFrame::Raised );
    QGridLayout *Frame1Layout = new QGridLayout( Frame1 );
    Frame1Layout->setSpacing( 6 );
    Frame1Layout->setMargin( 11 );
 
    QList<KWStyle> styles = const_cast<QList<KWStyle> & >(m_doc->styleList());
    numStyles = styles.count();
    m_stylesList = new QListBox( Frame1, "stylesList" );
    for ( unsigned int i = 0; i < styles.count(); i++ ) {
        m_stylesList->insertItem( styles.at( i )->name() );
    }
    m_currentStyle = styles.first();
 
    Frame1Layout->addMultiCellWidget( m_stylesList, 0, 0, 0, 1 );
 
    m_deleteButton = new QPushButton( Frame1, "deleteButton" );
    m_deleteButton->setText( i18n( "&Delete" ) );
    connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteStyle() ) );
 
    Frame1Layout->addWidget( m_deleteButton, 1, 1 );
 
    QPushButton *newButton = new QPushButton( Frame1, "newButton" );
    newButton->setText( i18n( "New" ) );
    connect( newButton, SIGNAL( clicked() ), this, SLOT( addStyle() ) );
 
    Frame1Layout->addWidget( newButton, 1, 0 );
 
    m_tabs = new QTabWidget( Frame1);
    Frame1Layout->addMultiCellWidget( m_tabs, 0, 1, 2, 2 );
    Form1Layout->addWidget( Frame1 );
 
    QHBoxLayout *Layout2 = new QHBoxLayout;
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );
 
    m_okButton = new QPushButton( this );
    m_okButton->setText( i18n( "&OK" ) );
    m_okButton->setDefault(true);
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    Layout2->addWidget( m_okButton );
 
    m_cancelButton = new QPushButton( this);
    m_cancelButton->setText( i18n( "&Cancel" ) );
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    Layout2->addWidget( m_cancelButton );
    Form1Layout->addLayout( Layout2 );

    connect( m_stylesList, SIGNAL( selectionChanged() ), this, SLOT( switchStyle() ) );
}

void KWStyleManager::addGeneralTab() {
    QWidget *tab = new QWidget( m_tabs );

    QGridLayout *tabLayout = new QGridLayout( tab );
    tabLayout->setSpacing( 6 );
    tabLayout->setMargin( 11 );

    preview = new KWStylePreview( i18n( "Preview" ), tab, m_currentStyle );
 
    tabLayout->addMultiCellWidget( preview, 2, 2, 0, 1 );
 
    m_nameString = new QLineEdit( tab );
    m_nameString->resize(m_nameString->sizeHint() );
    connect( m_nameString, SIGNAL( textChanged( const QString &) ), this, SLOT( renameStyle(const QString &) ) );
 
    tabLayout->addWidget( m_nameString, 0, 1 );
 
    QLabel *nameLabel = new QLabel( tab );
    nameLabel->setText( i18n( "Name:" ) );
    nameLabel->resize(nameLabel->sizeHint());
    nameLabel->setAlignment( AlignRight | AlignVCenter );

    tabLayout->addWidget( nameLabel, 0, 0 );
 
    m_styleCombo = new QComboBox( FALSE, tab, "styleCombo" );
 
    for ( unsigned int i = 0; i < m_stylesList->count(); i++ ) {
        m_styleCombo->insertItem( m_stylesList->text(i));
    }

    tabLayout->addWidget( m_styleCombo, 1, 1 );
 
    QLabel *nextStyleLabel = new QLabel( tab );
    nextStyleLabel->setText( i18n( "Next style:" ) );
 
    tabLayout->addWidget( nextStyleLabel, 1, 0 );
    m_tabs->insertTab( tab, i18n( "General" ) );
}

void KWStyleManager::switchStyle() {
    if(noSignals) return;
    noSignals=true;

    save();

    KWStyle *style = m_changedStyles[m_stylesList->currentText()];
    if(! style) {
kdDebug() << "creating new style: " << m_stylesList->currentText() << endl;
        style = new KWStyle("a");
        
        KWStyle *oldStyle=m_doc->findStyle(m_stylesList->currentText());
        style->format()=oldStyle->format();
        style->setFollowingStyle(oldStyle->followingStyle());
        style->paragLayout()=oldStyle->paragLayout();
        m_changedStyles.insert(m_stylesList->currentText(), style);
    }
    m_currentStyle = style;
    updateGUI();

    noSignals=false;
}

void KWStyleManager::updateGUI() {
    for (unsigned int i =0; m_tabsList.count(); i++) {
        m_tabsList.at(i)->update();
    }
    m_nameString->setText(m_currentStyle->name());
    
    for ( int i = 0; i < m_styleCombo->count(); i++ ) {
        if ( m_styleCombo->text( i ) == m_currentStyle->followingStyle() ) {
            m_styleCombo->setCurrentItem( i );
            break;
        }
    }

    // update delete button (can't delete first style);
    m_deleteButton->setEnabled(m_stylesList->currentItem() != 0);

    preview->setStyle(m_currentStyle);
    preview->repaint(true);
}

void KWStyleManager::save() {
    if(m_currentStyle) {
        // save changes from UI to object.
        for (unsigned int i =0; m_tabsList.count(); i++) {
            m_tabsList.at(i)->save();
        }
        if(m_currentStyle->name() != m_nameString->text())
            renameStyle(m_currentStyle->name(), m_nameString->text());
        m_currentStyle->setFollowingStyle(m_styleCombo->currentText());
    }
}

void KWStyleManager::addStyle() {
    save();

    QString str=i18n( "New Style Template ( %1 )" ).arg(numStyles++);
    KWStyle *oldStyle=m_currentStyle;
    m_currentStyle =new KWStyle(str);

    m_currentStyle->format()=oldStyle->format();
    m_currentStyle->paragLayout()=oldStyle->paragLayout();
    m_currentStyle->setFollowingStyle(oldStyle->followingStyle());
    m_currentStyle->paragLayout().setStyleName( str );

    m_changedStyles.insert(str, m_currentStyle);
    m_stylesList->insertItem( str );
    m_styleCombo->insertItem( str );
    m_stylesList->setCurrentItem( m_stylesList->count() - 1 );

    updateGUI();
}

void KWStyleManager::deleteStyle() {
    save();
    unsigned int cur = m_stylesList->currentItem();

    KWStyle *s = m_changedStyles.take(m_stylesList->currentText());
    m_deletedStyles.insert(m_stylesList->currentText(),s);
    m_stylesList->removeItem(cur);
    m_styleCombo->removeItem(cur);
    if(cur > m_stylesList->count()) cur--;
    m_stylesList->setCurrentItem(cur);

    numStyles--;

    updateGUI();
}

void KWStyleManager::slotCancel() {
    done(0);
}

void KWStyleManager::slotOk() {
    save();
    apply();
    done(1);
}

void KWStyleManager::apply() {
    // delete all styles from doc which are in deletedStyles;
    QDictIterator<KWStyle> it( m_deletedStyles );
    while ( it.currentKey()!= 0L ) {
kdDebug() << "deleting: " << it.currentKey() << endl;
        m_doc->removeStyleTemplate(it.currentKey());
        m_doc->applyStyleChange(it.currentKey());
        ++it;
    }
    //m_deletedStyles.clear();

    // update all styles in doc which are in updatedStyles
    QDictIterator<KWStyle> it2( m_changedStyles );
    while ( it2.current() ) {
        KWStyle *s=it2.current();
kdDebug() << "commiting: " << s->name() << endl;
        m_doc->addStyleTemplate(s);
        m_doc->applyStyleChange(it2.currentKey());
        ++it2;
    }

    m_doc->updateAllStyleLists();
}

void KWStyleManager::renameStyle(const QString &theText) {
    if(noSignals) return;
    noSignals=true;

    // rename only in the GUI, not even in the underlying objects.
    for ( int i = 0; i < m_styleCombo->count(); i++ ) {
        if ( m_styleCombo->text( i ) == m_stylesList->currentText() ) {
            m_styleCombo->changeItem(theText, i);
            break;
        }
    }
    m_stylesList->changeItem(theText, m_stylesList->currentItem());
    noSignals=false;
}

void KWStyleManager::renameStyle(QString oldName, QString newName) {
    if(! m_changedStyles[oldName]) return;

    KWStyle *s = m_changedStyles[oldName];
    m_changedStyles.remove(oldName);
    m_changedStyles.insert(newName, s);
    m_deletedStyles.insert(oldName, s);
    s->paragLayout().setStyleName( newName );

    //check all styles for followingStyle() and rename then as well.
    QList<KWStyle> styles = const_cast<QList<KWStyle> & >(m_doc->styleList());
    for ( unsigned int i = 0; i < styles.count(); i++ ) {
        KWStyle *s = styles.at(i);
        if(m_changedStyles[s->name()]) continue;
        if(m_deletedStyles[s->name()]) continue;
        if(s->followingStyle() == oldName) {
            KWStyle *newStyle=new KWStyle(s->name());

            newStyle->format()=s->format();
            newStyle->paragLayout()=s->paragLayout();
            newStyle->setFollowingStyle(newName);
            newStyle->paragLayout().setStyleName( s->name());
            m_changedStyles.insert(newName, newStyle);
            m_deletedStyles.insert(oldName, 0L);
        }
    }
    QDictIterator<KWStyle> it( m_changedStyles );
    while ( it.current()) {
        if(it.current()->followingStyle() == oldName)
            it.current()->setFollowingStyle(newName);
        ++it;
    }
}


/******************************************************************/
/* Class: KWStylePreview                                          */
/******************************************************************/

/*================================================================*/
void KWStylePreview::drawContents( QPainter *painter ) {
    QRect r = contentsRect();
    QFontMetrics fm( font() );

    painter->fillRect( r.x() + fm.width( 'W' ), r.y() + fm.height(),
                       r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height(), white );
    painter->setClipRect( r.x() + fm.width( 'W' ), r.y() + fm.height(),
                          r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height() );

    QFont f( style->format().font().family(), style->format().font().pointSize() );
    f.setBold( style->format().font().weight() == 75 ? true : false );
    f.setItalic( style->format().font().italic() );
    f.setUnderline( style->format().font().underline()  );
    f.setStrikeOut( style->format().font().strikeOut()  );
    QColor c( style->format().color() );

    painter->setPen( QPen( c ) );
    painter->setFont( f );

    fm = QFontMetrics( f );
    int y = height() / 2 - fm.height() / 2;

    painter->drawText( 20 + (int)( style->paragLayout().margins[QStyleSheetItem::MarginFirstLine]
                                   + style->paragLayout().margins[QStyleSheetItem::MarginLeft] ),
                       y, fm.width( i18n( "KWord, KOffice's Wordprocessor" ) ),
                       fm.height(), 0, i18n( "KWord, KOffice's Wordprocessor" ) );
}


#if 0


void KWStyleEditor::changeFont() {
    QFont f( style->format().font().family(), style->format().font().pointSize() );
    f.setBold( style->format().font().weight() == 75 ? true : false );
    f.setItalic( style->format().font().italic() );
    f.setUnderline( style->format().font().underline() );

    KWFontDia *fontDia = new KWFontDia( this, "",f,false, false,false );
    connect( fontDia, SIGNAL( okClicked() ), this, SLOT( slotFontDiaOk() ) );

    fontDia->show();
    delete fontDia;
}

void KWStyleEditor::slotFontDiaOk() {
     const KWFontDia * fontDia = static_cast<const KWFontDia*>(sender());
     style->format().setFont(fontDia->getNewFont());
     preview->repaint( true );
}

void KWStyleEditor::changeColor() {
  QColor c( style->format().color() );
  if ( KColorDialog::getColor( c ) ) {
    style->format().setColor( c );
    preview->repaint( true );
  }
}

void KWStyleEditor::changeSpacing() {
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_SPACING, doc );
    paragDia->setCaption( i18n( "Paragraph Spacing" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setSpaceBeforeParag( style->paragLayout().margins[QStyleSheetItem::MarginTop] );
    paragDia->setSpaceAfterParag( style->paragLayout().margins[QStyleSheetItem::MarginBottom] );
    paragDia->setLineSpacing( style->paragLayout().lineSpacing );
    paragDia->setLeftIndent( style->paragLayout().margins[QStyleSheetItem::MarginLeft] );
    paragDia->setFirstLineIndent( style->paragLayout().margins[QStyleSheetItem::MarginFirstLine] );
    paragDia->setRightIndent( style->paragLayout().margins[QStyleSheetItem::MarginRight] );
    paragDia->show();
}

void KWStyleEditor::changeAlign() {
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_ALIGN, doc );
    paragDia->setCaption( i18n( "Paragraph Alignment" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setAlign( style->paragLayout().alignment );
    paragDia->show();
}

void KWStyleEditor::changeBorders() {
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_BORDERS, doc );
    paragDia->setCaption( i18n( "Paragraph Borders" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setLeftBorder( style->paragLayout().leftBorder );
    paragDia->setRightBorder( style->paragLayout().rightBorder );
    paragDia->setTopBorder( style->paragLayout().topBorder );
    paragDia->setBottomBorder( style->paragLayout().bottomBorder);
    paragDia->setAfterInitBorder(true);
    paragDia->show();
}

void KWStyleEditor::changeNumbering() {
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_NUMBERING, doc );
    paragDia->setCaption( i18n( "Numbering" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    if ( !style->paragLayout().counter )
        style->paragLayout().counter = new Counter; // default one if none set
    paragDia->setCounter( *style->paragLayout().counter );
    paragDia->show();
}

void KWStyleEditor::changeTabulators() {
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_TABS, doc );
    paragDia->setCaption( i18n( "Tabulators" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setTabList( style->paragLayout().tabList() );
    paragDia->show();
}

void KWStyleEditor::paragDiaOk() {
   switch ( paragDia->getFlags() ) {
   case KWParagDia::PD_SPACING: {
     style->paragLayout().margins[QStyleSheetItem::MarginTop]=paragDia->spaceBeforeParag() ;
     style->paragLayout().margins[QStyleSheetItem::MarginBottom]=paragDia->spaceAfterParag() ;
     style->paragLayout().lineSpacing= paragDia->lineSpacing() ;
     style->paragLayout().margins[QStyleSheetItem::MarginLeft]=paragDia->leftIndent() ;
     style->paragLayout().margins[QStyleSheetItem::MarginFirstLine]= paragDia->firstLineIndent();
     style->paragLayout().margins[QStyleSheetItem::MarginRight]= paragDia->rightIndent();
   } break;
    case KWParagDia::PD_ALIGN:
      style->paragLayout().alignment=paragDia->align() ;
      break;
   case KWParagDia::PD_BORDERS: {
     style->paragLayout().leftBorder= paragDia->leftBorder() ;
     style->paragLayout().rightBorder= paragDia->rightBorder() ;
     style->paragLayout().topBorder= paragDia->topBorder() ;
     style->paragLayout().bottomBorder= paragDia->bottomBorder() ;
   } break;
   case KWParagDia::PD_NUMBERING:
     delete style->paragLayout().counter;
     style->paragLayout().counter = new Counter( paragDia->counter() );
     break;
   case KWParagDia::PD_TABS:
       style->paragLayout().setTabList(paragDia->tabListTabulator());
     break;
   default: break;
   }

    preview->repaint( true );
}

bool KWStyleEditor::apply() {
/*
    *ostyle = *style;

    // Apply name change
    if ( eName->text() != style->name() )
    {
        bool same = false;
	QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
        for ( unsigned int i = 0; i < styles.count() && !same; i++ ) {
            if ( styles.at( i )->name() == eName->text() )
                same = true;
        }

        if ( !same ) {
            ostyle->paragLayout().setStyleName( eName->text() );
            emit updateStyleList();
        }
    }

    // Apply "following style" change
    ostyle->setFollowingStyle( cFollowing->currentText() );

    // ### TODO a dirty flag for this !
    doc->applyStyleChange(eName->text());
*/
    return true; 
}

void KWStyleEditor::slotOk() {
   if (apply())
   {
      KDialogBase::slotOk();
   }
}
#endif
