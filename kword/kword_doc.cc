/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Document (header)                                      */
/******************************************************************/

#include "kword_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>

#include <kurl.h>

#include <qmsgbox.h>
#include <qcolor.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

#include "kword_doc.moc"

/******************************************************************/
/* Class: KWordChild                                              */
/******************************************************************/

/*================================================================*/
KWordChild::KWordChild(KWordDocument_impl *_wdoc,const QRect& _rect,OPParts::Document_ptr _doc)
  : KoDocumentChild(_rect,_doc)
{
  m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::KWordChild(KWordDocument_impl *_wdoc ) 
  : KoDocumentChild()
{
  m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::~KWordChild()
{
}


/******************************************************************/
/* Class: KWordDocument_impl                                      */
/******************************************************************/

/*================================================================*/
KWordDocument_impl::KWordDocument_impl()
{
  ADD_INTERFACE("IDL:OPParts/Print:1.0");

  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;
}

/*================================================================*/
CORBA::Boolean KWordDocument_impl::init()
{
  pageLayout.format = PG_DIN_A4;
  pageLayout.orientation = PG_PORTRAIT;
  pageLayout.width = PG_A4_WIDTH;
  pageLayout.height = PG_A4_HEIGHT;
  pageLayout.left = DEFAULT_LEFT_BORDER;
  pageLayout.right = DEFAULT_RIGHT_BORDER;
  pageLayout.top = DEFAULT_TOP_BORDER;
  pageLayout.bottom = DEFAULT_BOTTOM_BORDER;  
  pageLayout.unit = PG_MM;

  defaultUserFont = new KWUserFont(this,"Times");
  defaultParagLayout = new KWParagLayout(this);
  defaultParagLayout->setName("Standard");
    
  pages = 1;

  otherLayout.columns = 1; //STANDARD_COLUMNS;
  otherLayout.columnSpacing = STANDARD_COLUMN_SPACING;

  calcColumnWidth();

  KWParag *p = new KWParag( this, 0L, 0L, defaultParagLayout );
  parags->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, parags, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung." );
  p = new KWParag( this, p, 0L, defaultParagLayout );
  p->insertText( 0, "Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen." );
  return true;
}

/*================================================================*/
KWordDocument_impl::~KWordDocument_impl()
{
  sdeb("KWordDocument_impl::~KWordDocument_impl()\n");
  cleanUp();
  edeb("...KWordDocument_impl::~KWordDocument_impl()\n");
}

/*================================================================*/
void KWordDocument_impl::cleanUp()
{
  if (m_bIsClean) return;

  assert( m_lstViews.count() == 0 );
  
  m_lstChildren.clear();

  Document_impl::cleanUp();
}

/*================================================================*/
bool KWordDocument_impl::hasToWriteMultipart()
{  
  if (m_lstChildren.count() == 0) return false;
  
  return true;
}

/*================================================================*/
bool KWordDocument_impl::loadChildren(OPParts::MimeMultipartDict_ptr _dict)
{
  cerr << "bool KWordDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )" << endl;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      cerr << "Loading child" << endl;
      if (!it.current()->loadDocument(_dict))
	return false;
    }

  cerr << "Loading done" << endl;
  
  return true;
}

/*================================================================*/
bool KWordDocument_impl::load(KOMLParser& parser)
{
  return true;
}

/*================================================================*/
bool KWordDocument_impl::load(istream &in,bool _randomaccess)
{
  return true;
}

/*================================================================*/
bool KWordDocument_impl::save(ostream &out)
{
  return true;
}

/*================================================================*/
void KWordDocument_impl::makeChildListIntern(OPParts::Document_ptr _doc,const char *_path)
{
  cerr << "void KWordDocument_impl::makeChildList( OPParts::Document_ptr _doc, const char *_path )" << endl;
  
  int i = 0;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      QString tmp;
      tmp.sprintf("/%i",i++);
      QString path(_path);
      path += tmp.data();
      cerr << "SETTING NAME To " << path.data() << endl;
    
      OPParts::Document_var doc = it.current()->document();    
      doc->makeChildList(_doc,path);
    }
}

/*================================================================*/
QStrList KWordDocument_impl::outputFormats()
{
  return QStrList();
}

/*================================================================*/
QStrList KWordDocument_impl::inputFormats()
{
  return QStrList();
}

/*================================================================*/
void KWordDocument_impl::viewList(OPParts::Document::ViewList*& _list)
{
  (*_list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KWordView_impl> it(m_lstViews);
  for(;it.current();++it)
    {
      (*_list)[i++] = OPParts::View::_duplicate(it.current());
    }
}

/*================================================================*/
void KWordDocument_impl::addView(KWordView_impl *_view)
{
  m_lstViews.append(_view);
}

/*================================================================*/
void KWordDocument_impl::removeView(KWordView_impl *_view)
{
  m_lstViews.setAutoDelete(false);
  m_lstViews.removeRef(_view);
  m_lstViews.setAutoDelete(true);
}

/*================================================================*/
OPParts::View_ptr KWordDocument_impl::createView()
{
  KWordView_impl *p = new KWordView_impl(0L);
  p->setDocument(this);
  
  return OPParts::View::_duplicate(p);
}

/*================================================================*/
void KWordDocument_impl::insertObject(const QRect& _rect,const char *_server_name)
{
  OPParts::Document_var doc = imr_createDocByServerName(_server_name);
  if (CORBA::is_nil(doc)) return;

  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KWord Error"),i18n("Could not init"),i18n("Ok"));
      return;
    }

  KWordChild* ch = new KWordChild(this,_rect,doc);

  insertChild(ch);
}

/*================================================================*/
void KWordDocument_impl::insertChild(KWordChild *_child)
{
  m_lstChildren.append(_child);
  
  emit sig_insertObject(_child);
}

/*================================================================*/
void KWordDocument_impl::changeChildGeometry(KWordChild *_child,const QRect& _rect)
{
  _child->setGeometry(_rect);

  emit sig_updateChildGeometry(_child);
}

/*================================================================*/
QListIterator<KWordChild> KWordDocument_impl::childIterator()
{
  return QListIterator<KWordChild> (m_lstChildren);
}

/*================================================================*/
void KWordDocument_impl::draw(QPaintDevice* _dev,CORBA::Long _width,CORBA::Long _height)
{
}

/*================================================================*/
KWUserFont* KWordDocument_impl::findUserFont(char* _userfontname)
{
  KWUserFont* font;
  for (font = userFontList.first();font != 0L;font = userFontList.next())
    if (strcmp(font->getFontName(),_userfontname) == 0)
      return font;
  
  return 0L;
}

/*================================================================*/
KWDisplayFont* KWordDocument_impl::findDisplayFont(KWUserFont* _font,unsigned int _size,int _weight,bool _italic)
{
  KWDisplayFont* font;
  for (font = displayFontList.first();font != 0L;font = displayFontList.next())
    {
      if (font->getUserFont() == _font && font->getPTSize() == _size &&
	  font->weight() == _weight && font->italic() == _italic)
	return font;
    }
  
  return 0L;
}

/*================================================================*/
KWParagLayout* KWordDocument_impl::findParagLayout(const char *_name)
{
  KWParagLayout* p;
  for (p = paragLayoutList.first();p != 0L;p = paragLayoutList.next())
    if (strcmp( p->getName(),_name ) == 0)
      return p;
  
  return 0L;
}


/*================================================================*/
void KWordDocument_impl::calcColumnWidth()
{
  ptColumnWidth = (getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() - getPTColumnSpacing() * (otherLayout.columns - 1)) 
    / otherLayout.columns;
}

/*================================================================*/
bool KWordDocument_impl::isPTYIn(unsigned int _page,unsigned int _ypos)
{
  // Calculate the y-coordinate relative to page '_page'.
  unsigned int y = _ypos - (_page - 1) * getPTPaperHeight();
    
  // Is y inside the printable area of the paper ?
  if (y >= getPTTopBorder() && y <= getPTPaperHeight() - getPTBottomBorder())
    return true;
    
  return false;
}

/*================================================================*/
KWParag* KWordDocument_impl::findFirstParagOfPage(unsigned int _page)
{
  KWParag *p = parags;
  while (p)
    {
      if (p->getStartPage() == _page)
	return p;
      p = p->getNext();
    }
  
  return 0L;
}

/*================================================================*/
void KWordDocument_impl::printLine(KWFormatContext &_fc,QPainter &_painter,int xOffset,int yOffset)
{
  // Shortcut to the text memory segment
  const char *text = _fc.getParag()->getText();
  // Shortcut to the current paragraph layout
  KWParagLayout *lay = _fc.getParag()->getParagLayout();
  // Index in the text memory segment that points to the line start
  unsigned int pos = _fc.getLineStartPos();

  // First line ? Draw the counter ?
  if (pos == 0 && lay->getCounterNr() != -1)
    {
      KWFormat counterfm(_fc);
      counterfm.apply(lay->getCounterFormat());
      _painter.setFont(*(counterfm.loadFont(this,_painter)));
      _painter.setPen(counterfm.getColor());

      _painter.drawText(_fc.getPTCounterPos() - xOffset, 
			_fc.getPTY() + _fc.getPTMaxAscender() - yOffset, _fc.getCounterText());
	
    }
    
  // paint it character for character. Provisionally! !!HACK!!
  _fc.cursorGotoLineStart(_painter);

  char buffer[200];
  int i=0;
  unsigned int tmpPTPos=0;
  while (!_fc.isCursorAtLineEnd())
    {
      buffer[i] = text[_fc.getTextPos()];
      if (i == 0){
	tmpPTPos = _fc.getPTPos();
	_painter.setFont(*_fc.loadFont(this,_painter));
	_painter.setPen(_fc.getColor());
      }
      
      i++;
      if (_fc.cursorGotoNextChar(_painter) || 
	  _fc.getParag()->getText()[_fc.getTextPos()] == ' '
	  || i >= 199)
	{
	  // there was a blank _or_ there will be a font switch next, so print 
	  // what we have so far
	  buffer[i] = '\0';
	  _painter.drawText(tmpPTPos - xOffset,_fc.getPTY() + _fc.getPTMaxAscender() - yOffset,
			    buffer);
	  i = 0;
	  // Blanks are not printed at all
	  if (_fc.getParag()->getText()[_fc.getTextPos()] == ' ')
	    _fc.cursorGotoNextChar(_painter);
	}
    }
}

/*================================================================*/
void KWordDocument_impl::drawMarker(KWFormatContext &_fc,QPainter *_painter,int xOffset,int yOffset)
{
  RasterOp rop = _painter->rasterOp();
    
  _painter->setRasterOp(NotROP);
  QPen pen;
  pen.setWidth(2);
  _painter->setPen(pen);
  
  _painter->drawLine(_fc.getPTPos() - xOffset,
		     _fc.getPTY() + _fc.getPTMaxAscender() - _fc.getPTAscender() - yOffset,
		     _fc.getPTPos() - xOffset,
		     _fc.getPTY() + _fc.getPTMaxAscender() + _fc.getPTDescender() - yOffset);
  
  _painter->setRasterOp(rop);
}

/*================================================================*/
void KWordDocument_impl::updateAllViews(KWordView_impl *_view)
{
  KWordView_impl *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	if (viewPtr != _view) viewPtr->getGUI()->getPaperWidget()->repaint(false);
    }
}

/*================================================================*/
void KWordDocument_impl::updateAllRanges()
{
  KWordView_impl *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	viewPtr->getGUI()->setRanges();
    }
}


