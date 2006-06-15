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

#include "KWTableStyleManager.h"
#include "KWTableStyleManager.moc"
#include "KWImportStyleDia.h"

#include "KWDocument.h"

#include <KoParagCounter.h>
#include <KoTextDocument.h>
#include <KoTextParag.h>

#include "KoParagStyle.h"
#include "KWFrameStyleManager.h"
#include "KWStyleManager.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <QPushButton>
#include <QLabel>
#include <q3groupbox.h>
#include <QPainter>
#include <QLineEdit>
#include <QLayout>
#include <QComboBox>
#include <q3frame.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3PtrList>
#include <QList>


/******************************************************************/
/* Class: KWTableStylePreview                                     */
/******************************************************************/

KWTableStylePreview::KWTableStylePreview( const QString& title, const QString& text, QWidget* parent, const char* name )
    : Q3GroupBox( title, parent, name )
{
    m_zoomHandler = new KoTextZoomHandler;
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

#undef ptToPx
#define ptToPx qRound

void KWTableStylePreview::drawContents( QPainter *p )
{
    p->save();
    QRect r = contentsRect();

    int wid = ( r.width() - 40 );
    int hei = ( r.height() - 40 );


    // 1: create document background = white

    p->fillRect( QRect( 10, 20, r.width()-20, r.height() - 20 ), QColor("white") );

    // 2: create borders (KWFrameStyle)

    if (tableStyle->frameStyle()->topBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->frameStyle()->topBorder(), ptToPx( tableStyle->frameStyle()->topBorder().width() ), Qt::black) ); // Top border
        p->drawLine( 20 - int(tableStyle->frameStyle()->leftBorder().width()/2), 30,
                     20 + wid + int(tableStyle->frameStyle()->rightBorder().width()/2), 30 );
    }
    if (tableStyle->frameStyle()->leftBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->frameStyle()->leftBorder(), ptToPx( tableStyle->frameStyle()->leftBorder().width() ), Qt::black) ); // Left border
        p->drawLine( 20, 30 - int(tableStyle->frameStyle()->topBorder().width()/2),
                     20 , 30 + hei + int(tableStyle->frameStyle()->bottomBorder().width()/2) );
    }
    if (tableStyle->frameStyle()->bottomBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->frameStyle()->bottomBorder(), ptToPx( tableStyle->frameStyle()->bottomBorder().width() ), Qt::black) ); // Bottom border
        p->drawLine( 20 + wid + int(ceil(tableStyle->frameStyle()->rightBorder().width()/2)), 30 + hei,
                     20 - int(tableStyle->frameStyle()->leftBorder().width()/2), 30 + hei );
    }
    if (tableStyle->frameStyle()->rightBorder().width()>0) {
        p->setPen( KoBorder::borderPen(tableStyle->frameStyle()->rightBorder(), ptToPx( tableStyle->frameStyle()->rightBorder().width() ), Qt::black) ); // Right border
        p->drawLine( 20 + wid, 30 - int(tableStyle->frameStyle()->topBorder().width()/2) ,
                     20 + wid, 30 + hei + int(tableStyle->frameStyle()->bottomBorder().width()/2) );
    }

   // 3: create background whithin "frame"

    QRect fr( QPoint(20 + int(ceil(tableStyle->frameStyle()->leftBorder().width()/2)), 30 + int(ceil(tableStyle->frameStyle()->topBorder().width()/2))),
              QPoint(20 + wid - int(floor(tableStyle->frameStyle()->rightBorder().width()/2)+1), 30 + hei - int(floor(tableStyle->frameStyle()->bottomBorder().width()/2)+1)) );

    p->fillRect( fr, tableStyle->frameStyle()->backgroundColor() );

   // 4: create text (KoParagStyle)

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
    cg.setBrush( QColorGroup::Base, tableStyle->frameStyle()->backgroundColor() );

    m_textdoc->drawWYSIWYG( p, 1, 0, textRect.width() - 1, textRect.height(), cg, m_zoomHandler );

    p->restore();
}

void KWTableStylePreview::setTableStyle( KWTableStyle *_tableStyle )
{
    tableStyle = _tableStyle;

    KoTextParag * parag = m_textdoc->firstParag();
    parag->applyStyle( tableStyle->paragraphStyle() );

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

KWTableStyleManager::KWTableStyleManager( QWidget *_parent, KWDocument *_doc )
    : KDialog( _parent )
{
    setCaption( i18n("Table Style Manager") );
    setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::User1 );
    setDefaultButton( KDialog::Ok );
    m_doc = _doc;

    m_currentTableStyle = 0L;
    noSignals=true;

    m_tableStyles.setAutoDelete(false);

    setupWidget(); // build the widget with the buttons and the list selector.

    m_stylesList->setCurrentItem( 0 );
    noSignals=false;
    switchStyle();
    setInitialSize( QSize( 450, 450 ) );
    setButtonText( KDialog::User1, i18n("Import From File...") );
    connect(this, SIGNAL(user1Clicked()), this, SLOT(importFromFile()));

}

KWTableStyleManager::~KWTableStyleManager()
{
    m_tableStyles.setAutoDelete( true );
    m_tableStyles.clear();
}

void KWTableStyleManager::setupWidget()
{
    QFrame * frame1 = new QFrame( this );
    setMainWidget( frame1 );
    Q3GridLayout *frame1Layout = new Q3GridLayout( frame1, 0, 0, // auto
                                                 0, KDialog::spacingHint() );


    KWTableStyleCollection* collection = m_doc->tableStyleCollection();
    numTableStyles = collection->count();
    m_stylesList = new Q3ListBox( frame1, "stylesList" );
    m_stylesList->insertStringList( collection->displayNameList() );
    const QList<KoUserStyle*> styleList = collection->styleList();
    Q_ASSERT( !styleList.isEmpty() );
    for ( QList<KoUserStyle *>::const_iterator it = styleList.begin(), end = styleList.end();
          it != end ; ++it )
    {
        KWTableStyle* style = static_cast<KWTableStyle *>( *it );
        m_tableStyles.append( new KWTableStyleListItem(style,new KWTableStyle(*style) ) );
        m_styleOrder << style->name();
    }
    Q_ASSERT( m_stylesList->count() == m_styleOrder.count() );
    Q_ASSERT( m_styleOrder.count() == m_tableStyles.count() );

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
    Q3GridLayout *mainLayout = new Q3GridLayout( main );
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
    nameLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

    mainLayout->addWidget( nameLabel, 0, 0 );

    Q3GroupBox *adjustBox = new Q3GroupBox( 0, Qt::Vertical, i18n("Adjust"), main);
    adjustBox->layout()->setSpacing(KDialog::spacingHint());
    adjustBox->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout *adjustLayout = new Q3GridLayout( adjustBox->layout() );

    QLabel *frameStyleLabel = new QLabel( adjustBox );
    frameStyleLabel->setText( i18n( "Framestyle:" ) );
    frameStyleLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

    QLabel *styleLabel = new QLabel( adjustBox );
    styleLabel->setText( i18n( "Textstyle:" ) );
    styleLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

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
    kDebug() << "KWTableStyleManager::switchStyle noSignals=" << noSignals << endl;
    if(noSignals) return;
    noSignals=true;

    if(m_currentTableStyle !=0L)
        save();

    m_currentTableStyle = 0L;
    int num = tableStyleIndex( m_stylesList->currentItem() );

    kDebug() << "KWTableStyleManager::switchStyle switching to " << num << endl;
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
    kWarning() << "KWTableStyleManager::tableStyleIndex no style found at pos " << pos << endl;

#ifdef __GNUC_
#warning implement undo/redo
#endif

    return 0;
}

void KWTableStyleManager::updateGUI()
{
    kDebug() << "KWTableStyleManager::updateGUI m_currentTableStyle=" << m_currentTableStyle << " " << m_currentTableStyle->name() << endl;

    // Update name
    m_nameString->setText(m_currentTableStyle->displayName());
    // Update style and framestyle
    if ( m_doc->styleCollection()->findStyle( m_currentTableStyle->paragraphStyle()->name() ) )
        m_style->setCurrentText(m_currentTableStyle->paragraphStyle()->displayName());
    if ( m_doc->frameStyleCollection()->findStyle( m_currentTableStyle->frameStyle()->name() ) )
        m_frameStyle->setCurrentText(m_currentTableStyle->frameStyle()->displayName());

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
    m_currentTableStyle->setDisplayName( m_nameString->text() );
}

void KWTableStyleManager::addStyle()
{
    save();
    KWTableStyleCollection* collection = m_doc->tableStyleCollection();

    QString str = i18n( "New Tablestyle Template (%1)" ,numTableStyles++);
    if ( m_currentTableStyle )
    {
        m_currentTableStyle = new KWTableStyle( *m_currentTableStyle );
        m_currentTableStyle->setDisplayName( str );
    }
    else
    {
        KWTableStyle* defaultTableStyle = collection->findStyle( collection->defaultStyleName() );
        Q_ASSERT( defaultTableStyle ); // can't be 0 except if there are no styles at all

        KWFrameStyle *defaultFrameStyle = defaultTableStyle->frameStyle();
        KoParagStyle *defaultParagraphStyle = defaultTableStyle->paragraphStyle();

        m_currentTableStyle = new KWTableStyle( str, defaultParagraphStyle, defaultFrameStyle );
    }
    m_currentTableStyle->setName( collection->generateUniqueName() );

    noSignals=true;
    m_tableStyles.append(new KWTableStyleListItem(0L,m_currentTableStyle));
    m_stylesList->insertItem( str );
    m_styleOrder << m_currentTableStyle->name();
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
        Q3PtrList<KWTableStyle> list = dia.listOfTableStyleImported();
        addStyles( list);
    }
}

void KWTableStyleManager::addStyles(const Q3PtrList<KWTableStyle> &listStyle )
{
    save();

    Q3PtrListIterator<KWTableStyle> style( listStyle );
    for ( ; style.current() ; ++style )
    {
        noSignals=true;
        m_stylesList->insertItem( style.current()->displayName() );
        m_styleOrder << style.current()->name();
        m_tableStyles.append( new KWTableStyleListItem( 0L,new KWTableStyle(*style.current())) );
        noSignals=false;

    }

    updateGUI();
}


void KWTableStyleManager::deleteStyle()
{
    Q_ASSERT( m_currentTableStyle );

    unsigned int cur = tableStyleIndex( m_stylesList->currentItem() );
    m_styleOrder.remove( m_currentTableStyle->name() );
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
    Q_ASSERT( m_currentTableStyle );
    if ( m_currentTableStyle )
        save();

    const QString currentStyleName = m_currentTableStyle->name();
    const QString currentStyleDisplayName = m_stylesList->currentText();
    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 != -1 )
    {
        m_styleOrder.removeAt( pos2 );
        m_styleOrder.insert( pos2-1, currentStyleName );
    }


    int pos = m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text( pos-1 ), pos );

    m_stylesList->changeItem( currentStyleDisplayName, pos-1 );

    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWTableStyleManager::moveDownStyle()
{
    Q_ASSERT( m_currentTableStyle );
    if ( m_currentTableStyle )
        save();

    const QString currentStyleName = m_currentTableStyle->name();
    const QString currentStyleDisplayName = m_stylesList->currentText();
    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 != -1 )
    {
        m_styleOrder.removeAt( pos2 );
        m_styleOrder.insert( pos2+1, currentStyleName );
    }

    int pos = m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text( pos+1 ), pos );
    m_stylesList->changeItem( currentStyleDisplayName, pos+1 );
    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWTableStyleManager::slotOk() {
    save();
    apply();
    KDialog::accept();
}

void KWTableStyleManager::slotApply() {
    save();
    apply();
#warning "kde4 port it"
    //KDialog::slotApply();
}

void KWTableStyleManager::apply() {
    noSignals=true;
    for (unsigned int i =0 ; i < m_tableStyles.count() ; i++) {
        if(m_tableStyles.at(i)->origTableStyle() == 0) {           // newly added style
            kDebug() << "adding new tablestyle" << m_tableStyles.at(i)->changedTableStyle()->name() << " (" << i << ")" << endl;
            KWTableStyle *tmp = m_doc->tableStyleCollection()->addStyle( m_tableStyles.take(i)->changedTableStyle() );
            m_tableStyles.insert(i, new KWTableStyleListItem(0, tmp) );
        } else if(m_tableStyles.at(i)->changedTableStyle() == 0) { // deleted style
            kDebug() << "deleting orig tablestyle " << m_tableStyles.at(i)->origTableStyle()->name() << " (" << i << ")" << endl;

            KWTableStyle *orig = m_tableStyles.at(i)->origTableStyle();
            m_doc->tableStyleCollection()->removeStyle( orig );
        } else {
            kDebug() << "update tablestyle " << m_tableStyles.at(i)->changedTableStyle()->name() << " (" << i << ")" << endl;

            m_tableStyles.at(i)->apply();
        }
    }
    m_doc->tableStyleCollection()->updateStyleListOrder( m_styleOrder );
    m_doc->updateAllTableStyleLists();
    m_doc->setModified( true );
    noSignals=false;
}

void KWTableStyleManager::renameStyle(const QString &theText) {
    if(noSignals) return;
    noSignals=true;

    int index = m_stylesList->currentItem();
    kDebug() << "KWTableStyleManager::renameStyle " << index << " to " << theText << endl;

    // rename only in the GUI, not even in the underlying objects (save() does it).
    m_stylesList->changeItem( theText, index );
    //m_styleOrder[index]=theText; // not needed anymore, we use internal names
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
    enableButton( KDialog::User1, state );
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

/** Show the frame-style-manager and apply changes to the current selected tablestyle.
 */
void KWTableStyleManager::changeFrameStyle()
{
// 0. Save name, otherwise it will be gone when you return
    save();

// 1. Execute frame style manager
    KWFrameStyleManager frameStylist( this, m_doc, m_currentTableStyle->frameStyle()->name() );
    frameStylist.exec();

// 2. Apply changes
    updateAllStyleCombos();
    updateGUI();
}

/** Show the stylist and apply changes to the current selected tablestyle.
 */
void KWTableStyleManager::changeStyle()
{
// 0. Save name, otherwise it will be gone when you return
    save();

// 1. Execute stylist
    KWStyleManager styleManager( this, m_doc->unit(), m_doc, *m_doc->styleCollection());
    styleManager.exec();

// 2. Apply changes
    updateAllStyleCombos();
    updateGUI();
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
    if ( ( m_style->count() != m_doc->styleCollection()->styleList().count() ) &&
            ( m_style->findText( oldS ) != -1 ) ) {
        oldSindex = m_style->findText( oldS );
    }
    if ( ( m_frameStyle->count() != m_doc->frameStyleCollection()->count() ) &&
            ( m_frameStyle->findText( oldFS ) != -1 ) ) {
        oldFSindex = m_frameStyle->findText( oldFS );
    }

    // Update the comboboxes

    m_frameStyle->clear();
    m_frameStyle->insertStringList( m_doc->frameStyleCollection()->displayNameList() );
    m_frameStyle->setCurrentItem( oldFSindex );

    m_style->clear();
    m_style->insertStringList( m_doc->styleCollection()->displayNameList() );
    m_style->setCurrentItem( oldSindex );
}

void KWTableStyleManager::selectFrameStyle(int index)
{
    kDebug() << "KWTableStyleManager::selectFrameStyle index " << index << endl;

    if ( (index>=0) && ( index < (int)m_doc->frameStyleCollection()->count() ) )
        m_currentTableStyle->setFrameStyle( m_doc->frameStyleCollection()->frameStyleAt(index) );
    save();
    updateGUI();
}

void KWTableStyleManager::selectStyle(int index)
{
    kDebug() << "KWTableStyleManager::selectStyle index " << index << endl;
    if ( (index>=0) && ( index < (int)m_doc->styleCollection()->styleList().count() ) )
        m_currentTableStyle->setParagraphStyle( m_doc->styleCollection()->styleAt(index) );
    save();
    updateGUI();
}
