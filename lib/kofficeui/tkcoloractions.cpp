#include "tkcoloractions.h"
#include "tktoolbarbutton.h"

#include <qlayout.h>
#include <kcolordialog.h>
#include <ktoolbar.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <klocale.h>
#include <kdebug.h>

TKColorPopupMenu::TKColorPopupMenu( QWidget* parent, const char* name )
: KPopupMenu(parent,name)
{
}

TKColorPopupMenu::~TKColorPopupMenu()
{
}

void TKColorPopupMenu::updateItemSize()
{
  styleChange(style());
}
/****************************************************************************************/
class TKSelectColorActionPrivate
{
public:
    TKSelectColorActionPrivate()
    {
    }
    bool defaultColorMenu;
    QColor defaultColor;
};


TKSelectColorAction::TKSelectColorAction( const QString& text, Type type, QObject* parent, const char* name, bool menuDefaultColor )
: TKAction(parent,name)
{
    d=new TKSelectColorActionPrivate();
    d->defaultColorMenu=menuDefaultColor;
    d->defaultColor=QColor();
    setText(text);
    m_type = type;
    init();
}

TKSelectColorAction::TKSelectColorAction( const QString& text, Type type,
                                          QObject* receiver, const char* slot,
                                          QObject* parent, const char* name, bool menuDefaultColor)
: TKAction(parent,name)
{
    d=new TKSelectColorActionPrivate();
    d->defaultColorMenu=menuDefaultColor;
    d->defaultColor=QColor();
    setText(text);
    m_type = type;
    connect( this, SIGNAL( activated() ), receiver, slot );
    init();
}

void TKSelectColorAction::init()
{
  m_pStandardColor = new TKColorPanel();
  m_pRecentColor = new TKColorPanel();

  connect(m_pStandardColor,SIGNAL(colorSelected(const QColor&)),SLOT(panelColorSelected(const QColor&)));
  connect(m_pStandardColor,SIGNAL(reject()),SLOT(panelReject()));
  connect(m_pRecentColor,SIGNAL(colorSelected(const QColor&)),SLOT(panelColorSelected(const QColor&)));
  connect(m_pRecentColor,SIGNAL(reject()),SLOT(panelReject()));

  m_pRecentColor->clear();

  m_pMenu = new TKColorPopupMenu();
  m_pMenu->insertItem(m_pStandardColor);
  m_pMenu->insertSeparator();
  m_pMenu->insertItem(m_pRecentColor);
  m_pMenu->insertSeparator();

  switch (m_type) {
    case TextColor:
      m_pMenu->insertItem(i18n("More Text Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(black);
      setIcon("textcolor");
      break;
    case LineColor:
      m_pMenu->insertItem(i18n("More Line Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(black);
      setIcon("color_line");
      break;
    case FillColor:
      m_pMenu->insertItem(i18n("More Fill Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(white);
      setIcon("color_fill");
      break;
    case Color:
      break;
  }
  if(d->defaultColorMenu)
  {
      m_pMenu->insertSeparator();
      m_pMenu->insertItem(i18n("Default Color"),this,SLOT(defaultColor()));
  }

  connect(m_pStandardColor,SIGNAL(sizeChanged()),m_pMenu,SLOT(updateItemSize()));
  connect(m_pRecentColor,SIGNAL(sizeChanged()),m_pMenu,SLOT(updateItemSize()));
}

TKSelectColorAction::~TKSelectColorAction()
{
  delete m_pMenu;
  delete d;
}

void TKSelectColorAction::initToolBarButton(TKToolBarButton* b)
{
  QWhatsThis::add( b, whatsThis() );
  TKAction::initToolBarButton(b);
  b->setDelayedPopup( popupMenu() );
  updatePixmap(b);
  updatePixmap();
}

void TKSelectColorAction::defaultColor()
{
   m_pCurrentColor = d->defaultColor;
   emit activated();
}

void TKSelectColorAction::setDefaultColor(const QColor &_col)
{
    d->defaultColor=_col;
}

void TKSelectColorAction::updatePixmap()
{
  for( int id = 0; id < containerCount(); ++id ) {
    QWidget* w = container(id);
    if ( w->inherits("KToolBar") ) {
      QWidget* r = static_cast<KToolBar*>(w)->getWidget(itemId(id));
      if ( r->inherits("TKToolBarButton") ) {
        updatePixmap(static_cast<TKToolBarButton*>(r));
      }
    }
    else if(w->inherits("QPopupMenu") ) {
        QPixmap pix =iconSet(KIcon::Small).pixmap(QIconSet::Automatic,QIconSet::Active);
	if ( pix.isNull() )
	    return;
        QPainter p(&pix);
        switch (m_type) {
            case TextColor:
                p.fillRect(QRect(0,12,16,5), m_pCurrentColor);
                break;
            case LineColor:
                p.fillRect(QRect(0,13,16,5), m_pCurrentColor);
                p.fillRect(QRect(3,12,1,1), m_pCurrentColor);
                break;
            case FillColor:
                p.fillRect(QRect(0,13,16,5), m_pCurrentColor);
                p.fillRect(QRect(1,10,5,3), m_pCurrentColor);
                break;
            case Color:
                break;
        }
        p.end();
        setIconSet( pix );
    }
  }
}

void TKSelectColorAction::updatePixmap(TKToolBarButton* b)
{
  if (!b)
    return;
  // Not much point in painting with an invalid color
  if (!m_pCurrentColor.isValid())
    return;
  QPixmap pix =b->getActivePixmap();
  QPainter p(&pix);
  switch (m_type) {
    case TextColor:
      p.fillRect(QRect(0,12,16,5), m_pCurrentColor);
      break;
    case LineColor:
      p.fillRect(QRect(0,13,16,5), m_pCurrentColor);
      p.fillRect(QRect(3,12,1,1), m_pCurrentColor);
      break;
    case FillColor:
      p.fillRect(QRect(0,13,16,5), m_pCurrentColor);
      p.fillRect(QRect(1,10,5,3), m_pCurrentColor);
      break;
    case Color:
      break;
  }
  p.end();
  b->setPixmap(pix);
}

void TKSelectColorAction::setCurrentColor( const QColor& color )
{
    if ( color == m_pCurrentColor )
        return;
    m_pCurrentColor = color;
    setActiveColor( color );
    m_pRecentColor->setActiveColor(color );
  updatePixmap();
}

void TKSelectColorAction::setActiveColor( const QColor& color )
{
  m_pStandardColor->setActiveColor(color);
}

void TKSelectColorAction::selectColorDialog()
{
    QColor color;
    if ( d->defaultColorMenu )
    {
        if ( KColorDialog::getColor(color,d->defaultColor) == QDialog::Accepted )
        {
            setCurrentColor(color);
            m_pRecentColor->insertColor(m_pCurrentColor);
            activate();
        }

    }
    else
    {
        if ( KColorDialog::getColor(color) == QDialog::Accepted )
        {
            setCurrentColor(color);
            m_pRecentColor->insertColor(m_pCurrentColor);
            activate();
        }
    }
}

// Called when activating the menu item, or when clicking the main toolbar button
void TKSelectColorAction::slotActivated()
{
  //kdDebug() << "TKSelectColorAction::slotActivated" << endl;
  activate();
}

void TKSelectColorAction::activate()
{
  emit colorSelected(m_pCurrentColor);
  emit activated();
}

void TKSelectColorAction::panelColorSelected( const QColor& color )
{
  m_pMenu->hide();
  setCurrentColor(color);

  activate();
}

void TKSelectColorAction::panelReject()
{
  m_pMenu->hide();
}

class TKColorPanel::TKColorPanelPrivate
{
public:
  TKColorPanelPrivate()
  {
    panelCreated = false;
  }

  bool panelCreated;
};

/****************************************************************************************/
TKColorPanel::TKColorPanel( QWidget* parent, const char* name )
: QWidget(parent,name)
{
  d = new TKColorPanel::TKColorPanelPrivate();
  m_activeColor = black;

  //m_iX = 0;  // happens in setNumCols() -> resetGrid()
  //m_iY = 0;  // anyway, so...

  m_pLayout = 0L;
  setNumCols(15);
}

void TKColorPanel::setNumCols( int col )
{
  m_iWidth = col;
  resetGrid();

  QDictIterator<TKColorPanelButton> it(m_pColorDict);
  while ( it.current() ) {
    addToGrid(it.current());
    ++it;
  }
}

TKColorPanel::~TKColorPanel()
{
  delete d;
}

void TKColorPanel::resetGrid()
{
  m_iX = 0;
  m_iY = 0;

  delete m_pLayout;
  m_pLayout = new QGridLayout(this,0,m_iWidth+1,0,0);

  emit sizeChanged();
}

void TKColorPanel::clear()
{
  m_pColorDict.setAutoDelete(true);
  m_pColorDict.clear();
  m_pColorDict.setAutoDelete(false);
  d->panelCreated = true;  // we don't want to create the default
                           // panel anymore now (b/c of recent colors)
  resetGrid();
}

void TKColorPanel::insertColor( const QColor& color, const QString& text )
{
  if (m_pColorDict[color.name()])
    return;

  insertColor(color);
  QToolTip::add(m_pColorDict[color.name()],text);
}

void TKColorPanel::insertColor( const QColor& color )
{
  if (m_pColorDict[color.name()])
    return;

  m_pLayout->setMargin(3);
  TKColorPanelButton* f = new TKColorPanelButton(color,this);
  m_pColorDict.insert(color.name(),f);
  if ( m_activeColor == color )
      f->setActive(true);

  connect(f,SIGNAL(selected(const QColor&)),SLOT(selected(const QColor&)));

  addToGrid(f);
}

void TKColorPanel::addToGrid( TKColorPanelButton* f )
{
  m_pLayout->addWidget(f,m_iY,m_iX);
  f->show();  // yeehaaaw! ugly hack (Werner)
  m_iX++;
  if ( m_iX == m_iWidth ) {
    m_iX = 0;
    m_iY++;
  }
  emit sizeChanged();
}

void TKColorPanel::setActiveColor( const QColor& color )
{
    TKColorPanelButton* b = m_pColorDict[m_activeColor.name()];
  if (b)
    b->setActive(false);

  m_activeColor = color;

  b = m_pColorDict[m_activeColor.name()];
  if (b)
    b->setActive(true);
}

void TKColorPanel::mouseReleaseEvent( QMouseEvent* )
{
  reject();
}

void TKColorPanel::showEvent( QShowEvent *e )
{
  if ( !d->panelCreated )
    fillPanel();
  QWidget::showEvent(e);
}

void TKColorPanel::selected( const QColor& color )
{
  emit colorSelected(color);
}

void TKColorPanel::fillPanel()
{
  d->panelCreated = true;
  blockSignals(true); // don't emit sizeChanged() all the time

  insertColor(QColor( 255,   0,   0 ), i18n("Red"));
  insertColor(QColor( 255, 165,   0 ), i18n("Orange"));
  insertColor(QColor( 255,   0, 255 ), i18n("Magenta"));
  insertColor(QColor(   0,   0, 255 ), i18n("Blue"));
  insertColor(QColor(   0, 255, 255 ), i18n("Cyan"));
  insertColor(QColor(   0, 255,   0 ), i18n("Green"));
  insertColor(QColor( 255, 255,   0 ), i18n("Yellow"));
  insertColor(QColor( 165,  42,  42 ), i18n("Brown"));
  insertColor(QColor( 139,   0,   0 ), i18n("Darkred"));
  insertColor(QColor( 255, 140,   0 ), i18n("Dark Orange"));
  insertColor(QColor( 139,   0, 139 ), i18n("Dark Magenta"));
  insertColor(QColor(   0,   0, 139 ), i18n("Dark Blue"));
  insertColor(QColor(   0, 139, 139 ), i18n("Dark Cyan"));
  insertColor(QColor(   0, 100,   0 ), i18n("Dark Green"));
  insertColor(QColor( 130, 127,   0 ), i18n("Dark Yellow"));

  insertColor(QColor( 255, 255, 255 ), i18n("White"));
  insertColor(QColor( 229, 229, 229 ), i18n("Gray 90%"));
  insertColor(QColor( 204, 204, 204 ), i18n("Gray 80%"));
  insertColor(QColor( 178, 178, 178 ), i18n("Gray 70%"));
  insertColor(QColor( 153, 153, 153 ), i18n("Gray 60%"));
  insertColor(QColor( 127, 127, 127 ), i18n("Gray 50%"));
  insertColor(QColor( 102, 102, 102 ), i18n("Gray 40%"));
  insertColor(QColor(  76,  76,  76 ), i18n("Gray 30%"));
  insertColor(QColor(  51,  51,  51 ), i18n("Gray 20%"));
  insertColor(QColor(  25,  25,  25 ), i18n("Gray 10%"));
  insertColor(QColor(   0,   0,   0 ), i18n("Black"));

  insertColor(QColor( 255, 255, 240 ), i18n("Ivory"));
  insertColor(QColor( 255, 250, 250 ), i18n("Snow"));
  insertColor(QColor( 245, 255, 250 ), i18n("Mint Cream"));
  insertColor(QColor( 255, 250, 240 ), i18n("Floral White"));
  insertColor(QColor( 255, 255, 224 ), i18n("Light Yellow"));
  insertColor(QColor( 240, 255, 255 ), i18n("Azure"));
  insertColor(QColor( 248, 248, 255 ), i18n("Ghost White"));
  insertColor(QColor( 240, 255, 240 ), i18n("Honeydew"));
  insertColor(QColor( 255, 245, 238 ), i18n("Seashell"));
  insertColor(QColor( 240, 248, 255 ), i18n("Alice Blue"));
  insertColor(QColor( 255, 248, 220 ), i18n("Cornsilk"));
  insertColor(QColor( 255, 240, 245 ), i18n("Lavender Blush"));
  insertColor(QColor( 253, 245, 230 ), i18n("Old Lace"));
  insertColor(QColor( 245, 245, 245 ), i18n("White Smoke"));
  insertColor(QColor( 255, 250, 205 ), i18n("Lemon Chiffon"));
  insertColor(QColor( 224, 255, 255 ), i18n("Light Cyan"));
  insertColor(QColor( 250, 250, 210 ), i18n("Light Goldenrod Yellow"));
  insertColor(QColor( 250, 240, 230 ), i18n("Linen"));
  insertColor(QColor( 245, 245, 220 ), i18n("Beige"));
  insertColor(QColor( 255, 239, 213 ), i18n("Papaya Whip"));
  insertColor(QColor( 255, 235, 205 ), i18n("Blanched Almond"));
  insertColor(QColor( 250, 235, 215 ), i18n("Antique White"));
  insertColor(QColor( 255, 228, 225 ), i18n("Misty Rose"));
  insertColor(QColor( 230, 230, 250 ), i18n("Lavender"));
  insertColor(QColor( 255, 228, 196 ), i18n("Bisque"));
  insertColor(QColor( 255, 228, 181 ), i18n("Moccasin"));
  insertColor(QColor( 255, 222, 173 ), i18n("Navajo White"));
  insertColor(QColor( 255, 218, 185 ), i18n("Peach Puff"));
  insertColor(QColor( 238, 232, 170 ), i18n("Pale Goldenrod"));
  insertColor(QColor( 245, 222, 179 ), i18n("Wheat"));
  insertColor(QColor( 220, 220, 220 ), i18n("Gainsboro"));
  insertColor(QColor( 240, 230, 140 ), i18n("Khaki"));
  insertColor(QColor( 175, 238, 238 ), i18n("Pale Turquoise"));
  insertColor(QColor( 255, 192, 203 ), i18n("Pink"));
  insertColor(QColor( 238, 221, 130 ), i18n("Light Goldenrod"));
  insertColor(QColor( 211, 211, 211 ), i18n("Light Gray"));
  insertColor(QColor( 255, 182, 193 ), i18n("Light Pink"));
  insertColor(QColor( 176, 224, 230 ), i18n("Powder Blue"));
  insertColor(QColor( 127, 255, 212 ), i18n("Aquamarine"));
  insertColor(QColor( 216, 191, 216 ), i18n("Thistle"));
  insertColor(QColor( 173, 216, 230 ), i18n("Light Blue"));
  insertColor(QColor( 152, 251, 152 ), i18n("Pale Green"));
  insertColor(QColor( 255, 215,   0 ), i18n("Gold"));
  insertColor(QColor( 173, 255,  47 ), i18n("Green Yellow"));
  insertColor(QColor( 176, 196, 222 ), i18n("Light Steel Blue"));
  insertColor(QColor( 144, 238, 144 ), i18n("Light Green"));
  insertColor(QColor( 221, 160, 221 ), i18n("Plum"));
  insertColor(QColor( 190, 190, 190 ), i18n("Gray"));
  insertColor(QColor( 222, 184, 135 ), i18n("Burly Wood"));
  insertColor(QColor( 135, 206, 250 ), i18n("Light Skyblue"));
  insertColor(QColor( 255, 160, 122 ), i18n("Light Salmon"));
  insertColor(QColor( 135, 206, 235 ), i18n("Sky Blue"));
  insertColor(QColor( 210, 180, 140 ), i18n("Tan"));
  insertColor(QColor( 238, 130, 238 ), i18n("Violet"));
  insertColor(QColor( 244, 164,  96 ), i18n("Sandy Brown"));
  insertColor(QColor( 233, 150, 122 ), i18n("Dark Salmon"));
  insertColor(QColor( 189, 183, 107 ), i18n("Dark khaki"));
  insertColor(QColor( 127, 255,   0 ), i18n("Chartreuse"));
  insertColor(QColor( 169, 169, 169 ), i18n("Dark Gray"));
  insertColor(QColor( 124, 252,   0 ), i18n("Lawn Green"));
  insertColor(QColor( 255, 105, 180 ), i18n("Hot Pink"));
  insertColor(QColor( 250, 128, 114 ), i18n("Salmon"));
  insertColor(QColor( 240, 128, 128 ), i18n("Light Coral"));
  insertColor(QColor(  64, 224, 208 ), i18n("Turquoise"));
  insertColor(QColor( 143, 188, 143 ), i18n("Dark Seagreen"));
  insertColor(QColor( 218, 112, 214 ), i18n("Orchid"));
  insertColor(QColor( 102, 205, 170 ), i18n("Medium Aquamarine"));
  insertColor(QColor( 255, 127,  80 ), i18n("Coral"));
  insertColor(QColor( 154, 205,  50 ), i18n("Yellow Green"));
  insertColor(QColor( 218, 165,  32 ), i18n("Goldenrod"));
  insertColor(QColor(  72, 209, 204 ), i18n("Medium Turquoise"));
  insertColor(QColor( 188, 143, 143 ), i18n("Rosy Brown"));
  insertColor(QColor( 219, 112, 147 ), i18n("Pale VioletRed"));
  insertColor(QColor(   0, 250, 154 ), i18n("Medium Spring Green"));
  insertColor(QColor( 255,  99,  71 ), i18n("Tomato"));
  insertColor(QColor(   0, 255, 127 ), i18n("Spring Green"));
  insertColor(QColor( 205, 133,  63 ), i18n("Peru"));
  insertColor(QColor( 100, 149, 237 ), i18n("Cornflower Blue"));
  insertColor(QColor( 132, 112, 255 ), i18n("Light Slate Blue"));
  insertColor(QColor( 147, 112, 219 ), i18n("Medium Purple"));
  insertColor(QColor( 186,  85, 211 ), i18n("Medium Orchid"));
  insertColor(QColor(  95, 158, 160 ), i18n("Cadet Blue"));
  insertColor(QColor(   0, 206, 209 ), i18n("Dark Turquoise"));
  insertColor(QColor(   0, 191, 255 ), i18n("Deep Skyblue"));
  insertColor(QColor( 119, 136, 153 ), i18n("Light Slate Gray"));
  insertColor(QColor( 184, 134,  11 ), i18n("Dark Goldenrod"));
  insertColor(QColor( 123, 104, 238 ), i18n("MediumSlate Blue"));
  insertColor(QColor( 205,  92,  92 ), i18n("IndianRed"));
  insertColor(QColor( 210, 105,  30 ), i18n("Chocolate"));
  insertColor(QColor(  60, 179, 113 ), i18n("Medium Sea Green"));
  insertColor(QColor(  50, 205,  50 ), i18n("Lime Green"));
  insertColor(QColor(  32, 178, 170 ), i18n("Light Sea Green"));
  insertColor(QColor( 112, 128, 144 ), i18n("Slate Gray"));
  insertColor(QColor(  30, 144, 255 ), i18n("Dodger Blue"));
  insertColor(QColor( 255,  69,   0 ), i18n("Orange Red"));
  insertColor(QColor( 255,  20, 147 ), i18n("Deep Pink"));
  insertColor(QColor(  70, 130, 180 ), i18n("Steel Blue"));
  insertColor(QColor( 106,  90, 205 ), i18n("Slate Blue"));
  insertColor(QColor( 107, 142,  35 ), i18n("Olive Drab"));
  insertColor(QColor(  65, 105, 225 ), i18n("Royal Blue"));
  insertColor(QColor( 208,  32, 144 ), i18n("Violet Red"));
  insertColor(QColor( 153,  50, 204 ), i18n("Dark Orchid"));
  insertColor(QColor( 160,  32, 240 ), i18n("Purple"));
  insertColor(QColor( 105, 105, 105 ), i18n("Dim Gray"));
  insertColor(QColor( 138,  43, 226 ), i18n("Blue Violet"));
  insertColor(QColor( 160,  82,  45 ), i18n("Sienna"));
  insertColor(QColor( 199,  21, 133 ), i18n("Medium Violet Red"));
  insertColor(QColor( 176,  48,  96 ), i18n("Maroon"));
  insertColor(QColor(  46, 139,  87 ), i18n("Sea Green"));
  insertColor(QColor(  85, 107,  47 ), i18n("Dark Olive Green"));
  insertColor(QColor(  34, 139,  34 ), i18n("Forest Green"));
  insertColor(QColor( 139,  69,  19 ), i18n("Saddle Brown"));
  insertColor(QColor( 148,   0, 211 ), i18n("Darkviolet"));
  insertColor(QColor( 178,  34,  34 ), i18n("Fire Brick"));
  insertColor(QColor(  72,  61, 139 ), i18n("Dark Slate Blue"));
  insertColor(QColor(  47,  79,  79 ), i18n("Dark Slate Gray"));
  insertColor(QColor(  25,  25, 112 ), i18n("Midnight Blue"));
  insertColor(QColor(   0,   0, 205 ), i18n("Medium Blue"));
  insertColor(QColor(   0,   0, 128 ), i18n("Navy"));

  blockSignals(false);  // Signals allowed again
  emit sizeChanged();   // one call should be enough ;)
}

/****************************************************************************************/
TKColorPanelButton::TKColorPanelButton( const QColor& color, QWidget* parent, const char* name )
: QFrame(parent,name), m_Color(color), m_bActive(false)
{
  setFixedSize(16,16);
  setFrameStyle( NoFrame );
}

TKColorPanelButton::~TKColorPanelButton()
{
}

void TKColorPanelButton::enterEvent( QEvent* )
{
  if (!m_bActive)
    setFrameStyle( Panel | Sunken );
}

void TKColorPanelButton::leaveEvent( QEvent* )
{
  if (!m_bActive)
    setFrameStyle( NoFrame );
}

void TKColorPanelButton::paintEvent( QPaintEvent* ev )
{
  QFrame::paintEvent(ev);

  QPainter p(this,this);
  p.fillRect(2,2,12,12,m_Color);
  p.setPen(gray);
  p.drawRect(2,2,12,12);
  p.end();
}

void TKColorPanelButton::setActive( bool f )
{
  m_bActive = f;
  setFrameStyle( m_bActive ? Panel | Sunken : NoFrame );
}

void TKColorPanelButton::mouseReleaseEvent( QMouseEvent* )
{
  emit selected(m_Color);
}
#include "tkcoloractions.moc"
