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

#include "KWFrameStyleManager.h"
#include "KWFrameStyleManager.moc"
#include "KWImportStyleDia.h"

#include "KWDocument.h"
#include "KWFrameDia.h" // for KWBrushStylePreview

#include <KoParagDia.h>

#include <kcolorbutton.h>
#include <klocale.h>
#include <kiconloader.h>

#include <qtabwidget.h>
#include <QPushButton>
#include <QLabel>
#include <qbrush.h>
#include <q3groupbox.h>
#include <qpainter.h>
#include <QLayout>
//Added by qt3to4:
#include <QPaintEvent>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <Q3Frame>
#include <QList>
#include <QResizeEvent>

/******************************************************************/
/* Class: KWTableStylePreview                                     */
/******************************************************************/

#undef ptToPx
#define ptToPx qRound

void KWFrameStylePreview::paintEvent( QPaintEvent * )
{
    int wid = ( width() - 20 );
    int hei = ( height() - 20 );

    QPainter p;
    p.begin( this );

    // 1: create borders
    if (frameStyle->topBorder().width()>0) {
      p.setPen( KoBorder::borderPen(frameStyle->topBorder(), ptToPx( frameStyle->topBorder().width() ), Qt::black) ); // Top border
      p.drawLine( 10 - int(frameStyle->leftBorder().width()/2), 10,
                  10 + wid + int(frameStyle->rightBorder().width()/2), 10 );
    }
    if (frameStyle->leftBorder().width()>0) {
      p.setPen( KoBorder::borderPen(frameStyle->leftBorder(), ptToPx( frameStyle->leftBorder().width() ), Qt::black) ); // Left border
      p.drawLine( 10, 10 - int(frameStyle->topBorder().width()/2),
                  10 , 10 + hei + int(frameStyle->bottomBorder().width()/2) );
    }
    if (frameStyle->bottomBorder().width()>0) {
      p.setPen( KoBorder::borderPen(frameStyle->bottomBorder(), ptToPx( frameStyle->bottomBorder().width() ), Qt::black) ); // Bottom border
      p.drawLine( 10 + wid + int(ceil(frameStyle->rightBorder().width()/2)), 10 + hei,
                  10 - int(frameStyle->leftBorder().width()/2), 10 + hei );
    }
    if (frameStyle->rightBorder().width()>0) {
      p.setPen( KoBorder::borderPen(frameStyle->rightBorder(), ptToPx( frameStyle->rightBorder().width() ), Qt::black) ); // Right border
      p.drawLine( 10 + wid, 10 - int(frameStyle->topBorder().width()/2) ,
                  10 + wid, 10 + hei + int(frameStyle->bottomBorder().width()/2) );
    }

   // 2.1: create background

    p.fillRect( QRect( QPoint(10 + int(ceil(frameStyle->leftBorder().width()/2)), 10 + int(ceil(frameStyle->topBorder().width()/2))),
                      QPoint(10 + wid - int(floor(frameStyle->rightBorder().width()/2)+1), 10 + hei - int(floor(frameStyle->bottomBorder().width()/2)+1)) ),
                frameStyle->backgroundColor() );

    p.end();
}

void KWFrameStylePreview::setFrameStyle( KWFrameStyle *_frameStyle )
{
    if (!frameStyle) frameStyle = new KWFrameStyle("preview");

    frameStyle = _frameStyle;

    repaint( true );
}

/******************************************************************/
/* Class: KWFrameStyleListItem                                    */
/******************************************************************/

KWFrameStyleListItem::~KWFrameStyleListItem()
{
    delete m_changedFrameStyle;
}

void KWFrameStyleListItem::switchStyle()
{
    delete m_changedFrameStyle;
    m_changedFrameStyle = 0;

    if ( m_origFrameStyle )
        m_changedFrameStyle = new KWFrameStyle( *m_origFrameStyle );
}

void KWFrameStyleListItem::deleteStyle( KWFrameStyle *current )
{
    Q_ASSERT( m_changedFrameStyle == current );
    delete m_changedFrameStyle;
    m_changedFrameStyle =  0;
}

void KWFrameStyleListItem::apply()
{
    *m_origFrameStyle = *m_changedFrameStyle;
}

/******************************************************************/
/* Class: KWFrameStyleManager                                     */
/******************************************************************/

KWFrameStyleManager::KWFrameStyleManager( QWidget *_parent, KWDocument *_doc,
                                          const QString & activeStyleName )
    : KDialogBase( _parent, "", true,
                   i18n("Frame Style Manager"),
                   KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::Apply| KDialogBase::User1 )
{
    m_doc = _doc;

    m_currentFrameStyle = 0;
    noSignals=true;

    m_frameStyles.setAutoDelete(false);

    setupWidget(); // build the widget with the buttons and the list selector.

    addGeneralTab();

    KWFrameStyleBordersTab *bordersTab = new KWFrameStyleBordersTab( m_tabs );
    bordersTab->setWidget( new KoParagDecorationWidget( bordersTab ) );
    addTab( bordersTab );

    KWFrameStyleBackgroundTab *bgTab = new KWFrameStyleBackgroundTab( m_tabs );
    addTab( bgTab );

    Q3ListBoxItem * item = m_stylesList->findItem( activeStyleName );
    m_stylesList->setCurrentItem( item ? m_stylesList->index(item) : 0 );

    noSignals=false;
    switchStyle();
    setInitialSize( QSize( 600, 370 ) );
    setButtonText( KDialogBase::User1, i18n("Import From File...") );
    connect(this, SIGNAL(user1Clicked()), this, SLOT(importFromFile()));
}

KWFrameStyleManager::~KWFrameStyleManager()
{
    m_frameStyles.setAutoDelete( true );
    m_frameStyles.clear();
}

void KWFrameStyleManager::addTab( KWFrameStyleManagerTab * tab )
{
    m_tabsList.append( tab );
    m_tabs->insertTab( tab, tab->tabName() );
}

void KWFrameStyleManager::setupWidget()
{
    QFrame * frame1 = makeMainWidget();
    Q3GridLayout *frame1Layout = new Q3GridLayout( frame1, 0, 0, // auto
                                                 0, KDialog::spacingHint() );
    KWFrameStyleCollection* collection = m_doc->frameStyleCollection();
    numFrameStyles = collection->count();
    m_stylesList = new Q3ListBox( frame1, "stylesList" );
    m_stylesList->insertStringList( collection->displayNameList() );
    const QList<KoUserStyle*> styleList = collection->styleList();
    for ( QList<KoUserStyle *>::const_iterator it = styleList.begin(), end = styleList.end();
          it != end ; ++it )
    {
        KWFrameStyle* style = static_cast<KWFrameStyle *>( *it );
        m_frameStyles.append( new KWFrameStyleListItem( style, new KWFrameStyle(*style) ) );
        m_styleOrder << style->name();
    }
    Q_ASSERT( m_stylesList->count() == m_styleOrder.count() );
    Q_ASSERT( m_styleOrder.count() == m_frameStyles.count() );

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

    m_tabs = new QTabWidget( frame1 );
    frame1Layout->addMultiCellWidget( m_tabs, 0, 2, 2, 2 );

    connect( m_stylesList, SIGNAL( selectionChanged() ), this, SLOT( switchStyle() ) );
    connect( m_tabs, SIGNAL( currentChanged ( QWidget * ) ), this, SLOT( switchTabs() ) );
}

void KWFrameStyleManager::addGeneralTab()
{
    QWidget *tab = new QWidget( m_tabs );

    Q3GridLayout *tabLayout = new Q3GridLayout( tab );
    tabLayout->setSpacing( KDialog::spacingHint() );
    tabLayout->setMargin( KDialog::marginHint() );

    previewBox = new Q3GroupBox( 0, Qt::Vertical, i18n( "Preview" ), tab );
    previewBox->layout()->setSpacing(KDialog::spacingHint());
    previewBox->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout *previewLayout = new Q3GridLayout( previewBox->layout() );

    preview = new KWFrameStylePreview( previewBox );
    preview->resize(preview->sizeHint());

    previewLayout->addWidget( preview, 0, 0 );

    tabLayout->addMultiCellWidget( previewBox, 1, 1, 0, 1 );

    m_nameString = new QLineEdit( tab );
    m_nameString->resize(m_nameString->sizeHint() );
    connect( m_nameString, SIGNAL( textChanged( const QString &) ), this, SLOT( renameStyle(const QString &) ) );

    tabLayout->addWidget( m_nameString, 0, 1 );

    QLabel *nameLabel = new QLabel( tab );
    nameLabel->setText( i18n( "Name:" ) );
    nameLabel->resize(nameLabel->sizeHint());
    nameLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

    tabLayout->addWidget( nameLabel, 0, 0 );

    m_tabs->insertTab( tab, i18n( "General" ) );
}

void KWFrameStyleManager::switchStyle()
{
    kDebug() << "KWFrameStyleManager::switchStyle noSignals=" << noSignals << endl;
    if(noSignals) return;
    noSignals=true;

    if(m_currentFrameStyle !=0L)
        save();

    m_currentFrameStyle = 0L;
    int num = frameStyleIndex( m_stylesList->currentItem() );

    kDebug() << "KWFrameStyleManager::switchStyle switching to " << num << endl;
    if( m_frameStyles.at(num)->origFrameStyle() == m_frameStyles.at(num)->changedFrameStyle() )
        m_frameStyles.at(num)->switchStyle();
    else
        m_currentFrameStyle = m_frameStyles.at(num)->changedFrameStyle();

    updateGUI();

    noSignals=false;
}

void KWFrameStyleManager::switchTabs()
{
    save();
    updatePreview();
}

int KWFrameStyleManager::frameStyleIndex( int pos ) {
    int p = 0;
    for(unsigned int i=0; i < m_frameStyles.count(); i++) {
        // Skip deleted styles, they're no in m_stylesList anymore
        KWFrameStyle * style = m_frameStyles.at(i)->changedFrameStyle();
        if ( !style ) continue;
        if ( p == pos )
            return i;
        ++p;
    }
    kWarning() << "KWFrameStyleManager::frameStyleIndex no style found at pos " << pos << " count=" << m_frameStyles.count() << endl;

#ifdef __GNUC_
#warning implement undo/redo
#endif

    return 0;
}

void KWFrameStyleManager::updateGUI()
{
    kDebug() << "KWFrameStyleManager::updateGUI m_currentFrameStyle=" << m_currentFrameStyle << " " << m_currentFrameStyle->name() << endl;
    Q3PtrListIterator<KWFrameStyleManagerTab> it( m_tabsList );
    for ( ; it.current() ; ++it )
    {
        it.current()->setStyle( m_currentFrameStyle );
        it.current()->update();
    }

    m_nameString->setText(m_currentFrameStyle->displayName());

    // update delete button (can't delete first style);
    m_deleteButton->setEnabled(m_stylesList->currentItem() != 0);

    m_moveUpButton->setEnabled(m_stylesList->currentItem() != 0);
    m_moveDownButton->setEnabled(m_stylesList->currentItem()!=(int)m_stylesList->count()-1);

    updatePreview();
}

void KWFrameStyleManager::updatePreview()
{
    preview->setFrameStyle(m_currentFrameStyle);
}

void KWFrameStyleManager::save() {
    if(m_currentFrameStyle) {
        // save changes from UI to object.
        Q3PtrListIterator<KWFrameStyleManagerTab> it( m_tabsList );
        for ( ; it.current() ; ++it )
            it.current()->save();

        m_currentFrameStyle->setDisplayName( m_nameString->text() );
    }
}

void KWFrameStyleManager::importFromFile()
{
    // ## why not just use collection->displayNameList()?
    QStringList lst;
    for ( int i = 0; i<(int)m_stylesList->count();i++)
    {
        lst << m_stylesList->text(i);
    }

    KWImportFrameTableStyleDia dia( m_doc, lst, KWImportFrameTableStyleDia::frameStyle, this, 0 );
    if ( dia.listOfFrameStyleImported().count() > 0 && dia.exec() ) {
        addStyles( dia.listOfFrameStyleImported() );
    }
}

void KWFrameStyleManager::addStyles( const Q3PtrList<KWFrameStyle> &listStyle )
{
    save();

    Q3PtrListIterator<KWFrameStyle> style( listStyle );
    for ( ; style.current() ; ++style )
    {
        noSignals=true;
        m_stylesList->insertItem( style.current()->displayName() );
        m_frameStyles.append( new KWFrameStyleListItem( 0L,new KWFrameStyle(*style.current())) );
        m_styleOrder<<style.current()->name();
        noSignals=false;

    }
    updateGUI();
}

void KWFrameStyleManager::addStyle()
{
    save();

    QString str = i18n( "New Framestyle Template (%1)" ).arg(numFrameStyles++);
    if ( m_currentFrameStyle )
    {
        m_currentFrameStyle = new KWFrameStyle( *m_currentFrameStyle ); // Create a new style, initializing from the current one
        m_currentFrameStyle->setDisplayName( str );
        m_currentFrameStyle->setName( m_doc->frameStyleCollection()->generateUniqueName() );
    }
    else
        m_currentFrameStyle = new KWFrameStyle( str );

    noSignals=true;
    m_frameStyles.append(new KWFrameStyleListItem(0L,m_currentFrameStyle));
    m_stylesList->insertItem( str );
    m_styleOrder<< str;
    m_stylesList->setCurrentItem( m_stylesList->count() - 1 );
    noSignals=false;

    updateGUI();
}

void KWFrameStyleManager::deleteStyle()
{
    unsigned int cur = frameStyleIndex( m_stylesList->currentItem() );
    m_styleOrder.remove( m_stylesList->currentText());
    if ( !m_frameStyles.at(cur)->origFrameStyle() )
        m_frameStyles.take(cur );
    else {
        m_frameStyles.at( cur )->deleteStyle( m_currentFrameStyle );
        m_currentFrameStyle = 0L;
    }

    // Adjust GUI
    m_stylesList->removeItem(m_stylesList->currentItem());
    numFrameStyles--;
    m_stylesList->setSelected( m_stylesList->currentItem(), true );
}

void KWFrameStyleManager::moveUpStyle()
{
    if(m_currentFrameStyle !=0L)
        save();
    unsigned int pos = 0;
    QString currentStyleName = m_stylesList->currentText();
    if ( currentStyleName.isEmpty() )
        return;
    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 > 0 ) // neither -1 nor 0
    {
        m_styleOrder.removeAt(pos2);
        m_styleOrder.insert( pos2-1, currentStyleName);
    }


    pos=m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text ( pos-1 ),pos);

    m_stylesList->changeItem( currentStyleName, pos-1);

    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWFrameStyleManager::moveDownStyle()
{
    if(m_currentFrameStyle !=0L)
        save();
    unsigned int pos = 0;
    QString currentStyleName=m_stylesList->currentText ();
    if ( currentStyleName.isEmpty() )
        return;

    int pos2 = m_styleOrder.findIndex( currentStyleName );
    if ( pos2 != -1 )
    {
        m_styleOrder.removeAt( pos2 );
        m_styleOrder.insert( pos2+1, currentStyleName);
    }

    pos=m_stylesList->currentItem();
    noSignals=true;
    m_stylesList->changeItem( m_stylesList->text ( pos+1 ),pos);
    m_stylesList->changeItem( currentStyleName ,pos+1);
    m_stylesList->setCurrentItem( m_stylesList->currentItem() );
    noSignals=false;

    updateGUI();
}

void KWFrameStyleManager::slotOk()
{
    save();
    apply();
    KDialogBase::slotOk();
}

void KWFrameStyleManager::slotApply()
{
    save();
    apply();
    KDialogBase::slotApply();
}

void KWFrameStyleManager::apply()
{
    // ##### missing: undo/redo support
    noSignals=true;
    for (unsigned int i =0 ; i < m_frameStyles.count() ; i++) {
        if(m_frameStyles.at(i)->origFrameStyle() == 0) {           // newly added style

            kDebug() << "adding new " << m_frameStyles.at(i)->changedFrameStyle()->name() << " (" << i << ")" << endl;

            KWFrameStyle* style = m_doc->frameStyleCollection()->addStyle(m_frameStyles.take(i)->changedFrameStyle());
            m_frameStyles.insert( i, new KWFrameStyleListItem(0, style) );

        } else if(m_frameStyles.at(i)->changedFrameStyle() == 0) { // deleted style

            kDebug() << "deleting orig " << m_frameStyles.at(i)->origFrameStyle()->name() << " (" << i << ")" << endl;

            KWFrameStyle *orig = m_frameStyles.at(i)->origFrameStyle();
            m_doc->frameStyleCollection()->removeStyle( orig );

        } else {

            kDebug() << "update style " << m_frameStyles.at(i)->changedFrameStyle()->name() << " (" << i << ")" << endl;
            // TODO check if modified, so that we can do m_doc->setModified(true) only if a style was changed
            m_frameStyles.at(i)->apply();
        }
    }
    m_doc->frameStyleCollection()->updateStyleListOrder( m_styleOrder );
    m_doc->updateAllFrameStyleLists();
    m_doc->setModified( true );
    noSignals=false;
}

void KWFrameStyleManager::renameStyle(const QString &theText) {
    if(noSignals) return;
    noSignals=true;

    int index = m_stylesList->currentItem();
    kDebug() << "KWFrameStyleManager::renameStyle " << index << " to " << theText << endl;

    // rename only in the GUI, not even in the underlying objects (save() does it).
    m_stylesList->changeItem( theText, index );
    m_styleOrder[index]=theText;

    // Check how many styles with that name we have now
    int synonyms = 0;
    for ( uint i = 0; i < m_stylesList->count(); i++ ) {
        if ( m_stylesList->text( i ) == m_stylesList->currentText() )
            ++synonyms;
    }
    Q_ASSERT( synonyms > 0 ); // should have found 'index' at least !
    noSignals=false;
    // Can't close the dialog if two styles have the same name
    bool state=!theText.isEmpty() && (synonyms == 1);
    enableButtonOK(state );
    enableButtonApply(state);
    m_deleteButton->setEnabled(state&&(m_stylesList->currentItem() != 0));
    m_newButton->setEnabled(state);
    m_stylesList->setEnabled( state );
    enableButton( KDialogBase::User1, state );

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

/******************************************************************/
/* Class: KWFrameStyleBackgroundTab                               */
/******************************************************************/

KWFrameStyleBackgroundTab::KWFrameStyleBackgroundTab( QWidget * parent )
    : KWFrameStyleManagerTab( parent )
{
    bgwidget = this;
    m_backgroundColor.setStyle( Qt::SolidPattern );

    grid = new Q3GridLayout( bgwidget, 7, 2, KDialog::marginHint(), KDialog::spacingHint() );

    int row=0;

    brushPreview = new KWBrushStylePreview( bgwidget );
    grid->addMultiCellWidget(brushPreview,row,5,1,1);

    QLabel *l = new QLabel( i18n( "Frame background color:" ), bgwidget );

    grid->addWidget(l,row++,0);

    brushColor = new KColorButton( Qt::white, bgwidget );
    grid->addWidget(brushColor,row++,0);

    connect( brushColor, SIGNAL( changed( const QColor & ) ),
             this, SLOT( updateBrushConfiguration( const QColor & ) ) );

    QSpacerItem* spacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid->addItem( spacer,row,0 );

    updateBrushConfiguration( Qt::white );
}

void KWFrameStyleBackgroundTab::updateBrushConfiguration( const QColor & _color )
{
    m_backgroundColor.setColor( _color );
    brushPreview->show();
    brushPreview->setBrush( m_backgroundColor );
    brushPreview->repaint(true);
}

QString KWFrameStyleBackgroundTab::tabName()
{
    return i18n("Background");
}

void KWFrameStyleBackgroundTab::update()
{
    brushColor->setColor( m_style->backgroundColor().color() );
    updateBrushConfiguration( m_style->backgroundColor().color() );
}

void KWFrameStyleBackgroundTab::save()
{
    m_style->setBackgroundColor( m_backgroundColor );
}

/******************************************************************/
/* Class: KWFrameStyleBordersTab                                  */
/******************************************************************/

KWFrameStyleBordersTab::KWFrameStyleBordersTab( QWidget * parent )
    : KWFrameStyleManagerTab( parent )
{
    m_widget = 0L;
    m_borders = new KoParagLayout();
}

KWFrameStyleBordersTab::~KWFrameStyleBordersTab()
{
    delete m_borders;
}

QString KWFrameStyleBordersTab::tabName()
{
    return m_widget->tabName();
}

void KWFrameStyleBordersTab::update()
{
    m_borders->leftBorder = m_style->leftBorder();
    m_borders->rightBorder = m_style->rightBorder();
    m_borders->topBorder = m_style->topBorder();
    m_borders->bottomBorder = m_style->bottomBorder();


    m_widget->display( *m_borders );
}

void KWFrameStyleBordersTab::save()
{
    m_widget->save( *m_borders );
    m_style->setLeftBorder( m_borders->leftBorder );
    m_style->setRightBorder( m_borders->rightBorder );
    m_style->setTopBorder( m_borders->topBorder );
    m_style->setBottomBorder( m_borders->bottomBorder );
}

void KWFrameStyleBordersTab::setWidget( KoParagLayoutWidget * widget )
{
    m_widget = widget;
}

void KWFrameStyleBordersTab::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( m_widget ) m_widget->resize( size() );
}

