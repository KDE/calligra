#ifndef __kodocument_h__
#define __kodocument_h__

class KoDocumentChild;
class KoDocumentChildPicture;

#include <document_impl.h>

#include <komlParser.h>

#include <vector>
#include <string>
#include <iostream.h>

#include <qrect.h>
#include <qpicture.h>


/**
 * Holds an embedded object.
 */
class KoDocumentChild
{
public:
  KoDocumentChild( const QRect& _rect, OPParts::Document_ptr _doc );
  KoDocumentChild();
  virtual ~KoDocumentChild();
  
  virtual OPParts::Document_ptr document() { return OPParts::Document::_duplicate( m_rDoc ); }
  virtual const char* source() { return m_strSource.c_str(); }
  virtual const QRect& geometry() { return m_geometry; }
  virtual void setGeometry( const QRect& _rect );

  /**
   * Writes the OBJECT tag, but does NOT write the content of the
   * embedded document. Saveing the embedded documents themselves
   * is done in @ref Document_impl. This function just stores information
   * about the position and id of the embedded document.
   */
  virtual bool save( ostream& out );
  /**
   * Parses the OBJECT tag. This does NOT mean creating the child documents.
   * AFTER the 'parser' finished parsing, you must use @ref #loadDocument
   * or @ref #loadDocumentMimePart to actually load the embedded documents.
   */
  virtual bool load( KOMLParser& parser, vector<KOMLAttrib>& _attribs );
  virtual bool loadDocument( OPParts::MimeMultipartDict_ptr _dict );

  virtual bool isStoredExtern();
  
  /**
   * @param _force may be set to true. In this case this function
   *               always returns a valid QPicture even if the child does
   *               not support the printing extension. This QPicture will only
   *               paint a white rectangle instead of the childs content.
   *               Otherwise 0L is returned if the child does not support the
   *               printing Extension.
   */
  virtual QPicture* draw( bool _force = true );

  virtual OPParts::View_ptr createView( OPParts::PartShell_ptr _shell );
  
protected:
  /**
   * Creates a new document and loads it with the data in @ref #m_strSource.
   *
   * Use this function to load a document that has been saved in its
   * own file.
   *
   * Use only if you constructed the object with the "2 parameter constructor".
   * It tries then to load the document from @ref #m_strSource.
   */
  virtual bool loadDocument();
  /**
   * Creates a new document and loads it from the data stored in an
   * mime multipart file.
   * 
   * Use this function to load a document that has been saved embedded,
   * this means in a mime multipart file.
   *
   * Use only if you constructed the object with the "2 parameter constructor".
   * It tries then to load the document from @ref #m_strSource.
   */
  virtual bool loadDocumentAsMimePart( OPParts::MimeMultipartDict_ptr _dict, OPParts::MimeMultipartEntity_ptr _entity );

  Document_ref m_rDoc;
  /**
   * The geometry is assumed to be always unzoomed.
   */
  QRect m_geometry;
  /**
   * Holds the source of this object, for example "file:/home/weis/image.gif" or
   * "mime:/table1/2" if it stored in a compound document. If this string
   * is empty then the document was created from scratch and not saved yet.
   * Those documents are usually stored in a compound document later.
   */
  string m_strSource;
  string m_strMimeType;

  QPicture* m_pPicture;
  bool m_bHasPrintingExtension;
};


class KoDocumentChildPicture
{
public:
  KoDocumentChildPicture( KoDocumentChild *_child );
  virtual ~KoDocumentChildPicture();

  virtual KoDocumentChild* child() { return m_pChild; }
  virtual QPicture* picture() { return m_pChild->draw(); }
  virtual const QRect& geometry() { return m_pChild->geometry(); }
  
protected:
  KoDocumentChild* m_pChild;
};



class KoDocument : virtual public Document_impl
{
public:
  // C++
  KoDocument() { };
  virtual ~KoDocument() { };
  
protected:
  // IDL
  virtual CORBA::Boolean open( const char *_url );
  virtual CORBA::Boolean openMimePart( OPParts::MimeMultipartDict_ptr _dict, const char *_id );
  // C++
  /**
   * @param _randomaccess tells wether input stream is a serial stream or a random access stream,
   *                      usually a @ref ifstream or a istringstream.
   */
  virtual bool load( istream &, bool _randomaccess = false );
  virtual bool load( KOMLParser& ) { cerr << "KoDocument::load not implemented" << endl; assert(0); return false; };
  /**
   * An example implementation may look like this:
   * <PRE>
   * QListIterator<KSpreadChild> it( m_lstChildren );
   * for( ; it.current(); ++it )
   *  if ( !it.current()->loadDocument( _dict ) )
   *   return false;
   * return true;
   * </PRE>
   */
  virtual bool loadChildren( OPParts::MimeMultipartDict_ptr _dict ) = 0;

  // IDL
  virtual CORBA::Boolean saveAs( const char *_url, const char *_format );
  virtual CORBA::Boolean saveAsMimePart( const char *_url, const char *_format, const char *_boundary );
  // C++
  virtual bool save( ostream& out ) = 0;
  /**
   * Overload this function with your personal text.
   */
  virtual const char* copyright() { return ""; }
  virtual const char* comment() { return ""; }
  /**
   * An example implementation may look like this one:
   * <PRE>
   * QListIterator<KSpreadChild> it( m_lstChildren );
   * for( ; it.current(); ++it )
   * {
   *   if ( !it.current()->isStoredExtern() )
   *    return true;    
   * }
   * return false;
   * </PRE>
   */
  virtual bool hasToWriteMultipart() = 0;
};

#endif
