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
#include <kwtextdocument.h>

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

KWStyleManager::KWStyleManager( QWidget *_parent, KWDocument *_doc )
    : KDialogBase( _parent, "Stylist", true,
                   i18n("Stylist"),
                   KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::Apply )
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

    setInitialSize( QSize( 600, 570 ) );
}

void KWStyleManager::addTab( KWStyleManagerTab * tab )
{
    m_tabsList.append( tab );
    m_tabs->insertTab( tab, tab->tabName() );
}

void KWStyleManager::setupWidget()
{
    QFrame * frame1 = makeMainWidget();
    QGridLayout *frame1Layout = new QGridLayout( frame1, 0, 0, // auto
                                                 KDialog::marginHint(), KDialog::spacingHint() );

    QListIterator<KWStyle> style( m_doc->styleList() );
    numStyles = m_doc->styleList().count();
    m_stylesList = new QListBox( frame1, "stylesList" );
    for ( ; style.current() ; ++style )
    {
        m_stylesList->insertItem( i18n("KWord style", style.current()->name().utf8() ) );
        m_origStyles.append( style.current() );
        m_changedStyles.append( style.current() );
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

    connect( m_stylesList, SIGNAL( selectionChanged() ), this, SLOT( switchStyle() ) );
    connect( m_tabs, SIGNAL( currentChanged ( QWidget * ) ), this, SLOT( switchTabs() ) );
}

void KWStyleManager::addGeneralTab() {
    QWidget *tab = new QWidget( m_tabs );

    QGridLayout *tabLayout = new QGridLayout( tab );
    tabLayout->setSpacing( 6 );
    tabLayout->setMargin( 11 );

    preview = new KWStylePreview( i18n( "Preview" ), tab );

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
    kdDebug() << "KWStyleManager::switchStyle noSignals=" << noSignals << endl;
    if(noSignals) return;
    noSignals=true;

    if(m_currentStyle !=0L)
        save();

    m_currentStyle = 0L;
    int num = styleIndex( m_stylesList->currentItem() );
    kdDebug() << "KWStyleManager::switchStyle switching to " << num << endl;
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
    updatePreview();
}

// Return the index of the a style from its position in the GUI
// (e.g. in m_stylesList or m_styleCombo). This index is used in
// the m_origStyles and m_changedStyles lists.
// The reason for the difference is that a deleted style is removed
// from the GUI but not from the internal lists.
int KWStyleManager::styleIndex( int pos ) {
    int p = 0;
    for(unsigned int i=0; i < m_changedStyles.count(); i++) {
        // Skip deleted styles, they're no in m_stylesList anymore
        KWStyle * style = m_changedStyles.at(i);
        if ( !style ) continue;
        if ( p == pos )
            return i;
        ++p;
    }
    kdWarning() << "KWStyleManager::styleIndex no style found at pos " << pos << endl;
    return 0;
}

void KWStyleManager::updateGUI() {
    kdDebug() << "KWStyleManager::updateGUI m_currentStyle=" << m_currentStyle << " " << m_currentStyle->name() << endl;
    QListIterator<KWStyleManagerTab> it( m_tabsList );
    for ( ; it.current() ; ++it )
    {
        it.current()->setStyle( m_currentStyle );
        it.current()->update();
    }

    m_nameString->setText(m_currentStyle->name());

    kdDebug() << "KWStyleManager::updateGUI updating combo to " << m_currentStyle->followingStyle()->name() << endl;
    for ( int i = 0; i < m_styleCombo->count(); i++ ) {
        if ( m_styleCombo->text( i ) == i18n( "KWord style", m_currentStyle->followingStyle()->name().utf8() ) ) {
            m_styleCombo->setCurrentItem( i );
            kdDebug() << "found at " << i << endl;
            break;
        }
    }

    // update delete button (can't delete first style);
    m_deleteButton->setEnabled(m_stylesList->currentItem() != 0);
    updatePreview();
}

void KWStyleManager::updatePreview()
{
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

        int indexNextStyle = styleIndex( m_styleCombo->currentItem() );
        m_currentStyle->setFollowingStyle( m_changedStyles.at( indexNextStyle ) );
    }
}

void KWStyleManager::addStyle() {
    save();

    QString str = i18n( "New Style Template (%1)" ).arg(numStyles++);
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

    unsigned int cur = styleIndex( m_stylesList->currentItem() );
    unsigned int curItem = m_stylesList->currentItem();
    KWStyle *s = m_changedStyles.at(cur);
    ASSERT( s == m_currentStyle );
    delete s;
    m_currentStyle = 0L;
    m_changedStyles.remove(cur);
    m_changedStyles.insert(cur,0L);

    // Done with noSignals still false, so that when m_stylesList changes the current item
    // we display it automatically
    m_stylesList->removeItem(curItem);
    m_styleCombo->removeItem(curItem);
    numStyles--;
    m_stylesList->setSelected( m_stylesList->currentItem(), true );
}

void KWStyleManager::slotOk() {
    save();
    apply();
    KDialogBase::slotOk();
}

void KWStyleManager::slotApply() {
    save();
    apply();
    KDialogBase::slotApply();
}

void KWStyleManager::apply() {
    noSignals=true;
    for (unsigned int i =0 ; m_origStyles.count() > i ; i++) {
        if(m_origStyles.at(i) == 0) {           // newly added style
            kdDebug() << "adding new " << m_changedStyles.at(i)->name() << " (" << i << ")" << endl;
            KWStyle *tmp = m_doc->addStyleTemplate(m_changedStyles.take(i));
            m_changedStyles.insert(i, tmp);
        } else if(m_changedStyles.at(i) == 0) { // deleted style
            kdDebug() << "deleting orig " << m_origStyles.at(i)->name() << " (" << i << ")" << endl;

            KWStyle *orig = m_origStyles.at(i);
            m_doc->applyStyleChange( orig, -1, -1 );
            m_doc->removeStyleTemplate( orig );
            // Note that the style is never deleted (we'll need it for undo/redo purposes)

        } else if(m_changedStyles.at(i) != m_origStyles.at(i)) {
            kdDebug() << "update style " << m_changedStyles.at(i)->name() << " (" << i << ")" << endl;
                                                // simply updated style
            KWStyle *orig = m_origStyles.at(i);
            KWStyle *changed = m_changedStyles.at(i);

            int paragLayoutChanged = orig->paragLayout().compare( changed->paragLayout() );
            int formatChanged = orig->format().compare( changed->format() );
            //kdDebug() << "old format " << orig->format().key() << " pointsize " << orig->format().pointSizeFloat() << endl;
            //kdDebug() << "new format " << changed->format().key() << " pointsize " << changed->format().pointSizeFloat() << endl;

            // Copy everything from changed to orig
            *orig = *changed;

            // Apply the change selectively - i.e. only what changed
            m_doc->applyStyleChange( orig, paragLayoutChanged, formatChanged );

        }// else
         //     kdDebug() << "has not changed " <<  m_changedStyles.at(i)->name() << " (" << i << ")" <<  endl;
    }

    m_doc->updateAllStyleLists();
    noSignals=false;
}

void KWStyleManager::renameStyle(const QString &theText) {
    if(noSignals) return;
    noSignals=true;

    int index = m_stylesList->currentItem();
    kdDebug() << "KWStyleManager::renameStyle " << index << " to " << theText << endl;

    // rename only in the GUI, not even in the underlying objects (save() does it).
    kdDebug() << "KWStyleManager::renameStyle before " << m_styleCombo->currentText() << endl;
    m_styleCombo->changeItem( theText, index );
    kdDebug() << "KWStyleManager::renameStyle after " << m_styleCombo->currentText() << endl;
    m_stylesList->changeItem( theText, index );

    // Check how many styles with that name we have now
    int synonyms = 0;
    for ( int i = 0; i < m_styleCombo->count(); i++ ) {
        if ( m_styleCombo->text( i ) == m_stylesList->currentText() )
            ++synonyms;
    }
    ASSERT( synonyms > 0 ); // should have found 'index' at least !
    noSignals=false;
    // Can't close the dialog if two styles have the same name
    bool state=!theText.isEmpty() && (synonyms == 1);
    enableButtonOK(state );
    enableButtonApply(state);
    m_deleteButton->setEnabled(state&&(m_stylesList->currentItem() != 0));
    m_newButton->setEnabled(state);
}

/******************************************************************/
/* Class: KWStylePreview                                          */
/******************************************************************/
KWStylePreview::KWStylePreview( const QString &title, QWidget *parent )
    : QGroupBox( title, parent, "" )
{
    m_zoomHandler = new KoZoomHandler;
    m_textdoc = new KWTextDocument( m_zoomHandler );
    KWTextParag * parag = static_cast<KWTextParag *>(m_textdoc->firstParag());
    parag->insert( 0, i18n( "KWord, KOffice's Word Processor" ) );
}

KWStylePreview::~KWStylePreview()
{
    delete m_textdoc;
    delete m_zoomHandler;
}

void KWStylePreview::setStyle( KWStyle *style )
{
    KWTextParag * parag = static_cast<KWTextParag *>(m_textdoc->firstParag());
    parag->applyStyle( style );
    repaint(true);
}

void KWStylePreview::drawContents( QPainter *painter )
{
    // see also KWNumPreview::drawContents
    painter->save();
    QRect r = contentsRect();
    //kdDebug() << "KWStylePreview::drawContents contentsRect=" << DEBUGRECT(r) << endl;

    QRect whiteRect( r.x() + 10, r.y() + 10,
                     r.width() - 20, r.height() - 20 );
    QColorGroup cg = QApplication::palette().active();
    painter->fillRect( whiteRect, cg.brush( QColorGroup::Base ) );

    KWTextParag * parag = static_cast<KWTextParag *>(m_textdoc->firstParag());
    if ( m_textdoc->width() != whiteRect.width() )
    {
        // For centering to work, and to even get word wrapping when the thing is too big :)
        m_textdoc->setWidth( whiteRect.width() );
        parag->invalidate(0);
    }

    parag->format();
    QRect textRect = parag->rect();

    // Center vertically, but not horizontally, to keep the parag alignment working,
    textRect.moveTopLeft( QPoint( whiteRect.x() + 10,
                                  whiteRect.y() + ( whiteRect.height() - textRect.height() ) / 2 ) );
    //kdDebug() << "KWStylePreview::drawContents textRect=" << DEBUGRECT(textRect)
    //          << " textSize=" << textSize.width() << "," << textSize.height() << endl;
    painter->setClipRect( textRect.intersect( whiteRect ) );
    painter->translate( textRect.x(), textRect.y() );

    m_textdoc->draw( painter, 0, 0, textRect.width(), textRect.height(), cg );
    painter->restore();
}

/////////////

void KWStyleParagTab::update()
{
     m_widget->display( m_style->paragLayout() );
}

void KWStyleParagTab::save()
{
     m_widget->save( m_style->paragLayout() );
}

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
    m_chooser->setColor( m_style->format().color() );
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
    m_style->format().setColor( m_chooser->color() );
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

