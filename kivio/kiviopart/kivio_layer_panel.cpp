#include "kivio_canvas.h"
#include "kivio_dlg_layername.h"
#include "kivio_doc.h"
#include "kivio_layer.h"
#include "kivio_layer_panel.h"
#include "kivio_page.h"
#include "kivio_view.h"

#include <qmessagebox.h>
#include <kmessagebox.h>

#include <qpixmap.h>
#include <kdebug.h>

static const char * blank_xpm[] = {
"1 1 1 1",
"   c None",
" "
};

static const char * conn_xpm[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #48C641",
"                ",
"           .... ",
"           .++. ",
"           .++. ",
"          ..... ",
"         ...    ",
"        ...     ",
"       ...      ",
"      ...       ",
"     ...        ",
"    ...         ",
" .....          ",
" .++.           ",
" .++.           ",
" ....           ",
"                "};

static const char * eye_xpm[] = {
"16 16 27 1",
"       c None",
".      c #313131",
"+      c #000000",
"@      c #454545",
"#      c #070707",
"$      c #242424",
"%      c #0C0C0C",
"&      c #171717",
"*      c #050505",
"=      c #353535",
"-      c #626262",
";      c #4B4B4B",
">      c #929292",
",      c #9E9E9E",
"'      c #262626",
")      c #131313",
"!      c #6E6E6E",
"~      c #9C9C9C",
"{      c #080808",
"]      c #818181",
"^      c #1B1B1B",
"/      c #2D2D2D",
"(      c #555555",
"_      c #2A2A2A",
":      c #151515",
"<      c #0E0E0E",
"[      c #494949",
"                ",
"                ",
"                ",
"                ",
"     .++++.     ",
"   @++++++++@   ",
"  ##$%++&*%$##  ",
" ++  =++-&=  @+ ",
"+;  >++++++>  ,'",
")!  ~{++++{~  ]^",
" /+  (++++(   / ",
"  _:  <++<  :_  ",
"    %$*++*$%    ",
"      [[[[      ",
"                ",
"                "};

static const char * plus_xpm[] = {
"16 16 19 1",
"       c None",
".      c #000000",
"+      c #090909",
"@      c #0D0D0D",
"#      c #252525",
"$      c #313131",
"%      c #353535",
"&      c #4A4A4A",
"*      c #585858",
"=      c #5D5D5D",
"-      c #6D6D6D",
";      c #7F7F7F",
">      c #868686",
",      c #8C8C8C",
"'      c #A4A4A4",
")      c #AEAEAE",
"!      c #C5C5C5",
"~      c #D6D6D6",
"{      c #FFFFFF",
"                ",
"     ......     ",
"     ..+@+.     ",
"     .#$%$.     ",
"     .&*=*.     ",
" .....-;>;..... ",
" ..#&-,')',-&#. ",
" .+$*;'!~!';*$. ",
" .@%=>)~{~)>=%. ",
" .+$*;'!~!';*$. ",
" .....,')'..... ",
"     .-;>;.     ",
"     .&*=*.     ",
"     .#$%$.     ",
"     ......     ",
"                "};

static const char * minus_xpm[] = {
"16 16 19 1",
"       c None",
".      c #000000",
"+      c #252525",
"@      c #4A4A4A",
"#      c #6D6D6D",
"$      c #8C8C8C",
"%      c #A4A4A4",
"&      c #AEAEAE",
"*      c #090909",
"=      c #313131",
"-      c #585858",
";      c #7F7F7F",
">      c #C5C5C5",
",      c #D6D6D6",
"'      c #0D0D0D",
")      c #353535",
"!      c #5D5D5D",
"~      c #868686",
"{      c #FFFFFF",
"                ",
"                ",
"                ",
"                ",
"                ",
" .............. ",
" ..+@#$%&%$#@+. ",
" .*=-;%>,>%;-=. ",
" .')!~&,{,&~!). ",
" .*=-;%>,>%;-=. ",
" .............. ",
"                ",
"                ",
"                ",
"                ",
"                "};

static const char *name_xpm[] = {
"16 16 3 1",
"  c Black",
". c #808080",
"X c None",
"XXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXX",
"XXXXXXX XXXXXXXX",
"XXXXXX. .XXXXXXX",
"XXXXXX   XXXXXXX",
"XXXXX.   .XXXXXX",
"XXXXX  X  XXXXXX",
"XXXX. XX  .XXXXX",
"XXXX       XXXXX",
"XXX. XXXX  .XXXX",
"XXX  XXXX.  XXXX",
"XX. XXXXX.  .XXX",
"X    XXX      XX",
"XXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXX"
};

KivioLayerPanel::KivioLayerPanel( QWidget *parent, KivioView *pView, KivioCanvas *pCanvas )
    : QWidget( parent, "KivioLayerPanel" )
{
    m_pView = pView;
    m_pCanvas = pCanvas;

    m_pEyePic = new QPixmap((const char **)eye_xpm);
    m_pConnPic = new QPixmap((const char **)conn_xpm);
    m_pNullPic = new QPixmap((const char **)blank_xpm);

    m_pGrid = new QGridLayout(this, 2, 7);

    m_pEye = new QToolButton(this);
    m_pEye->setPixmap(*m_pEyePic);
    QObject::connect( m_pEye, SIGNAL(clicked()), this, SLOT(toggleVisibility()));

    m_pConnector = new QToolButton(this);
    m_pConnector->setPixmap(*m_pConnPic);
    QObject::connect( m_pConnector, SIGNAL(clicked()), this, SLOT(toggleConnectable()));

    m_pProperties = new QToolButton(this);
    m_pProperties->setPixmap(QPixmap((const char **)name_xpm));
    QObject::connect( m_pProperties, SIGNAL(clicked()), this, SLOT(setLayerName()));

    m_pPlus = new QToolButton(this);
    m_pPlus->setPixmap(QPixmap((const char **)plus_xpm));
    QObject::connect( m_pPlus, SIGNAL(clicked()), this, SLOT(insertLayer()));

    m_pMinus  = new QToolButton(this);
    m_pMinus->setPixmap(QPixmap((const char **)minus_xpm));
    QObject::connect( m_pMinus, SIGNAL(clicked()), this, SLOT(removeLayer()));

    m_pUp = new QToolButton( UpArrow, this );
    QObject::connect( m_pUp, SIGNAL(clicked()), this, SLOT(layerUp()));

    m_pDown = new QToolButton( DownArrow, this );
    QObject::connect( m_pDown, SIGNAL(clicked()), this, SLOT(layerDown()));

    m_pListView = new QListView(this);
    m_pListView->setAllColumnsShowFocus(true);
    m_pListView->addColumn("", 22);
    m_pListView->addColumn("", 22);
    m_pListView->addColumn("Layer Name", 110);
    QObject::connect( m_pListView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectLayer(QListViewItem*)));
    QObject::connect( m_pListView, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(dblClicked(QListViewItem*)));
    QObject::connect( m_pListView, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int )), this, SLOT(rightClick(QListViewItem *, const QPoint &, int )));


    m_pGrid->addWidget( m_pEye, 0, 0 );
    m_pGrid->addWidget( m_pConnector, 0, 1  );
    m_pGrid->addWidget( m_pProperties, 0, 2 );
    m_pGrid->addWidget( m_pPlus, 0, 3 );
    m_pGrid->addWidget( m_pMinus, 0, 4 );
    m_pGrid->addWidget( m_pUp, 0, 5 );
    m_pGrid->addWidget( m_pDown, 0, 6 );

    m_pGrid->addMultiCellWidget( m_pListView, 1, 1, 0, 6 );

    m_pGrid->setRowStretch( 1, 1 );

    m_pGrid->activate();

    //setMinimumSize( 120, 120 );
}

KivioLayerPanel::~KivioLayerPanel()
{
    delete m_pConnPic;
    delete m_pEyePic;
}

void KivioLayerPanel::updateView()
{
    KivioPage *pPage;
    KivioLayer *pLayer, *pCurLayer;

    QListViewItem *pItem;

    pPage = m_pView->activePage();

    pCurLayer = pPage->curLayer();

    pLayer = pPage->curLayer();

    m_pCurItem = NULL;
    m_pListView->clear();

    pLayer = pPage->firstLayer();
    while( pLayer )
    {
        pItem = new QListViewItem( m_pListView, "", "", pLayer->name() );
        if( pLayer==pCurLayer )
            m_pCurItem = pItem;

        if( pLayer->visible() )
            pItem->setPixmap( 0, *m_pEyePic );

        if( pLayer->connectable() )
            pItem->setPixmap( 1, *m_pConnPic );

        pLayer = pPage->nextLayer();
    }

    if( m_pCurItem )
    {
        m_pListView->setCurrentItem( m_pCurItem );
    }
    else
    {
        m_pCurItem = m_pListView->firstChild();
        if( !m_pCurItem )
        {
            kdDebug() << "KivioLayerPanel::updateView() - Very Bad Error.  There are no layers!" << endl;
        }
        else
        {
            m_pListView->setCurrentItem( m_pCurItem );
        }
    }
}

void KivioLayerPanel::selectLayer(QListViewItem *pNew)
{
    int i=m_pView->activePage()->layers()->count()-1;

    m_pCurItem = pNew;

    QListViewItem *pItem = m_pListView->firstChild();
    while( pItem )
    {
        if( pItem == m_pCurItem )
        {
            m_pView->activePage()->setCurLayer( m_pView->activePage()->layerAt(i) );

            // Switching layers unselects all stencils.  We REALLY don't want multiple
            // selections between layers.  If you don't do this, and multiple stencils
            // on various layers do occur and a group operation occurs, I have NO idea
            // what will happen since I didn't code it with multi-layer-group in mind...
            // or did I?  Anyway, if you get rid of this, you have to go check all the
            // multi-select operations.
            m_pView->activePage()->unselectAllStencils();

            m_pView->doc()->updateView(m_pView->activePage());
            return;
        }

        i--;

        pItem = pItem->nextSibling();
    }
}

void KivioLayerPanel::insertLayer()
{
    static int i=2;
    KivioLayer *pLayer = new KivioLayer( m_pView->activePage() );

    // Set the layer name to something a bit different. This isn't
    // guaranteed to be unique, but it's better than "Untitled"
    pLayer->setName("Layer " + QString::number(i));
    m_pView->activePage()->addLayer( pLayer );
    m_pView->activePage()->setCurLayer( pLayer );

    updateView();
    m_pView->doc()->updateView(m_pView->activePage());
    i++;
}

void KivioLayerPanel::removeLayer()
{
    if( m_pView->activePage()->layers()->count() <= 1 )
    {
        KMessageBox::information( this, i18n("You cannot delete the last layer of a page."), i18n("Cannot Remove Layer Layer") );
        return;
    }

    if( KMessageBox::questionYesNo( this, i18n("Are you sure you want to remove this layer?"), i18n("Remove Layer?") )==KMessageBox::No )
    {
        return;
    }

    m_pView->activePage()->removeCurrentLayer();

    updateView();

    m_pView->doc()->updateView(m_pView->activePage());
}

void KivioLayerPanel::toggleVisibility()
{
    KivioLayer *pLayer = m_pView->activePage()->curLayer();

    if( pLayer->visible() )
    {
        m_pCurItem->setPixmap(0,*m_pNullPic);
        pLayer->setVisible(false);
    }
    else
    {
        m_pCurItem->setPixmap(0,*m_pEyePic);
        pLayer->setVisible(true);
    }

    m_pView->doc()->setModified(true);

    m_pCanvas->repaint();
}

void KivioLayerPanel::toggleConnectable()
{
    KivioLayer *pLayer = m_pView->activePage()->curLayer();

    if( pLayer->connectable() )
    {
        m_pCurItem->setPixmap(1,*m_pNullPic);
        pLayer->setConnectable(false);
    }
    else
    {
        m_pCurItem->setPixmap(1,*m_pConnPic);
        pLayer->setConnectable(true);
    }

    m_pView->doc()->setModified(true);

    m_pCanvas->repaint();
}

/**
 * Moves this layer closer to the top (bottom of the list)
 */
void KivioLayerPanel::layerUp()
{
    KivioPage *pPage = m_pView->activePage();
    KivioLayer *pLayer = pPage->curLayer();

    int pos = pPage->layers()->find( pLayer );

    // It's already the top layer... return
    if( pos == (int)pPage->layers()->count()-1 )
        return;

    pLayer = pPage->layers()->take();
    if( !pLayer )
    {
        kdDebug() << "KivioLayerPanel::layerUp() - Error taking layer... bad" << endl;
        return;
    }

    pPage->layers()->insert( pos+1, pLayer );
    pPage->setCurLayer( pLayer );
    updateView();

    m_pView->doc()->setModified(true);

    m_pCanvas->repaint();
}

/**
 * Moves this layer closer to the bottom (top of the list... 0)
 */
void KivioLayerPanel::layerDown()
{
    KivioPage *pPage = m_pView->activePage();
    KivioLayer *pLayer = pPage->curLayer();

    int pos = pPage->layers()->find( pLayer );

    // It's already the bottom layer... return
    if( pos == 0 )
        return;

    pLayer = pPage->layers()->take();
    if( !pLayer )
    {
        kdDebug() << "KivioLayerPanel::layerDown() - Error taking layer... bad" << endl;
        return;
    }

    pPage->layers()->insert( pos-1, pLayer );
    pPage->setCurLayer( pLayer );
    updateView();

    m_pView->doc()->setModified(true);

    m_pCanvas->repaint();
}

void KivioLayerPanel::setLayerName()
{
    QString activeName;
    QString newName;

    // Store the current name of the active page
    KivioLayer *pLayer = m_pView->activePage()->curLayer();
    activeName = pLayer->name();

    KivioLayerName tndlg( m_pView, "LayerName" , activeName );
    if ( tndlg.exec() )
    {
        // Have a different name ?
        if ( ( newName = tndlg.layerName() ) != activeName )
        {
            m_pCurItem->setText( 2, newName );
            pLayer->setName( newName );
            m_pView->doc()->setModified(true);
        }
    }
}

void KivioLayerPanel::dblClicked( QListViewItem * )
{
    setLayerName();
}

void KivioLayerPanel::rightClick( QListViewItem *, const QPoint &, int )
{
    toggleVisibility();
}
#include "kivio_layer_panel.moc"
