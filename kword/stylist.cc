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

/* keep 2 qlists with the styles.
   1 of the origs, another with the changed ones (in order of the stylesList)
   When an orig entry is empty and the other is not, a new one has to be made,
   when the orig is present and the other is not, the orig has to be deleted.
   Otherwise all changes are copied from the changed ones to the origs on OK.
   OK also frees all the changed ones and updates the doc if styles are deleted.
*/

/*================================================================*/
KWStyleManager::KWStyleManager( QWidget *_parent, KWDocument *_doc )
    : KDialogBase( _parent, "Stylist", true,
                   i18n("Stylist"),
                   KDialogBase::Ok | KDialogBase::Cancel /* TODO apply */ )
{
    //setWFlags(getWFlags() || WDestructiveClose);
    m_doc = _doc;
    m_currentStyle =0L;
    noSignals=true;
    m_origStyles.setAutoDelete(false);
    m_changedStyles.setAutoDelete(false);
    KWUnit::Unit unit = m_doc->getUnit();

    setupWidget(); // build the widget with the buttons and the list selector.

    addGeneralTab();

    KWStyleFontTab * fontTab = new KWStyleFontTab( m_tabs );
    addTab( fontTab );

    KWStyleParagTab *newTab = new KWStyleParagTab( m_tabs );
    newTab->setWidget( new KWIndentSpacingWidget( unit, newTab ) );
    addTab( newTab );

    newTab = new KWStyleParagTab( m_tabs );
    newTab->setWidget( new KWParagAlignWidget( newTab ) );
    addTab( newTab );

    newTab = new KWStyleParagTab( m_tabs );
    newTab->setWidget( new KWParagBorderWidget( newTab ) );
    addTab( newTab );

    newTab = new KWStyleParagTab( m_tabs );
    newTab->setWidget( new KWParagCounterWidget( newTab ) );
    addTab( newTab );

    newTab = new KWStyleParagTab( m_tabs );
    newTab->setWidget( new KWParagTabulatorsWidget( unit, newTab ) );
    addTab( newTab );

    m_stylesList->setCurrentItem( 0 );
    noSignals=false;
    switchStyle();

    setInitialSize( QSize( 600, 540 ) );
}

void KWStyleManager::addTab( KWStyleManagerTab * tab )
{
    m_tabsList.append( tab );
    m_tabs->insertTab( tab, tab->tabName() );
}

void KWStyleManager::setupWidget()
{
    // This takes space, and no other dialog has a frame around it -> removed (DF)
    //QVBoxLayout *form1Layout = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
    //QFrame *frame1 = new QFrame( this );
    //frame1->setFrameShape( QFrame::StyledPanel );
    //frame1->setFrameShadow( QFrame::Raised );

    QFrame * frame1 = makeMainWidget();
    QGridLayout *frame1Layout = new QGridLayout( frame1, 0, 0, // auto
                                                 KDialog::marginHint(), KDialog::spacingHint() );

    QList<KWStyle> styles( m_doc->styleList() );
    numStyles = styles.count();
    m_stylesList = new QListBox( frame1, "stylesList" );
    for ( unsigned int i = 0; i < styles.count(); i++ ) {
        m_stylesList->insertItem( styles.at( i )->name() );
        m_origStyles.append(styles.at(i));
        m_changedStyles.append(styles.at(i));

kdDebug() << " style " << i << ": " << styles.at(i)->name() << " (" << styles.at(i) << ") with following: " << styles.at(i)->followingStyle()->name() << " (" << styles.at(i)->followingStyle() << ")" << endl;
    }

    frame1Layout->addMultiCellWidget( m_stylesList, 0, 0, 0, 1 );

    m_deleteButton = new QPushButton( frame1, "deleteButton" );
    m_deleteButton->setText( i18n( "&Delete" ) );
    connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteStyle() ) );

    frame1Layout->addWidget( m_deleteButton, 1, 1 );

    m_newButton = new QPushButton( frame1, "newButton" );
    m_newButton->setText( i18n( "New" ) );
    connect( m_newButton, SIGNAL( clicked() ), this, SLOT( addStyle() ) );

    frame1Layout->addWidget( m_newButton, 1, 0 );

    m_tabs = new QTabWidget( frame1 );
    frame1Layout->addMultiCellWidget( m_tabs, 0, 1, 2, 2 );

#if 0 // we're using KDialogBase now
    form1Layout->addWidget( frame1 );
    QHBoxLayout *layout2 = new QHBoxLayout( 0L, KDialog::marginHint(), 0 /*KDialog::spacingHint()*/ );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    m_okButton = new QPushButton( this );
    m_okButton->setText( i18n( "&OK" ) );
    m_okButton->setDefault(true);
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    layout2->addWidget( m_okButton );

    m_cancelButton = new QPushButton( this);
    m_cancelButton->setText( i18n( "&Cancel" ) );
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    layout2->addWidget( m_cancelButton );
    form1Layout->addLayout( layout2 );
#endif
    connect( m_stylesList, SIGNAL( selectionChanged() ), this, SLOT( switchStyle() ) );
    connect( m_tabs, SIGNAL( currentChanged ( QWidget * ) ), this, SLOT( switchTabs() ) );
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

    if(m_currentStyle !=0L)
        save();

    m_currentStyle = 0L;
    int num = m_stylesList->currentItem();
    if(m_origStyles.at(num) == m_changedStyles.at(num)) {
        m_currentStyle = new KWStyle( *m_origStyles.at(num) );
        m_changedStyles.take(num);
        m_changedStyles.insert(num, m_currentStyle);
    } else {
        m_currentStyle = m_changedStyles.at(num);
    }
    updateGUI();

    noSignals=false;
}

void KWStyleManager::switchTabs()
{
    // Called when the user switches tabs
    // We call save() to update our style, for the preview on the 1st tab
    save();
}

int KWStyleManager::getStyleByName(const QString & name) {
    for(unsigned int i=0; i < m_changedStyles.count(); i++) {

kdDebug() << " style " << i << ": " << m_changedStyles.at(i)->name() << " (" << m_changedStyles.at(i) << ") with following: " << m_changedStyles.at(i)->followingStyle()->name() << " (" << m_changedStyles.at(i)->followingStyle() << ")" << endl;
        if(m_changedStyles.at(i)->name() == name)
            return i;
    }
    return 0;
}

void KWStyleManager::updateGUI() {
    QListIterator<KWStyleManagerTab> it( m_tabsList );
    for ( ; it.current() ; ++it )
    {
        it.current()->setStyle( m_currentStyle );
        it.current()->update();
    }

    m_nameString->setText(m_currentStyle->name());

    for ( int i = 0; i < m_styleCombo->count(); i++ ) {
        if ( m_styleCombo->text( i ) == m_currentStyle->followingStyle()->name() ) {
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
        QListIterator<KWStyleManagerTab> it( m_tabsList );
        for ( ; it.current() ; ++it )
            it.current()->save();
        m_currentStyle->setName( m_nameString->text() );
kdDebug() << "following style1: " << m_styleCombo->currentText() << endl;
kdDebug() << "following style2: " << getStyleByName(m_styleCombo->currentText()) << endl;
kdDebug() << "following style3: " << m_changedStyles.at(getStyleByName(m_styleCombo->currentText())) << endl;
kdDebug() << "following style4: " << m_changedStyles.at(getStyleByName(m_styleCombo->currentText()))->name() << endl;
        m_currentStyle->setFollowingStyle(m_changedStyles.at(getStyleByName(m_styleCombo->currentText())));
    }
    preview->repaint(true);
}

void KWStyleManager::addStyle() {
    save();

    QString str = i18n( "New Style Template ( %1 )" ).arg(numStyles++);
    if ( m_currentStyle )
    {
        m_currentStyle = new KWStyle( *m_currentStyle ); // Create a new style, initializing from the current one
        m_currentStyle->setName( str );
    }
    else
        m_currentStyle = new KWStyle( str );

    noSignals=true;
    m_origStyles.append(0L);
    m_changedStyles.append(m_currentStyle);
    m_stylesList->insertItem( str );
    m_styleCombo->insertItem( str );
    m_stylesList->setCurrentItem( m_stylesList->count() - 1 );
    noSignals=false;

    updateGUI();
}

void KWStyleManager::deleteStyle() {

    save();
    unsigned int cur = getStyleByName(m_stylesList->currentText());
    KWStyle *s = m_changedStyles.at(cur);
    m_changedStyles.remove(cur);
    m_changedStyles.insert(cur,0L);
    delete s;

    noSignals=true;

    m_stylesList->removeItem(cur);
    m_styleCombo->removeItem(cur);
    if(cur > m_stylesList->count())
        cur--;

    numStyles--;
    m_stylesList->setCurrentItem(cur);

    noSignals=false;

    updateGUI();
}

void KWStyleManager::slotOk() {
    save();
    apply();
    KDialogBase::slotOk();
}

void KWStyleManager::apply() {
    noSignals=true;
    for (unsigned int i =0 ; m_origStyles.count() > i ; i++) {
kdDebug() << "i: " << i << endl;
        if(m_origStyles.at(i) == 0) {           // newly added style
kdDebug() << "adding new " <<  m_changedStyles.at(i)->name() << " (" << i << ")" <<  endl;
            m_doc->addStyleTemplate(m_changedStyles.at(i));
        } else if(m_changedStyles.at(i) == 0) { // deleted style
kdDebug() << "deleting orig " <<  m_origStyles.at(i)->name() << " (" << i << ")" <<  endl;
            m_doc->removeStyleTemplate(m_origStyles.at(i));
        } else if(m_changedStyles.at(i) != m_origStyles.at(i)) {
kdDebug() << "update style " <<  m_changedStyles.at(i)->name() << " (" << i << ")" <<  endl;
                                                // simply updated style
            KWStyle *orig = m_origStyles.at(i);
            KWStyle *changed = m_changedStyles.at(i);

            // Copy everything from changed to orig
            *orig = *changed;
        } else {
kdDebug() << "has not changed " <<  m_changedStyles.at(i)->name() << " (" << i << ")" <<  endl;
        }
    }

    m_doc->updateAllStyleLists();
    noSignals=false;
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
    bool state=!theText.isEmpty();
    enableButtonOK(state);
    m_deleteButton->setEnabled(state&&(m_stylesList->currentItem() != 0));
    m_newButton->setEnabled(state);
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

    QFont f( style->format().font() );
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
    QFont f( style->format().font() );

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
            ostyle->setName( eName->text() );
            emit updateStyleList();
        }
    }

    // Apply "following style" change
    ostyle->setFollowingStyle( cFollowing->currentText() );

    // ### TODO a dirty flag for this !
    doc->applyStyleChange( ostyle );
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

void KWStyleParagTab::setWidget( KWParagLayoutWidget * widget )
{
    m_widget = widget;
}

void KWStyleParagTab::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( m_widget ) m_widget->resize( size() );
}

KWStyleFontTab::KWStyleFontTab( QWidget * parent )
    : KWStyleManagerTab( parent )
{
    m_chooser = new KWFontChooser( this );
}

void KWStyleFontTab::update()
{
    bool subScript = m_style->format().vAlign() == QTextFormat::AlignSubScript;
    bool superScript = m_style->format().vAlign() == QTextFormat::AlignSuperScript;
    m_chooser->setFont( m_style->format().font(), subScript, superScript );
}

void KWStyleFontTab::save()
{
    m_style->format().setFont( m_chooser->getNewFont() );
    if ( m_chooser->getSubScript() )
        m_style->format().setVAlign( QTextFormat::AlignSubScript );
    else if ( m_chooser->getSuperScript() )
        m_style->format().setVAlign( QTextFormat::AlignSuperScript );
    else
        m_style->format().setVAlign( QTextFormat::AlignNormal );
}

QString KWStyleFontTab::tabName()
{
    return i18n("Font");
}

void KWStyleFontTab::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( m_chooser ) m_chooser->resize( size() );
}

