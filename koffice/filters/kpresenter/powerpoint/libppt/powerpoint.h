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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
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
  virtual void setData( unsigned size, const unsigned char* data, unsigned lastSize );

  /**
    Sets the position of the record in the OLE stream.
   */
  void setPosition( unsigned pos );
  
  /**
    Gets the position of this record in the OLE stream. 
   */
  unsigned position() const;

  void setInstance( unsigned inst );
  unsigned instance() const;
  
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

   unsigned record_instance;
   
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


class BookmarkCollectionContainer: public Container
{
public:
  static const unsigned int id;
  BookmarkCollectionContainer();
  const char* name(){ return "BookmarkCollectionContainer"; }

private:
  // no copy or assign
  BookmarkCollectionContainer( const BookmarkCollectionContainer& );
  BookmarkCollectionContainer& operator=( const BookmarkCollectionContainer& );   
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

class ExLinkContainer : public Container
{
public:
  static const unsigned int id;
  ExLinkContainer ();
  const char* name(){ return "ExLinkContainer "; }

private:
  // no copy or assign
  ExLinkContainer ( const ExLinkContainer & );
  ExLinkContainer & operator=( const ExLinkContainer & );   
};

class RunArrayContainer: public Container
{
public:
  static const unsigned int id;
  RunArrayContainer();
  
  const char* name(){ return "RunArrayContainer"; }

private:
  // no copy or assign
  RunArrayContainer( const RunArrayContainer& );
  RunArrayContainer& operator=( const RunArrayContainer& );   
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

class HandoutContainer: public Container
{
public:
  static const unsigned int id;
  HandoutContainer();
  const char* name(){ return "HandoutContainer"; }

private:
  // no copy or assign
  HandoutContainer( const HandoutContainer& );
  HandoutContainer& operator=( const HandoutContainer& );   
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

class SummaryContainer : public Container
{
public:
  static const unsigned int id;
  SummaryContainer ();
  const char* name(){ return "SummaryContainer "; }

private:
  // no copy or assign
  SummaryContainer ( const SummaryContainer & );
  SummaryContainer & operator=( const SummaryContainer & );   
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

class msofbtDgContainer: public Container
{
public:
  static const unsigned int id;
  msofbtDgContainer();
  const char* name(){ return "msofbtDgContainer"; }

private:
  // no copy or assign
  msofbtDgContainer( const msofbtDgContainer& );
  msofbtDgContainer& operator=( const msofbtDgContainer& );   
};

class msofbtSpContainer: public Container
{
public:
  static const unsigned int id;
  msofbtSpContainer();
  const char* name(){ return "msofbtSpContainer"; }

private:
  // no copy or assign
  msofbtSpContainer( const msofbtSpContainer& );
  msofbtSpContainer& operator=( const msofbtSpContainer& );   
};

class msofbtSpgrContainer: public Container
{
public:
  static const unsigned int id;
  msofbtSpgrContainer();
  const char* name(){ return "msofbtSpgrContainer"; }

private:
  // no copy or assign
  msofbtSpgrContainer( const msofbtSpgrContainer& );
  msofbtSpgrContainer& operator=( const msofbtSpgrContainer& );   
};

class msofbtDggContainer: public Container
{
public:
  static const unsigned int id;
  msofbtDggContainer();
  const char* name(){ return "msofbtDggContainer"; }

private:
  // no copy or assign
  msofbtDggContainer( const msofbtDggContainer& );
  msofbtDggContainer& operator=( const msofbtDggContainer& );   
};

class msofbtBstoreContainer: public Container
{
public:
  static const unsigned int id;
  msofbtBstoreContainer();
  const char* name(){ return "msofbtBstoreContainer"; }

private:
  // no copy or assign
  msofbtBstoreContainer( const msofbtBstoreContainer& );
  msofbtBstoreContainer& operator=( const msofbtBstoreContainer& );   
};

class msofbtSolverContainer: public Container
{
public:
  static const unsigned int id;
  msofbtSolverContainer();
  const char* name(){ return "msofbtSolverContainer"; }

private:
  // no copy or assign
  msofbtSolverContainer( const msofbtSolverContainer& );
  msofbtSolverContainer& operator=( const msofbtSolverContainer& );   
};

class BookmarkEntityAtom : public Record
{
public:
  static const unsigned int id;
  BookmarkEntityAtom ();
  ~BookmarkEntityAtom ();
  
  int bookmarkID() const;
  void setBookmarkID( int bookmarkID );
  int bookmarkName() const;
  void setBookmarkName( int bookmarkName );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "BookmarkEntityAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  BookmarkEntityAtom ( const BookmarkEntityAtom & );
  BookmarkEntityAtom & operator=( const BookmarkEntityAtom & );   
  
  class Private;
  Private *d;  
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

class ColorSchemeAtom : public Record
{
public:
  static const unsigned int id;
  ColorSchemeAtom();
  ~ColorSchemeAtom();

  int background() const;
  void setBackground( int background );
  int textAndLines() const;
  void setTextAndLines( int textAndLines );
  int shadows() const;
  void setShadows( int shadows );
  int titleText() const;
  void setTitleText( int titleText );
  int fills() const;
  void setFills( int fills );
  int accent() const;
  void setAccent( int accent );
  int accentAndHyperlink() const;
  void setAccentAndHyperlink ( int accentAndHyperlink );
  int accentAndFollowedHyperlink() const;
  void setAccentAndFollowedHyperlink( int accentAndFollowedHyperlink );
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

  int size() const;
  void setSize( int size );
  int magic() const;
  void setMagic( int magic );
  int offsetToCurrentEdit() const;
  void setOffsetToCurrentEdit( int offsetToCurrentEdit );
  int lenUserName() const;
  void setLenUserName( int lenUserName );
  int docFileVersion() const;
  void setDocFileVersion( int docFileVersion );
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

class ExLinkAtom : public Record
{
public:
  static const unsigned int id;
  ExLinkAtom();
  ~ExLinkAtom();

  int exObjId() const; 
  void setExObjId( int exObjId); 
  int flags() const; 
  void setFlags( int flags); 
  int unavailable() const; 
  void setUnavailable( int unavailable); 
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "ExLinkAtom"; }
  void dump( std::ostream& out ) const; 
private:  
   // no copy or assign
  ExLinkAtom  ( const ExLinkAtom  & );
  ExLinkAtom  & operator=( const ExLinkAtom  & );   
  
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

class FontEntityAtom : public Record
{
public:
  static const unsigned int id;
  FontEntityAtom();
  ~FontEntityAtom();
  
  UString ustring() const;
  void setUString( const UString& ustr );
  int charset() const; 
  void setCharset( int charset ) ; 
  int clipPrecision() const; 
  void setClipPrecision( int clipPrecision);
  int quality() const; 
  void setQuality( int quality );
  int pitchAndFamily() const;
  void setPitchAndFamily( int pitchAndFamily ); 

  void setData( unsigned size, const unsigned char* data );
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

class PersistIncrementalBlockAtom : public Record
{
public:
  static const unsigned int id;
  PersistIncrementalBlockAtom();
  ~PersistIncrementalBlockAtom(); 

  unsigned entryCount() const;
  unsigned long reference( unsigned index ) const;
  unsigned long offset( unsigned index ) const;

  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "PersistIncrementalBlockAtom "; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  PersistIncrementalBlockAtom ( const PersistIncrementalBlockAtom & );
  PersistIncrementalBlockAtom & operator=( const PersistIncrementalBlockAtom & );   
  
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
  Record1044 ( const Record1044 & );
  Record1044 & operator=( const Record1044 & );   
  
  class Private;
  Private *d;  
};

class SSlideLayoutAtom  : public Record
{
public:
  static const unsigned int id;
  SSlideLayoutAtom ();
  ~SSlideLayoutAtom ();

  int geom() const; 
  void setGeom( int geom); 
  int placeholderId() const; 
  void setPlaceholderId( int placeholderId); 
   
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "SSlideLayoutAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  SSlideLayoutAtom  ( const SSlideLayoutAtom  & );
  SSlideLayoutAtom  & operator=( const SSlideLayoutAtom  & );   
  
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
  
class SlidePersistAtom   : public Record
{
public:
  static const unsigned int id;
  SlidePersistAtom   ();
  virtual ~SlidePersistAtom   ();
  
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
  
  int penColorRed() const;
  void setPenColorRed( int penColorRed );
  int penColorGreen() const;
  void setPenColorGreen( int penColorGreen );
  int penColorBlue() const;
  void setPenColorBlue( int penColorBlue );
  int penColorIndex() const;
  void setPenColorIndex( int penColorIndex );
  int restartTime() const;
  void setRestartTime( int restartTime );
  int startSlide() const;
  void setStartSlide( int startSlide );
  int endSlide() const;
  void setEndSlide( int endSlide );
  int namedShow() const; 
  void setNamedShow(int namedShow);
  int flags() const; 
  void setFlags(int flags);
         
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

class StyleTextPropAtom   : public Record
{
public:
  static const unsigned int id;
  StyleTextPropAtom ();
  ~StyleTextPropAtom (); 
  
  // paragraph properties 
  int charCount( unsigned index ) const;
  int depth( unsigned index ) const;
  int bulletOn ( unsigned index ) const;
  int bulletHardFont( unsigned index ) const;
  int bulletHardColor( unsigned index ) const;
  int bulletChar ( unsigned index ) const;
  int bulletFont( unsigned index ) const;
  int bulletHeight( unsigned index ) const; 
  int bulletColor( unsigned index ) const;
  int align( unsigned index ) const;
  int lineFeed( unsigned index ) const; 
  int upperDist( unsigned index ) const; 
  int lowerDist( unsigned index ) const; 
  int asianLB1( unsigned index ) const; 
  int asianLB2( unsigned index ) const; 
  int asianLB3( unsigned index ) const; 
  int biDi( unsigned index ) const;

// character properties
  int charMask() const; 
  int charFlags() const;

  unsigned listSize() const;

  void setData( unsigned size, const unsigned char* data, unsigned lastSize );  
  const char* name(){ return "StyleTextPropAtom   "; }
  void dump( std::ostream& out ) const; 
private:  
  // character properties
  void setCharMask ( int charMask );
  void setCharFlags( int charFlags );

  // no copy or assign
  StyleTextPropAtom   ( const StyleTextPropAtom   & );
  StyleTextPropAtom   & operator=( const StyleTextPropAtom   & );   
  
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
  const char* name(){ return "SSSlideInfoAtom"; }
  void dump( std::ostream& out ) const;

private:
  // no copy or assign
  SSSlideInfoAtom( const SSSlideInfoAtom& );
  SSSlideInfoAtom& operator=( const SSSlideInfoAtom& );

  class Private;
  Private *d;
};

class SrKinsokuAtom : public Record
{
public:
  static const unsigned int id;
  SrKinsokuAtom ();
  ~SrKinsokuAtom ();
    
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

class TxCFStyleAtom   : public Record
{
public:
  static const unsigned int id;
  TxCFStyleAtom ();
  ~TxCFStyleAtom (); 
    
  int flags1() const; 
  void setFlags1( int flags1 ); 
  int flags2() const;
  void setFlags2( int flags2 ); 
  int flags3() const;
  void setFlags3( int flags3 ); 
  int n1() const;
  void setN1( int n1 ); 
  int fontHeight() const;
  void setFontHeight( int fontHeight ); 
  int fontColor() const; 
  void setFontColor( int fontColor ); 

  void setData( unsigned size, const unsigned char* data );
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
  
  unsigned listSize() const;
  UString strValue( unsigned index ) const;
  void setText( UString ustring );
   
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

class TextHeaderAtom : public Record
{
public:
  static const unsigned int id;
  TextHeaderAtom ();
  ~TextHeaderAtom ();
  
  int textType() const;
  void setTextType( int type );
  
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
  
  int charCount() const;
  void setCharCount( int txSpecInfo );
  int flags() const;
  void setFlags( int flags );

  
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

class TextBookmarkAtom: public Record
{
public:
  static const unsigned int id;
  TextBookmarkAtom();
  ~TextBookmarkAtom();
  
  int begin() const;
  void setBegin( int begin );
  int end() const; 
  void setEnd( int end );
  int bookmarkID() const; 
  void setBookmarkID( int bookmarkID );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextBookmarkAtom"; }
  void dump( std::ostream& out ) const; 
  
private:
  // no copy or assign
  TextBookmarkAtom( const TextBookmarkAtom& );
  TextBookmarkAtom& operator=( const TextBookmarkAtom& );   
  
  class Private;
  Private *d;  
};

class TextBytesAtom : public Record
{
public:
  static const unsigned int id;
  TextBytesAtom ();
  ~TextBytesAtom (); 
  
  unsigned listSize() const;
  unsigned stringLength() const;
  void setStringLength( unsigned stringLength );

  UString strValue( unsigned index ) const;
  void setText( UString ustring );
   
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "TextBytesAtom   "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  TextBytesAtom   ( const TextBytesAtom   & );
  TextBytesAtom   & operator=( const TextBytesAtom   & );   
  
  class Private;
  Private *d;  
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

  unsigned long offsetLastEdit() const;
  void setOffsetLastEdit( unsigned long ofs );
  unsigned long offsetPersistDir() const;
  void setOffsetPersistDir( unsigned long ofs ) const;
  unsigned long documentRef() const; 
  void setDocumentRef( unsigned long ref ) const; 

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

class ViewInfoAtom : public Record
{
public:
  static const unsigned int id;
  ViewInfoAtom  ();
  ~ViewInfoAtom  ();

  int curScaleXNum() const; 
  void setCurScaleXNum( int curScaleXNum); 
  int curScaleXDen() const; 
  void setCurScaleXDen( int curScaleXDen); 
  int curScaleYNum() const; 
  void setCurScaleYNum( int curScaleYNum); 
  int curScaleYDen() const; 
  void setCurScaleYDen( int curScaleYDen); 
  int prevScaleXNum() const; 
  void setPrevScaleXNum( int prevScaleXNum); 
  int prevScaleXDen() const; 
  void setPrevScaleXDen( int prevScaleXDen); 
  int prevScaleYNum() const; 
  void setPrevScaleYNum( int prevScaleYNum); 
  int prevScaleYDen() const; 
  void setPrevScaleYDen( int prevScaleYDen); 
  int viewSizeX() const; 
  void setViewSizeX( int viewSizeX); 
  int viewSizeY() const; 
  void setViewSizeY( int viewSizeY); 
  int originX() const; 
  void setOriginX( int originX); 
  int originY() const; 
  void setOriginY( int originY); 
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

class msofbtDgAtom : public Record
{
public:
  static const unsigned int id;
  msofbtDgAtom ();
  ~msofbtDgAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtDgAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtDgAtom ( const msofbtDgAtom  & );
  msofbtDgAtom & operator=( const msofbtDgAtom  & );   
  
  class Private;
  Private *d;  
};

class msofbtSpgrAtom : public Record
{
public:
  static const unsigned int id;
  msofbtSpgrAtom ();
  ~msofbtSpgrAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtSpgrAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtSpgrAtom ( const msofbtSpgrAtom  & );
  msofbtSpgrAtom & operator=( const msofbtSpgrAtom  & );   
  
  class Private;
  Private *d;  
};

class msofbtSpAtom : public Record
{
public:
  enum
   {
   msosptMin = 0,
   msosptNotPrimitive = msosptMin,
   msosptRectangle = 1,
   msosptRoundRectangle = 2,
   msosptEllipse = 3,
   msosptDiamond = 4,
   msosptIsoscelesTriangle = 5,
   msosptRightTriangle = 6,
   msosptParallelogram = 7,
   msosptTrapezoid = 8,
   msosptHexagon = 9,
   msosptOctagon = 10,
   msosptPlus = 11,
   msosptStar = 12,
   msosptArrow = 13,
   msosptThickArrow = 14,
   msosptHomePlate = 15,
   msosptCube = 16,
   msosptBalloon = 17,
   msosptSeal = 18,
   msosptArc = 19,
   msosptLine = 20,
   msosptPlaque = 21,
   msosptCan = 22,
   msosptDonut = 23,
   msosptTextSimple = 24,
   msosptTextOctagon = 25,
   msosptTextHexagon = 26,
   msosptTextCurve = 27,
   msosptTextWave = 28,
   msosptTextRing = 29,
   msosptTextOnCurve = 30,
   msosptTextOnRing = 31,
   msosptStraightConnector1 = 32,
   msosptBentConnector2 = 33,
   msosptBentConnector3 = 34,
   msosptBentConnector4 = 35,
   msosptBentConnector5 = 36,
   msosptCurvedConnector2 = 37,
   msosptCurvedConnector3 = 38,
   msosptCurvedConnector4 = 39,
   msosptCurvedConnector5 = 40,
   msosptCallout1 = 41,
   msosptCallout2 = 42,
   msosptCallout3 = 43,
   msosptAccentCallout1 = 44,
   msosptAccentCallout2 = 45,
   msosptAccentCallout3 = 46,
   msosptBorderCallout1 = 47,
   msosptBorderCallout2 = 48,
   msosptBorderCallout3 = 49,
   msosptAccentBorderCallout1 = 50,
   msosptAccentBorderCallout2 = 51,
   msosptAccentBorderCallout3 = 52,
   msosptRibbon = 53,
   msosptRibbon2 = 54,
   msosptChevron = 55,
   msosptPentagon = 56,
   msosptNoSmoking = 57,
   msosptSeal8 = 58,
   msosptSeal16 = 59,
   msosptSeal32 = 60,
   msosptWedgeRectCallout = 61,
   msosptWedgeRRectCallout = 62,
   msosptWedgeEllipseCallout = 63,
   msosptWave = 64,
   msosptFoldedCorner = 65,
   msosptLeftArrow = 66,
   msosptDownArrow = 67,
   msosptUpArrow = 68,
   msosptLeftRightArrow = 69,
   msosptUpDownArrow = 70,
   msosptIrregularSeal1 = 71,
   msosptIrregularSeal2 = 72,
   msosptLightningBolt = 73,
   msosptHeart = 74,
   msosptPictureFrame = 75,
   msosptQuadArrow = 76,
   msosptLeftArrowCallout = 77,
   msosptRightArrowCallout = 78,
   msosptUpArrowCallout = 79,
   msosptDownArrowCallout = 80,
   msosptLeftRightArrowCallout = 81,
   msosptUpDownArrowCallout = 82,
   msosptQuadArrowCallout = 83,
   msosptBevel = 84,
   msosptLeftBracket = 85,
   msosptRightBracket = 86,
   msosptLeftBrace = 87,
   msosptRightBrace = 88,
   msosptLeftUpArrow = 89,
   msosptBentUpArrow = 90,
   msosptBentArrow = 91,
   msosptSeal24 = 92,
   msosptStripedRightArrow = 93,
   msosptNotchedRightArrow = 94,
   msosptBlockArc = 95,
   msosptSmileyFace = 96,
   msosptVerticalScroll = 97,
   msosptHorizontalScroll = 98,
   msosptCircularArrow = 99,
   msosptNotchedCircularArrow = 100,
   msosptUturnArrow = 101,
   msosptCurvedRightArrow = 102,
   msosptCurvedLeftArrow = 103,
   msosptCurvedUpArrow = 104,
   msosptCurvedDownArrow = 105,
   msosptCloudCallout = 106,
   msosptEllipseRibbon = 107,
   msosptEllipseRibbon2 = 108,
   msosptFlowChartProcess = 109,
   msosptFlowChartDecision = 110,
   msosptFlowChartInputOutput = 111,
   msosptFlowChartPredefinedProcess = 112,
   msosptFlowChartInternalStorage = 113,
   msosptFlowChartDocument = 114,
   msosptFlowChartMultidocument = 115,
   msosptFlowChartTerminator = 116,
   msosptFlowChartPreparation = 117,
   msosptFlowChartManualInput = 118,
   msosptFlowChartManualOperation = 119,
   msosptFlowChartConnector = 120,
   msosptFlowChartPunchedCard = 121,
   msosptFlowChartPunchedTape = 122,
   msosptFlowChartSummingJunction = 123,
   msosptFlowChartOr = 124,
   msosptFlowChartCollate = 125,
   msosptFlowChartSort = 126,
   msosptFlowChartExtract = 127,
   msosptFlowChartMerge = 128,
   msosptFlowChartOfflineStorage = 129,
   msosptFlowChartOnlineStorage = 130,
   msosptFlowChartMagneticTape = 131,
   msosptFlowChartMagneticDisk = 132,
   msosptFlowChartMagneticDrum = 133,
   msosptFlowChartDisplay = 134,
   msosptFlowChartDelay = 135,
   msosptTextPlainText = 136,
   msosptTextStop = 137,
   msosptTextTriangle = 138,
   msosptTextTriangleInverted = 139,
   msosptTextChevron = 140,
   msosptTextChevronInverted = 141,
   msosptTextRingInside = 142,
   msosptTextRingOutside = 143,
   msosptTextArchUpCurve = 144,
   msosptTextArchDownCurve = 145,
   msosptTextCircleCurve = 146,
   msosptTextButtonCurve = 147,
   msosptTextArchUpPour = 148,
   msosptTextArchDownPour = 149,
   msosptTextCirclePour = 150,
   msosptTextButtonPour = 151,
   msosptTextCurveUp = 152,
   msosptTextCurveDown = 153,
   msosptTextCascadeUp = 154,
   msosptTextCascadeDown = 155,
   msosptTextWave1 = 156,
   msosptTextWave2 = 157,
   msosptTextWave3 = 158,
   msosptTextWave4 = 159,
   msosptTextInflate = 160,
   msosptTextDeflate = 161,
   msosptTextInflateBottom = 162,
   msosptTextDeflateBottom = 163,
   msosptTextInflateTop = 164,
   msosptTextDeflateTop = 165,
   msosptTextDeflateInflate = 166,
   msosptTextDeflateInflateDeflate = 167,
   msosptTextFadeRight = 168,
   msosptTextFadeLeft = 169,
   msosptTextFadeUp = 170,
   msosptTextFadeDown = 171,
   msosptTextSlantUp = 172,
   msosptTextSlantDown = 173,
   msosptTextCanUp = 174,
   msosptTextCanDown = 175,
   msosptFlowChartAlternateProcess = 176,
   msosptFlowChartOffpageConnector = 177,
   msosptCallout90 = 178,
   msosptAccentCallout90 = 179,
   msosptBorderCallout90 = 180,
   msosptAccentBorderCallout90 = 181,
   msosptLeftRightUpArrow = 182,
   msosptSun = 183,
   msosptMoon = 184,
   msosptBracketPair = 185,
   msosptBracePair = 186,
   msosptSeal4 = 187,
   msosptDoubleWave = 188,
   msosptActionButtonBlank = 189,
   msosptActionButtonHome = 190,
   msosptActionButtonHelp = 191,
   msosptActionButtonInformation = 192,
   msosptActionButtonForwardNext = 193,
   msosptActionButtonBackPrevious = 194,
   msosptActionButtonEnd = 195,
   msosptActionButtonBeginning = 196,
   msosptActionButtonReturn = 197,
   msosptActionButtonDocument = 198,
   msosptActionButtonSound = 199,
   msosptActionButtonMovie = 200,
   msosptHostControl = 201,
   msosptTextBox = 202,
   msosptMax,
   msosptNil = 0x0FFF
 } ; 

  static const unsigned int id;
  msofbtSpAtom ();
  ~msofbtSpAtom ();

  unsigned long shapeId() const;
  void setShapeId( unsigned long id );
  const char* shapeTypeAsString() const;
  unsigned long persistentFlag() const;
  void setPersistentFlag( unsigned long persistentFlag );

  bool isBackground() const;
  void setBackground( bool bg );
  bool isVerFlip() const; 
  void setVerFlip( bool vFlip ); 
  bool isHorFlip() const; 
  void setHorFlip( bool hFlip ); 

  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtSpAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtSpAtom ( const msofbtSpAtom  & );
  msofbtSpAtom & operator=( const msofbtSpAtom  & );   
  
  class Private;
  Private *d;  
};

class msofbtOPTAtom : public Record
{
public:
  enum {               // PID 
    Rotation = 4,
    FillType = 384,
    FillColor = 385,
    LineColor = 448,
    LineOpacity = 449,
    LineBackColor = 450,
    LineType = 452,
    LineWidth =  459,
    LineDashing = 462,
    LineStartArrowhead = 464,
    LineEndArrowhead = 465,
    LineStartArrowWidth = 466,
    LineStartArrowLength = 467,
    LineEndArrowWidth = 468,
    LineEndArrowLength = 469,
    FlagNoLineDrawDash = 511,
    ShadowColor = 513,
    ShadowOpacity =  516,
    ShadowOffsetX = 517,
    ShadowOffsetY = 518
  };

  enum {
   FillSolid,             // Fill with a solid color
   FillPattern,           // Fill with a pattern (bitmap)
   FillTexture,           // A texture (pattern with its own color map)
   FillPicture,           // Center a picture in the shape
   FillShade,             // Shade from start to end points
   FillShadeCenter,       // Shade from bounding rectangle to end point
   FillShadeShape,        // Shade from shape outline to end point
   FillShadeScale,        // Similar to msofillShade, but the fillAngle
   FillShadeTitle,        // special type - shade to title ---  for PP 
   FillBackground         // Use the background fill color/pattern
  }; // MSOFILLTYPE

  enum {   
   LineSolid,              // Solid (continuous) pen           0
   LineDashSys,            // PS_DASH system   dash style      1
   LineDotSys,             // PS_DOT system   dash style       2
   LineDashDotSys,         // PS_DASHDOT system dash style      3
   LineDashDotDotSys,      // PS_DASHDOTDOT system dash style   4
   LineDotGEL,             // square dot style                  5                      
   LineDashGEL,            // dash style                       6
   LineLongDashGEL,        // long dash style                       7  
   LineDashDotGEL,         // dash short dash                 8
   LineLongDashDotGEL,     // long dash short dash             9
   LineLongDashDotDotGEL   // long dash short dash short dash        10   
  }; // MSOLINEDASHING


   enum {
   LineNoEnd,
   LineArrowEnd,
   LineArrowStealthEnd,
   LineArrowDiamondEnd,
   LineArrowOvalEnd,
   LineArrowOpenEnd
   };  // MSOLINEEND - line end effect


   enum {
   LineNarrowArrow,
   LineMediumWidthArrow,
   LineWideArrow
   }; // MSOLINEENDWIDTH - size of arrowhead

   enum {
   LineShortArrow,
   LineMediumLenArrow,
   LineLongArrow
   };   // MSOLINEENDLENGTH - size of arrowhead

  static const unsigned int id;
  msofbtOPTAtom ();
  ~msofbtOPTAtom ();

  unsigned propertyCount() const;
  unsigned propertyId( unsigned index ) const;
  unsigned long propertyValue( unsigned index ) const;
  void setProperty( unsigned id, unsigned long value );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtOPTAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtOPTAtom ( const msofbtOPTAtom  & );
  msofbtOPTAtom & operator=( const msofbtOPTAtom  & );   
  
  class Private;
  Private *d;  
};

class msofbtChildAnchorAtom : public Record
{
public:
  static const unsigned int id;
  msofbtChildAnchorAtom ();
  ~msofbtChildAnchorAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtChildAnchorAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtChildAnchorAtom ( const msofbtChildAnchorAtom  & );
  msofbtChildAnchorAtom & operator=( const msofbtChildAnchorAtom  & );

  class Private;
  Private *d;
};

class msofbtClientAnchorAtom : public Record
{
public:
  static const unsigned int id;
  msofbtClientAnchorAtom ();
  ~msofbtClientAnchorAtom ();

  int left() const;
  void setLeft( int left );
  int top() const;
  void setTop( int top );
  int right() const;
  void setRight( int right );
  int bottom() const;
  void setBottom( int bottom );
  
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtClientAnchorAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtClientAnchorAtom ( const msofbtClientAnchorAtom  & );
  msofbtClientAnchorAtom & operator=( const msofbtClientAnchorAtom  & );   
  
  class Private;
  Private *d;  
};

class msofbtClientDataAtom : public Record
{
public:
  enum
  { None = 0,
    MasterTitle,
    MasterBody,
    MasterCenteredTitle,
    MasterNotesSlideImage,
    MasterNotesBodyImage,
    MasterDate,
    MasterSlideNumber,
    MasterFooter,
    MasterHeader,
    MasterSubtitle,
    Generic,
    Title,
    Body,
    NotesBody,
    CenteredTitle,
    Subtitle,
    VerticalTextTitle,
    VerticalTextBody,
    NotesSlideImage,
    Object,
    Graph,
    Table,
    ClipArt,
    OrganizationChart,
    MediaClip
  };

  static const unsigned int id;
  msofbtClientDataAtom ();
  ~msofbtClientDataAtom ();

  unsigned placementId() const;
  void setPlacementId( unsigned id );
  unsigned placeholderId() const;
  void setPlaceholderId( unsigned id );
  const char* placeholderIdAsString() const;

  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtClientDataAtom "; }
  void dump( std::ostream& out ) const;

private:
  // no copy or assign
  msofbtClientDataAtom ( const msofbtClientDataAtom  & );
  msofbtClientDataAtom & operator=( const msofbtClientDataAtom  & );

  class Private;
  Private *d;
};

class msofbtClientTextboxAtom : public Record
{
public:
  static const unsigned int id;
  msofbtClientTextboxAtom ();
  ~msofbtClientTextboxAtom ();

  UString ustring() const;
  void setUString( const UString& ustr );
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtClientTextboxAtom "; }
  void dump( std::ostream& out ) const;

private:
  // no copy or assign
  msofbtClientTextboxAtom ( const msofbtClientTextboxAtom  & );
  msofbtClientTextboxAtom & operator=( const msofbtClientTextboxAtom  & );

  class Private;
  Private *d;
};

class msofbtOleObjectAtom : public Record
{
public:
  static const unsigned int id;
  msofbtOleObjectAtom ();
  ~msofbtOleObjectAtom ();
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtOleObjectAtom "; }
  void dump( std::ostream& out ) const; 
private:
  // no copy or assign
  msofbtOleObjectAtom ( const msofbtOleObjectAtom  & );
  msofbtOleObjectAtom & operator=( const msofbtOleObjectAtom  & );
  class Private;
  Private *d;
};

class msofbtDeletedPsplAtom : public Record
{
public:
  static const unsigned int id;
  msofbtDeletedPsplAtom ();
  ~msofbtDeletedPsplAtom ();
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtDeletedPsplAtom "; }
  void dump( std::ostream& out ) const;

private:
  // no copy or assign
  msofbtDeletedPsplAtom ( const msofbtDeletedPsplAtom  & );
  msofbtDeletedPsplAtom & operator=( const msofbtDeletedPsplAtom  & );
  class Private;
  Private *d;

};

class msofbtDggAtom : public Record
{
public:
  static const unsigned int id;
  msofbtDggAtom ();
  ~msofbtDggAtom ();
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtDggAtom "; }
  void dump( std::ostream& out ) const;

private:
  // no copy or assign
  msofbtDggAtom ( const msofbtDggAtom  & );
  msofbtDggAtom & operator=( const msofbtDggAtom  & );
  
  class Private;
  Private *d;
};

class msofbtColorMRUAtom : public Record
{
public:
  static const unsigned int id;
  msofbtColorMRUAtom ();
  ~msofbtColorMRUAtom ();
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtColorMRUAtom "; }
  void dump( std::ostream& out ) const; 

private:  
  // no copy or assign
  msofbtColorMRUAtom ( const msofbtColorMRUAtom  & );
  msofbtColorMRUAtom & operator=( const msofbtColorMRUAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtSplitMenuColorsAtom : public Record
{
public:
  static const unsigned int id;
  msofbtSplitMenuColorsAtom ();
  ~msofbtSplitMenuColorsAtom ();
  
  unsigned fillColor() const;
  void setFillColor( unsigned fillColor );
  unsigned lineColor() const;
  void setLineColor( unsigned lineColor );
  unsigned shadowColor() const;
  void setShadowColor( unsigned shadowColor );
  unsigned threeDColor() const;
  void setThreeDColor( unsigned threeDColor );

  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtSplitMenuColorsAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtSplitMenuColorsAtom ( const msofbtSplitMenuColorsAtom  & );
  msofbtSplitMenuColorsAtom & operator=( const msofbtSplitMenuColorsAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtBSEAtom : public Record
{
public:
  static const unsigned int id;
  msofbtBSEAtom ();
  ~msofbtBSEAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtBSEAtom "; }
  void dump( std::ostream& out ) const; 

private:  
  // no copy or assign
  msofbtBSEAtom ( const msofbtBSEAtom  & );
  msofbtBSEAtom & operator=( const msofbtBSEAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtCLSIDAtom : public Record
{
public:
  static const unsigned int id;
  msofbtCLSIDAtom ();
  ~msofbtCLSIDAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtCLSIDAtom "; }
  void dump( std::ostream& out ) const; 
  

private:  
  // no copy or assign
  msofbtCLSIDAtom ( const msofbtCLSIDAtom  & );
  msofbtCLSIDAtom & operator=( const msofbtCLSIDAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtRegroupItemsAtom : public Record
{
public:
  static const unsigned int id;
  msofbtRegroupItemsAtom ();
  ~msofbtRegroupItemsAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtRegroupItemsAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtRegroupItemsAtom ( const msofbtRegroupItemsAtom  & );
  msofbtRegroupItemsAtom & operator=( const msofbtRegroupItemsAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtColorSchemeAtom : public Record
{
public:
  static const unsigned int id;
  msofbtColorSchemeAtom ();
  ~msofbtColorSchemeAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtColorSchemeAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtColorSchemeAtom ( const msofbtColorSchemeAtom  & );
  msofbtColorSchemeAtom & operator=( const msofbtColorSchemeAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtAnchorAtom : public Record
{
public:
  static const unsigned int id;
  msofbtAnchorAtom ();
  ~msofbtAnchorAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtAnchorAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtAnchorAtom ( const msofbtAnchorAtom  & );
  msofbtAnchorAtom & operator=( const msofbtAnchorAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtConnectorRuleAtom : public Record
{
public:
  static const unsigned int id;
  msofbtConnectorRuleAtom ();
  ~msofbtConnectorRuleAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtConnectorRuleAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtConnectorRuleAtom ( const msofbtConnectorRuleAtom  & );
  msofbtConnectorRuleAtom & operator=( const msofbtConnectorRuleAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtAlignRuleAtom : public Record
{
public:
  static const unsigned int id;
  msofbtAlignRuleAtom ();
  ~msofbtAlignRuleAtom ();
  
  int ruid() const; // rule ID
  void setRuid( int ruid );
  int align() const;
  void setAlign( int align );// alignment
  int cProxies() const;
  void setCProxies( int cProxies );// number of shapes governed by rule
    
  void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtAlignRuleAtom "; }
  void dump( std::ostream& out ) const; 

private:  
  // no copy or assign
  msofbtAlignRuleAtom ( const msofbtAlignRuleAtom  & );
  msofbtAlignRuleAtom & operator=( const msofbtAlignRuleAtom  & );

  class Private;
  Private *d;  
};

class msofbtArcRuleAtom : public Record
{
public:
  static const unsigned int id;
  msofbtArcRuleAtom ();
  ~msofbtArcRuleAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtArcRuleAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtArcRuleAtom ( const msofbtArcRuleAtom  & );
  msofbtArcRuleAtom & operator=( const msofbtArcRuleAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtClientRuleAtom : public Record
{
public:
  static const unsigned int id;
  msofbtClientRuleAtom ();
  ~msofbtClientRuleAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtClientRuleAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtClientRuleAtom ( const msofbtClientRuleAtom  & );
  msofbtClientRuleAtom & operator=( const msofbtClientRuleAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtCalloutRuleAtom : public Record
{
public:
  static const unsigned int id;
  msofbtCalloutRuleAtom ();
  ~msofbtCalloutRuleAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtCalloutRuleAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtCalloutRuleAtom ( const msofbtCalloutRuleAtom  & );
  msofbtCalloutRuleAtom & operator=( const msofbtCalloutRuleAtom  & );  
  
  class Private;
  Private *d;  
};

class msofbtSelectionAtom : public Record
{
public:
  static const unsigned int id;
  msofbtSelectionAtom ();
  ~msofbtSelectionAtom ();
  
 // void setData( unsigned size, const unsigned char* data );
  const char* name(){ return "msofbtSelectionAtom "; }
  void dump( std::ostream& out ) const; 
  
private:  
  // no copy or assign
  msofbtSelectionAtom ( const msofbtSelectionAtom  & );
  msofbtSelectionAtom & operator=( const msofbtSelectionAtom  & );  
  
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

  void loadUserEdit();
  void loadMaster();
  void loadSlides();
  void loadDocument();
  int indexPersistence( unsigned long offset );

  void loadRecord( Record* parent );
  void handleRecord( Record* record, int type );
  void handleContainer( Container* container, int type, unsigned size );

  void handleDocumentAtom( DocumentAtom* r );
  void handleSlidePersistAtom( SlidePersistAtom* r );
  void handleTextHeaderAtom( TextHeaderAtom* r );
  void handleTextCharsAtom( TextCharsAtom* r );
  void handleTextBytesAtom( TextBytesAtom* r ); 
  void handleStyleTextPropAtom ( StyleTextPropAtom* r );
  void handleColorSchemeAtom( ColorSchemeAtom* r );

  void handleDrawingContainer( msofbtDgContainer* r, unsigned size ); 
  void handleEscherGroupContainer( msofbtSpgrContainer* r, unsigned size ); 
  void handleSPContainer( msofbtSpContainer* r, unsigned size ); 
  void handleEscherGroupAtom( msofbtSpgrAtom* r );
  void handleEscherSpAtom( msofbtSpAtom* r );
  void handleEscherPropertiesAtom( msofbtOPTAtom* atom );
  void handleEscherClientDataAtom( msofbtClientDataAtom* r );
  void handleEscherClientAnchorAtom( msofbtClientAnchorAtom* r );
  void handleEscherTextBoxAtom( msofbtClientTextboxAtom* r);

private:  
  // no copy or assign
  PPTReader( const PPTReader& );
  PPTReader& operator=( const PPTReader& );
  
  class Private;
  Private* d;
};

}

#endif /* LIBPPT_POWERPOINT */
