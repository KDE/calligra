/* libppt - library to read PowerPoint presentation
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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
   Boston, MA 02111-1307, US
*/

#include "powerpoint.h"
#include "presentation.h"
#include "pole.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <stdio.h>

// Use anonymous namespace to cover following functions
namespace{

static inline unsigned long readU16( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8);
}

static inline signed long readS16( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8);
}

static inline unsigned long readU32( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

static inline signed long readS32( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

}

namespace Libppt 
{
std::ostream& operator<<( std::ostream& s, UString ustring )
{
  char* str = ustring.ascii();
  s << str;
  return s;
}

}


using namespace Libppt;


// ========== base record ==========

const unsigned int Record::id = 0; // invalid of-course

Record::Record()
{
  stream_position = 0;
  record_parent = 0;
}

Record::~Record()
{
}

Record* Record::create( unsigned type )
{
  Record* record = 0;
  
  if( type == DocumentContainer::id )
    record = new DocumentContainer();
    
  else if( type == EnvironmentContainer::id )
    record = new EnvironmentContainer();  
    
  else if( type == ExObjListContainer::id )
    record = new ExObjListContainer();  
    
  else if( type == ExOleObjStgContainer::id )
    record = new ExOleObjStgContainer();  
    
  else if( type == ExHyperlinkContainer::id )
    record = new ExHyperlinkContainer();        
    
  else if( type == ExEmbedContainer::id )
    record = new ExEmbedContainer();    
    
  else if( type == FontCollectionContainer::id )
    record = new FontCollectionContainer();
    
  else if( type == HeadersFootersContainer::id )
    record = new HeadersFootersContainer();
    
  else if( type == ListContainer::id )
    record = new ListContainer();  
    
  else if( type == SlideContainer::id )
    record = new SlideContainer(); 

  else if( type == SlideBaseContainer::id )
    record = new SlideBaseContainer(); 
      
  else if( type == MainMasterContainer::id )
    record = new MainMasterContainer(); 
    
  else if( type == NotesContainer::id )
    record = new NotesContainer();   
    
  else if( type == SlideListWithTextContainer::id )
    record = new SlideListWithTextContainer();  
    
  else if( type == SlideViewInfoContainer::id )
    record = new SlideViewInfoContainer();  
    
  else if( type == SorterViewInfoContainer::id )
    record = new SorterViewInfoContainer();    
    
  else if( type == SrKinsokuContainer::id )
    record = new SrKinsokuContainer();    
    
  else if( type == OutlineViewInfoContainer::id )
    record = new OutlineViewInfoContainer();   
    
  else if( type == ProgStringTagContainer ::id )
    record = new ProgStringTagContainer ();    
    
  else if( type == PPDrawingGroupContainer ::id )
    record = new PPDrawingGroupContainer ();      
    
  else if( type == PPDrawingContainer ::id )
    record = new PPDrawingContainer ();    
    
  else if( type == ProgBinaryTagContainer ::id )
    record = new ProgBinaryTagContainer ();  
    
  else if( type == ProgTagsContainer ::id )
   record = new ProgTagsContainer ();  
   
  else if( type == VBAInfoContainer::id )
    record = new VBAInfoContainer();  
    
  else if( type == ViewInfoContainer::id )
    record = new ViewInfoContainer();  
    
    
       
  else if( type == CStringAtom::id )
    record = new CStringAtom();
    
  else if( type == ColorSchemeAtom::id )
    record = new ColorSchemeAtom();  
    
  else if( type == CurrentUserAtom::id )
    record = new CurrentUserAtom();    
    
  else if( type == DocumentAtom::id )
    record = new DocumentAtom();
    
  else if( type == EndDocumentAtom::id )
    record = new EndDocumentAtom();
    
  else if( type == ExEmbedAtom::id )
    record = new ExEmbedAtom();  
    
  else if( type == ExHyperlinkAtom::id )
    record = new ExHyperlinkAtom();   
    
  else if( type == ExObjListAtom::id )
    record = new ExObjListAtom();  
    
  else if( type == ExOleObjAtom::id )
    record = new ExOleObjAtom();   
     
  else if( type == FontEntityAtom::id )
    record = new FontEntityAtom();  
    
  else if( type == GuideAtom::id )
    record = new GuideAtom();
    
  else if( type == HeadersFootersAtom ::id )
    record = new HeadersFootersAtom ();
    
  else if( type == NotesAtom::id )
    record = new NotesAtom();  
    
  else if( type == PersistPtrIncrementalBlockAtom::id )
    record = new PersistPtrIncrementalBlockAtom();     
          
  else if( type == Record1043::id )
    record = new Record1043 (); 
  else if( type == Record1044::id )
    record = new Record1044 ();  
    
  else if( type == SrKinsokuAtom::id )
    record = new SrKinsokuAtom();  
    
  else if( type == SlideAtom::id )
    record = new SlideAtom();  
    
  else if( type == SlidePersistAtom::id )
    record = new SlidePersistAtom();  
       
  else if( type == StyleTextPropAtom::id )
    record = new StyleTextPropAtom(); 
      
  else if( type == SlideViewInfoAtom::id )
    record = new SlideViewInfoAtom();   
    
  else if( type == SSSlideInfoAtom ::id )
    record = new SSSlideInfoAtom();   
     
  else if( type == TextHeaderAtom ::id )
    record = new TextHeaderAtom();   
     
  else if( type == TextBytesAtom::id )
    record = new TextBytesAtom ();  
    
  else if( type == TextCharsAtom::id )
    record = new TextCharsAtom ();  
    
  else if( type == TextSpecInfoAtom  ::id )
    record = new TextSpecInfoAtom();   
    
  else if( type == TxCFStyleAtom  ::id )
    record = new TxCFStyleAtom();  
    
  else if( type == TxMasterStyleAtom  ::id )
    record = new TxMasterStyleAtom();   
    
  else if( type == TxPFStyleAtom  ::id )
    record = new TxPFStyleAtom(); 
    
  else if( type == TxSIStyleAtom  ::id )
    record = new TxSIStyleAtom();         
    
  else if( type == UserEditAtom::id )
    record = new UserEditAtom();     
    
  else if( type == ViewInfoAtom::id )
    record = new ViewInfoAtom();     
    
  return record;
};

void Record::setParent( Record* parent )
{
  record_parent = parent;
}

const Record* Record::parent() const
{
  return record_parent;
}

void Record::setPosition( unsigned pos )
{
  stream_position = pos;
}
  
unsigned Record::position() const
{
  return stream_position;
}

void Record::setData( unsigned, const unsigned char* )
{
}

void Record::dump( std::ostream& ) const
{
  // nothing to dump
}

// ========== Container ==========

Container::Container()
{
}

// ========== DocumentContainer ==========

const unsigned int DocumentContainer::id = 1000;

DocumentContainer::DocumentContainer()
{
}

// ========== NotesContainer ==========

const unsigned int NotesContainer::id = 1008;

NotesContainer::NotesContainer()
{
}

// ========== ExOleObjStgContainer ==========

const unsigned int ExOleObjStgContainer::id = 4113;

ExOleObjStgContainer::ExOleObjStgContainer()
{
}

// ========== FontCollectionContainer ==========

const unsigned int FontCollectionContainer::id = 2005;

FontCollectionContainer::FontCollectionContainer()
{
}

// ========== ExObjListContainer ==========

const unsigned int ExObjListContainer::id = 1033;

ExObjListContainer::ExObjListContainer()
{
}

// ========== SlideContainer ==========

const unsigned int SlideContainer::id = 1006;

SlideContainer::SlideContainer()
{
}

// ========== SlideBaseContainer ==========

const unsigned int SlideBaseContainer::id = 1004;

SlideBaseContainer::SlideBaseContainer()
{
}


// ========== SlideListWithTextContainer ==========

const unsigned int SlideListWithTextContainer::id = 4080;

SlideListWithTextContainer::SlideListWithTextContainer()
{
}

// ========== SlideViewInfoContainer ==========

const unsigned int SlideViewInfoContainer::id = 1018;

SlideViewInfoContainer::SlideViewInfoContainer()
{
}

// ========== OutlineViewInfoContainer ==========

const unsigned int OutlineViewInfoContainer::id = 1031;

OutlineViewInfoContainer::OutlineViewInfoContainer()
{
}

// ========== SorterViewInfoContainer  ==========

const unsigned int SorterViewInfoContainer::id = 1032;

SorterViewInfoContainer::SorterViewInfoContainer()
{
}

// ========== ListContainer ==========

const unsigned int ListContainer::id = 2000;

ListContainer::ListContainer()
{
}

// ========== ExEmbedContainer ==========

const unsigned int ExEmbedContainer::id = 4044;

ExEmbedContainer::ExEmbedContainer()
{
}

// ========== ExHyperlinkContainer ==========

const unsigned int ExHyperlinkContainer::id = 4055;

ExHyperlinkContainer::ExHyperlinkContainer()
{
}

// ========== MainMasterContainer ==========

const unsigned int MainMasterContainer::id = 1016;

MainMasterContainer::MainMasterContainer()
{
}

// ========== EnvironmentContainer ==========

const unsigned int EnvironmentContainer::id = 1010;

EnvironmentContainer::EnvironmentContainer()
{
}

// ========== HeadersFootersContainer ==========

const unsigned int HeadersFootersContainer::id = 4057;

HeadersFootersContainer::HeadersFootersContainer()
{
}

// ========== SrKinsokuContainer ==========

const unsigned int SrKinsokuContainer::id = 4040;

SrKinsokuContainer::SrKinsokuContainer()
{
}

// ========== ProgTagsContainer ==========

const unsigned int ProgTagsContainer::id = 5000;

ProgTagsContainer::ProgTagsContainer()
{
}

// ========== ProgBinaryTagContainer ==========

const unsigned int ProgBinaryTagContainer::id = 5002;

ProgBinaryTagContainer::ProgBinaryTagContainer()
{
}

// ========== ProgBinaryTagContainer ==========

const unsigned int ProgStringTagContainer::id = 5001;

ProgStringTagContainer::ProgStringTagContainer()
{
}

// ========== PPDrawingGroupContainer ==========

const unsigned int PPDrawingGroupContainer::id = 1035;

PPDrawingGroupContainer::PPDrawingGroupContainer()
{
}

// ========== PPDrawingContainer ==========

const unsigned int PPDrawingContainer::id = 1036;

PPDrawingContainer::PPDrawingContainer()
{
}

// ========== VBAInfoContainer ==========

const unsigned int VBAInfoContainer::id = 1023;

VBAInfoContainer::VBAInfoContainer()
{
}

// ========== ViewInfoContainer ==========

const unsigned int ViewInfoContainer::id = 1020;

ViewInfoContainer::ViewInfoContainer()
{
}

// ========== CStringAtom ==========

const unsigned int CStringAtom::id = 4026;

class CStringAtom::Private
{
public:
  UString ustring;
};


CStringAtom::CStringAtom()
{
  d = new Private;
}

CStringAtom::~CStringAtom()
{
  delete d;
}
  
UString CStringAtom::ustring() const
{
  return d->ustring;
}

void CStringAtom::setUString( const UString& ustr )
{
  d->ustring = ustr;
}

void CStringAtom::setData( unsigned size, const unsigned char* data )
{
  UString str;
  for( unsigned k=0; k<size; k++ )
  {
    unsigned uchar = readU16( data + k*2 );
    str.append( UString(uchar) );
  }
  setUString( str );
}
  
void CStringAtom::dump( std::ostream& out ) const
{
  out << "CStringAtom" << std::endl;
  out << "String : [" << ustring() << "]" << std::endl;
}

// ========== DocumentAtom ==========

const unsigned int DocumentAtom::id = 1001;

class DocumentAtom::Private
{
public:
  int slideWidth;
  int slideHeight;
  int notesWidth;
  int notesHeight;
  int zoomNumer;
  int zoomDenom;
  int notesMasterPersist; 
  int handoutMasterPersist;  
  int firstSlideNum; 
  int slideSizeType; 
  int saveWithFonts; 
  int omitTitlePlace;   
  int rightToLeft;   
  int showComments; 

 
};

DocumentAtom::DocumentAtom()
{
  d = new Private;
  d->slideWidth = 5760;   // 10 inches
  d->slideHeight = 4320;  // 7.5 inches
  d->notesWidth = 4320; 
  d->notesHeight = 5760;
  d->zoomNumer = 1;
  d->zoomDenom = 2;
  d->notesMasterPersist = 0; 
  d->handoutMasterPersist = 0;  
  d->firstSlideNum = 0; 
  d->slideSizeType = 0; 
  d->saveWithFonts = 0; 
  d->omitTitlePlace = 0;   
  d->rightToLeft = 0 ;   
  d->showComments = 0; 
}

DocumentAtom::~DocumentAtom()
{
  delete d;
}

int DocumentAtom::slideWidth() const
{
  return d->slideWidth;
}

void DocumentAtom::setSlideWidth( int w ) 
{
  d->slideWidth = w;
}

int DocumentAtom::slideHeight() const
{
  return d->slideHeight;
}

void DocumentAtom::setSlideHeight( int h )
{
  d->slideHeight = h;
}

int DocumentAtom::notesWidth() const
{
  return d->notesWidth; 
}

void DocumentAtom::setNotesWidth( int nw )
{
  d->notesWidth = nw; 
}

int DocumentAtom::notesHeight() const
{
  return d->notesHeight; 
}

void DocumentAtom::setNotesHeight( int nh )
{
  d->notesHeight = nh; 
}

int DocumentAtom::zoomNumer () const
{
  return d->zoomNumer; 
}

void DocumentAtom::setZoomNumer( int numer )
{
  d->zoomNumer = numer; 
}

int DocumentAtom::zoomDenom() const
{
  return d->zoomDenom; 
}

void DocumentAtom::setZoomDenom( int denom )
{
  d->zoomDenom = denom; 
}  

int DocumentAtom::notesMasterPersist() const
{
  return d->notesMasterPersist; 
}

void DocumentAtom::setNotesMasterPersist( int notesMasterPersist )
{
  d->notesMasterPersist = notesMasterPersist; 
}
int DocumentAtom::handoutMasterPersist() const
{
  return d->handoutMasterPersist;
}
void DocumentAtom::setHandoutMasterPersist(int handoutMasterPersist)  
{
  d->handoutMasterPersist = handoutMasterPersist; 
}  
int DocumentAtom::firstSlideNum() const
{
  return d->firstSlideNum; 
}
void DocumentAtom::setFirstSlideNum( int firstSlideNum )
{
  d->firstSlideNum = firstSlideNum;
}  
int DocumentAtom::slideSizeType() const
{
  return d->slideSizeType; 
}
void DocumentAtom::setSlideSizeType( int slideSizeType )
{
  d->slideSizeType = slideSizeType; 
}  
int DocumentAtom::saveWithFonts() const
{
  return d->saveWithFonts; 
}
void DocumentAtom::setSaveWithFonts( int saveWithFonts )
{
  d->saveWithFonts = saveWithFonts; 
}  
int DocumentAtom::omitTitlePlace() const
{
  return d->omitTitlePlace; 
}
void DocumentAtom::setOmitTitlePlace( int omitTitlePlace )
{
  d->omitTitlePlace = omitTitlePlace; 
}  
int DocumentAtom::rightToLeft() const
{
  return d->rightToLeft; 
}
void DocumentAtom::setRightToLeft( int rightToLeft )
{
  d->rightToLeft = rightToLeft; 
}  
int DocumentAtom::showComments() const
{
  return d->showComments; 
}
void DocumentAtom::setShowComments( int showComments)
{
  d->showComments = showComments; 
}  
 
void DocumentAtom::setData( unsigned size, const unsigned char* data )
{
  setSlideWidth( readU32( data+0 ) );
  setSlideHeight( readU32( data+4 ) );
  setNotesWidth( readU32( data+8 ) );
  setNotesHeight( readU32( data+12 ) );
  setZoomNumer( readS32( data+16 ) );
  setZoomDenom( readS32( data+20) );
  setNotesMasterPersist( readU32( data+24 ) );
  setHandoutMasterPersist ( readU32( data+28 ) );
  setFirstSlideNum( readU16( data+32 ) );
  setSlideSizeType( readS16( data+34 ) );
  setSaveWithFonts(data[36]); 
  setOmitTitlePlace(data[37]); 
  setRightToLeft(data[38]); 
  setShowComments(data[39]); 
  
  
}

void DocumentAtom::dump( std::ostream& out ) const
{
  out << "DocumentAtom" << std::endl;
  out << "slide width " << slideWidth() << std::endl;
  out << "slide height " << slideHeight() << std::endl;
  out << "notes width " << notesWidth() << std::endl;
  out << "notes height " << notesHeight() << std::endl;
  out << "zoom numer " << zoomNumer() << std::endl;
  out << "zoom denum " << zoomDenom() << std::endl;
  out << "notesMasterPersist " << notesMasterPersist() << std::endl;
  out << "handoutMasterPersist " << handoutMasterPersist() << std::endl;
  out << "firstSlideNum " << firstSlideNum() << std::endl;
  out << "slideSizeType " << slideSizeType() << std::endl;
  out << "saveWithFonts " << saveWithFonts() << std::endl;
  out << "omitTitlePlace " << omitTitlePlace() << std::endl;
  out << "rightToLeft " << rightToLeft() << std::endl;
  out << "showComments " << showComments() << std::endl;  
}

// ========== EndDocumentAtom ==========

const unsigned int EndDocumentAtom::id = 1002;

EndDocumentAtom::EndDocumentAtom()
{
}

void EndDocumentAtom::dump( std::ostream& out ) const
{
  out << "EndDocumentAtom" << std::endl;
}


// ========== FontEntityAtom ==========

const unsigned int FontEntityAtom::id = 4023;

FontEntityAtom::FontEntityAtom()
{
}

FontEntityAtom::~FontEntityAtom()
{
}

void FontEntityAtom::dump( std::ostream& out ) const
{
  out << "FontEntityAtom - not yet implemented" << std::endl;
}

// ========== TextCharsAtom ==========

const unsigned int TextCharsAtom::id = 4000;

class TextCharsAtom::Private
{
public:
  UString ustring;
};

TextCharsAtom::TextCharsAtom()
{
  d = new Private;
}

TextCharsAtom::~TextCharsAtom()
{
  delete d;
}
  
UString TextCharsAtom::ustring() const
{
  return d->ustring;
}

void TextCharsAtom::setUString( const UString& ustr )
{
  d->ustring = ustr;
}

void TextCharsAtom::setData( unsigned size, const unsigned char* data )
{
  UString str;
  for( unsigned k=0; k<size; k++ )
  {
    unsigned uchar = readU16( data + k*2 );
    str.append( UString(uchar) );
  }
  setUString( str );
}
  
void TextCharsAtom::dump( std::ostream& out ) const
{
  out << "TextCharsAtom" << std::endl;
  out << "String : [" << ustring() << "]" << std::endl;
}

// ========== GuideAtom  ==========

const unsigned int GuideAtom::id = 1019;
  
class GuideAtom::Private
{
public:
  int type;
  int pos;  
};

GuideAtom::GuideAtom ()
{
  d = new Private;
  d->type = 0;
  d->pos = 0;
}

GuideAtom::~GuideAtom ()
{
  delete d;
}

int GuideAtom::type() const
{
  return d->type;
}

void GuideAtom::setType(int type)
{
  d->type= type;
}

int GuideAtom::pos() const
{
  return d->pos;
}

void GuideAtom::setPos(int pos)
{
  d->pos= pos;
}

void GuideAtom::setData( unsigned size, const unsigned char* data )
{
  setType( readS32( data + 0 ) );
  setPos( readS32( data + 4 ) );
}

void GuideAtom::dump( std::ostream& out ) const
{
  out << "GuideAtom" << std::endl;
  out << "type " << type() << std::endl;
  out << "pos " << pos() << std::endl;
}

// ========== NotesAtom  ==========

const unsigned int NotesAtom::id = 1009;
  
class NotesAtom ::Private
{
public:
  int slideId;
  int flags;  
};

NotesAtom::NotesAtom ()
{
  d = new Private;
  d->slideId = 0;
  d->flags = 0;
}

NotesAtom::~NotesAtom ()
{
  delete d;
}

int NotesAtom::slideId() const
{
  return d->slideId;
}

void NotesAtom::setSlideId(int slideId)
{
  d->slideId= slideId;
}

int NotesAtom::flags() const
{
  return d->flags;
}

void NotesAtom::setFlags(int flags)
{
  d->flags= flags;
}

void NotesAtom ::setData( unsigned size, const unsigned char* data )
{
  setSlideId( readS32( data + 0 ) );
  setFlags( readU16( data + 4 ) );
}

void NotesAtom ::dump( std::ostream& out ) const
{
  out << "NotesAtom" << std::endl;
  out << "slideId " << slideId() << std::endl;
  out << "flags " << flags() << std::endl;
}

// ========== ExObjListAtom  ==========

const unsigned int ExObjListAtom::id = 1034;
  
class ExObjListAtom ::Private
{
public:
  int objectIdSeed; 
};

ExObjListAtom::ExObjListAtom ()
{
  d = new Private;
  d->objectIdSeed = 0;
}

ExObjListAtom::~ExObjListAtom ()
{
  delete d;
}

int ExObjListAtom::objectIdSeed() const
{
  return d->objectIdSeed;
}

void ExObjListAtom::setObjectIdSeed(int objectIdSeed)
{
  d->objectIdSeed= objectIdSeed;
}

void ExObjListAtom ::setData( unsigned size, const unsigned char* data )
{ // check later for valid value
  setObjectIdSeed( readU32( data + 0 ) ); 
}

void ExObjListAtom ::dump( std::ostream& out ) const
{
  out << "ExObjListAtom" << std::endl;
  out << "objectIdSeed " << objectIdSeed() << std::endl;
}


// ========== ExEmbedAtom  ==========

const unsigned int ExEmbedAtom::id = 4045;
  
class ExEmbedAtom ::Private
{
public:
  int followColorScheme;
  int cantLockServerB;  
  int noSizeToServerB;
  int isTable;  
};

ExEmbedAtom::ExEmbedAtom ()
{
  d = new Private;
  d->followColorScheme = 0;
  d->cantLockServerB = 0;
  d->noSizeToServerB = 0;
  d->isTable = 0;
}

ExEmbedAtom::~ExEmbedAtom ()
{
  delete d;
}

int ExEmbedAtom::followColorScheme() const
{
  return d->followColorScheme;
}

void ExEmbedAtom::setFollowColorScheme(int followColorScheme)
{
  d->followColorScheme= followColorScheme;
}

int ExEmbedAtom::cantLockServerB() const
{
  return d->cantLockServerB;
}

void ExEmbedAtom::setCantLockServerB(int cantLockServerB)
{
  d->cantLockServerB= cantLockServerB;
}

int ExEmbedAtom::noSizeToServerB() const
{
  return d->noSizeToServerB;
}

void ExEmbedAtom::setNoSizeToServerB(int noSizeToServerB)
{
  d->noSizeToServerB= noSizeToServerB;
}

int ExEmbedAtom::isTable() const
{
  return d->isTable;
}

void ExEmbedAtom::setIsTable(int isTable)
{
  d->isTable= isTable;
}

void ExEmbedAtom ::setData( unsigned size, const unsigned char* data )
{
  setFollowColorScheme( readS32( data + 0 ) );
  setCantLockServerB( data [4] );
  setNoSizeToServerB(  data[5]  );
  setIsTable( data[6]  );
}

void ExEmbedAtom ::dump( std::ostream& out ) const
{
  out << "ExEmbedAtom" << std::endl;
  out << "followColorScheme " << followColorScheme() << std::endl;
  out << "cantLockServerB " << cantLockServerB() << std::endl;
  out << "noSizeToServerB " << noSizeToServerB() << std::endl;
  out << "isTable " << isTable() << std::endl;
}

// ========== ExOleObjAtom  ==========

const unsigned int ExOleObjAtom::id = 4035;
  
class ExOleObjAtom ::Private
{
public:
  int drawAspect;
  int type;  
  int objID;  
  int subType;  
  int objStgDataRef;  
  int isBlank;  
};

ExOleObjAtom::ExOleObjAtom ()
{
  d = new Private;
  d->drawAspect = 0;
  d->type = 0;
  d->objID = 0;
  d->subType = 0;
  d->objStgDataRef = 0;
  d->isBlank = 0;
}

ExOleObjAtom::~ExOleObjAtom ()
{
  delete d;
}

int ExOleObjAtom::drawAspect() const
{
  return d->drawAspect;
}

void ExOleObjAtom::setDrawAspect(int drawAspect)
{
  d->drawAspect= drawAspect;
}

int ExOleObjAtom::type() const
{
  return d->type;
}

void ExOleObjAtom::setType(int type)
{
  d->type= type;
}

int ExOleObjAtom::objID() const
{
  return d->objID;
}

void ExOleObjAtom::setObjID(int objID)
{
  d->objID= objID;
}

int ExOleObjAtom::subType() const
{
  return d->subType;
}

void ExOleObjAtom::setSubType(int subType)
{
  d->subType= subType;
}

int ExOleObjAtom::objStgDataRef() const
{
  return d->objStgDataRef;
}

void ExOleObjAtom::setObjStgDataRef(int objStgDataRef)
{
  d->objStgDataRef= objStgDataRef;
}

int ExOleObjAtom::isBlank() const
{
  return d->isBlank;
}

void ExOleObjAtom::setIsBlank(int isBlank)
{
  d->isBlank= isBlank;
}

void ExOleObjAtom ::setData( unsigned size, const unsigned char* data )
{
  setDrawAspect( readU32( data + 0 ) );
  setType( readS32( data + 4 ) );
  setObjID( readS32( data + 8 ) );
  setSubType( readS32( data + 12 ) );
  setObjStgDataRef( readS32( data + 16 ) );
  setIsBlank( data[20] );
}

void ExOleObjAtom ::dump( std::ostream& out ) const
{
  out << "ExOleObjAtom" << std::endl;
  out << "drawAspect " << drawAspect() << std::endl;
  out << "type " << type() << std::endl;
  out << "objID " << objID() << std::endl;
  out << "subType " << subType() << std::endl;
  out << "objID " << objID() << std::endl;
  out << "objStgDataRef " << objStgDataRef() << std::endl;
  out << "isBlank " << isBlank() << std::endl;
  
}


// ========== PersistPtrIncrementalBlockAtom ==========

const unsigned int PersistPtrIncrementalBlockAtom::id = 6002;

PersistPtrIncrementalBlockAtom::PersistPtrIncrementalBlockAtom()
{
}

PersistPtrIncrementalBlockAtom::~PersistPtrIncrementalBlockAtom()
{
}

void PersistPtrIncrementalBlockAtom ::dump( std::ostream& out ) const
{
  out << "PersistPtrIncrementalBlockAtom - not yet implemented" << std::endl;
} 


// ========== HeadersFootersAtom  ==========

const unsigned int HeadersFootersAtom::id = 4058;
  
class HeadersFootersAtom ::Private
{
public:
  int formatId;
  int flags;  
};

HeadersFootersAtom::HeadersFootersAtom ()
{
  d = new Private;
  d->formatId = 0;
  d->flags = 0;
}

HeadersFootersAtom::~HeadersFootersAtom ()
{
  delete d;
}

int HeadersFootersAtom::formatId() const
{
  return d->formatId;
}

void HeadersFootersAtom::setFormatId(int formatId)
{
  d->formatId= formatId;
}

int HeadersFootersAtom::flags() const
{
  return d->flags;
}

void HeadersFootersAtom::setFlags(int flags)
{
  d->flags= flags;
}

void HeadersFootersAtom::setData( unsigned size, const unsigned char* data )
{
  setFormatId( readS16( data + 0 ) );
  setFlags( readU16( data + 2 ) );
}

void HeadersFootersAtom ::dump( std::ostream& out ) const
{
  out << "NotesAtom" << std::endl;
  out << "formatId " << formatId() << std::endl;
  out << "flags " << flags() << std::endl;
}


// ========== ColorSchemeAtom  ==========

const unsigned int ColorSchemeAtom::id = 2032;
  
class ColorSchemeAtom ::Private
{
public:
  int Background;
  int TextAndLines; 
  int Shadows; 
  int TitleText; 
  int Fills; 
  int Accent; 
  int AccentAndHyperlink; 
  int AccentAndFollowedHyperlink; 
  
};

ColorSchemeAtom::ColorSchemeAtom ()
{
  d = new Private; 
  d->Background = 0 ;
  d->TextAndLines = 0; 
  d->Shadows = 0 ; 
  d->TitleText = 0 ; 
  d->Fills = 0; 
  d->Accent = 0; 
  d->AccentAndHyperlink = 0; 
  d->AccentAndFollowedHyperlink = 0; 
}


ColorSchemeAtom::~ColorSchemeAtom ()
{
  delete d;
}

int ColorSchemeAtom::Background() const
{
  return d->Background; 
}
 
void ColorSchemeAtom::setBackground( int Background )
{ 
  d->Background = Background; 
}  

int ColorSchemeAtom::TextAndLines() const
{
  return d->TextAndLines; 
}

void ColorSchemeAtom::setTextAndLines( int TextAndLines )
{
  d->TextAndLines = TextAndLines; 
}

int ColorSchemeAtom::Shadows() const
{
  return d->Shadows; 
}  

void ColorSchemeAtom::setShadows( int Shadows )
{
  d->Shadows = Shadows; 
}
  
int ColorSchemeAtom::TitleText() const
{
  return d->TitleText; 
} 

void ColorSchemeAtom::setTitleText( int TitleText )
{
  d->TitleText = TitleText; 
}
   
int ColorSchemeAtom::Fills() const
{
  return d->Fills; 
}  

void ColorSchemeAtom::setFills( int Fills )
{
  d->Fills = Fills; 
}  

int ColorSchemeAtom::Accent() const
{
  return d->Accent; 
}  

void ColorSchemeAtom::setAccent( int Accent )
{
  d->Accent = Accent; 
}  

int ColorSchemeAtom::AccentAndHyperlink() const
{
  return d->AccentAndHyperlink; 
}  

void ColorSchemeAtom::setAccentAndHyperlink ( int AccentAndHyperlink )
{
  d->AccentAndHyperlink  = AccentAndHyperlink; 
}  

int ColorSchemeAtom::AccentAndFollowedHyperlink() const
{
  return d->AccentAndFollowedHyperlink; 
}  

void ColorSchemeAtom::setAccentAndFollowedHyperlink( int AccentAndFollowedHyperlink )
{ 
  d->AccentAndFollowedHyperlink = AccentAndFollowedHyperlink; 
}

void ColorSchemeAtom ::setData( unsigned size, const unsigned char* data )
{
  setBackground( readS32( data + 0 ) );
  setTextAndLines( readU32( data + 4 ) );
  setShadows( readU32( data + 8 ) );
  setTitleText( readU32( data + 12 ) );
  setFills( readU32( data + 16 ) );
  setAccent( readU32( data + 20 ) );
  setAccentAndHyperlink( readU32( data + 24 ) );
  setAccentAndFollowedHyperlink( readU32( data + 28 ) );
       
}

void ColorSchemeAtom ::dump( std::ostream& out ) const
{
  out << "ColorSchemeAtom" << std::endl;
  out << "Background " << Background() << std::endl;
  out << "Shadows " << Shadows() << std::endl;
  out << "TitleText " << TitleText() << std::endl;
  out << "Fills " << Fills() << std::endl;
  out << "Accent " << Accent() << std::endl;
  out << "AccentAndHyperlink " << AccentAndHyperlink() << std::endl;
  out << "AccentAndFollowedHyperlink " << AccentAndFollowedHyperlink() << std::endl;
}

// ========== CurrentUserAtom  ==========

const unsigned int CurrentUserAtom::id = 4086;
  
class CurrentUserAtom ::Private
{
public:
  int Size;
  int Magic; 
  int OffsettoCurrentEdit; 
  int LenUserName; 
  int DocFileVersion; 
  int majorVersion; 
  int minorVersion; 
};

CurrentUserAtom::CurrentUserAtom ()
{
  d = new Private; 
  d->Size = 0 ;
  d->Magic = 0 ;
  d->OffsettoCurrentEdit = 0; 
  d->LenUserName = 0 ; 
  d->DocFileVersion = 0 ; 
  d->majorVersion = 0; 
  d->minorVersion = 0; 
}

CurrentUserAtom::~CurrentUserAtom ()
{
  delete d;
}

int CurrentUserAtom::Size() const
{
  return d->Size; 
}
 
void CurrentUserAtom::setSize( int Size )
{ 
  d->Size = Size; 
}  

int CurrentUserAtom::Magic() const
{
  return d->Magic; 
}
 
void CurrentUserAtom::setMagic( int Magic )
{ 
  d->Magic = Magic; 
}  

int CurrentUserAtom::OffsettoCurrentEdit() const
{
  return d->OffsettoCurrentEdit; 
}
 
void CurrentUserAtom::setOffsettoCurrentEdit( int OffsettoCurrentEdit )
{ 
  d->OffsettoCurrentEdit = OffsettoCurrentEdit; 
}  

int CurrentUserAtom::LenUserName() const
{
  return d->LenUserName; 
}
 
void CurrentUserAtom::setLenUserName( int LenUserName )
{ 
  d->LenUserName = LenUserName; 
}  

int CurrentUserAtom::DocFileVersion() const
{
  return d->DocFileVersion; 
}
 
void CurrentUserAtom::setDocFileVersion( int DocFileVersion )
{ 
  d->DocFileVersion = DocFileVersion; 
}  

int CurrentUserAtom::majorVersion() const
{
  return d->majorVersion; 
}
 
void CurrentUserAtom::setMajorVersion( int majorVersion )
{ 
  d->majorVersion = majorVersion; 
}  

int CurrentUserAtom::minorVersion() const
{
  return d->minorVersion; 
}
 
void CurrentUserAtom::setMinorVersion( int minorVersion )
{ 
  d->minorVersion = minorVersion; 
}  

void CurrentUserAtom ::setData( unsigned size, const unsigned char* data )
{
  setSize( readU32( data + 0 ) );
  setMagic( readU32( data + 4 ) );
  setOffsettoCurrentEdit( readU32( data + 8 ) );
  setLenUserName( readU16( data + 12 ) );
  setDocFileVersion( readU32( data + 14 ) );
  setMajorVersion(  data[18] );
  setMinorVersion(  data[19] );       
}

void CurrentUserAtom ::dump( std::ostream& out ) const
{
  out << "Size" << Size() << std::endl;
  out << "Magic " << Magic() << std::endl;
  out << "OffsettoCurrentEdit " << OffsettoCurrentEdit() << std::endl;
  out << "LenUserName " << LenUserName() << std::endl;
  out << "DocFileVersion " << DocFileVersion() << std::endl;
  out << "majorVersion " << majorVersion() << std::endl;
  out << "minorVersion " << minorVersion() << std::endl;
}


// ========== UserEditAtom ==========

const unsigned int UserEditAtom::id = 4085;

class UserEditAtom::Private
{
public:
  int lastSlideId;
  int majorVersion; 
  int minorVersion;
};

UserEditAtom::UserEditAtom()
{
  d = new Private;
  d->lastSlideId = 0;
  d->majorVersion = 0;
  d->minorVersion = 0;
  
}

UserEditAtom::~UserEditAtom()
{
  delete d;
}

int UserEditAtom::lastSlideId() const
{
  return d->lastSlideId;
}

void UserEditAtom::setLastSlideId( int id )
{
  d->lastSlideId = id;
}

int UserEditAtom::majorVersion() const
{
  return d->majorVersion;
}

void UserEditAtom::setMajorVersion( int majorVersion )
{
  d->majorVersion = majorVersion;
}

int UserEditAtom::minorVersion() const
{
  return d->minorVersion;
}

void UserEditAtom::setMinorVersion( int minorVersion )
{
  d->minorVersion = minorVersion;
}

void UserEditAtom::setData( unsigned size, const unsigned char* data )
{
  setLastSlideId( readU32( data + 0 ) );
  setMinorVersion( readU16( data + 4 ) );
  setMajorVersion( readU16( data + 6 ) );
  
}

void UserEditAtom::dump( std::ostream& out ) const
{
  out << "UserEditAtom" << std::endl;
  out << "LastSlideID " << lastSlideId() << std::endl;
  out << "MajorVersion " << majorVersion() << std::endl;
  out << "MinorVersion " << minorVersion() << std::endl;
}

// ========== SSDocInfoAtom ==========

const unsigned int SSDocInfoAtom::id = 1025;

class SSDocInfoAtom::Private
{
public:
  int PenColorRed;
  int PenColorGreen;
  int PenColorBlue;
  int PenColorIndex;
  int RestartTime;
  int StartSlide; 
  int EndSlide;
  int NamedShow;
  int Flags;
};

SSDocInfoAtom::SSDocInfoAtom()
{
  d = new Private;
  d->PenColorRed = 0;
  d->PenColorGreen = 0;
  d->PenColorBlue = 0;
  d->PenColorIndex = 0;
  d->RestartTime = 0;
  d->StartSlide = 0;
  d->EndSlide = 0;
  d->NamedShow = 0;
  d->Flags = 0;  
}

SSDocInfoAtom::~SSDocInfoAtom()
{
  delete d;
}

int SSDocInfoAtom::PenColorRed() const
{
  return d->PenColorRed;
}
void SSDocInfoAtom::setPenColorRed( int PenColorRed )
{
  d->PenColorRed = PenColorRed;
}

int SSDocInfoAtom::PenColorGreen() const
{
  return d->PenColorGreen;
}
void SSDocInfoAtom::setPenColorGreen( int PenColorGreen )
{
  d->PenColorGreen = PenColorGreen;
}

int SSDocInfoAtom::PenColorBlue() const
{
  return d->PenColorBlue;
}
void SSDocInfoAtom::setPenColorBlue( int PenColorBlue )
{
  d->PenColorBlue = PenColorBlue;
}

int SSDocInfoAtom::PenColorIndex() const
{
  return d->PenColorIndex;
}
void SSDocInfoAtom::setPenColorIndex( int PenColorIndex )
{
  d->PenColorIndex = PenColorIndex;
}

int SSDocInfoAtom::RestartTime() const
{
  return d->RestartTime;
}

void SSDocInfoAtom::setRestartTime( int RestartTime )
{
  d->RestartTime = RestartTime;
}

int SSDocInfoAtom::StartSlide() const
{
  return d->StartSlide;
}

void SSDocInfoAtom::setStartSlide( int StartSlide )
{
  d->StartSlide = StartSlide;
}

int SSDocInfoAtom::EndSlide() const
{
  return d->EndSlide;
}

void SSDocInfoAtom::setEndSlide( int EndSlide )
{
  d->EndSlide = EndSlide;
}

int SSDocInfoAtom::NamedShow() const
{
  return d->NamedShow;
}

void SSDocInfoAtom::setNamedShow( int NamedShow )
{
  d->NamedShow = NamedShow;
}

int SSDocInfoAtom::Flags() const
{
  return d->Flags;
}

void SSDocInfoAtom::setFlags( int Flags )
{
  d->Flags = Flags;
}

void SSDocInfoAtom::setData( unsigned size, const unsigned char* data )
{
  setPenColorRed( data[0]  );
  setPenColorGreen( data[1] );
  setPenColorBlue(  data[2]);
  setPenColorIndex( data[3] );
  setRestartTime( readS32( data + 4 ) );
  setStartSlide( readS16( data + 8 ) );
  setEndSlide( readS16( data + 10 ) );
  setNamedShow( readU16( data + 12 ) ); // 2 bytes repeat 32x
  setFlags( readU16( data + 76 ) ); // offset correct ?
  
}

void SSDocInfoAtom::dump( std::ostream& out ) const
{
  out << "UserEditAtom" << std::endl;
  out << "PenColorRed " << PenColorRed() << std::endl;
  out << "PenColorGreen " << PenColorGreen() << std::endl;
  out << "PenColorBlue " << PenColorBlue() << std::endl;
  out << "PenColorIndex " << PenColorIndex() << std::endl;
  out << "RestartTime " << RestartTime() << std::endl;
  out << "StartSlide " << StartSlide() << std::endl;
  out << "EndSlide " << EndSlide() << std::endl;
  out << "NamedShow " << NamedShow() << std::endl;
  out << "Flags " << Flags() << std::endl;
}

// ========== SrKinsokuAtom ==========

const unsigned int SrKinsokuAtom::id = 4050;

SrKinsokuAtom::SrKinsokuAtom()
{
}

void SrKinsokuAtom::dump( std::ostream& out ) const
{
  out << "SrKinsokuAtom - not yet implemented" << std::endl;
}

// ========== TxMasterStyleAtom ==========

const unsigned int TxMasterStyleAtom::id = 4003;

TxMasterStyleAtom::TxMasterStyleAtom()
{
}

TxMasterStyleAtom::~TxMasterStyleAtom()
{
}

void TxMasterStyleAtom::dump( std::ostream& out ) const
{
  out << "TxMasterStyleAtom - not yet implemented" << std::endl;
}

// ========== SlideViewInfoAtom  ==========

const unsigned int SlideViewInfoAtom::id = 1022;
  
class SlideViewInfoAtom ::Private
{
public:
  int showGuides;
  int snapToGrid;  
  int snapToShape; 
};

SlideViewInfoAtom::SlideViewInfoAtom ()
{
  d = new Private;
  d->showGuides = 0;
  d->snapToGrid = 0;
  d->snapToShape = 0;
}

SlideViewInfoAtom::~SlideViewInfoAtom ()
{
  delete d;
}

int SlideViewInfoAtom::showGuides() const
{
  return d->showGuides;
}

void SlideViewInfoAtom::setShowGuides(int showGuides)
{
  d->showGuides= showGuides;
}

int SlideViewInfoAtom::snapToGrid() const
{
  return d->snapToGrid;
}

void SlideViewInfoAtom::setSnapToGrid(int snapToGrid)
{
  d->snapToGrid= snapToGrid;
}

int SlideViewInfoAtom::snapToShape() const
{
  return d->snapToShape;
}

void SlideViewInfoAtom::setSnapToShape(int snapToShape)
{
  d->snapToGrid= snapToShape;
}

void SlideViewInfoAtom ::setData( unsigned size, const unsigned char* data )
{
  setShowGuides(data[0]);
  setSnapToGrid(data[1]);
  setSnapToShape(data[2]); 
}

void SlideViewInfoAtom ::dump( std::ostream& out ) const
{
  out << "SlideViewInfoAtom" << std::endl;
  out << "showGuides " << showGuides() << std::endl;
  out << "snapToGrid " << snapToGrid() << std::endl;
  out << "snapToShape " << snapToShape() << std::endl;
}

// ========== ViewInfoAtom  ==========

const unsigned int ViewInfoAtom::id = 1021;
 
class ViewInfoAtom ::Private
{
public:
  int CurScaleXNum;
  int CurScaleXDen;  
  int CurScaleYNum; 
  int CurScaleYDen; 
  int PrevScaleXNum;
  int PrevScaleXDen;  
  int PrevScaleYNum; 
  int PrevScaleYDen; 
  int ViewSizeX;
  int ViewSizeY;  
  int OriginX; 
  int OriginY; 
  int varScale; 
  int draftMode; 
  int padding; 
};

ViewInfoAtom::ViewInfoAtom ()
{
  d = new Private;
  d->CurScaleXNum = 0;
  d->CurScaleXDen = 0;
  d->CurScaleYNum = 0;
  d->CurScaleYDen = 0;
  d->PrevScaleXNum = 0;
  d->PrevScaleXDen = 0;
  d->PrevScaleYNum = 0;
  d->PrevScaleYDen = 0;
  d->ViewSizeX = 0;
  d->ViewSizeY = 0;
  d->OriginX = 0;
  d->OriginY = 0;
  d->varScale = 0; 
  d->draftMode = 0; 
  d->padding = 0;
}

ViewInfoAtom::~ViewInfoAtom ()
{
  delete d;
}

int ViewInfoAtom::varScale() const
{
  return d->varScale;
}

void ViewInfoAtom::setVarScale(int varScale)
{
  d->varScale= varScale;
}

int ViewInfoAtom::draftMode() const
{
  return d->draftMode;
}

void ViewInfoAtom::setDraftMode(int draftMode)
{
  d->draftMode= draftMode;
}

int ViewInfoAtom::padding() const
{
  return d->padding;
}

void ViewInfoAtom::setPadding(int padding)
{
  d->padding= padding;
}

int ViewInfoAtom::ViewSizeX() const
{
  return d->ViewSizeX;
}

void ViewInfoAtom::setViewSizeX(int ViewSizeX)
{
  d->ViewSizeX= ViewSizeX;
}

int ViewInfoAtom::ViewSizeY() const
{
  return d->ViewSizeY;
}

void ViewInfoAtom::setViewSizeY(int ViewSizeY)
{
  d->ViewSizeY= ViewSizeY;
}

int ViewInfoAtom::OriginX() const
{
  return d->OriginX;
}

void ViewInfoAtom::setOriginX(int OriginX)
{
  d->OriginX= OriginX;
}

int ViewInfoAtom::OriginY() const
{
  return d->OriginY;
}

void ViewInfoAtom::setOriginY (int OriginY)
{
  d->OriginY= OriginY;
}


int ViewInfoAtom::PrevScaleXNum() const
{
  return d->PrevScaleXNum;
}

void ViewInfoAtom::setPrevScaleXNum(int PrevScaleXNum)
{
  d->PrevScaleXNum= PrevScaleXNum;
}

int ViewInfoAtom::PrevScaleXDen() const
{
  return d->PrevScaleXDen;
}

void ViewInfoAtom::setPrevScaleXDen(int PrevScaleXDen)
{
  d->PrevScaleXDen= PrevScaleXDen;
}

int ViewInfoAtom::PrevScaleYNum() const
{
  return d->PrevScaleYNum;
}

void ViewInfoAtom::setPrevScaleYNum(int PrevScaleYNum)
{
  d->PrevScaleYNum= PrevScaleYNum;
}

int ViewInfoAtom::PrevScaleYDen() const
{
  return d->PrevScaleYDen;
}

void ViewInfoAtom::setPrevScaleYDen(int PrevScaleYDen)
{
  d->PrevScaleYDen= PrevScaleYDen;
}


int ViewInfoAtom::CurScaleXNum() const
{
  return d->CurScaleXNum;
}

void ViewInfoAtom::setCurScaleXNum(int CurScaleXNum)
{
  d->CurScaleXNum= CurScaleXNum;
}

int ViewInfoAtom::CurScaleXDen() const
{
  return d->CurScaleXDen;
}

void ViewInfoAtom::setCurScaleXDen(int CurScaleXDen)
{
  d->CurScaleXDen= CurScaleXDen;
}

int ViewInfoAtom::CurScaleYNum() const
{
  return d->CurScaleYNum;
}

void ViewInfoAtom::setCurScaleYNum(int CurScaleYNum)
{
  d->CurScaleYNum= CurScaleYNum;
}

int ViewInfoAtom::CurScaleYDen() const
{
  return d->CurScaleYDen;
}

void ViewInfoAtom::setCurScaleYDen(int CurScaleYDen)
{
  d->CurScaleYDen= CurScaleYDen;
}

void ViewInfoAtom ::setData( unsigned size, const unsigned char* data )
{
  setCurScaleXNum(readS32( data + 0));
  setCurScaleXDen(readS32( data + 4 ));
  setCurScaleYNum(readS32( data + 8)); 
  setCurScaleYDen(readS32( data + 12 )); 
  setPrevScaleXNum(readS32( data + 16 )); 
  setPrevScaleXDen(readS32( data + 20 )); 
  setPrevScaleYNum(readS32( data + 24 )); 
  setPrevScaleYDen(readS32( data + 28 )); 
  setViewSizeX(readS32( data + 32 ));
  setViewSizeY(readS32( data + 36 ));
  setOriginX(readS32( data + 40 )); 
  setOriginY(readS32( data + 44)); 
  setVarScale(data[48]); 
  setDraftMode(data[49]); 
  setPadding(readU16( data + 50 )); 
}

void ViewInfoAtom ::dump( std::ostream& out ) const
{
  out << "ViewInfoAtom" << std::endl;
  out << "CurScaleXNum " << CurScaleXNum() << std::endl;
  out << "CurScaleXDen " << CurScaleXDen() << std::endl;
  out << "CurScaleYNum " << CurScaleYNum() << std::endl;
  out << "CurScaleYDen " << CurScaleYDen() << std::endl;
  out << "PrevScaleXNum " << PrevScaleXNum() << std::endl;
  out << "PrevScaleXDen " << PrevScaleXNum() << std::endl;
  out << "PrevScaleYNum " << PrevScaleYNum() << std::endl;
  out << "PrevScaleYDen " << PrevScaleYNum() << std::endl;  
  out << "ViewSizeX " << ViewSizeX() << std::endl;
  out << "ViewSizeY " << ViewSizeY() << std::endl;   
  out << "OriginX " << OriginX() << std::endl;
  out << "OriginY " << OriginY() << std::endl;   
  out << "varScale " << varScale() << std::endl;  
  out << "draftMode " << draftMode() << std::endl;  
  out << "padding " << padding() << std::endl;  
}

// ========== StyleTextPropAtom   ==========

const unsigned int StyleTextPropAtom ::id = 4001;

StyleTextPropAtom::StyleTextPropAtom  ()
{
}

StyleTextPropAtom::~StyleTextPropAtom  ()
{
}

void StyleTextPropAtom  ::dump( std::ostream& out ) const
{
  out << "StyleTextPropAtom - need special parse code " << std::endl;
}

// ========== TxCFStyleAtom  ==========

const unsigned int TxCFStyleAtom ::id = 4004;

TxCFStyleAtom ::TxCFStyleAtom ()
{
}

TxCFStyleAtom ::~TxCFStyleAtom ()
{
}

void TxCFStyleAtom ::dump( std::ostream& out ) const
{
  out << "TxCFStyleAtom  - need special parse code" << std::endl;
}

// ========== TxPFStyleAtom ==========

const unsigned int TxPFStyleAtom::id = 4005;

TxPFStyleAtom::TxPFStyleAtom()
{
}

TxPFStyleAtom::~TxPFStyleAtom()
{
}

void TxPFStyleAtom::dump( std::ostream& out ) const
{
  out << "TxPFStyleAtom - need special parse code" << std::endl;
}

// ========== TxSIStyleAtom  ==========

const unsigned int TxSIStyleAtom ::id = 4009;

TxSIStyleAtom ::TxSIStyleAtom ()
{
}

TxSIStyleAtom ::~TxSIStyleAtom ()
{
}

void TxSIStyleAtom ::dump( std::ostream& out ) const
{
  out << "TxSIStyleAtom - need special parse code" << std::endl;
}

// ========== Record1043 ==========

const unsigned int Record1043::id = 1043;

Record1043::Record1043()
{
}

void Record1043::dump( std::ostream& out ) const
{
  out << "Record1043 - not known" << std::endl;
}

// ========== Record1044 ==========

const unsigned int Record1044::id = 1044;

Record1044::Record1044()
{
}

void Record1044::dump( std::ostream& out ) const
{
  out << "Record1044 - not known" << std::endl;
}

// ========== SlideAtom ==========

const unsigned int SlideAtom::id = 1007;
  
class SlideAtom::Private
{
public:
  int layoutGeom;
  int layoutPlaceholderId1; 
  int layoutPlaceholderId2; 
  int layoutPlaceholderId3; 
  int layoutPlaceholderId4; 
  int layoutPlaceholderId5; 
  int layoutPlaceholderId6; 
  int layoutPlaceholderId7; 
  int layoutPlaceholderId8; 
  int masterId; 
  int notesId;
  int flags; 
};

SlideAtom::SlideAtom()
{
  d = new Private;
  d->layoutGeom = 0;
  d->layoutPlaceholderId1 = 0; 
  d->layoutPlaceholderId2 = 0; 
  d->layoutPlaceholderId3 = 0; 
  d->layoutPlaceholderId4 = 0; 
  d->layoutPlaceholderId5 = 0; 
  d->layoutPlaceholderId6 = 0; 
  d->layoutPlaceholderId7 = 0; 
  d->layoutPlaceholderId8 = 0;     
  d->masterId = 0;
  d->notesId = 0;
  d->flags = 0;
}

SlideAtom::~SlideAtom()
{
  delete d;
}

int SlideAtom::layoutGeom() const
{
  return d->layoutGeom;
}

void SlideAtom::setLayoutGeom(int layoutGeom )
{
  d->layoutGeom = layoutGeom;
}


int SlideAtom::layoutPlaceholderId() const
{
   return d->layoutPlaceholderId1;
   return d->layoutPlaceholderId2;
   return d->layoutPlaceholderId3;
   return d->layoutPlaceholderId4;
   return d->layoutPlaceholderId5;
   return d->layoutPlaceholderId6;
   return d->layoutPlaceholderId7;
   return d->layoutPlaceholderId8;
}

void SlideAtom::setLayoutPlaceholderId(int layoutPlaceholderId1, int layoutPlaceholderId2,int layoutPlaceholderId3,int layoutPlaceholderId4,int layoutPlaceholderId5,int layoutPlaceholderId6,int layoutPlaceholderId7,int layoutPlaceholderId8)
{
     d->layoutPlaceholderId1 = layoutPlaceholderId1;
     d->layoutPlaceholderId2 = layoutPlaceholderId2;
     d->layoutPlaceholderId3 = layoutPlaceholderId3;
     d->layoutPlaceholderId4 = layoutPlaceholderId4;
     d->layoutPlaceholderId5 = layoutPlaceholderId5;
     d->layoutPlaceholderId6 = layoutPlaceholderId6;
     d->layoutPlaceholderId7 = layoutPlaceholderId7;
     d->layoutPlaceholderId8 = layoutPlaceholderId8;
}

int SlideAtom::masterId() const
{
  return d->masterId;
}

void SlideAtom::setMasterId(int masterId )
{
  d->masterId = masterId;
}

int SlideAtom::notesId() const
{
  return d->notesId;
}

void SlideAtom::setNotesId(int notesId)
{
  d->notesId = notesId;
}


int SlideAtom::flags() const
{
  return d->flags;
}

void SlideAtom::setFlags( int flags)
{
  d->flags = flags;
}

void SlideAtom::setData( unsigned size, const unsigned char* data )
{
  setLayoutGeom(readS32( data + 0 ) ); 
  setLayoutPlaceholderId (data[4], data[5], data[6], data[7], data[8], data[9], data[10],data[11]);
  setMasterId( readS32( data + 12 ) );
  setNotesId( readS32( data + 16 ) );
  setFlags( readU16( data + 20 ) );
  
}

void SlideAtom::dump( std::ostream& out ) const
{
  out << "SlideAtom" << std::endl;
  out << "layoutGeom " << layoutGeom() << std::endl;
  // out << "layoutPlaceholderId " << layoutPlaceholderId() <<std::endl;
  out << "layoutPlaceholderId1 " << d->layoutPlaceholderId1 <<std::endl;
  out << "layoutPlaceholderId2 " << d->layoutPlaceholderId2 <<std::endl;
  out << "layoutPlaceholderId3 " << d->layoutPlaceholderId3 <<std::endl;
  out << "layoutPlaceholderId4 " << d->layoutPlaceholderId4 <<std::endl;
  out << "layoutPlaceholderId5 " << d->layoutPlaceholderId5 <<std::endl;
  out << "layoutPlaceholderId6 " << d->layoutPlaceholderId6 <<std::endl;
  out << "layoutPlaceholderId7 " << d->layoutPlaceholderId7 <<std::endl;
  out << "layoutPlaceholderId8 " << d->layoutPlaceholderId8 <<std::endl;
  out << "masterId " << masterId() << std::endl;
  out << "notesId " << notesId() << std::endl;
  out << "flags " << flags() << std::endl;
}

// ========== SSSlideInfoAtom  ==========

const unsigned int SSSlideInfoAtom::id = 1017;
  
class SSSlideInfoAtom ::Private
{
public:
  int transType; 
  int speed; 
  int direction; 
  int slideTime; 
  int buildFlags; 
  int soundRef; 
};

SSSlideInfoAtom ::SSSlideInfoAtom ()
{
  d = new Private;
  d->transType = 0;
  d->speed = 0;
  d->direction = 0;
  d->slideTime = 0;
  d->buildFlags = 0;
  d->soundRef = 0;
}

SSSlideInfoAtom ::~SSSlideInfoAtom ()
{
  delete d;
}

int SSSlideInfoAtom ::transType() const
{
  return d->transType;
}

void SSSlideInfoAtom ::settransType(int transType)
{
  d->transType = transType;
}

int SSSlideInfoAtom ::speed() const
{
  return d->transType;
}

void SSSlideInfoAtom ::setspeed(int speed)
{
  d->speed = speed;
}

int SSSlideInfoAtom ::direction() const
{
  return d->transType;
}

void SSSlideInfoAtom ::setdirection(int direction)
{
  d->direction = direction;
}

int SSSlideInfoAtom ::slideTime() const
{
  return d->slideTime;
}

void SSSlideInfoAtom ::setslideTime(int slideTime)
{
  d->slideTime = slideTime;
}

int SSSlideInfoAtom ::buildFlags() const
{
  return d->buildFlags;
}

void SSSlideInfoAtom ::setbuildFlags(int buildFlags)
{
  d->buildFlags = buildFlags;
}

int SSSlideInfoAtom ::soundRef() const
{
  return d->soundRef;
}

void SSSlideInfoAtom ::setsoundRef(int soundRef)
{
  d->soundRef = soundRef;
}

void SSSlideInfoAtom ::setData( unsigned size, const unsigned char* data )
{
  settransType( readU32( data + 0 ) );
  setspeed(readS32( data + 4 ));
  setdirection(readS32( data + 8 ));
  setslideTime(readS32( data + 12 ));
  setbuildFlags(readS32( data + 16 ));
  setsoundRef(readS32( data + 20 ));
}

void SSSlideInfoAtom ::dump( std::ostream& out ) const
{  
  out << "SSSlideInfoAtom" << std::endl;
  out << "speed " << speed() << std::endl;
  out << "direction " << direction() << std::endl;
  out << "slideTime " << slideTime() << std::endl;
  out << "buildFlags " << buildFlags() << std::endl;
  out << "soundRef " << soundRef() << std::endl;
}

// ========== TextHeaderAtom  ==========

const unsigned int TextHeaderAtom::id = 3999;
  
class TextHeaderAtom ::Private
{
public:
  int txType; 
};

TextHeaderAtom ::TextHeaderAtom ()
{
  d = new Private;
  d->txType = 0;
}

TextHeaderAtom ::~TextHeaderAtom ()
{
  delete d;
}

int TextHeaderAtom ::txType() const
{
  return d->txType;
}

void TextHeaderAtom ::setTxType(int txType)
{
  d->txType = txType;
}

void TextHeaderAtom ::setData( unsigned size, const unsigned char* data )
{
  setTxType( readU32( data + 0 ) );
  
}

void TextHeaderAtom ::dump( std::ostream& out ) const
{  
  out << "TextHeaderAtom" << std::endl;
  out << "txType " << txType() << std::endl;
}

// ========== TextBytesAtom   ==========

const unsigned int TextBytesAtom ::id = 4008;
  
class TextBytesAtom::Private
{
public:   
 UString ustring; 
};

UString TextBytesAtom::ustring() const
{
  return d->ustring;
}

void TextBytesAtom::setUString( const UString& ustr )
{
  d->ustring = ustr;
}

TextBytesAtom::TextBytesAtom ()
{
  d = new Private;
  d->ustring = 0;
}

TextBytesAtom ::~TextBytesAtom ()
{
  delete d;
}

void TextBytesAtom::setData( unsigned size, const unsigned char* data )
{
  UString str;
  for( unsigned k=0; k<size; k++ )
  {
    unsigned uchar = ( readU32( data + k ) );
    str.append( UString(uchar) );
  }
  setUString( str );
}

void TextBytesAtom ::dump( std::ostream& out ) const
{
  
  out << "TextBytesAtom" << std::endl;
  out << "String : [" << ustring() << "]" << std::endl;
}

// ========== ExHyperlinkAtom  ==========

const unsigned int ExHyperlinkAtom::id = 4051;
  
class ExHyperlinkAtom ::Private
{
public:
  int objID; 
};

ExHyperlinkAtom ::ExHyperlinkAtom ()
{
  d = new Private;
  d->objID = 0;
}

ExHyperlinkAtom ::~ExHyperlinkAtom ()
{
  delete d;
}

int ExHyperlinkAtom ::objID() const
{
  return d->objID;
}

void ExHyperlinkAtom ::setObjID(int objID)
{
  d->objID = objID;
}

void ExHyperlinkAtom::setData( unsigned size, const unsigned char* data )
{
  setObjID( readU32( data + 0 ) );
}

void ExHyperlinkAtom ::dump( std::ostream& out ) const
{
  out << "ExHyperlinkAtom" << std::endl;
  out << "objID " << objID() << std::endl;   
}


// ========== TextSpecInfoAtom  ==========

const unsigned int TextSpecInfoAtom::id = 4010;
  
class TextSpecInfoAtom ::Private
{
public:
  int txSpecInfo; 
};

TextSpecInfoAtom ::TextSpecInfoAtom ()
{
  d = new Private;
  d->txSpecInfo = 0;
}

TextSpecInfoAtom ::~TextSpecInfoAtom ()
{
  delete d;
}

int TextSpecInfoAtom ::txSpecInfo() const
{
  return d->txSpecInfo;
}

void TextSpecInfoAtom ::setTxSpecInfo(int txSpecInfo)
{
  d->txSpecInfo = txSpecInfo;
}

void TextSpecInfoAtom::setData( unsigned size, const unsigned char* data )
{
  setTxSpecInfo( readU32( data + 0 ) );
}

void TextSpecInfoAtom ::dump( std::ostream& out ) const
{
  out << "TextSpecInfoAtom" << std::endl;
  out << "txSpecInfo " << txSpecInfo() << std::endl;   
}

// ========== SlidePersistAtom   ==========

const unsigned int SlidePersistAtom ::id = 1011;
  
class SlidePersistAtom::Private
{
public:
  int psrReference; 
  int flags; 
  int numberTexts; 
  int slideId; 
  int reserved; 
};

SlidePersistAtom::SlidePersistAtom  ()
{
  d = new Private;
  d->psrReference = 0;
  d->flags = 0; 
  d->numberTexts=0; 
  d->slideId=0; 
  d->reserved=0; 
 }

SlidePersistAtom  ::~SlidePersistAtom  ()
{
  delete d;
}

int SlidePersistAtom::psrReference() const
{  
  return d->psrReference;
}

void SlidePersistAtom::setPsrReference( int psrReference )
{
  d->psrReference = psrReference;
}  

int SlidePersistAtom::flags() const
{
  return d->flags; 
}

void SlidePersistAtom::setFlags( int flags )
{
  d->flags = flags;
}  

int SlidePersistAtom::numberTexts() const
{
  return d->numberTexts; 
}

void SlidePersistAtom::setNumberTexts( int numberTexts )
{
  d->numberTexts = numberTexts;
}  

int SlidePersistAtom::slideId() const
{
  return d->slideId; 
}

void SlidePersistAtom::setSlideId( int slideId )
{
   d->slideId = slideId;
}     

int SlidePersistAtom::reserved() const
{
  return d->reserved; 
}

void SlidePersistAtom::setReserved( int reserved )
{
   d->reserved = reserved;
}     

void SlidePersistAtom::setData( unsigned size, const unsigned char* data )
{
  setPsrReference( readU32( data + 0 ) );
  setFlags( readU32( data + 4 ) );
  setNumberTexts( readS32( data + 8 ) );
  setSlideId( readS32( data + 12 ) );
  setReserved(readU32( data + 16) ); 
}

void SlidePersistAtom  ::dump( std::ostream& out ) const
{
  out << "SlidePersistAtom" << std::endl;
  out << "psrReference " << psrReference() << std::endl;
  out << "flags " << flags() << std::endl;
  out << "numberTexts " << numberTexts() << std::endl;
  out << "slideId " << slideId() << std::endl;
  out << "reserved " << reserved() << " always 0."<<std::endl;
}

// ========== PPTReader ==========

class PPTReader::Private
{
public:
  int test;
  POLE::Stream* stream;
};


PPTReader::PPTReader()
{
  d = new Private;
  d->test = 0;
}

PPTReader::~PPTReader()
{
  delete d;
}

void PPTReader::handleRecord( Record* record )
{
  if( !record ) return;

  // take care of indentation
  int indent = 1;
  for( const Record* r = record->parent(); r; indent += 4 )
    r = r->parent();  
  char* spaces = new char[indent+1];
  for( int c = 0; c < indent; c++ )
    spaces[c] = ' ';
  spaces[indent] = 0;  
  
  std::ostringstream t;
  record->dump( t );
  std::string d = t.str();
  std::cout << spaces;
  for( unsigned i = 0; i < d.size(); i++ )
    if( d[i] != '\n' )
      std::cout << d[i];    
    else
      std::cout << std::endl << spaces;
  
  delete [] spaces;
}

bool PPTReader::load( Presentation* pr, const char* filename )
{
  POLE::Storage storage( filename );
  if( !storage.open() )
  {
    std::cerr << "Cannot open " << filename << std::endl;
    return false;
  }
  
  std::cout << "Loading file " << filename << std::endl;

  d->stream = new POLE::Stream( &storage, "/PowerPoint Document" );
  if( d->stream->fail() )
  {
    std::cerr << filename << " is not PowerPoint presentation" << std::endl;
    delete d->stream;
    d->stream = 0;
    return false;
  }

  unsigned long stream_size = d->stream->size();
  std::cout << "stream size " << stream_size << std::endl;
   
  while( d->stream->tell() < stream_size )
    loadRecord( 0 );

  delete d->stream;
  d->stream = 0;
  
  storage.close();

  pr->clear();
  
  std::cout << std::endl <<  filename << " loaded. Done." << std::endl;

  return false;
}



void PPTReader::loadRecord( Record* parent )
{
  // FIXME
  unsigned char buffer[65536];

  // get record type and data size
  unsigned long pos = d->stream->tell();
  unsigned bytes_read = d->stream->read( buffer, 8 );
  if( bytes_read != 8 ) return;
    
  unsigned long type = readU16( buffer + 2 );
  unsigned long size = readU32( buffer + 4 );
  unsigned long nextpos = d->stream->tell() + size;

  // take care of indentation
  int indent = 1;
  for( const Record* r = parent; r; indent += 4 )
    r = r->parent();  
  char* spaces = new char[indent+1];
  for( int c = 0; c < indent; c++ )
    spaces[c] = ' ';
  spaces[indent] = 0;  
  
  // basic information for troubleshooting
  std::cout << std::endl;
  std::cout << spaces;
  std::cout << "Record " << type;
  std::cout << " pos=" << pos;
  std::cout << " size=" << size;
  std::cout << std::endl;

  // create the record using the factory
  Record* record = Record::create( type );
  if( record )
  {
    // container or record?
    if( record->isContainer() )
    {
      record->setParent( parent );
      record->setPosition( pos );
      std::cout << spaces << record->name() << std::endl;
      while( d->stream->tell() < nextpos )
        loadRecord( record );    
    }
    else
    {
      // load actual record data
      d->stream->read( buffer, size );
      
      // setup the record and invoke handler
      record->setParent( parent );
      record->setData( size, buffer );
      record->setPosition( pos );
      handleRecord( record );
      delete record;
    }
  }
  
  d->stream->seek( nextpos );
  delete [] spaces;
}

