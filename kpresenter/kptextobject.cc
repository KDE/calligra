/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
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
/* Module: ktextobject object                                     */
/******************************************************************/

#include "kptextobject.h"
#include "kptextobject.moc"

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPTextObject::KPTextObject()
  : KPObject(), ktextobject(0,"")
{
  ktextobject.hide();
  ktextobject.setAutoFocus(true);
}

/*======================= set size ===============================*/
void KPTextObject::setSize(int _width,int _height)
{
  KPObject::setSize(_width,_height);
  ktextobject.resize(ext);
}

/*======================= set size ===============================*/
void KPTextObject::resizeBy(int _dx,int _dy)
{
  KPObject::resizeBy(_dx,_dy);
  ktextobject.resize(ext);
}

/*========================= save =================================*/
void KPTextObject::save(ostream& out)
{
  out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
  out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
  out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
      << shadowDirection << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
      << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
  out << indent << "<EFFECTS effect=\"" << static_cast<int>(effect) << "\" effect2=\"" 
      << static_cast<int>(effect2) << "\"/>" << endl;
  out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
  out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
  saveKTextObject(out);
}

/*========================== load ================================*/
void KPTextObject::load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
      // orig
      if (name == "ORIG")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "x")
		orig.setX(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "y")
		orig.setY(atoi((*it).m_strValue.c_str()));
	    }
	}

      // size
      else if (name == "SIZE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "width")
		ext.setWidth(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "height")
		ext.setHeight(atoi((*it).m_strValue.c_str()));
	    }
	}

      // shadow
      else if (name == "SHADOW")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "distance")
		shadowDistance = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "direction")
		shadowDirection = (ShadowDirection)atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "red")
		shadowColor.setRgb(atoi((*it).m_strValue.c_str()),
				   shadowColor.green(),shadowColor.blue());
	      if ((*it).m_strName == "green")
		shadowColor.setRgb(shadowColor.red(),atoi((*it).m_strValue.c_str()),
				   shadowColor.blue());
	      if ((*it).m_strName == "blue")
		shadowColor.setRgb(shadowColor.red(),shadowColor.green(),
				   atoi((*it).m_strValue.c_str()));
	    }
	}

      // effects
      else if (name == "EFFECTS")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "effect")
		  effect = (Effect)atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "effect2")
		  effect2 = (Effect2)atoi((*it).m_strValue.c_str());
	    }
	}
      
      // angle
      else if (name == "ANGLE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		angle = atof((*it).m_strValue.c_str());
	    }
	}

      // presNum
      else if (name == "PRESNUM")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		presNum = atoi((*it).m_strValue.c_str());
	    }
	}

      // KTextObject
      else if (name == "TEXTOBJ")
	{
	  ktextobject.clear(false);
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "objType")
		ktextobject.setObjType((KTextObject::ObjType)atoi((*it).m_strValue.c_str()));
	      else
		cerr << "Unknown attrib TEXTOBJ:'" << (*it).m_strName << "'" << endl;
	    }
	  loadKTextObject(parser,lst);
	}

      else
	cerr << "Unknown tag '" << tag << "' in TEXT_OBJECT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
  setSize(ext.width(),ext.height());
}

/*========================= draw =================================*/
void KPTextObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();
  QRect r;

  QRect cr = getBoundingRect(_diffx,_diffy);
  int _x = cr.x(), _y = cr.y(), _w = cr.width(), _h = cr.height();

  QPicture *pic;

  _painter->save();
  r = _painter->viewport();

  if (shadowDistance > 0)
    {
      _painter->save();
      ktextobject.enableDrawAllInOneColor(shadowColor);
		    
      if (specEffects)
	{
	  switch (effect2)
	    {
	    case EF2T_PARA:
	      pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,subPresStep,subPresStep,ownClipping);
	      break;
	    default:
	      pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,-1,-1,ownClipping);
	    }
	}
      else
	pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,-1,-1,ownClipping);
      
      ktextobject.disableDrawAllInOneColor();
      
      if (angle == 0)
	{
	  int sx = ox;
	  int sy = oy;
	  getShadowCoords(sx,sy,shadowDirection,shadowDistance);

	  _painter->setViewport(sx,sy,r.width(),r.height());
			
	  _painter->drawPicture(*pic);
	}
      else
	{
	  _painter->setViewport(ox,oy,r.width(),r.height());
	  
	  QRect br = QRect(0,0,ow,oh);
	  int pw = br.width();
	  int ph = br.height();
	  QRect rr = br;
	  int yPos = -rr.y();
	  int xPos = -rr.x();
	  br.moveTopLeft(QPoint(-br.width() / 2,-br.height() / 2));
	  rr.moveTopLeft(QPoint(-rr.width() / 2,-rr.height() / 2));
			
	  QWMatrix m,mtx;
	  mtx.rotate(angle);
	  m.translate(pw / 2,ph / 2);
	  m = mtx * m;
			
	  _painter->setWorldMatrix(m);

	  int sx = 0;
	  int sy = 0;
	  getShadowCoords(sx,sy,shadowDirection,shadowDistance);
	  
	  _painter->translate(rr.left() + xPos + sx,rr.top() + yPos + sy);
			
	  _painter->drawPicture(*pic);
	}
      _painter->restore();
    }
  
  if (specEffects)
    {
      switch (effect2)
	{
	case EF2T_PARA:
	  pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,subPresStep,subPresStep,ownClipping);
	  break;
	default:
	  pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,-1,-1,ownClipping);
	}
    }
  else
    pic = ktextobject.getPic(_x,_y,_w,_h,zoomed,-1,-1,ownClipping);
  
  _painter->setViewport(ox,oy,r.width(),r.height());
  
  if (angle == 0)
    _painter->drawPicture(*pic);
  else
    {
      QRect br = QRect(0,0,ow,oh);
      int pw = br.width();
      int ph = br.height();
      QRect rr = br;
      int yPos = -rr.y();
      int xPos = -rr.x();
      br.moveTopLeft(QPoint(-br.width() / 2,-br.height() / 2));
      rr.moveTopLeft(QPoint(-rr.width() / 2,-rr.height() / 2));
      
      QWMatrix m,mtx;
      mtx.rotate(angle);
      m.translate(pw / 2,ph / 2);
      m = mtx * m;
      
      _painter->setWorldMatrix(m);
      _painter->translate(rr.left() + xPos,rr.top() + yPos);
      
      _painter->drawPicture(*pic);
    }
  
  _painter->setViewport(r);
  _painter->restore();
  
  KPObject::draw(_painter,_diffx,_diffy);
}

/*========================== activate ============================*/
void KPTextObject::activate(QWidget *_widget,int diffx,int diffy)
{
  ktextobject.recreate(_widget,0,QPoint(orig.x() - diffx,orig.y() - diffy),false);
  ktextobject.resize(ext.width(),ext.height());
  ktextobject.show();
  ktextobject.setCursor(ibeamCursor);
  ktextobject.setAutoFocus(true);
}

/*========================== deactivate ==========================*/
void KPTextObject::deactivate()
{
  ktextobject.recreate(0,0,QPoint(0,0),false);
  ktextobject.hide();
}

/*========================= zoom =================================*/
void KPTextObject::zoom(float _fakt)
{
  KPObject::zoom(_fakt);
  ktextobject.resize(ext);
  ktextobject.zoom(_fakt);
}

/*==================== zoom orig =================================*/
void KPTextObject::zoomOrig()
{
  KPObject::zoomOrig();
  ktextobject.resize(ext);
  ktextobject.zoomOrig();
}

/*=========================== save ktextobject ===================*/
void KPTextObject::saveKTextObject(ostream& out)
{
  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QFont font;

  out << otag << "<TEXTOBJ objType=\"" << ktextobject.objType() << "\">" << endl;
  out << indent << "<ENUMLISTTYPE type=\"" << ktextobject.enumListType().type << "\" before=\""
      << ktextobject.enumListType().before << "\" after=\"" << ktextobject.enumListType().after
      << "\" start=\"" << ktextobject.enumListType().start << "\" family=\"" 
      << ktextobject.enumListType().font.family() << "\" pointSize=\"" << ktextobject.enumListType().font.pointSize()
      << "\" bold=\"" << ktextobject.enumListType().font.bold() << "\" italic=\"" << ktextobject.enumListType().font.italic()
      << "\" underline=\"" << ktextobject.enumListType().font.underline() << "\" red=\"" 
      << ktextobject.enumListType().color.red() << "\" green=\"" << ktextobject.enumListType().color.green() 
      << "\" blue=\"" << ktextobject.enumListType().color.blue() << "\"/>" << endl; 
  out << indent << "<UNSORTEDLISTTYPE type=\"" << ktextobject.enumListType().type << "\" family=\"" 
      << ktextobject.unsortListType().font.family() << "\" pointSize=\"" << ktextobject.unsortListType().font.pointSize()
      << "\" bold=\"" << ktextobject.unsortListType().font.bold() << "\" italic=\"" << ktextobject.unsortListType().font.italic()
      << "\" underline=" << ktextobject.unsortListType().font.underline() << " red=\"" 
      << ktextobject.unsortListType().color.red() << "\" green=\"" << ktextobject.unsortListType().color.green() 
      << "\" blue=\"" << ktextobject.unsortListType().color.blue() << "\" chr=\"" << ktextobject.unsortListType().chr
      << "\"/>" << endl; 

  for (i = 0;i < ktextobject.paragraphs();i++)
    {
      txtParagraph = ktextobject.paragraphAt(i);

      out << otag << "<PARAGRAPH horzAlign=\"" << txtParagraph->horzAlign() << "\">" << endl; 

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);

	  out << otag << "<LINE>" << endl;

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      font = txtObj->font();
	      
	      out << otag << "<OBJ>" << endl;
	      out << indent << "<TYPE value=\"" << txtObj->type() << "\"/>" << endl;
	      out << indent << "<FONT family=\"" << font.family() << "\" pointSize=\""
		  << font.pointSize() << "\" bold=\"" << font.bold() << "\" italic=\"" << font.italic()
		  << "\" underline=\"" << font.underline() << "\"/>" << endl;
	      out << indent << "<COLOR red=\"" << txtObj->color().red() << "\" green=\""
		  << txtObj->color().green() << "\" blue=\"" << txtObj->color().blue() << "\"/>" << endl;
	      out << indent << "<VERTALIGN value=\"" << txtObj->vertAlign() << "\"/>" << endl;
	      out << indent << "<TEXT value=\"" << txtObj->text() << "\"/>" << endl;
	      out << etag << "</OBJ>" << endl;
	    }

	  out << etag << "</LINE>" << endl;

	}
      
      out << etag << "</PARAGRAPH>" << endl;

    }

  out << etag << "</TEXTOBJ>" << endl;
}

/*====================== load ktextobject ========================*/
void KPTextObject::loadKTextObject(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  KTextObject::EnumListType elt;
  KTextObject::UnsortListType ult;
  QFont font;
  QColor color;
  int r = 0,g = 0,b = 0;
  TxtParagraph *txtParagraph;
  TxtObj *objPtr;
  
  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // enumListType
      if (name == "ENUMLISTTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "type")
		elt.type = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "before")
		elt.before = (*it).m_strValue.c_str();
	      if ((*it).m_strName == "after")
		elt.after = (*it).m_strValue.c_str();
	      if ((*it).m_strName == "start")
		elt.start = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "family")
		font.setFamily((*it).m_strValue.c_str());
	      if ((*it).m_strName == "pointSize")
		font.setPointSize(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "bold")
		font.setBold((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "italic")
		font.setItalic((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "underline")
		font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "red")
		r = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "green")
		g = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "blue")
		b = atoi((*it).m_strValue.c_str());
	    }
	  color.setRgb(r,g,b);
	  elt.font = font;
	  elt.color = color;
	  ktextobject.setEnumListType(elt);
	}
      
      // unsortListType
      else if (name == "UNSORTEDLISTTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "chr")
		ult.chr = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "family")
		font.setFamily((*it).m_strValue.c_str());
	      if ((*it).m_strName == "pointSize")
		font.setPointSize(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "bold")
		font.setBold((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "italic")
		font.setItalic((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "underline")
		font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "red")
		r = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "green")
		g = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "blue")
		b = atoi((*it).m_strValue.c_str());
	    }
	  color.setRgb(r,g,b);
	  ult.font = font;
	  ult.color = color;
	  ktextobject.setUnsortListType(ult);
	}

      // paragraph
      else if (name == "PARAGRAPH")
	{
	  txtParagraph = ktextobject.addParagraph();
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "horzAlign")
		txtParagraph->setHorzAlign((TxtParagraph::HorzAlign)atoi((*it).m_strValue.c_str()));
	    }

	  while (parser.open(0L,tag))
	    {
	      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
	      // line
	      if (name == "LINE")
		{
		  while (parser.open(0L,tag))
		    {
		      KOMLParser::parseTag(tag.c_str(),name,lst);
		      
		      // object
		      if (name == "OBJ")
			{
			  objPtr = new TxtObj();

			  while (parser.open(0L,tag))
			    {
			      KOMLParser::parseTag(tag.c_str(),name,lst);
			      
			      // type
			      if (name == "TYPE")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->setType((TxtObj::ObjType)atoi((*it).m_strValue.c_str()));
				    }
				}

			      // font
			      else if (name == "FONT")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "family")
					font.setFamily((*it).m_strValue.c_str());
				      if ((*it).m_strName == "pointSize")
					font.setPointSize(atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "bold")
					font.setBold((bool)atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "italic")
					font.setItalic((bool)atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "underline")
					font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
				    }
				  objPtr->setFont(font);
				}

			      // color
			      else if (name == "COLOR")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "red")
					r = atoi((*it).m_strValue.c_str());
				      if ((*it).m_strName == "green")
					g = atoi((*it).m_strValue.c_str());
				      if ((*it).m_strName == "blue")
					b = atoi((*it).m_strValue.c_str());
				    }
				  color.setRgb(r,g,b);
				  objPtr->setColor(color);
				}

			      // vertical align
			      else if (name == "VERTALIGN")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->setVertAlign((TxtObj::VertAlign)atoi((*it).m_strValue.c_str()));
				    }
				}

			      // text
			      else if (name == "TEXT")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->append((*it).m_strValue.c_str());
				    }
				}

			      else
				cerr << "Unknown tag '" << tag << "' in OBJ" << endl;    
			      
			      if (!parser.close(tag))
				{
				  cerr << "ERR: Closing Child" << endl;
				  return;
				}

			    }
			  txtParagraph->append(objPtr);
			}
		      
		      else
			cerr << "Unknown tag '" << tag << "' in LINE" << endl;    
		      
		      if (!parser.close(tag))
			{
			  cerr << "ERR: Closing Child" << endl;
			  return;
			}
		    }
		}

	      else
		cerr << "Unknown tag '" << tag << "' in PARAGRAPH" << endl;    
	      
	      if (!parser.close(tag))
		{
		  cerr << "ERR: Closing Child" << endl;
		  return;
		}
	    }
	}
	  
      else
	cerr << "Unknown tag '" << tag << "' in TEXTOBJ" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

