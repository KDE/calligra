#include "tkcoloractions.h"
#include "tktoolbarbutton.h"

#include <kcolordialog.h>
#include <ktoolbar.h>
#include <qcolor.h>
#include <qiconset.h>
#include <qtooltip.h>

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
TKSelectColorAction::TKSelectColorAction( const QString& text, Type type, QObject* parent, const char* name )
: TKAction(parent,name)
{
  setText(text);
  iconColorRect = QRect(0,12,16,4);

  m_pStandartColor = new TKColorPanel();
  m_pRecentColor = new TKColorPanel();

  connect(m_pStandartColor,SIGNAL(colorSelected(const QColor&)),SLOT(panelColorSelected(const QColor&)));
  connect(m_pStandartColor,SIGNAL(reject()),SLOT(panelReject()));
  connect(m_pRecentColor,SIGNAL(colorSelected(const QColor&)),SLOT(panelColorSelected(const QColor&)));
  connect(m_pRecentColor,SIGNAL(reject()),SLOT(panelReject()));

  m_pRecentColor->clear();

  m_pMenu = new TKColorPopupMenu();
  m_pMenu->insertItem(m_pStandartColor);
  m_pMenu->insertSeparator();
  m_pMenu->insertItem(m_pRecentColor);
  m_pMenu->insertSeparator();

  m_type = type;

  switch (type) {
    case TextColor:
      m_pMenu->insertItem(i18n("More Text Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(black);
      setIcon("textcolor");
      break;
    case LineColor:
      m_pMenu->insertItem(i18n("More Line Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(black);
      setIcon("pencolor");
      break;
    case FillColor:
      m_pMenu->insertItem(i18n("More Fill Colors..."),this,SLOT(selectColorDialog()));
      setCurrentColor(white);
      setIcon("fillcolor");
      break;
    case Color:
      break;
  }

  connect(m_pStandartColor,SIGNAL(sizeChanged()),m_pMenu,SLOT(updateItemSize()));
  connect(m_pRecentColor,SIGNAL(sizeChanged()),m_pMenu,SLOT(updateItemSize()));
}

TKSelectColorAction::~TKSelectColorAction()
{
  delete m_pMenu;
}

void TKSelectColorAction::initToolBarButton(TKToolBarButton* b)
{
  b->setDelayedPopup( popupMenu() );
  updatePixmap(b);
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
  }
}

void TKSelectColorAction::updatePixmap(TKToolBarButton* b)
{
  if (!b)
    return;

  QPixmap* pix = (QPixmap*)b->pixmap();
  QPainter p(pix);
  p.fillRect(iconColorRect,m_pCurrentColor);
  p.end();

  b->setPixmap(*pix);
}

void TKSelectColorAction::setCurrentColor( const QColor& color )
{
  m_pCurrentColor = color;
  updatePixmap();
}

void TKSelectColorAction::setActiveColor( const QColor& color )
{
  m_pStandartColor->setActiveColor(color);
}

void TKSelectColorAction::selectColorDialog()
{
  if ( KColorDialog::getColor(m_pCurrentColor) == QDialog::Accepted ) {
    setCurrentColor(m_pCurrentColor);
    m_pRecentColor->insertColor(m_pCurrentColor);
    activate();
  }
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
/****************************************************************************************/
TKColorPanel::TKColorPanel( QWidget* parent, const char* name  )
: QWidget(parent,name)
{
  m_activeColor = black;

  m_iX = 0;
  m_iY = 0;

  m_pLayout = 0L;
  setNumCols(15);

  insertColor(qRgb(     255     ,       0       ,       0       ),      "Red");
  insertColor(qRgb(	255	,	165	,	0	),	"Orange");
  insertColor(qRgb(	255	,	0	,	255	),	"Magenta");
  insertColor(qRgb(	0	,	0	,	255	),	"Blue");
  insertColor(qRgb(	0	,	255	,	255	),	"Cyan");
  insertColor(qRgb(     0       ,       255     ,       0       ),      "Green");
  insertColor(qRgb(	255	,	255	,	0	),	"Yellow");
  insertColor(qRgb(	165	,	42	,	42	),	"Brown");
  insertColor(qRgb(	139	,	0	,	0	),	"Darkred");
  insertColor(qRgb(	255	,	140	,	0	),	"Dark Orange");
  insertColor(qRgb(	139	,	0	,	139	),	"Dark Magenta");
  insertColor(qRgb(	0	,	0	,	139	),	"Dark Blue");
  insertColor(qRgb(	0	,	139	,	139	),	"Dark Cyan");
  insertColor(qRgb(	0	,	100	,	0	),	"Dark Green");
  insertColor(qRgb(     130     ,       127     ,       0       ),      "Dark Yellow");

  insertColor(qRgb(255,255,255), "White");
  insertColor(qRgb(229,229,229), "Grey 90%");
  insertColor(qRgb(204,204,204), "Grey 80%");
  insertColor(qRgb(178,178,178), "Grey 70%");
  insertColor(qRgb(153,153,153), "Grey 60%");
  insertColor(qRgb(127,127,127), "Grey 50%");
  insertColor(qRgb(102,102,102), "Grey 40%");
  insertColor(qRgb(76,76,76),    "Grey 30%");
  insertColor(qRgb(51,51,51),    "Grey 20%");
  insertColor(qRgb(25,25,25),    "Grey 10%");
  insertColor(qRgb(0,0,0),       "Black");

  insertColor(qRgb(	255	,	255	,	240	),	"Ivory");
  insertColor(qRgb(	255	,	250	,	250	),	"Snow");
  insertColor(qRgb(	245	,	255	,	250	),	"Mint Cream");
  insertColor(qRgb(	255	,	250	,	240	),	"Floral White");
  insertColor(qRgb(	255	,	255	,	224	),	"Light Yellow");
  insertColor(qRgb(	240	,	255	,	255	),	"Azure");
  insertColor(qRgb(	248	,	248	,	255	),	"Ghost White");
  insertColor(qRgb(	240	,	255	,	240	),	"Honeydew");
  insertColor(qRgb(	255	,	245	,	238	),	"Seashell");
  insertColor(qRgb(	240	,	248	,	255	),	"Alice Blue");
  insertColor(qRgb(	255	,	248	,	220	),	"Cornsilk");
  insertColor(qRgb(	255	,	240	,	245	),	"Lavender Blush");
  insertColor(qRgb(	253	,	245	,	230	),	"Old Lace");
  insertColor(qRgb(	245	,	245	,	245	),	"White Smoke");
  insertColor(qRgb(	255	,	250	,	205	),	"Lemon Chiffon");
  insertColor(qRgb(	224	,	255	,	255	),	"Light Cyan");
  insertColor(qRgb(	250	,	250	,	210	),	"Light Goldenrod Yellow");
  insertColor(qRgb(	250	,	240	,	230	),	"Linen");
  insertColor(qRgb(	245	,	245	,	220	),	"Beige");
  insertColor(qRgb(	255	,	239	,	213	),	"Papaya Whip");
  insertColor(qRgb(	255	,	235	,	205	),	"Blanched Almond");
  insertColor(qRgb(	250	,	235	,	215	),	"Antique White");
  insertColor(qRgb(	255	,	228	,	225	),	"Misty Rose");
  insertColor(qRgb(	230	,	230	,	250	),	"Lavender");
  insertColor(qRgb(	255	,	228	,	196	),	"Bisque");
  insertColor(qRgb(	255	,	228	,	181	),	"Moccasin");
  insertColor(qRgb(	255	,	222	,	173	),	"Navajo White");
  insertColor(qRgb(	255	,	218	,	185	),	"Peach Puff");
  insertColor(qRgb(	238	,	232	,	170	),	"Pale Goldenrod");
  insertColor(qRgb(	245	,	222	,	179	),	"Wheat");
  insertColor(qRgb(	220	,	220	,	220	),	"Gainsboro");
  insertColor(qRgb(	240	,	230	,	140	),	"Khaki");
  insertColor(qRgb(	175	,	238	,	238	),	"Pale Turquoise");
  insertColor(qRgb(	255	,	192	,	203	),	"Pink");
  insertColor(qRgb(	238	,	221	,	130	),	"Light Goldenrod");
  insertColor(qRgb(	211	,	211	,	211	),	"Light Grey");
  insertColor(qRgb(	255	,	182	,	193	),	"Light Pink");
  insertColor(qRgb(	176	,	224	,	230	),	"Powder Blue");
  insertColor(qRgb(	127	,	255	,	212	),	"Aquamarine");
  insertColor(qRgb(	216	,	191	,	216	),	"Thistle");
  insertColor(qRgb(	173	,	216	,	230	),	"Light Blue");
  insertColor(qRgb(	152	,	251	,	152	),	"Pale Green");
  insertColor(qRgb(	255	,	215	,	0	),	"Gold");
  insertColor(qRgb(	173	,	255	,	47	),	"Green Yellow");
  insertColor(qRgb(	176	,	196	,	222	),	"Light Steel Blue");
  insertColor(qRgb(	144	,	238	,	144	),	"Light Green");
  insertColor(qRgb(	221	,	160	,	221	),	"Plum");
  insertColor(qRgb(	190	,	190	,	190	),	"Gray");
  insertColor(qRgb(	222	,	184	,	135	),	"Burly Wood");
  insertColor(qRgb(	135	,	206	,	250	),	"Light Skyblue");
  insertColor(qRgb(	255	,	160	,	122	),	"Light Salmon");
  insertColor(qRgb(	135	,	206	,	235	),	"Sky Blue");
  insertColor(qRgb(	210	,	180	,	140	),	"Tan");
  insertColor(qRgb(	238	,	130	,	238	),	"Violet");
  insertColor(qRgb(	244	,	164	,	96	),	"Sandy Brown");
  insertColor(qRgb(	233	,	150	,	122	),	"Dark Salmon");
  insertColor(qRgb(	189	,	183	,	107	),	"Dark khaki");
  insertColor(qRgb(	127	,	255	,	0	),	"Chartreuse");
  insertColor(qRgb(	169	,	169	,	169	),	"Dark Gray");
  insertColor(qRgb(	124	,	252	,	0	),	"Lawn Green");
  insertColor(qRgb(	255	,	105	,	180	),	"Hot Pink");
  insertColor(qRgb(	250	,	128	,	114	),	"Salmon");
  insertColor(qRgb(	240	,	128	,	128	),	"Light Coral");
  insertColor(qRgb(	64	,	224	,	208	),	"Turquoise");
  insertColor(qRgb(	143	,	188	,	143	),	"Dark Seagreen");
  insertColor(qRgb(	218	,	112	,	214	),	"Orchid");
  insertColor(qRgb(	102	,	205	,	170	),	"Medium Aquamarine");
  insertColor(qRgb(	255	,	127	,	80	),	"Coral");
  insertColor(qRgb(	154	,	205	,	50	),	"Yellow Green");
  insertColor(qRgb(	218	,	165	,	32	),	"Goldenrod");
  insertColor(qRgb(	72	,	209	,	204	),	"Medium Turquoise");
  insertColor(qRgb(	188	,	143	,	143	),	"Rosy Brown");
  insertColor(qRgb(	219	,	112	,	147	),	"Pale VioletRed");
  insertColor(qRgb(	0	,	250	,	154	),	"Medium Spring Green");
  insertColor(qRgb(	255	,	99	,	71	),	"Tomato");
  insertColor(qRgb(	0	,	255	,	127	),	"Spring Green");
  insertColor(qRgb(	205	,	133	,	63	),	"Peru");
  insertColor(qRgb(	100	,	149	,	237	),	"Cornflower Blue");
  insertColor(qRgb(	132	,	112	,	255	),	"Light Slate Blue");
  insertColor(qRgb(	147	,	112	,	219	),	"Medium Purple");
  insertColor(qRgb(	186	,	85	,	211	),	"Medium Orchid");
  insertColor(qRgb(	95	,	158	,	160	),	"Cadet Blue");
  insertColor(qRgb(	0	,	206	,	209	),	"Dark Turquoise");
  insertColor(qRgb(	0	,	191	,	255	),	"Deep Skyblue");
  insertColor(qRgb(	119	,	136	,	153	),	"Light Slate Grey");
  insertColor(qRgb(	184	,	134	,	11	),	"Dark Goldenrod");
  insertColor(qRgb(	123	,	104	,	238	),	"MediumSlate Blue");
  insertColor(qRgb(	205	,	92	,	92	),	"IndianRed");
  insertColor(qRgb(	210	,	105	,	30	),	"Chocolate");
  insertColor(qRgb(	60	,	179	,	113	),	"Medium Sea Green");
  insertColor(qRgb(	50	,	205	,	50	),	"Lime Ggreen");
  insertColor(qRgb(	32	,	178	,	170	),	"Light Sea Green");
  insertColor(qRgb(	112	,	128	,	144	),	"Slate Gray");
  insertColor(qRgb(	30	,	144	,	255	),	"Dodger Blue");
  insertColor(qRgb(	255	,	69	,	0	),	"Orange Red");
  insertColor(qRgb(	255	,	20	,	147	),	"Deep Pink");
  insertColor(qRgb(	70	,	130	,	180	),	"Steel Blue");
  insertColor(qRgb(	106	,	90	,	205	),	"Slate Blue");
  insertColor(qRgb(	107	,	142	,	35	),	"Olive Drab");
  insertColor(qRgb(	65	,	105	,	225	),	"Royal Blue");
  insertColor(qRgb(	208	,	32	,	144	),	"Violet Red");
  insertColor(qRgb(	153	,	50	,	204	),	"Dark Orchid");
  insertColor(qRgb(	160	,	32	,	240	),	"Purple");
  insertColor(qRgb(	105	,	105	,	105	),	"Dim Gray");
  insertColor(qRgb(	138	,	43	,	226	),	"Blue Violet");
  insertColor(qRgb(	160	,	82	,	45	),	"Sienna");
  insertColor(qRgb(	199	,	21	,	133	),	"Medium Violet Red");
  insertColor(qRgb(	176	,	48	,	96	),	"Maroon");
  insertColor(qRgb(	46	,	139	,	87	),	"Sea Green");
  insertColor(qRgb(	85	,	107	,	47	),	"Dark Olive Green");
  insertColor(qRgb(	34	,	139	,	34	),	"Forest Green");
  insertColor(qRgb(	139	,	69	,	19	),	"Saddle Brown");
  insertColor(qRgb(	148	,	0	,	211	),	"Darkviolet");
  insertColor(qRgb(	178	,	34	,	34	),	"Fire Brick");
  insertColor(qRgb(	72	,	61	,	139	),	"Dark Slate Blue");
  insertColor(qRgb(	47	,	79	,	79	),	"Dark Slate Gray");
  insertColor(qRgb(	25	,	25	,	112	),	"Midnight Blue");
  insertColor(qRgb(	0	,	0	,	205	),	"Medium Blue");
  insertColor(qRgb(	0	,	0	,	128	),	"Navy");


/*
  insertColor(qRgb(	255	,	255	,	255	),	"white");
  insertColor(qRgb(	255	,	255	,	240	),	"ivory");
  insertColor(qRgb(	255	,	250	,	250	),	"snow");
  insertColor(qRgb(	245	,	255	,	250	),	"mintcream");
  insertColor(qRgb(	255	,	250	,	240	),	"floralwhite");
  insertColor(qRgb(	255	,	255	,	224	),	"lightyellow");
  insertColor(qRgb(	240	,	255	,	255	),	"azure");
  insertColor(qRgb(	248	,	248	,	255	),	"ghostwhite");
  insertColor(qRgb(	240	,	255	,	240	),	"honeydew");
  insertColor(qRgb(	255	,	245	,	238	),	"seashell");
  insertColor(qRgb(	240	,	248	,	255	),	"aliceblue");
  insertColor(qRgb(	255	,	248	,	220	),	"cornsilk");
  insertColor(qRgb(	255	,	240	,	245	),	"lavenderblush");
  insertColor(qRgb(	253	,	245	,	230	),	"oldlace");
  insertColor(qRgb(	245	,	245	,	245	),	"whitesmoke");
  insertColor(qRgb(	255	,	250	,	205	),	"lemonchiffon");
  insertColor(qRgb(	224	,	255	,	255	),	"lightcyan");
  insertColor(qRgb(	250	,	250	,	210	),	"lightgoldenrodyellow");
  insertColor(qRgb(	250	,	240	,	230	),	"linen");
  insertColor(qRgb(	245	,	245	,	220	),	"beige");
  insertColor(qRgb(	255	,	239	,	213	),	"papayawhip");
  insertColor(qRgb(	255	,	235	,	205	),	"blanchedalmond");
  insertColor(qRgb(	250	,	235	,	215	),	"antiquewhite");
  insertColor(qRgb(	255	,	228	,	225	),	"mistyrose");
  insertColor(qRgb(	230	,	230	,	250	),	"lavender");
  insertColor(qRgb(	255	,	228	,	196	),	"bisque");
  insertColor(qRgb(	255	,	228	,	181	),	"moccasin");
  insertColor(qRgb(	255	,	222	,	173	),	"navajowhite");
  insertColor(qRgb(	255	,	218	,	185	),	"peachpuff");
  insertColor(qRgb(	238	,	232	,	170	),	"palegoldenrod");
  insertColor(qRgb(	245	,	222	,	179	),	"wheat");
  insertColor(qRgb(	220	,	220	,	220	),	"gainsboro");
  insertColor(qRgb(	240	,	230	,	140	),	"khaki");
  insertColor(qRgb(	175	,	238	,	238	),	"paleturquoise");
  insertColor(qRgb(	255	,	192	,	203	),	"pink");
  insertColor(qRgb(	255	,	255	,	0	),	"yellow");
  insertColor(qRgb(	238	,	221	,	130	),	"lightgoldenrod");
  insertColor(qRgb(	211	,	211	,	211	),	"lightgrey");
  insertColor(qRgb(	255	,	182	,	193	),	"lightpink");
  insertColor(qRgb(	176	,	224	,	230	),	"powderblue");
  insertColor(qRgb(	127	,	255	,	212	),	"aquamarine");
  insertColor(qRgb(	216	,	191	,	216	),	"thistle");
  insertColor(qRgb(	173	,	216	,	230	),	"lightblue");
  insertColor(qRgb(	152	,	251	,	152	),	"palegreen");
  insertColor(qRgb(	255	,	215	,	0	),	"gold");
  insertColor(qRgb(	173	,	255	,	47	),	"greenyellow");
  insertColor(qRgb(	176	,	196	,	222	),	"lightsteelblue");
  insertColor(qRgb(	144	,	238	,	144	),	"lightgreen");
  insertColor(qRgb(	221	,	160	,	221	),	"plum");
  insertColor(qRgb(	190	,	190	,	190	),	"gray");
  insertColor(qRgb(	222	,	184	,	135	),	"burlywood");
  insertColor(qRgb(	135	,	206	,	250	),	"lightskyblue");
  insertColor(qRgb(	255	,	160	,	122	),	"lightsalmon");
  insertColor(qRgb(	135	,	206	,	235	),	"skyblue");
  insertColor(qRgb(	210	,	180	,	140	),	"tan");
  insertColor(qRgb(	238	,	130	,	238	),	"violet");
  insertColor(qRgb(	244	,	164	,	96	),	"sandybrown");
  insertColor(qRgb(	233	,	150	,	122	),	"darksalmon");
  insertColor(qRgb(	189	,	183	,	107	),	"darkkhaki");
  insertColor(qRgb(	127	,	255	,	0	),	"chartreuse");
  insertColor(qRgb(	255	,	165	,	0	),	"orange");
  insertColor(qRgb(	169	,	169	,	169	),	"darkgray");
  insertColor(qRgb(	124	,	252	,	0	),	"lawngreen");
  insertColor(qRgb(	255	,	105	,	180	),	"hotpink");
  insertColor(qRgb(	250	,	128	,	114	),	"salmon");
  insertColor(qRgb(	0	,	255	,	255	),	"cyan");
  insertColor(qRgb(	240	,	128	,	128	),	"lightcoral");
  insertColor(qRgb(	64	,	224	,	208	),	"turquoise");
  insertColor(qRgb(	143	,	188	,	143	),	"darkseagreen");
  insertColor(qRgb(	218	,	112	,	214	),	"orchid");
  insertColor(qRgb(	102	,	205	,	170	),	"mediumaquamarine");
  insertColor(qRgb(	255	,	127	,	80	),	"coral");
  insertColor(qRgb(	154	,	205	,	50	),	"yellowgreen");
  insertColor(qRgb(	218	,	165	,	32	),	"goldenrod");
  insertColor(qRgb(	72	,	209	,	204	),	"mediumturquoise");
  insertColor(qRgb(	188	,	143	,	143	),	"rosybrown");
  insertColor(qRgb(	255	,	140	,	0	),	"darkorange");
  insertColor(qRgb(	219	,	112	,	147	),	"palevioletred");
  insertColor(qRgb(	0	,	250	,	154	),	"mediumspringgreen");
  insertColor(qRgb(	255	,	99	,	71	),	"tomato");
  insertColor(qRgb(	0	,	255	,	127	),	"springgreen");
  insertColor(qRgb(	205	,	133	,	63	),	"peru");
  insertColor(qRgb(	100	,	149	,	237	),	"cornflowerblue");
  insertColor(qRgb(	132	,	112	,	255	),	"lightslateblue");
  insertColor(qRgb(	147	,	112	,	219	),	"mediumpurple");
  insertColor(qRgb(	186	,	85	,	211	),	"mediumorchid");
  insertColor(qRgb(	95	,	158	,	160	),	"cadetblue");
  insertColor(qRgb(	0	,	206	,	209	),	"darkturquoise");
  insertColor(qRgb(	0	,	191	,	255	),	"deepskyblue");
  insertColor(qRgb(	119	,	136	,	153	),	"lightslategrey");
  insertColor(qRgb(	184	,	134	,	11	),	"darkgoldenrod");
  insertColor(qRgb(	123	,	104	,	238	),	"mediumslateblue");
  insertColor(qRgb(	205	,	92	,	92	),	"indianred");
  insertColor(qRgb(	210	,	105	,	30	),	"chocolate");
  insertColor(qRgb(	60	,	179	,	113	),	"mediumseagreen");
  insertColor(qRgb(	0	,	255	,	0	),	"green");
  insertColor(qRgb(	50	,	205	,	50	),	"limegreen");
  insertColor(qRgb(	255	,	0	,	255	),	"magenta");
  insertColor(qRgb(	32	,	178	,	170	),	"lightseagreen");
  insertColor(qRgb(	112	,	128	,	144	),	"slategray");
  insertColor(qRgb(	30	,	144	,	255	),	"dodgerblue");
  insertColor(qRgb(	255	,	69	,	0	),	"orangered");
  insertColor(qRgb(	255	,	20	,	147	),	"deeppink");
  insertColor(qRgb(	70	,	130	,	180	),	"steelblue");
  insertColor(qRgb(	106	,	90	,	205	),	"slateblue");
  insertColor(qRgb(	107	,	142	,	35	),	"olivedrab");
  insertColor(qRgb(	65	,	105	,	225	),	"royalblue");
  insertColor(qRgb(	208	,	32	,	144	),	"violetred");
  insertColor(qRgb(	153	,	50	,	204	),	"darkorchid");
  insertColor(qRgb(	160	,	32	,	240	),	"purple");
  insertColor(qRgb(	105	,	105	,	105	),	"dimgray");
  insertColor(qRgb(	138	,	43	,	226	),	"blueviolet");
  insertColor(qRgb(	160	,	82	,	45	),	"sienna");
  insertColor(qRgb(	199	,	21	,	133	),	"mediumvioletred");
  insertColor(qRgb(	176	,	48	,	96	),	"maroon");
  insertColor(qRgb(	46	,	139	,	87	),	"seagreen");
  insertColor(qRgb(	0	,	139	,	139	),	"darkcyan");
  insertColor(qRgb(	85	,	107	,	47	),	"darkolivegreen");
  insertColor(qRgb(	255	,	0	,	0	),	"red");
  insertColor(qRgb(	34	,	139	,	34	),	"forestgreen");
  insertColor(qRgb(	139	,	69	,	19	),	"saddlebrown");
  insertColor(qRgb(	165	,	42	,	42	),	"brown");
  insertColor(qRgb(	148	,	0	,	211	),	"darkviolet");
  insertColor(qRgb(	178	,	34	,	34	),	"firebrick");
  insertColor(qRgb(	72	,	61	,	139	),	"darkslateblue");
  insertColor(qRgb(	139	,	0	,	139	),	"darkmagenta");
  insertColor(qRgb(	47	,	79	,	79	),	"darkslategray");
  insertColor(qRgb(	0	,	100	,	0	),	"darkgreen");
  insertColor(qRgb(	139	,	0	,	0	),	"darkred");
  insertColor(qRgb(	0	,	0	,	255	),	"blue");
  insertColor(qRgb(	25	,	25	,	112	),	"midnightblue");
  insertColor(qRgb(	0	,	0	,	205	),	"mediumblue");
  insertColor(qRgb(	0	,	0	,	139	),	"darkblue");
  insertColor(qRgb(	0	,	0	,	128	),	"navy");
  insertColor(qRgb(	0	,	0	,	0	),	"black");
*/
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
}

void TKColorPanel::resetGrid()
{
  m_iX = 0;
  m_iY = 0;

  if (m_pLayout)
    delete m_pLayout;
  m_pLayout = new QGridLayout(this,0,m_iWidth+1,0,0);

  emit sizeChanged();
}

void TKColorPanel::clear()
{
  m_pColorDict.setAutoDelete(true);
  m_pColorDict.clear();
  m_pColorDict.setAutoDelete(false);

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

void TKColorPanel::selected( const QColor& color )
{
  emit colorSelected(color);
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
  if (m_bActive)
    setFrameStyle( Panel | Sunken );
  else
    setFrameStyle( NoFrame );
}

void TKColorPanelButton::mouseReleaseEvent( QMouseEvent* )
{
  emit selected(m_Color);
}
#include "tkcoloractions.moc"
