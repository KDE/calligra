/*
  $Id$

  Actually this code is a modification of the KFontDialog class, written
  by Bernd Johannes Wuebben  (wuebben@math.cornell.edu).
  
  Copyright (C) 1996 Bernd Johannes Wuebben   
  wuebben@math.cornell.edu
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifdef HAVE_CONFIG_H
// #include <config.h>
#endif

#include "FontSelector.h"
#include "FontSelector.moc"

#include "stdio.h"
#include "stdlib.h"
#include <qfile.h>
#include <qstrlist.h> 
#include <qfile.h>
#include <qtextstream.h> 
#include <qapplication.h>
#include <X11/Xlib.h>

#include <klocale.h>
#include <kapp.h>

#include "version.h"

#define YOFFSET  5
#define XOFFSET  5
#define LABLE_LENGTH  40
#define LABLE_HEIGHT 20
#define SIZE_X 400
#define SIZE_Y 280
#define FONTLABLE_LENGTH 60
#define COMBO_BOX_HEIGHT 28
#define COMBO_ADJUST 3
#define OKBUTTONY 260
#define BUTTONHEIGHT 25

#define CHARSETS_COUNT 9
static const char *charsetsStr[CHARSETS_COUNT]={"ISO-8859-1","Any",
                                   "ISO-8859-2",
                                   "ISO-8859-3",
                                   "ISO-8859-4",
                                   "ISO-8859-5",
                                   "ISO-8859-6",
                                   "ISO-8859-7",
                                   "ISO-8859-8"};

static QFont::CharSet charsetsIds[CHARSETS_COUNT]={ QFont::ISO_8859_1,
                                             QFont::AnyCharSet,
					     QFont::ISO_8859_2,
					     QFont::ISO_8859_3,
					     QFont::ISO_8859_4,
					     QFont::ISO_8859_5,
					     QFont::ISO_8859_6,
					     QFont::ISO_8859_7,
					     QFont::ISO_8859_8};
                   

FontSelector::FontSelector (QWidget *parent, const char *name, 
			    const char* example, 
			    const QStrList* fontlist) : QWidget (parent, name) {

  box1 = new QGroupBox(this, "Box1");
  box1->setGeometry(XOFFSET,YOFFSET,SIZE_X -  XOFFSET
		   ,150);
  box1->setTitle( i18n("Requested Font") );

  box1 = new QGroupBox(this, "Box2");
  box1->setGeometry(XOFFSET,160,SIZE_X -  XOFFSET
		   ,130);
  box1->setTitle( i18n("Actual Font") );
  

  family_label = new QLabel(this,"family");
  family_label->setText( i18n("Family:") );
  family_label->setGeometry(3*XOFFSET,8*YOFFSET -5,LABLE_LENGTH,LABLE_HEIGHT);

  actual_family_label = new QLabel(this,"afamily");
  actual_family_label->setText(i18n("Family:"));
  actual_family_label->setGeometry(3*XOFFSET,200,50,LABLE_HEIGHT);

  actual_family_label_data = new QLabel(this,"afamilyd");
  actual_family_label_data->setGeometry(3*XOFFSET +60 ,200,110,LABLE_HEIGHT);

  charset_label = new QLabel(this,"charset");
  charset_label->setText(i18n("Charset:"));
  charset_label->setGeometry(3*XOFFSET,
    			    11*YOFFSET - COMBO_ADJUST +65 , LABLE_LENGTH +10,
			     LABLE_HEIGHT);

  actual_charset_label = new QLabel(this,"acharset");
  actual_charset_label->setText(i18n("Charset:"));
  actual_charset_label->setGeometry(3*XOFFSET,200 - LABLE_HEIGHT,50,LABLE_HEIGHT);

  actual_charset_label_data = new QLabel(this,"acharsetd");
  actual_charset_label_data->setGeometry(3*XOFFSET +60 ,200 - LABLE_HEIGHT,110,LABLE_HEIGHT);

  size_label = new QLabel(this,"size");
  size_label->setText(i18n("Size:"));
  size_label->setGeometry(6*XOFFSET + LABLE_LENGTH + 12*XOFFSET +2* FONTLABLE_LENGTH,
			  8*YOFFSET -5,LABLE_LENGTH,LABLE_HEIGHT);

  actual_size_label = new QLabel(this,"asize");
  actual_size_label->setText(i18n("Size:"));
  actual_size_label->setGeometry(3*XOFFSET,200 +LABLE_HEIGHT ,
				 LABLE_LENGTH +10,LABLE_HEIGHT);

  actual_size_label_data = new QLabel(this,"asized");
  actual_size_label_data->setGeometry(3*XOFFSET +60 ,200 + LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);

  weight_label = new QLabel(this,"weight");
  weight_label->setText(i18n("Weight:"));
  weight_label->setGeometry(3*XOFFSET,15*YOFFSET + LABLE_HEIGHT -20 
			  ,LABLE_LENGTH,LABLE_HEIGHT);

  actual_weight_label = new QLabel(this,"aweight");
  actual_weight_label->setText(i18n("Weight:"));
  actual_weight_label->setGeometry(3*XOFFSET,200 + 2*LABLE_HEIGHT ,
				 LABLE_LENGTH +10,LABLE_HEIGHT);

  actual_weight_label_data = new QLabel(this,"aweightd");
  actual_weight_label_data->setGeometry(3*XOFFSET +60 ,200 + 2*LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);

  style_label = new QLabel(this,"style");
  style_label->setText(i18n("Style:"));
  style_label->setGeometry(6*XOFFSET + LABLE_LENGTH + 12*XOFFSET + 
			   2*FONTLABLE_LENGTH,
			   15*YOFFSET + LABLE_HEIGHT  -20
			 ,LABLE_LENGTH,
			   LABLE_HEIGHT);

  actual_style_label = new QLabel(this,"astyle");
  actual_style_label->setText(i18n("Style:"));
  actual_style_label->setGeometry(3*XOFFSET,200 + 3*LABLE_HEIGHT ,
				 LABLE_LENGTH +10,LABLE_HEIGHT);

  actual_style_label_data = new QLabel(this,"astyled");
  actual_style_label_data->setGeometry(3*XOFFSET +60 ,200 + 3*LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);


  family_combo = new QComboBox(true, this, "Family" );
  family_combo->setInsertionPolicy(QComboBox::NoInsertion);


  family_combo->setGeometry(6*XOFFSET + LABLE_LENGTH
			    ,8*YOFFSET - COMBO_ADJUST -5 ,4* LABLE_LENGTH,COMBO_BOX_HEIGHT);
#ifdef NEWKDE
  connect( family_combo, SIGNAL(activated(const QString &)),
	   SLOT(family_chosen_slot(const QString &)) );
#else
  connect( family_combo, SIGNAL(activated(const char *)),
	   SLOT(family_chosen_slot(const char *)) );
#endif
  //   QToolTip::add( family_combo, "Select Font Family" );

  if (fontlist != 0L){
    if(fontlist->count() !=0){

      family_combo->insertStrList(fontlist, -1);

      // this is a dirty fix due to a bug in Qt 1.2
      family_combo->setCurrentItem(1);
      family_combo->setCurrentItem(0);
      
    }
  }
  else{
    fill_family_combo();
  }


  charset_combo = new QComboBox( TRUE, this, i18n("Charset") );
  charset_combo->setGeometry(6*XOFFSET + LABLE_LENGTH
			    ,11*YOFFSET - COMBO_ADJUST +60 ,
			     4* LABLE_LENGTH,COMBO_BOX_HEIGHT);
  for(int i=0;i<CHARSETS_COUNT;i++)
      charset_combo->insertItem( charsetsStr[i] );

  charset_combo->setInsertionPolicy(QComboBox::NoInsertion);
  connect( charset_combo, SIGNAL(activated(int)),
	   SLOT(charset_chosen_slot(int)) );
  // QToolTip::add( charset_combo, "Select Font Weight" );

  size_combo = new QComboBox( true, this, i18n("Size") );
  size_combo->insertItem( "4" );
  size_combo->insertItem( "5" );
  size_combo->insertItem( "6" );
  size_combo->insertItem( "7" );
  size_combo->insertItem( "8" );
  size_combo->insertItem( "9" );
  size_combo->insertItem( "10" );
  size_combo->insertItem( "11" );
  size_combo->insertItem( "12" );
  size_combo->insertItem( "13" );
  size_combo->insertItem( "14" );
  size_combo->insertItem( "15" );
  size_combo->insertItem( "16" );
  size_combo->insertItem( "17" );
  size_combo->insertItem( "18" );
  size_combo->insertItem( "19" );
  size_combo->insertItem( "20" );
  size_combo->insertItem( "22" );
  size_combo->insertItem( "24" );
  size_combo->insertItem( "26" );
  size_combo->insertItem( "28" );
  size_combo->insertItem( "32" );
  size_combo->insertItem( "48" );
  size_combo->insertItem( "64" );

  // we may want to allow the user to choose another size, since I
  // can really not presume to have listed all useful sizes.

  //  size_combo->setInsertionPolicy(QComboBox::NoInsertion);

  size_combo->setGeometry(10*XOFFSET + 6*LABLE_LENGTH
			    ,8*YOFFSET - COMBO_ADJUST -5
			  ,2*LABLE_LENGTH + 20,COMBO_BOX_HEIGHT);
#ifdef NEWKDE
  connect( size_combo, SIGNAL(activated(const QString&)),
	   SLOT(size_chosen_slot(const QString&)) );
#else
  connect( size_combo, SIGNAL(activated(const char *)),
	   SLOT(size_chosen_slot(const char *)) );
#endif
  //  QToolTip::add( size_combo, "Select Font Size in Points" );


  weight_combo = new QComboBox( TRUE, this, i18n("Weight") );
  weight_combo->insertItem( i18n("normal") );
  weight_combo->insertItem( i18n("bold") );
  weight_combo->setGeometry(6*XOFFSET + LABLE_LENGTH
			    ,19*YOFFSET - COMBO_ADJUST -20
			    ,4*LABLE_LENGTH,COMBO_BOX_HEIGHT);
  weight_combo->setInsertionPolicy(QComboBox::NoInsertion);
#ifdef NEWKDE
  connect( weight_combo, SIGNAL(activated(const QString&)),
	   SLOT(weight_chosen_slot(const QString&)) );
#else
  connect( weight_combo, SIGNAL(activated(const char *)),
	   SLOT(weight_chosen_slot(const char *)) );
#endif
  // QToolTip::add( weight_combo, "Select Font Weight" );

  style_combo = new QComboBox( TRUE, this, i18n("Style") );
  style_combo->insertItem( i18n("roman") );
  style_combo->insertItem( i18n("italic") );
  style_combo->setGeometry(10*XOFFSET + 6*LABLE_LENGTH
			    ,19*YOFFSET- COMBO_ADJUST - 20
			   ,2*LABLE_LENGTH + 20,COMBO_BOX_HEIGHT);
  style_combo->setInsertionPolicy(QComboBox::NoInsertion);
#ifdef NEWKDE
  connect( style_combo, SIGNAL(activated(const QString &)),
	   SLOT(style_chosen_slot(const QString &)) );
#else
  connect( style_combo, SIGNAL(activated(const char *)),
	   SLOT(style_chosen_slot(const char *)) );
#endif
  //QToolTip::add( style_combo, "Select Font Style" );
  

/*
  cancel_button = new QPushButton( i18n("Cancel"),this);

  cancel_button->setGeometry( 3*XOFFSET +100, OKBUTTONY +40, 80, BUTTONHEIGHT );
  connect( cancel_button, SIGNAL( clicked() ), SLOT( reject() ) );

  ok_button = new QPushButton( i18n(OK), this );
  ok_button->setGeometry( 3*XOFFSET, OKBUTTONY +40,80, BUTTONHEIGHT );
  connect( ok_button, SIGNAL( clicked() ), SLOT( accept() ) );	
*/

  example_label = new QLabel(this,"examples");

  example_label->setFont(selFont);

  example_label->setGeometry(200,190,190, 80);
  example_label->setAlignment(AlignCenter);
  //  example_label->setBackgroundColor(white);
  example_label->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  example_label->setLineWidth( 1 );
  example_label->setText(example);
  //  example_label->setAutoResize(true);

  connect(this,SIGNAL(fontSelected( const QFont&  )),
	  this,SLOT(display_example( const QFont&)));

  this->setMaximumSize(405,330);
  this->setMinimumSize(405,330);

  // let's initialize the display if possible
  if(family_combo->count() != 0){
    this->setFont(QFont(family_combo->text(0),12,QFont::Normal));
  }

  
  setColors();

#if NEWKDE
  connect(KApplication::kApplication(),SIGNAL(kdisplayPaletteChanged()),
          this,SLOT(setColors()));
#else
  connect(KApplication::getKApplication(),SIGNAL(kdisplayPaletteChanged()),
	  this,SLOT(setColors()));
#endif
}


void FontSelector::charset_chosen_slot(int index){

  selFont.setCharSet(charsetsIds[index]);
  emit fontSelected(selFont);
}

void FontSelector::setFont( const QFont& aFont){

  selFont = aFont;
  setCombos();
  display_example(selFont);
}  

void FontSelector::family_chosen_slot(const char *family){
  family_chosen_slot(QString(family));
}

void FontSelector::family_chosen_slot(const QString &family){

  selFont.setFamily(family);
  //display_example();
  emit fontSelected(selFont);
}

void FontSelector::size_chosen_slot(const char* size){
  size_chosen_slot(QString(size));
}

void FontSelector::size_chosen_slot(const QString &size_string){
  selFont.setPointSize(size_string.toInt());
  //display_example();
  emit fontSelected(selFont);
}

void FontSelector::weight_chosen_slot(const char* weight) {
  weight_chosen_slot(QString(weight));
}

void FontSelector::weight_chosen_slot(const QString &weight_string){
  if ( weight_string == QString(i18n("normal")))
    selFont.setBold(false);
  if ( weight_string == QString(i18n("bold")))
       selFont.setBold(true);
  // display_example();
  emit fontSelected(selFont);
}

void FontSelector::style_chosen_slot(const char* style){
  style_chosen_slot(QString(style));
}

void FontSelector::style_chosen_slot(const QString &style_string){
  if ( style_string == QString(i18n("roman")))
    selFont.setItalic(false);
  if ( style_string == QString(i18n("italic")))
    selFont.setItalic(true);
  //  display_example();
  emit fontSelected(selFont);
}
       

void FontSelector::display_example(const QFont& font){

  QString string;
  int i;

  example_label->setFont(font);

  QFontInfo info = example_label->fontInfo();
  actual_family_label_data->setText(info.family());
  
  string.setNum(info.pointSize());
  actual_size_label_data->setText(string);

  if (info.bold())
    actual_weight_label_data->setText(i18n("Bold"));
  else
    actual_weight_label_data->setText(i18n("Normal"));
 
  if (info.italic())
    actual_style_label_data->setText(i18n("italic"));
  else
    actual_style_label_data->setText(i18n("roman"));
  
 QFont::CharSet charset=info.charSet();
  for(i = 0;i<CHARSETS_COUNT;i++)
    if (charset==charsetsIds[i]){
      actual_charset_label_data->setText(charsetsStr[i]);
      break;
    }
  
}

void FontSelector::setCombos(){

 QString string;
 QComboBox* combo;
 int number_of_entries, i=0; 
 bool found;

 number_of_entries =  family_combo->count(); 
 string = selFont.family();
 combo = family_combo; 
 found = false;

 for (i = 0;i < number_of_entries ; i++){
   //   printf("%s with %s\n",string.data(), ((QString) combo->text(i)).data());
   if ( string.lower() == ((QString) combo->text(i)).lower()){
     combo->setCurrentItem(i);
     //printf("Found Font %s\n",string.data());
     found = true;
     break;
   }
 }

 
 number_of_entries =  size_combo->count(); 
 string.setNum(selFont.pointSize());
 combo = size_combo; 
 found = false;

 for (i = 0;i < number_of_entries - 1; i++){
   if ( string == (QString) combo->text(i)){
     combo->setCurrentItem(i);
     found = true;
     // printf("Found Size %s setting to item %d\n",string.data(),i);
     break;
   }
 }

 if (selFont.bold()){
   //weight_combo->setCurrentItem(0);
   weight_combo->setCurrentItem(1);
 }else
   weight_combo->setCurrentItem(0);

  if (selFont.italic())
   style_combo->setCurrentItem(1);
 else
   style_combo->setCurrentItem(0);

 QFont::CharSet charset=selFont.charSet();
 for(i = 0;i<CHARSETS_COUNT;i++)
   if (charset==charsetsIds[i]){
     charset_combo->setCurrentItem(i);
     break;
   }

}

bool FontSelector::loadKDEInstalledFonts(){

  QString fontfilename;

  //TODO replace by QDir::homePath();

  fontfilename =  getenv("HOME");
  fontfilename = fontfilename + "/.kde/share/config/kdefonts";

  QFile fontfile(fontfilename);

  if (!fontfile.exists())
    return false;

  if(!fontfile.open(IO_ReadOnly)){
    return false;
  }

  if (!fontfile.isReadable())
    return false;
  
  
  QTextStream t(&fontfile);

  while ( !t.eof() ) {
    QString s = t.readLine();
    s = s.stripWhiteSpace();
    if(!s.isEmpty())
      family_combo->insertItem( s ,-1 );
  }

  fontfile.close();

  
  return true;

}


void FontSelector::fill_family_combo(){

  int numFonts;
  Display *kde_display;
  char** fontNames;
  char** fontNames_copy;
  QString qfontname;


    
  QStrList fontlist(TRUE);
  
  kde_display = XOpenDisplay( 0L );

  // now try to load the KDE fonts

  bool have_installed = loadKDEInstalledFonts();
  
  // if available we are done, the kde fonts are now in the family_combo

  if (have_installed)
    return;

  fontNames = XListFonts(kde_display, "*", 32767, &numFonts);
  fontNames_copy = fontNames;

  for(int i = 0; i < numFonts; i++){
    
    if (**fontNames != '-'){ 
      
      // The font name doesn't start with a dash -- an alias
      // so we ignore it. It is debatable whether this is the right
      // behaviour so I leave the following snippet of code around.
      // Just uncomment it if you want those aliases to be inserted as well.
      
      /*
      qfontname = "";
      qfontname = *fontNames;
      if(fontlist.find(qfontname) == -1)
          fontlist.inSort(qfontname);
      */

      fontNames ++;
      continue;
    };
      
    qfontname = "";
    qfontname = *fontNames;
    int dash = qfontname.find ('-', 1, TRUE); // find next dash

    if (dash == -1) { // No such next dash -- this shouldn't happen.
                      // but what do I care -- lets skip it.
      fontNames ++;
      continue;
    }

    // the font name is between the second and third dash so:
    // let's find the third dash:

    int dash_two = qfontname.find ('-', dash + 1 , TRUE); 

    if (dash == -1) { // No such next dash -- this shouldn't happen.
                      // But what do I care -- lets skip it.
      fontNames ++;
      continue;
    }

    // fish the name of the font info string

    qfontname = qfontname.mid(dash +1, dash_two - dash -1);

    if( !qfontname.contains("open look", TRUE)){
      if(qfontname != "nil"){
	if(fontlist.find(qfontname) == -1)
	  fontlist.inSort(qfontname);
      }
    }
  

    fontNames ++;

  }

  for(fontlist.first(); fontlist.current(); fontlist.next())
      family_combo->insertItem(fontlist.current(),-1);

  XFreeFontNames(fontNames_copy);
  XCloseDisplay(kde_display);


}


void FontSelector::setColors(){
 
  /* this is to the the backgound of a widget to white and the
     text color to black -- some lables such as the one of the
     font manager really shouldn't follow colorschemes The
     primary task of those label is to display the text clearly
     an visibly and not to look pretty ...*/

  QPalette mypalette = (example_label->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(black,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),black,white);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  example_label->setPalette(mypalette);
  example_label->setBackgroundColor(white);
 
}

