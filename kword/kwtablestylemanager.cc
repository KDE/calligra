/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kwtablestylemanager.h"
#include "kwtablestylemanager.moc"
#include "kwimportstyledia.h"

#include "kwdoc.h"

#include <koparagcounter.h>
#include <kotextdocument.h>

#include "kwstyle.h"
#include "kwframestylemanager.h"
#include "kwstylemanager.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qframe.h>


/******************************************************************/
/* Class: KWTableStylePreview                                     */
/******************************************************************/

KWTableStylePreview::KWTableStylePreview( const QString& title, const QString& text, QWidget* parent, const char* name )
    : QGroupBox( title, parent, name )
{
    m_zoomHandler = new KoZoomHandler;
    QFont font = KoGlobal::defaultFont();
    m_textdoc = new KoTextDocument( m_zoomHandler, new KoTextFormatCollection( font, QColor(),KGlobal::locale()->language(), false ) );
    KoTextParag * parag = m_textdoc->firstParag();
    parag->insert( 0, text );
}

KWTableStylePreview::~KWTableStylePreview()
{

    delete m_textdoc;
    delete m_zoomHandler;

}

void KWTableStylePreview::drawContents( QPainter *p )
{
    p->save();
    QRect r = contentsRect();

    int wid = ( r.width() - 40 );
    int hei = ( r.height() - 40 );


    // 1: create document background = white

    p->fillRect( QRect( 10, 20, r.width()-20, r.height() - 20 ), QColor("white") );

    // 2: create borders (KWFrameStyle)

    if (tableStyle->pFrameStyle()->topBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->pFrameStyle()->topBorder(), tableStyle->pFrameStyle()->topBorder().width(),black) ); // Top border
        p->drawLine( 20 - int(tableStyle->pFrameStyle()->leftBorder().width()/2), 30,
                     20 + wid + int(tableStyle->pFrameStyle()->rightBorder().width()/2), 30 );
    }
    if (tableStyle->pFrameStyle()->leftBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->pFrameStyle()->leftBorder(), tableStyle->pFrameStyle()->leftBorder().width(),black) ); // Left border
        p->drawLine( 20, 30 - int(tableStyle->pFrameStyle()->topBorder().width()/2),
                     20 , 30 + hei + int(tableStyle->pFrameStyle()->bottomBorder().width()/2) );
    }
    if (tableStyle->pFrameStyle()->bottomBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->pFrameStyle()->bottomBorder(), tableStyle->pFrameStyle()->bottomBorder().width(),black) ); // Bottom border
        p->drawLine( 20 + wid + int(ceil(tableStyle->pFrameStyle()->rightBorder().width()/2)), 30 + hei,
                     20 - int(tableStyle->pFrameStyle()->leftBorder().width()/2), 30 + hei );
    }
    if (tableStyle->pFrameStyle()->rightBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->pFrameStyle()->rightBorder(), tableStyle->pFrameStyle()->rightBorder().width(),black) ); // Right border
        p->drawLine( 20 + wid, 30 - int(tableStyle->pFrameStyle()->topBorder().width()/2) ,
                     20 + wid, 30 + hei + int(tableStyle->pFrameStyle()->bottomBorder().width()/2) );
    }

   // 3: create background whithin "frame"

    QRect fr( QPoint(20 + int(ceil(tableStyle->pFrameStyle()->leftBorder().width()/2)), 30 + int(ceil(tableStyle->pFrameStyle()->topBorder().width()/2))),
              QPoint(20 + wid - int(floor(tableStyle->pFrameStyle()->rightBorder().width()/2)+1), 30 + hei - int(floor(tableStyle->pFrameStyle()->bottomBorder().width()/2)+1)) );

    p->fillRect( fr, tableStyle->pFrameStyle()->backgroundColor() );

   // 4: create text (KWStyle)

    KoTextParag * parag = m_textdoc->firstParag();
    int widthLU = m_zoomHandler->pixelToLayoutUnitX( fr.width() - 2 ); // keep one pixel border horizontally
    if ( m_textdoc->width() != widthLU )
    {
        // For centering to work, and to even get word wrapping when the thing is too big :)
        m_textdoc->setWidth( widthLU );
        parag->invalidate(0);
    }

    parag->format();
    QRect textRect = parag->pixelRect( m_zoomHandler );

    // Center vertically, but not horizontally, to keep the parag alignment working,
    textRect.moveTopLeft( QPoint( fr.x(), fr.y() + ( fr.height() - textRect.height() ) / 2 ) );

    p->setClipRect( textRect.intersect( fr ) );
    p->translate( textRect.x(), textRect.y() );

    QColorGroup cg = QApplication::palette().active();
    cg.setBrush( QColorGroup::Base, tableStyle->pFrameStyle()->backgroundColor() );

    m_textdoc->drawWYSIWYG( p, 1, 0, textRect.width() - 1, textRect.height(), cg, m_zoomHandler );

    p->restore();
}

void KWTableStylePreview::setTableStyle( KWTableStyle *_tableStyle )
{
    tableStyle = _tableStyle;

    KoTextParag * parag = m_textdoc->firstParag();
    parag->applyStyle( tableStyle->pStyle() );

    repaint(true);
}

/******************************************************************/
/* Class: KWTableStyleListItem                                    */
/******************************************************************/

KWTableStyleListItem::~KWTableStyleListItem()
{
}

void KWTableStyleListItem::switchStyle()
{
    delete m_changedTableStyle;

    if ( m_origTableStyle )
        m_changedTableStyle = new KWTableStyle( *m_origTableStyle );
}

void KWTableStyleListItem::deleteStyle( KWTableStyle *current )
{
    Q_ASSERT( m_changedTableStyle == current );
    delete m_changedTableStyle;
    m_changedTableStyle = 0L;
}

void KWTableStyleListItem::apply()
{
    *m_origTableStyle = *m_changedTableStyle;
}

/******************************************************************/
/* Class: KWTableStyleManager                                     */
/******************************************************************/

// Proof reader comment: stylist sounds like a hair dresser

KWTableStyleManager::KWTableStyleManager( QWidget *_parent, KWDocument *_doc, const QPtrList<KWTableStyle> & style)
    : KDialogBase( _parent, "Tablestylist", true,
                   i18n("Table Style Manager"),
                   KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::Apply | KDialogBase::User1 )
{
    m_doc = _doc;

    QPtrListIterator<KWTableStyle> it( style );

    m_defaultFrameStyle = it.current()->pFrameStyle();
    m_defaultStyle = it.current()->pStyle();

    m_currentTableStyle = 0L;
    noSignals=true;

    m_tableStyles.setAutoDelete(false);

    setupWidget(style); // build the widget with the buttons and the list selector.

    m_stylesList->setCurrentItem( 0 );
    noSignals=false;
    switchStyle();
    setInitialSize( QSize( 450, 450 ) );
    setButtonText( KDialogBase::User1, i18n("Import From File...") );
    connect(this, SIGNAL(user1Clicked()), this, SLOT(importFromFile()));

}

KWTableStyleManager::~KWTableStyleManager()
{
    m_tableStyles.setAutoDelete( true );
    m_tableStyles.clear();
}

void KWTableStyleManager::setupWidget(const QPtrList<KWTableStyle> & styleList)
{
    QFrame * frame1 = makeMainWidget();
    QGridLayout *frame1Layout = new QGridLayout( frame1, 0, 0, // auto
                                                 0, KDialog::spacingHint() );
    QPtrListIterator<KWTableStyle> style( styleList );
    numTableStyles = styleList.count();
    m_stylesList = new QListBox( frame1, "stylesList" );
    for ( ; style.current() ; ++style )
    {
        m_stylesList->insertItem( style.current()->displayName() );
        m_tableStyles.append( new KWTableStyleListItem(style.current(),new KWTableStyle(*style.current()) ) );
        m_styleOrder<<style.current()->name();
    }

    frame1Layout->addMultiCellWidget( m_stylesList, 0, 0, 0, 1 );


    m_moveUpButton = new QPushButton( frame1, "moveUpButton" );
    m_moveUpButton->setIconSet( SmallIconSet( "up" ) );
    connect( m_moveUpButton, SIGNAL( clicked() ), this, SLOT( moveUpStyle() ) );
    frame1Layout->addWidget( m_moveUpButton, 1, 1 );

    m_moveDownButton = new QPushButton( frame1, "moveDownButton" );
    m_moveDownButton->setIconSet( SmallIconSet( "down" ) );
    connect( m_moveDownButton, SIGNAL( clicked() ), this, SLOT( moveDownStyle() ) );
    frame1Layout->addWidget( m_moveDownButton, 1, 0 );


    m_deleteButton = new QPushButton( frame1, "deleteButton" );
    m_deleteButton->setText( i18n( "&Delete" ) );
    connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteStyle() ) );

    frame1Layout->addWidget( m_deleteButton, 2, 1 );

    m_newButton = new QPushButton( frame1, "newButton" );
    m_newButton->setText( i18n( "New" ) );
    connect( m_newButton, SIGNAL( clicked() ), this, SLOT( addStyle() ) );

    frame1Layout->addWidget( m_newButton, 2, 0 );

    main = new QWidget( frame1 );

    setupMain();

    frame1Layout->addMultiCellWidget( main, 0, 2, 2, 2 );

    connect( m_stylesList, SIGNAL( selectionChanged() ), this, SLOT( switchStyle() ) );
}

void KWTableStyleManager::setupMain()
{
    QGridLayout *mainLayout = new QGridLayout( main );
    mainLayout->setSpacing( KDialog::spacingHint() );

    preview = new KWTableStylePreview( i18n("Preview"), i18n("Tablestyles preview"), main );
    preview->resize(preview->sizeHint());

    mainLayout->addMultiCellWidget( preview, 1, 1, 0, 1 );

    m_nameString = new QLineEdit( main );
    m_nameString->resize(m_nameString->sizeHint() );
    connect( m_nameString, SIGNAL( textChanged( const QString &) ), this, SLOT( renameStyle(const QString &) ) );

    mainLayout->addWidget( m_nameString, 0, 1 );

    QLabel *nameLabel = new QLabel( main );
    nameLabel->setText( i18n( "Name:" ) );
    nameLabel->resize(nameLabel->sizeHint());
    nameLabel->setAlignment( AlignRight | AlignVCenter );

    mainLayout->addWidget( nameLabel, 0, 0 );

    QGroupBox *adjustBox = new QGroupBox( 0, Qt::Vertical, i18n("Adjust"), main);
    adjustBox->layout()->setSpacing(KDialog::spacingHint());
    adjustBox->layout()->setMargin(KDialog::marginHint());
    QGridLayout *adjustLayout = new QGridLayout( adjustBox->layout() );

    QLabel *frameStyleLabel = new QLabel( adjustBox );
    frameStyleLabel->setText( i18n( "Framestyle:" ) );
    frameStyleLabel->setAlignment( AlignRight | AlignVCenter );

    QLabel *styleLabel = new QLabel( adjustBox );
    styleLabel->setText( i18n( "Textstyle:" ) );
    styleLabel->setAlignment( AlignRight | AlignVCenter );

    m_frameStyle = new QComboBox( adjustBox );
    m_style = new QComboBox( adjustBox );
    updateAllStyleCombos();

    connect( m_frameStyle, SIGNAL( activated(int) ), this, SLOT( selectFrameStyle(int) ) );
    connect( m_style, SIGNAL( activated(int) ), this, SLOT( selectStyle(int) ) );

    m_changeFrameStyleButton = new QPushButton( adjustBox );
    m_changeFrameStyleButton->setText( i18n( "Change..." ) );
    connect( m_changeFrameStyleButton, SIGNAL( clicked() ), this, SLOT( changeFrameStyle() ) );

    m_changeStyleButton = new QPushButton( adjustBox );
    m_changeStyleButton->setText( i18n( "Change..." ) );
    connect( m_changeStyleButton, SIGNAL( clicked() ), this, SLOT( changeStyle() ) );

    adjustLayout->addWidget( frameStyleLabel, 0, 0 );
    adjustLayout->addWidget( styleLabel, 1, 0 );

    adjustLayout->addWidget( m_frameStyle, 0, 1 );
    adjustLayout->addWidget( m_style, 1, 1 );

    adjustLayout->addWidget( m_changeFrameStyleButton, 0, 2 );
    adjustLayout->addWidget( m_changeStyleButton, 1, 2 );

    adjustBox->setMaximumHeight(120);
    mainLayout->addMultiCellWidget( adjustBox, 2, 2, 0, 1);
}

void KWTableStyleManager::switchStyle()
{
    kdDebug() << "KWTableStyleManager::switchStyle noSignals=" << noSignals << endl;
    if(noSignals) return;
    noSignals=true;

    if(m_currentTableStyle !=0L)
        save();

    m_currentTableStyle = 0L;
    int num = tableStyleIndex( m_stylesList->currentItem() );

    kdDebug() << "KWTableStyleManager::switchStyle switching to " << num << endl;
    if( m_tableStyles.at(num)->origTableStyle() == m_tableStyles.at(num)->changedTableStyle() )
        m_tableStyles.at(num)->switchStyle();
    else
        m_currentTableStyle = m_tableStyles.at(num)->changedTableStyle();

    updateGUI();

    noSignals=false;
}

int KWTableStyleManager::tableStyleIndex( int pos )
{
    int p = 0;
    for(unsigned int i=0; i < m_tableStyles.count(); i++) {
        // Skip deleted styles, they're no in m_stylesList anymore
        KWTableStyle * style = m_tableStyles.at(i)->changedTableStyle();
        if ( !style ) continue;
        if ( p == pos )
            return i;
        ++p;
    }
    kdWarning() << "KWTableStyleManager::tableStyleIndex no style found at pos " << pos << endl;

#ifdef __GNUC_
#warning implement undo/redo
#endif

    return 0;
}

void KWTableStyleManager::updateGUI()
{
    kdDebug() << "KWTableStyleManager::updateGUI m_currentTableStyle=" << m_currentTableStyle << " " << m_currentTableStyle->name() << endl;

    // Update name
    m_nameString->setText(m_currentTableStyle->displayName());
    // Update style and framestyle
    if ( m_doc->styleCollection()->findStyle( m_currentTableStyle->pStyle()->name() ) )
        m_style->setCurrentText(m_currentTableStyle->pStyle()->displayName());
    if ( m_doc->frameStyleCollection()->findFrameStyle( m_currentTableStyle->pFrameStyle()->name() ) )
        m_frameStyle->setCurrentText(m_currentTableStyle->pFrameStyle()->displayName());

    // update delete button (can't delete first style);
    m_deleteButton->setEnabled(m_stylesList->currentItem() != 0);

    m_moveUpButton->setEnabled(m_stylesList->currentItem() != 0);
    m_moveDownButton->setEnabled(m_stylesList->currentItem()!=(int)m_stylesList->count()-1);

    updatePreview();
}

void KWTableStyleManager::updatePreview()
{
    preview->setTableStyle(m_currentTableStyle);
}

void KWTableStyleManager::save()
{
    m_currentTableStyle->setName( m_nameString->text() );
}

void KWTableStyleManager::addStyle()
{
    save();

    QString str = i18n( "New Tablestyle Template (%1)" ).arg(numTableStyles++);
    m_currentTableStyle = new KWTableStyle( str, m_defaultStyle, m_defaultFrameStyle );

    noSignals=true;
    m_tableStyles.append(new KWTableStyleListItem(0L,m_currentTableStyle));
    m_stylesList->insertItem( str );
    m_styleOrder<< str;
    m_stylesList->setCurrentItem( m_stylesList->count() - 1 );
    noSignals=false;

    updateGUI();
}

void KWTableStyleManager::importFromFile()
{
    QStringList lst;
    for (unsigned int i = 0; i<m_stylesList->count();i++)
    {
        lst << m_stylesList->text(i );
    }

    KWImportFrameTableStyleDia dia( m_doc, lst, KWImportFrameTableStyleDia::TableStyle, this, 0 );
    if ( dia.listOfTableStyleImported().count() > 0 && dia.exec() ) {
        QPtrList<KWTableStyle> list = dia.listOfTableStyleImported();
        addStyle( list);
    }
}

void KWTableStyleManager::addStyle(const QPtrList<KWTableStyle> &listStyle )
{
    save();

    QPtrListIterator<KWTableStyle> style( listStyle );
    for ( ; style.current() ; ++style )
    {
        noSignals=true;
        m_stylesList->insertItem( style.current()->displayName() );
        m_styleOrder<<style.current()->name();
        m_tableStyles.append( new KWTableStyleListItem( 0L,new KWTableStyle(*style.current())) );
        noSignals=false;

    }

    updateGUI();
}


void KWTableStyleManager::deleteStyle()
{

    unsigned int cur = tableStyleIndex( m_stylesList->currentItem() );
    m_styleOrder.remove( m_stylesList->currentText());
    if ( !m_tableStyles.at(cur)->origTableStyle() )
        m_tableStyles.take( cur );
    else {
        m_tableStyles.at(cur)->deleteStyle( m_currentTableStyle );
        m_currentTableStyle = 0L;
    }

    // Update GUI
    m_stylesList->removeItem( m_stylesList->currentItem() );
    numTableStyles--;
    m_stylesList->setSelected( m_stylesList->currentItem(), true );
}

void KWTableStyleManager::moveUpStyle()
{
    if(m_currentTableStyle !=0L)
        save();

    unsigned int pos = 0;
    QString currentStyleName=m_stylesList->currentText ();
    if ( currentStyleName.isEmpty() )
        return;
    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 != -1 )
    {
        m_styleOrder.remove( m_styleOrder.at(pos2));
        m_styleOrder.insert( m_styleOrder.at(pos2-1), currentStyleName);
    }


    pos=m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text ( pos-1 ),pos);

    m_stylesList->changeItem( currentStyleName ,pos-1);

    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWTableStyleManager::moveDownStyle()
{
    if(m_currentTableStyle !=0L)
        save();

    unsigned int pos = 0;
    QString currentStyleName=m_stylesList->currentText ();
    if ( currentStyleName.isEmpty() )
        return;
    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 != -1 )
    {
        m_styleOrder.remove( m_styleOrder.at(pos2));
        m_styleOrder.insert( m_styleOrder.at(pos2+1), currentStyleName);
    }

    pos=m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text ( pos+1 ),pos);
    m_stylesList->changeItem( currentStyleName ,pos+1);
    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWTableStyleManager::slotOk() {
    save();
    apply();
    KDialogBase::slotOk();
}

void KWTableStyleManager::slotApply() {
    save();
    apply();
    KDialogBase::slotApply();
}

void KWTableStyleManager::apply() {
    noSignals=true;
    for (unsigned int i =0 ; i < m_tableStyles.count() ; i++) {
        if(m_tableStyles.at(i)->origTableStyle() == 0) {           // newly added style
            kdDebug() << "adding new tablestyle" << m_tableStyles.at(i)->changedTableStyle()->name() << " (" << i << ")" << endl;
            KWTableStyle *tmp = addTableStyleTemplate(m_tableStyles.take(i)->changedTableStyle());
            m_tableStyles.insert(i, new KWTableStyleListItem(0, tmp) );
        } else if(m_tableStyles.at(i)->changedTableStyle() == 0) { // deleted style
            kdDebug() << "deleting orig tablestyle " << m_tableStyles.at(i)->origTableStyle()->name() << " (" << i << ")" << endl;

            KWTableStyle *orig = m_tableStyles.at(i)->origTableStyle();
            removeTableStyleTemplate( orig );
        } else {
            kdDebug() << "update tablestyle " << m_tableStyles.at(i)->changedTableStyle()->name() << " (" << i << ")" << endl;

            m_tableStyles.at(i)->apply();
        }
    }
    updateTableStyleListOrder( m_styleOrder);
    updateAllStyleLists();
    noSignals=false;
}

void KWTableStyleManager::renameStyle(const QString &theText) {
    if(noSignals) return;
    noSignals=true;

    int index = m_stylesList->currentItem();
    kdDebug() << "KWTableStyleManager::renameStyle " << index << " to " << theText << endl;

    // rename only in the GUI, not even in the underlying objects (save() does it).
    m_stylesList->changeItem( theText, index );
    m_styleOrder[index]=theText;
    // Check how many styles with that name we have now
    int synonyms = 0;
    for ( unsigned int i = 0; i < m_stylesList->count(); i++ ) {
        if ( m_stylesList->text( i ) == m_stylesList->currentText() )
            ++synonyms;
    }
    Q_ASSERT( synonyms > 0 ); // should have found 'index' at least !
    noSignals=false;
    // Can't close the dialog if two styles have the same name
    bool state=!theText.isEmpty() && (synonyms == 1);
    enableButtonOK(state );
    enableButtonApply(state);
    enableButton( KDialogBase::User1, state );
    m_deleteButton->setEnabled(state&&(m_stylesList->currentItem() != 0));
    m_newButton->setEnabled(state);
    m_stylesList->setEnabled( state );
    if ( state )
    {
        m_moveUpButton->setEnabled(m_stylesList->currentItem() != 0);
        m_moveDownButton->setEnabled(m_stylesList->currentItem()!=(int)m_stylesList->count()-1);
    }
    else
    {
        m_moveUpButton->setEnabled(false);
        m_moveDownButton->setEnabled(false);
    }

}

/** Show the framestylist and apply changes to the current selected tablestyle.
 */
void KWTableStyleManager::changeFrameStyle()
{
// 0. Save name, otherwise it will be gone when you return
    save();

// 1. Execute frameStylist
    KWFrameStyleManager *frameStylist = new KWFrameStyleManager( this, m_doc, m_doc->frameStyleCollection()->frameStyleList() );

    frameStylist->exec();

// 2. Apply changes
    updateAllStyleCombos();
    updateGUI();

// 3. Cleanup
    delete frameStylist;
}

/** Show the stylist and apply changes to the current selected tablestyle.
 */
void KWTableStyleManager::changeStyle()
{
// 0. Save name, otherwise it will be gone when you return
    save();

// 1. Execute stylist
    KWStyleManager * styleManager = new KWStyleManager( this, m_doc->unit(),m_doc, m_doc->styleCollection()->styleList());
    styleManager->exec();

// 2. Apply changes
    updateAllStyleCombos();
    updateGUI();

// 3. Cleanup
    delete styleManager;
}

void KWTableStyleManager::updateAllStyleCombos()
{
    unsigned int oldSindex = 0;
    unsigned int oldFSindex = 0;
    QString oldS = "";
    QString oldFS = "";

    if (m_style->currentItem()>=0) {
        oldSindex = m_style->currentItem();
        oldS = m_style->currentText();
    }
    if (m_frameStyle->currentItem()>=0) {
        oldFSindex = m_frameStyle->currentItem();
        oldFS = m_frameStyle->currentText();
    }

    // Let's check if there were deleted framestyles or styles.
    // If that is the case than we search for the old name
    // If it exists then we use that index, else we use
    // the old index.
    // Problems:
    // - 1. Count is the same, but the user has delete the same number as he added
    // - 2. Old name is not in new list, old index is wrong index in new list
    if ( ( static_cast<unsigned int>(m_style->count())!=m_doc->styleCollection()->styleList().count() ) &&
            ( m_style->listBox()->findItem( oldS ) ) ) {
        oldSindex = m_style->listBox()->index( m_style->listBox()->findItem( oldS ) );
    }
    if ( ( static_cast<unsigned int>(m_frameStyle->count())!=m_doc->frameStyleCollection()->frameStyleList().count() ) &&
            ( m_frameStyle->listBox()->findItem( oldFS ) ) ) {
        oldFSindex = m_frameStyle->listBox()->index( m_frameStyle->listBox()->findItem( oldFS ) );
    }

    // Update the comboboxes

    m_frameStyle->clear();
    QPtrListIterator<KWFrameStyle> styleIt( m_doc->frameStyleCollection()->frameStyleList() );
    for ( int pos = 0 ; styleIt.current(); ++styleIt, ++pos )
    {
        m_frameStyle->insertItem( styleIt.current()->name() );
    }
    m_frameStyle->setCurrentItem( oldFSindex );

    m_style->clear();
    QPtrListIterator<KWStyle> styleIt2( m_doc->styleCollection()->styleList() );
    for ( int pos = 0 ; styleIt2.current(); ++styleIt2, ++pos )
    {
        m_style->insertItem( styleIt2.current()->name() );
    }
    m_style->setCurrentItem( oldSindex );
}

void KWTableStyleManager::selectFrameStyle(int index)
{
    kdDebug() << "KWTableStyleManager::selectFrameStyle index " << index << endl;

    if ( (index>=0) && ( index < (int)m_doc->frameStyleCollection()->frameStyleList().count() ) )
        m_currentTableStyle->setFrameStyle( m_doc->frameStyleCollection()->frameStyleAt(index) );
    save();
    updateGUI();
}

void KWTableStyleManager::selectStyle(int index)
{
    kdDebug() << "KWTableStyleManager::selectStyle index " << index << endl;
    if ( (index>=0) && ( index < (int)m_doc->styleCollection()->styleList().count() ) )
        m_currentTableStyle->setStyle( m_doc->styleCollection()->styleAt(index) );
    save();
    updateGUI();
}

KWTableStyle* KWTableStyleManager::addTableStyleTemplate(KWTableStyle *style)
{
    return m_doc->tableStyleCollection()->addTableStyleTemplate(style);
}

void KWTableStyleManager::removeTableStyleTemplate( KWTableStyle *style )
{
    m_doc->tableStyleCollection()->removeTableStyleTemplate(style);
}

void KWTableStyleManager::updateTableStyleListOrder( const QStringList &list )
{
    m_doc->tableStyleCollection()->updateTableStyleListOrder( list );
}

void KWTableStyleManager::updateAllStyleLists()
{
    m_doc->updateAllTableStyleLists();
}

