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

#ifndef LIBPPT_POWERPOINT
#define LIBPPT_POWERPOINT

#include <iostream>
#include "ustring.h"

namespace Libppt
{

// forward declaration
class Presentation;

class Record
{
public:

  /**
    Static ID of the record. Subclasses should override this value
    with the id of the record they handle.
  */  
  static const unsigned int id;
  
  virtual unsigned int rtti(){
	  return this->id;
  }
  
  /**
    Creates a new generic record.
  */  
  Record();
  
  /**
    Destroys the record.
  */
  virtual ~Record();
  
  /**
   * Record factory, create a new record of specified type.
   */
  static Record* create( unsigned type );
  
  /**
   * Returns true if this is a Container.
   */
  virtual bool isContainer() const { return false; }
  
  void setParent( Record* parent );
  
  const Record* parent() const;
  
  /**
    Sets the data for this record.
   */
  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
    Sets the position of the record in the OLE stream. Somehow this is
    required to process BoundSheet and BOF(Worksheet) properly.
   */
  void setPosition( unsigned pos );
  
  /**
    Gets the position of this record in the OLE stream. 
   */
  unsigned position() const;
  
  /**
    Returns the name of the record. For debugging only.
   */
  virtual const char* name(){ return "Unknown"; }

  /**
    Dumps record information to output stream. For debugging only.
   */
  virtual void dump( std::ostream& out ) const;
  
protected:

   // position of this record in the OLE stream
   unsigned stream_position;
   
   Record* record_parent;
   
private:
   // no copy or assign
   Record( const Record& );
   Record& operator=( const Record& );   
};

// Container is a special record, it is for holding another records
class Container: public Record
{
public:
  Container();
  virtual bool isContainer() const { return true; }
  const char* name(){ return "Container"; }

private:
   // no copy or assign
   Container( const Container& );
   Container& operator=( const Container& );   
};

class DocumentContainer: public Container
{
public:
  static const unsigned int id;
  DocumentContainer();
  const char* name(){ return "DocumentContainer"; }

private:
  // no copy or assign
  DocumentContainer( const DocumentContainer& );
  DocumentContainer& operator=( const DocumentContainer& );   
};

class EnvironmentContainer: public Container
{
public:
  static const unsigned int id;
  EnvironmentContainer();
  const char* name(){ return "EnvironmentContainer"; }

private:
  // no copy or assign
  EnvironmentContainer( const EnvironmentContainer& );
  EnvironmentContainer& operator=( const EnvironmentContainer& );   
};

class ExObjListContainer: public Container
{
public:
  static const unsigned int id;
  ExObjListContainer();
  const char* name(){ return "ExObjListContainer"; }

private:
  // no copy or assign
  ExObjListContainer( const ExObjListContainer& );
  ExObjListContainer& operator=( const ExObjListContainer& );   
};

class ExHyperlinkContainer : public Container
{
public:
  static const unsigned int id;
  ExHyperlinkContainer ();
  const char* name(){ return "ExHyperlinkContainer "; }

private:
  // no copy or assign
  ExHyperlinkContainer ( const ExHyperlinkContainer & );
  ExHyperlinkContainer & operator=( const ExHyperlinkContainer & );   
};

class ExEmbedContainer : public Container
{
public:
  static const unsigned int id;
  ExEmbedContainer ();
  const char* name(){ return "ExEmbedContainer "; }

private:
  // no copy or assign
  ExEmbedContainer ( const ExEmbedContainer & );
  ExEmbedContainer & operator=( const ExEmbedContainer & );   
};

class ListContainer: public Container
{
public:
  static const unsigned int id;
  ListContainer();
  const char* name(){ return "ListContainer"; }

private:
  // no copy or assign
  ListContainer( const ListContainer& );
  ListContainer& operator=( const ListContainer& );   
};

class MainMasterContainer: public Container
{
public:
  static const unsigned int id;
  MainMasterContainer();
  const char* name(){ return "MainMasterContainer"; }

private:
  // no copy or assign
  MainMasterContainer( const MainMasterContainer& );
  MainMasterContainer& operator=( const MainMasterContainer& );   
};

class OutlineViewInfoContainer : public Container
{
public:
  static const unsigned int id;
  OutlineViewInfoContainer ();
  const char* name(){ return "OutlineViewInfoContainer "; }

private:
  // no copy or assign
  OutlineViewInfoContainer ( const OutlineViewInfoContainer & );
  OutlineViewInfoContainer & operator=( const OutlineViewInfoContainer & );   
};

class ProgTagsContainer : public Container
{
public:
  static const unsigned int id;
  ProgTagsContainer ();
  const char* name(){ return "ProgTagsContainer "; }

private:
  // no copy or assign
  ProgTagsContainer ( const ProgTagsContainer & );
  ProgTagsContainer & operator=( const ProgTagsContainer & );   
};

class PPDrawingContainer : public Container
{
public:
  static const unsigned int id;
  PPDrawingContainer ();
  const char* name(){ return "PPDrawingContainer "; }

private:
  // no copy or assign
  PPDrawingContainer ( const PPDrawingContainer & );
  PPDrawingContainer & operator=( const PPDrawingContainer & );   
};

class PPDrawingGroupContainer : public Container
{
public:
  static const unsigned int id;
  PPDrawingGroupContainer ();
  const char* name(){ return "PPDrawingGroupContainer "; }

private:
  // no copy or assign
  PPDrawingGroupContainer ( const PPDrawingGroupContainer & );
  PPDrawingGroupContainer & operator=( const PPDrawingGroupContainer & );   
};

class SorterViewInfoContainer : public Container
{
public:
  static const unsigned int id;
  SorterViewInfoContainer ();
  const char* name(){ return "SorterViewInfoContainer "; }

private:
  // no copy or assign
  SorterViewInfoContainer ( const SorterViewInfoContainer & );
  SorterViewInfoContainer & operator=( const SorterViewInfoContainer & );   
};


class SlideContainer: public Container
{
public:
  static const unsigned int id;
  SlideContainer();
  const char* name(){ return "SlideContainer"; }

private:
  // no copy or assign
  SlideContainer( const SlideContainer& );
  SlideContainer& operator=( const SlideContainer& );   
};

class SlideBaseContainer: public Container
{
public:
  static const unsigned int id;
  SlideBaseContainer();
  const char* name(){ return "SlideBaseContainer"; }

private:
  // no copy or assign
  SlideBaseContainer( const SlideBaseContainer& );
  SlideBaseContainer& operator=( const SlideBaseContainer& );   
};

class SlideListWithTextContainer: public Container
{
public:
  static const unsigned int id;
  SlideListWithTextContainer();
  const char* name(){ return "SlideListWithTextContainer"; }

private:
  // no copy or assign
  SlideListWithTextContainer( const SlideListWithTextContainer& );
  SlideListWithTextContainer& operator=( const SlideListWithTextContainer& );   
};

class FontCollectionContainer: public Container
{
public:
  static const unsigned int id;
  FontCollectionContainer();
  const char* name(){ return "FontCollectionContainer"; }

private:
  // no copy or assign
  FontCollectionContainer( const FontCollectionContainer& );
  FontCollectionContainer& operator=( const FontCollectionContainer& );   
};

 
class SlideViewInfoContainer: public Container
{
public:
  static const unsigned int id;
  SlideViewInfoContainer();
  const char* name(){ return "SlideViewInfoContainer"; }

private:
  // no copy or assign
  SlideViewInfoContainer( const SlideViewInfoContainer& );
  SlideViewInfoContainer& operator=( const SlideViewInfoContainer& );   
};


class ExOleObjStgContainer: public Container
{
public:
  static const unsigned int id;
  ExOleObjStgContainer();
  const char* name(){ return "ExOleObjStgContainer"; }

private:
  // no copy or assign
  ExOleObjStgContainer( const ExOleObjStgContainer& );
  ExOleObjStgContainer& operator=( const ExOleObjStgContainer& );   
};

class HeadersFootersContainer: public Container
{
public:
  static const unsigned int id;
  HeadersFootersContainer();
  const char* name(){ return "HeadersFootersContainer"; }

private:
  // no copy or assign
  HeadersFootersContainer( const DocumentContainer& );
  HeadersFootersContainer& operator=( const DocumentContainer& );   
};



class NotesContainer: public Container
{
public:
  static const unsigned int id;
  NotesContainer();
  const char* name(){ return "NotesContainer"; }

private:
  // no copy or assign
  NotesContainer( const NotesContainer& );
  NotesContainer& operator=( const NotesContainer& );   
};


class SrKinsokuContainer: public Container
{
public:
  static const unsigned int id;
  SrKinsokuContainer();
  const char* name(){ return "SrKinsokuContainer"; }

private:
  // no copy or assign
  SrKinsokuContainer( const SrKinsokuContainer& );
  SrKinsokuContainer& operator=( const SrKinsokuContainer& );   
};

class ProgStringTagContainer: public Container
{
public:
  static const unsigned int id;
  ProgStringTagContainer();
  const char* name(){ return "ProgStringTagContainer"; }

private:
  // no copy or assign
  ProgStringTagContainer( const ProgStringTagContainer& );
  ProgStringTagContainer& operator=( const ProgStringTagContainer& );   
};

class ProgBinaryTagContainer: public Container
{
public:
  static const unsigned int id;
  ProgBinaryTagContainer();
  const char* name(){ return "ProgBinaryTagContainer"; }

private:
  // no copy or assign
  ProgBinaryTagContainer( const ProgBinaryTagContainer& );
  ProgBinaryTagContainer& operator=( const ProgBinaryTagContainer& );   
};

class VBAInfoContainer: public Container
{
public:
  static const unsigned int id;
  VBAInfoContainer();
  const char* name(){ return "VBAInfoContainer"; }

private:
  // no copy or assign
  VBAInfoContainer( const VBAInfoContainer& );
  VBAInfoContainer& operator=( const VBAInfoContainer& );   
};

class ViewInfoContainer: public Container
{
public:
  static const unsigned int id;
  ViewInfoContainer();
  const char* name(){ return "ViewInfoContainer"; }

private:
  // no copy or assign
  ViewInfoContainer( const ViewInfoContainer& );
  ViewInfoContainer& operator=( const ViewInfoContainer& );   
};



class CStringAtom: public Record
{
public:
  static const unsigned int id;
  CStringAtom();
  ~CStringAtom();
  
  UString ustring() const;
  void setUString( const UString& ustr );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "CStringAtom"; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  CStringAtom( const CStringAtom& );
  CStringAtom& operator=( const CStringAtom& );   
  
  class Private;
  Private *d;  
};

class DocumentAtom : public Record
{
public:
  static const unsigned int id;
  DocumentAtom();
  ~DocumentAtom();
  
  int slideWidth() const;
  void setSlideWidth( int w );
  int slideHeight() const;
  void setSlideHeight( int h );
  int notesWidth() const;
  void setNotesWidth( int nw );
  int notesHeight() const;
  void setNotesHeight( int nh );
  int zoomNumer() const;
  void setZoomNumer( int numer );
  int zoomDenom() const;
  void setZoomDenom( int denom );
  
  int notesMasterPersist() const; 
  void setNotesMasterPersist( int notesMasterPersist );
  
  int handoutMasterPersist() const; 
  void setHandoutMasterPersist(int handoutMasterPersist);    
  
  int firstSlideNum() const; 
  void setFirstSlideNum( int firstSlideNum ); 
  
  int slideSizeType() const; 
  void setSlideSizeType( int slideSizeType ); 
  
  int saveWithFonts() const; 
  void setSaveWithFonts( int saveWithFonts ); 
  
  int omitTitlePlace() const; 
  void setOmitTitlePlace( int omitTitlePlace ); 
  
  int rightToLeft() const; 
  void setRightToLeft( int rightToLeft ); 
  
  int showComments() const; 
  void setShowComments( int showComments); 
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "DocumentAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  DocumentAtom ( const DocumentAtom & );
  DocumentAtom & operator=( const DocumentAtom & );   
  
  class Private;
  Private *d;  
};

class SlideViewInfoAtom  : public Record
{
public:
  static const unsigned int id;
  SlideViewInfoAtom ();
  ~SlideViewInfoAtom ();

  int showGuides() const; 
  void setShowGuides( int showGuides); 
  int snapToGrid() const; 
  void setSnapToGrid( int snapToGrid); 
  int snapToShape() const; 
  void setSnapToShape( int snapToShape); 
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SlideViewInfoAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  SlideViewInfoAtom  ( const SlideViewInfoAtom  & );
  SlideViewInfoAtom  & operator=( const SlideViewInfoAtom  & );   
  
  class Private;
  Private *d;  
};  
  
  
class ViewInfoAtom : public Record
{
public:
  static const unsigned int id;
  ViewInfoAtom  ();
  ~ViewInfoAtom  ();

  int CurScaleXNum() const; 
  void setCurScaleXNum( int CurScaleXNum); 
  int CurScaleXDen() const; 
  void setCurScaleXDen( int CurScaleXDen); 
  int CurScaleYNum() const; 
  void setCurScaleYNum( int CurScaleYNum); 
  int CurScaleYDen() const; 
  void setCurScaleYDen( int CurScaleYDen); 
  int PrevScaleXNum() const; 
  void setPrevScaleXNum( int PrevScaleXNum); 
  int PrevScaleXDen() const; 
  void setPrevScaleXDen( int PrevScaleXDen); 
  int PrevScaleYNum() const; 
  void setPrevScaleYNum( int PrevScaleYNum); 
  int PrevScaleYDen() const; 
  void setPrevScaleYDen( int PrevScaleYDen); 
  int ViewSizeX() const; 
  void setViewSizeX( int ViewSizeX); 
  int ViewSizeY() const; 
  void setViewSizeY( int ViewSizeY); 
  int OriginX() const; 
  void setOriginX( int OriginX); 
  int OriginY() const; 
  void setOriginY( int OriginY); 
  int varScale() const; 
  void setVarScale( int varScale); 
  int draftMode() const; 
  void setDraftMode( int draftMode); 
  int padding() const; 
  void setPadding( int padding); 
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ViewInfoAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ViewInfoAtom   ( const ViewInfoAtom   & );
  ViewInfoAtom   & operator=( const ViewInfoAtom   & );   
  
  class Private;
  Private *d;  
};    

class FontEntityAtom : public Record
{
public:
  static const unsigned int id;
  FontEntityAtom();
  ~FontEntityAtom();
  
  const char* name(){ return "FontEntityAtom"; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  FontEntityAtom ( const FontEntityAtom & );
  FontEntityAtom & operator=( const FontEntityAtom & );   
  
  class Private;
  Private *d;  
};

class GuideAtom  : public Record
{
public:
  static const unsigned int id;
  GuideAtom ();
  ~GuideAtom ();
  
  int type() const; 
  void setType(int type); 
  int pos() const;
  void setPos(int pos);  
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "GuideAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  GuideAtom  ( const GuideAtom  & );
  GuideAtom  & operator=( const GuideAtom  & );   
  
  class Private;
  Private *d;  
};

class ExOleObjAtom  : public Record
{
public:
  static const unsigned int id;
  ExOleObjAtom ();
  ~ExOleObjAtom ();
  
  int drawAspect() const; 
  void setDrawAspect(int drawAspect); 
  int type() const;
  void setType(int type); 
  int objID() const; 
  void setObjID(int objID); 
  int subType() const;
  void setSubType(int subType);
  int objStgDataRef() const; 
  void setObjStgDataRef(int objStgDataRef); 
  int isBlank() const;
  void setIsBlank(int isBlank); 
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ExOleObjAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ExOleObjAtom  ( const ExOleObjAtom  & );
  ExOleObjAtom  & operator=( const ExOleObjAtom  & );   
  
  class Private;
  Private *d;  
};

class ExEmbedAtom  : public Record
{
public:
  static const unsigned int id;
  ExEmbedAtom ();
  ~ExEmbedAtom ();
  
  int followColorScheme() const; 
  void setFollowColorScheme(int followColorScheme); 
  int cantLockServerB() const;
  void setCantLockServerB(int cantLockServerB); 
  int noSizeToServerB() const; 
  void setNoSizeToServerB(int noSizeToServerB); 
  int isTable() const;
  void setIsTable(int isTable);
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ExEmbedAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ExEmbedAtom  ( const ExEmbedAtom  & );
  ExEmbedAtom  & operator=( const ExEmbedAtom  & );   
  
  class Private;
  Private *d;  
};

class ColorSchemeAtom : public Record
{
public:
  static const unsigned int id;
  ColorSchemeAtom();
  ~ColorSchemeAtom();

  int Background() const;
  void setBackground( int Background );
  int TextAndLines() const;
  void setTextAndLines( int TextAndLines );
  int Shadows() const;
  void setShadows( int Shadows );
  int TitleText() const;
  void setTitleText( int TitleText );
  int Fills() const;
  void setFills( int Fills );
  int Accent() const;
  void setAccent( int Accent );
  int AccentAndHyperlink() const;
  void setAccentAndHyperlink ( int AccentAndHyperlink );
  int AccentAndFollowedHyperlink() const;
  void setAccentAndFollowedHyperlink( int AccentAndFollowedHyperlink );
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ColorSchemeAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ColorSchemeAtom ( const ColorSchemeAtom & );
  ColorSchemeAtom & operator=( const ColorSchemeAtom & );   
  
  class Private;
  Private *d;  
};

class CurrentUserAtom : public Record
{
public:
  static const unsigned int id;
  CurrentUserAtom();
  ~CurrentUserAtom();

  int Size() const;
  void setSize( int Size );
  int Magic() const;
  void setMagic( int Magic );
  int OffsettoCurrentEdit() const;
  void setOffsettoCurrentEdit( int OffsettoCurrentEdit );
  int LenUserName() const;
  void setLenUserName( int LenUserName );
  int DocFileVersion() const;
  void setDocFileVersion( int DocFileVersion );
  int majorVersion() const;
  void setMajorVersion( int majorVersion );
  int minorVersion() const;
  void setMinorVersion ( int minorVersion );
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ColorSchemeAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  CurrentUserAtom ( const CurrentUserAtom & );
  CurrentUserAtom & operator=( const CurrentUserAtom & );   
  
  class Private;
  Private *d;  
};

class ExObjListAtom : public Record
{
public:
  static const unsigned int id;
  ExObjListAtom();
  ~ExObjListAtom();
  
  int objectIdSeed() const;
  void setObjectIdSeed( int objectIdSeed );
   
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ExObjListAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ExObjListAtom ( const ExObjListAtom & );
  ExObjListAtom & operator=( const ExObjListAtom & );   
  
  class Private;
  Private *d;  
};

class NotesAtom : public Record
{
public:
  static const unsigned int id;
  NotesAtom();
  ~NotesAtom();
  
  int slideId() const;
  void setSlideId( int slideId );
  int flags() const;
  void setFlags( int flags );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "NotesAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  NotesAtom ( const NotesAtom & );
  NotesAtom & operator=( const NotesAtom & );   
  
  class Private;
  Private *d;  
};

class HeadersFootersAtom  : public Record
{
public:
  static const unsigned int id;
  HeadersFootersAtom ();
  ~HeadersFootersAtom ();
  
  int formatId() const;
  void setFormatId( int slideId );
  int flags() const;
  void setFlags( int flags );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "HeadersFootersAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  HeadersFootersAtom  ( const HeadersFootersAtom  & );
  HeadersFootersAtom  & operator=( const HeadersFootersAtom  & );   
  
  class Private;
  Private *d;  
};


class StyleTextPropAtom   : public Record
{
public:
  static const unsigned int id;
  StyleTextPropAtom ();
  ~StyleTextPropAtom (); 
    
  const char* name(){ return "StyleTextPropAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  StyleTextPropAtom   ( const StyleTextPropAtom   & );
  StyleTextPropAtom   & operator=( const StyleTextPropAtom   & );   
  
  class Private;
  Private *d;  
};

class TxMasterStyleAtom  : public Record
{
public:
  static const unsigned int id;
  TxMasterStyleAtom();
  ~TxMasterStyleAtom(); 
    
  const char* name(){ return "TxMasterStyleAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TxMasterStyleAtom  ( const TxMasterStyleAtom  & );
  TxMasterStyleAtom  & operator=( const TxMasterStyleAtom  & );   
  
  class Private;
  Private *d;  
};

class PersistPtrIncrementalBlockAtom : public Record
{
public:
  static const unsigned int id;
  PersistPtrIncrementalBlockAtom();
  ~PersistPtrIncrementalBlockAtom(); 
  const char* name(){ return "PersistPtrIncrementalBlockAtom "; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  PersistPtrIncrementalBlockAtom ( const PersistPtrIncrementalBlockAtom & );
  PersistPtrIncrementalBlockAtom & operator=( const PersistPtrIncrementalBlockAtom & );   
};

class ExHyperlinkAtom : public Record
{
public:
  static const unsigned int id;
  ExHyperlinkAtom ();
  ~ExHyperlinkAtom (); 
    
  int objID() const; 
  void setObjID (int objID); 
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ExHyperlinkAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  ExHyperlinkAtom   ( const ExHyperlinkAtom   & );
  ExHyperlinkAtom   & operator=( const ExHyperlinkAtom   & );   
  
  class Private;
  Private *d;  
};


class TxCFStyleAtom   : public Record
{
public:
  static const unsigned int id;
  TxCFStyleAtom ();
  ~TxCFStyleAtom (); 
    
  const char* name(){ return "TxCFStyleAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TxCFStyleAtom   ( const TxCFStyleAtom   & );
  TxCFStyleAtom   & operator=( const TxCFStyleAtom   & );   
  
  class Private;
  Private *d;  
};

class TextCharsAtom   : public Record
{
public:
  static const unsigned int id;
  TextCharsAtom ();
  ~TextCharsAtom (); 
  
  UString ustring() const;
  void setUString( const UString& ustr );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextCharsAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TextCharsAtom   ( const TextCharsAtom   & );
  TextCharsAtom   & operator=( const TextCharsAtom   & );   
  
  class Private;
  Private *d;  
};


class TxPFStyleAtom   : public Record
{
public:
  static const unsigned int id;
  TxPFStyleAtom ();
  ~TxPFStyleAtom (); 
    
  const char* name(){ return "TxPFStyleAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TxPFStyleAtom   ( const TxPFStyleAtom  & );
  TxPFStyleAtom   & operator=( const TxPFStyleAtom  & );   
  
  class Private;
  Private *d;  
};

class TxSIStyleAtom    : public Record
{
public:
  static const unsigned int id;
  TxSIStyleAtom  ();
  ~TxSIStyleAtom  (); 
    
  const char* name(){ return "TxSIStyleAtom    "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TxSIStyleAtom    ( const TxSIStyleAtom   & );
  TxSIStyleAtom    & operator=( const TxSIStyleAtom   & );   
  
  class Private;
  Private *d;  
};


class SrKinsokuAtom : public Record
{
public:
  static const unsigned int id;
  SrKinsokuAtom ();
    
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SrKinsokuAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  SrKinsokuAtom ( const SrKinsokuAtom & );
  SrKinsokuAtom & operator=( const SrKinsokuAtom & );   
  
  class Private;
  Private *d;  
};

class Record1043 : public Record
{
public:
  static const unsigned int id;
  Record1043 ();
    
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "Record1043 "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  Record1043 ( const Record1043 & );
  Record1043 & operator=( const Record1043 & );   
  
  class Private;
  Private *d;  
};

class Record1044 : public Record
{
public:
  static const unsigned int id;
  Record1044 ();
    
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "Record1044 "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  Record1044 ( const SrKinsokuAtom & );
  Record1044 & operator=( const SrKinsokuAtom & );   
  
  class Private;
  Private *d;  
};

class TextHeaderAtom : public Record
{
public:
  static const unsigned int id;
  TextHeaderAtom ();
  ~TextHeaderAtom ();
  
  int txType() const;
  void setTxType( int txType );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextHeaderAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TextHeaderAtom ( const TextHeaderAtom & );
  TextHeaderAtom & operator=( const TextHeaderAtom & );   
  
  class Private;
  Private *d;  
};


class TextSpecInfoAtom  : public Record
{
public:
  static const unsigned int id;
  TextSpecInfoAtom  ();
  ~TextSpecInfoAtom  ();
  
  int txSpecInfo() const;
  void setTxSpecInfo( int txSpecInfo );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextSpecInfoAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TextSpecInfoAtom ( const TextSpecInfoAtom & );
  TextSpecInfoAtom & operator=( const TextSpecInfoAtom & );   
  
  class Private;
  Private *d;  
};

 
class TextBytesAtom  : public Record
{
public:
  static const unsigned int id;
  TextBytesAtom  ();
  ~TextBytesAtom  ();
  
  //int txSpecInfo() const;
  //void setTxSpecInfo( int txSpecInfo );
  
  UString ustring() const;
  void setUString( const UString& ustr );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextBytesAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TextBytesAtom ( const TextBytesAtom & );
  TextBytesAtom & operator=( const TextBytesAtom & );   
  
  class Private;
  Private *d;  
};


class SlidePersistAtom   : public Record
{
public:
  static const unsigned int id;
  SlidePersistAtom   ();
  ~SlidePersistAtom   ();
  
  int psrReference() const;
  void setPsrReference( int psrReference );
  
  int flags() const;
  void setFlags( int flags );
  int numberTexts() const;
  void setNumberTexts( int numberTexts );
  int slideId() const;
  void setSlideId( int slideId );
  int reserved() const; 
  void setReserved(int reserved);
     
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SlidePersistAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  SlidePersistAtom  ( const SlidePersistAtom  & );
  SlidePersistAtom  & operator=( const SlidePersistAtom  & );   
  
  class Private;
  Private *d;  
};

class SSDocInfoAtom : public Record
{
public:
  static const unsigned int id;
  SSDocInfoAtom   ();
  ~SSDocInfoAtom   ();
  
  int PenColorRed() const;
  void setPenColorRed( int PenColorRed );
  int PenColorGreen() const;
  void setPenColorGreen( int PenColorGreen );
  int PenColorBlue() const;
  void setPenColorBlue( int PenColorBlue );
  int PenColorIndex() const;
  void setPenColorIndex( int PenColorIndex );
  int RestartTime() const;
  void setRestartTime( int RestartTime );
  int StartSlide() const;
  void setStartSlide( int StartSlide );
  int EndSlide() const;
  void setEndSlide( int EndSlide );
  int NamedShow() const; 
  void setNamedShow(int NamedShow);
  int Flags() const; 
  void setFlags(int Flags);
         
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SSDocInfoAtom  "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  SSDocInfoAtom  ( const SSDocInfoAtom  & );
  SSDocInfoAtom  & operator=( const SSDocInfoAtom  & );   
  
  class Private;
  Private *d;  
};


class EndDocumentAtom: public Record
{
public:
  static const unsigned int id;
  EndDocumentAtom();
  const char* name(){ return "EndDocumentAtom"; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  EndDocumentAtom( const EndDocumentAtom& );
  EndDocumentAtom& operator=( const EndDocumentAtom& );   
};

class UserEditAtom: public Record
{
public:
  static const unsigned int id;
  UserEditAtom();
  ~UserEditAtom();
  
  int lastSlideId() const;
  void setLastSlideId( int id );
  int majorVersion() const; 
  void setMajorVersion( int majorVersion );
  int minorVersion() const; 
  void setMinorVersion( int minorVersion );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "UserEditAtom"; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  UserEditAtom( const UserEditAtom& );
  UserEditAtom& operator=( const UserEditAtom& );   
  
  class Private;
  Private *d;  
};

class SlideAtom: public Record
{
public:
  static const unsigned int id;
  SlideAtom();
  ~SlideAtom();
  
  int layoutGeom() const; 
  void setLayoutGeom( int layoutGeom );
  
 // see OEPlaceHolderAtom 
  int layoutPlaceholderId() const; 
 // void setLayoutPlaceholderId(int layoutPlaceholderId);
  void setLayoutPlaceholderId(int layoutPlaceholderId1, int layoutPlaceholderId2,int layoutPlaceholderId3,int layoutPlaceholderId4,int layoutPlaceholderId5,int layoutPlaceholderId6,int layoutPlaceholderId7,int layoutPlaceholderId8); 
  int masterId() const; 
  void setMasterId( int masterId );
  int notesId() const; 
  void setNotesId( int notesId );
  int flags() const; 
  void setFlags( int flags );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SlideAtom"; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  SlideAtom( const SlideAtom& );
  SlideAtom& operator=( const SlideAtom& );   
  
  class Private;
  Private *d;  
};

class SSSlideInfoAtom: public Record
{
public:
  static const unsigned int id;
  SSSlideInfoAtom();
  ~SSSlideInfoAtom();
  
  int transType() const; 
  void settransType( int transType );
  int speed() const; 
  void setspeed(int speed);
  int direction() const; 
  void setdirection( int direction );
  int slideTime() const; 
  void setslideTime( int slideTime );
  int buildFlags() const; 
  void setbuildFlags( int buildFlags );
  int soundRef() const; 
  void setsoundRef( int soundRef );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SlideAtom"; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  SSSlideInfoAtom( const SSSlideInfoAtom& );
  SSSlideInfoAtom& operator=( const SSSlideInfoAtom& );   
  
  class Private;
  Private *d;  
};

class PPTReader
{
public:
  PPTReader();
  virtual ~PPTReader();
  bool load( Presentation* pr, const char* filename );
  
protected:  
  void loadRecord( Record* parent );
  virtual void handleRecord( Record* record );
//     
 
private:  
  // no copy or assign
  PPTReader( const PPTReader& );
  PPTReader& operator=( const PPTReader& );
  
  class Private;
  Private* d;
};

};

#endif /* LIBPPT_POWERPOINT */
