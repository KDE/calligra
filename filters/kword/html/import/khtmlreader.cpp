/***************************************************************************
                          khtmlreader.cpp  -  description
                             -------------------
    begin                : Sun Sep 9 2001
    copyright            : (C) 2001 by Frank Dekervel
    email                : Frank.Dekervel@student.kuleuven.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU lesser General Public License as        *
 *   published by                                                          *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "khtmlreader.h"

#include "khtmlreader.moc"
#include <kdebug.h>
#include <dom/dom_text.h>
#include <dom/dom2_views.h>
#include <dom/dom_doc.h>
#include <qcolor.h>
#include <dom/dom_element.h>
#include <dom/html_table.h>
#include <khtmlview.h>
#include <qwidget.h>
#include <kapplication.h>
#include <dom/html_misc.h>


KHTMLReader::KHTMLReader(KWDWriter *w){
	_html=new KHTMLPart();
	_writer=w;
	_it_worked=false;
}

// if a troll ever sees this, he can't kill me anyway. (and he should kill dfaure first)
void qt_enter_modal( QWidget *widget );
void qt_leave_modal( QWidget *widget );


bool KHTMLReader::filter(KURL url) {
	kdDebug(30503) << "KHTMLReader::filter" << endl;
	QObject::connect(_html,SIGNAL(completed()),this,SLOT(completed()));

	_state.clear();
	_list_depth=0;

	_html->view()->resize(600,530);
	_html->setAutoloadImages(false);
	_html->setJScriptEnabled(false);
	_html->setPluginsEnabled(false);
	_html->setJavaEnabled(false);
	if (_html->openURL(url) == false) {
		kdWarning(30503) << "openURL returned false" << endl;
		return false;
	}

	//FIXME use synchronous IO instead of this hack if possible.
	QWidget dummy(0,0,WType_Dialog | WShowModal);
	qt_enter_modal(&dummy);
	qApp->enter_loop();
	qt_leave_modal(&dummy);
	return _it_worked;
}

HTMLReader_state *KHTMLReader::state() {
	if (_state.count() == 0) {
		HTMLReader_state *s=new HTMLReader_state;
		s->frameset=_writer->mainFrameset();
		s->paragraph = _writer->addParagraph(s->frameset);
		s->format=_writer->currentFormat(s->paragraph,true);
		s->layout=_writer->currentLayout(s->paragraph);
		_state.push(s);
	}
	return _state.top();
}

HTMLReader_state *KHTMLReader::pushNewState() {
        HTMLReader_state *s=new HTMLReader_state;
        s->frameset=state()->frameset;
        s->paragraph=state()->paragraph;
        s->format=state()->format;
        s->layout=state()->layout;
        _writer->cleanUpParagraph(s->paragraph);
        _state.push(s);
        return s;
}


void KHTMLReader::popState() {

	HTMLReader_state *s=_state.pop();

	/**
	   the recursion trough html is somewhat clumsy still, i'm working on a better method.
	   popState gets called when a tag is closed, but since a closed tag doesn't mean the end
	   of a (kword) "tag" we have to copy some things over from the closed tag:
	   	- the paragraph (after a </B>, we still are in the same paragraph, but
			inside the <B></B> , there might have been a <BR>)
	   if we go back into another frameset, we start a new paragraph.
	 **/
	if (s->frameset == state()->frameset)
		{
			state()->paragraph=s->paragraph;
			if ((state()->layout != s->layout)) {
				startNewLayout(false,state()->layout);
			}
		state()->format=_writer->startFormat(state()->paragraph, state()->format);
	}
	delete(s);
}

void KHTMLReader::startNewLayout(bool startNewFormat) {
	QDomElement layout;
	startNewLayout(startNewFormat,layout);
}

void KHTMLReader::startNewLayout(bool startNewFormat, QDomElement layout) {
	if (!(_writer->getText(state()->paragraph).isEmpty())) {
		startNewParagraph(startNewFormat,true);
	}
	state()->layout=_writer->setLayout(state()->paragraph,layout);
}


void KHTMLReader::completed() {
	kdDebug(30503) << "KHTMLReader::completed" << endl;
        qApp->exit_loop();
	DOM::Document doc=_html->document(); // FIXME parse <HEAD> too
	DOM::NodeList list=doc.getElementsByTagName("body");
	DOM::Node docbody=list.item(0);

	if (docbody.isNull()) {
		kdWarning(30503) << "no <BODY>, giving up" << endl;
		_it_worked=false;
		return;
	}


	parseNode(docbody);

	list = doc.getElementsByTagName("head");
	DOM::Node dochead=list.item(0);
	if (!dochead.isNull())
		parse_head(dochead);
	else
		kdWarning(30503) << "WARNING: no html <HEAD> section" << endl;

	_writer->cleanUpParagraph(state()->paragraph);
        _it_worked=_writer->writeDoc();
}


void KHTMLReader::parseNode(DOM::Node node) {

        // check if this is a text node.
	DOM::Text t=node;
	if (!t.isNull()) {
	   _writer->addText(state()->paragraph,t.data().string());
	   return; // no children anymore...
	}

	// is this really needed ? it can't do harm anyway.
	state()->format=_writer->currentFormat(state()->paragraph,true);
	state()->layout=_writer->currentLayout(state()->paragraph);
	pushNewState();

	DOM::Element e=node;

	bool go_recursive=true;

	if (!e.isNull()) {
                // get the CSS information
                parseStyle(e);
	        // get the tag information
	        go_recursive=parseTag(e);
	}
	if (go_recursive) {
		for (DOM::Node q=node.firstChild(); !q.isNull(); q=q.nextSibling()) {
			parseNode(q);
		}
	} 
	popState();


}

void KHTMLReader::parse_head(DOM::Element e) {
	for (DOM::Element items=e.firstChild();!items.isNull();items=items.nextSibling()) {
		if (items.tagName().string().lower() == "title") {
			DOM::Text t=items.firstChild();
			if (!t.isNull()) {
				_writer->createDocInfo("HTML import filter",t.data().string());
			}
		}
	}
}

#define _PP(x) { \
	if (e.tagName().lower() == #x) \
		return parse_##x(e); \
	}

#define _PF(x,a,b,c) { \
	if (e.tagName().lower() == #x) \
	 	{ \
			 _writer->formatAttribute(state()->paragraph, #a,#b,#c); \
			 return true; \
		} \
	}

// the state->layout=_writer->setLayout is meant to tell popState something changed in the layout, and a new
// layout should probably be started after closing.

#define _PL(x,a,b,c) { \
		if (e.tagName().lower() == #x) \
			{ \
				state()->layout=_writer->setLayout(state()->paragraph,state()->layout);\
				if (!(_writer->getText(state()->paragraph).isEmpty())) \
					startNewParagraph(false,false); \
				_writer->layoutAttribute(state()->paragraph, #a,#b,#c); \
				return true; \
			} \
		}


bool KHTMLReader::parseTag(DOM::Element e) {
	_PP(p);
	_PP(br);
	// _PP(table); <- disabled for beta.
	_PP(pre);
	_PP(ul);
	_PP(ol);
	_PP(font);
	_PP(hr);

	// FIXME we can get rid of these, make things tons more simple
	// when khtml finally implements getComputedStyle
	_PF(b,WEIGHT,value,75);
	_PF(strong,WEIGHT,value,75);
	_PF(u,UNDERLINE,value,1);
	_PF(i,ITALIC,value,1);

	_PL(center,FLOW,align,center);
	_PL(right,FLOW,align,right);
	_PL(left,FLOW,align,left);

	_PL(h1,NAME,value,h1);
	_PL(h2,NAME,value,h2);
	_PL(h3,NAME,value,h3);
	_PL(h4,NAME,value,h4);
	_PL(h5,NAME,value,h5);
	_PL(h6,NAME,value,h6);
	return true;
}



void KHTMLReader::parseStyle(DOM::Element e) {
  // styles are broken broken broken broken broken broken.
  //FIXME: wait until getComputedStyle is more than
  // 'return 0' in khtml
  kdDebug() << "entering parseStyle" << endl;
     DOM::CSSStyleDeclaration s1=e.style();
     DOM::Document doc=_html->document();
     DOM::CSSStyleDeclaration s2=doc.defaultView().getComputedStyle(e,"");

     kdDebug() << "font-weight=" << s1.getPropertyValue("font-weight").string();
     if ( s1.getPropertyValue("font-weight").string() == "bolder" )
     {
	_writer->formatAttribute(state()->paragraph,"WEIGHT","value","75");
     }
     if ( s1.getPropertyValue("font-weight").string() == "bold" )
     {
	_writer->formatAttribute(state()->paragraph,"WEIGHT","value","75");
     }
     /*if (DOM::PROPV("font-weight") == "bolder")
	_writer->formatAttribute(state()->paragraph,"WEIGHT","value","75");
     if (PROPV("font-weight") == "bold")
	_writer->formatAttribute(state()->paragraph,"WEIGHT","value","75");
/*
     // debugging code.
     kdDebug(30503) << "e.style()" << endl;
     for (unsigned int i=0;i<s1.length();i++) {
        kdDebug(30503) << QString("%1: %2").arg(s1.item(i).string()).arg(s1.getPropertyValue(s1.item(i)).string()) << endl;
     }
     kdDebug(30503) << "override style" << endl;
     for (unsigned int i=0;i<s2.length();i++) {
        kdDebug(30503) << QString("%1: %2").arg(s2.item(i).string()).arg(s2.getPropertyValue(s2.item(i)).string()) << endl;
     }
*/
}

void KHTMLReader::startNewParagraph(bool startnewformat, bool startnewlayout) {

	QDomElement qf=state()->format;
	QDomElement ql=state()->layout;

	_writer->cleanUpParagraph(state()->paragraph);

        if ((startnewlayout==true) || ql.isNull())
        	{state()->paragraph=_writer->addParagraph(state()->frameset);}
        else
	        {state()->paragraph=
	        	_writer->addParagraph(state()->frameset,state()->layout);}



        if (qf.isNull() || (startnewformat==true)) {
	        state()->format=_writer->startFormat(state()->paragraph/*,state()->format*/);
	}  else {
		state()->format=_writer->startFormat(state()->paragraph,qf);
	}

	/**
	  support lists: if we are in a list, and we start a new paragraph,
	  we don't want to start a new item, but we want to retain the list state.
	  we do this by incrementing the 'environment depth' and changing the numbering type to 'no numbering'
	 **/
	QString ct=_writer->getLayoutAttribute(state()->paragraph,"COUNTER","type");
	if ((!ct.isNull()) && (ct != "0")) {
		_writer->layoutAttribute(state()->paragraph,"COUNTER","type","0");
		_writer->layoutAttribute(state()->paragraph,"COUNTER","numberingtype","0");
		_writer->layoutAttribute(state()->paragraph,"COUNTER","righttext","");
		int currdepth=(_writer->getLayoutAttribute(state()->paragraph,"COUNTER","depth")).toInt();
		_writer->layoutAttribute(state()->paragraph,"COUNTER","depth",QString("%1").arg(currdepth+1));
	}
}

KHTMLReader::~KHTMLReader(){
}






//==============================================================
//                          tag parsing
//==============================================================


bool KHTMLReader::parse_CommonAttributes(DOM::Element e) {
        QString s=e.getAttribute("align").string();
        if (!s.isEmpty()) {
              _writer->formatAttribute(state()->paragraph,"FLOW","align",s);
        }
        return true;
}



bool KHTMLReader::parse_p(DOM::Element e) {
	startNewParagraph();
	parse_CommonAttributes(e);

	return true;
}

bool KHTMLReader::parse_hr(DOM::Element e) {
	startNewParagraph();
	_writer->createHR(state()->paragraph);
	startNewParagraph();
	return true;
}

bool KHTMLReader::parse_br(DOM::Element e) {
	startNewParagraph(false,false); //keep the current format and layout
	return false; // a BR tag has no childs.
}

QColor parsecolor(QString colorstring) {
      QColor color;
      if (colorstring[0]=='#') {
            color.setRgb(
            colorstring.mid(1,2).toInt(0,16),
            colorstring.mid(3,2).toInt(0,16),
            colorstring.mid(5,2).toInt(0,16)
            );
      } else {
            QString colorlower=colorstring.lower();
            // Grays
            if (colorlower=="black")
                  color.setRgb(0,0,0);
            else if (colorlower=="white")
                  color.setRgb(255,255,255);
            else if (colorlower=="silver")
                  color.setRgb(0xc0,0xc0,0xc0);
            else if (colorlower=="gray")
                  color.setRgb(128,128,128);
            // "full" colors
            else if (colorlower=="red")
                  color.setRgb(255,0,0);
            else if (colorlower=="lime")
                  color.setRgb(0,255,0);
            else if (colorlower=="blue")
                  color.setRgb(0,0,255);
            else if (colorlower=="yellow")
                  color.setRgb(255,255,0);
            else if (colorlower=="fuchsia")
                  color.setRgb(255,0,255);
            else if (colorlower=="aqua")
                  color.setRgb(0,255,255);
            // "half" colors
            else if (colorlower=="maroon")
                  color.setRgb(128,0,0);
            else if (colorlower=="green")
                  color.setRgb(0,128,0);
            else if (colorlower=="navy")
                  color.setRgb(0,0,128);
            else if (colorlower=="olive")
                  color.setRgb(128,128,0);
            else if (colorlower=="purple")
                  color.setRgb(128,0,128);
            else if (colorlower=="teal")
                  color.setRgb(0,128,128);
            else {
                  // H'm, we have still not found the color!
                  // Let us see if QT can do better!
                  color.setNamedColor(colorstring);
            }
      }
      return colorstring;
}


bool KHTMLReader::parse_table(DOM::Element e) {
	int tableno=_writer->createTable();
 	int nrow=0;
 	int ncol=0;
 	int has_borders=false;
	QColor bgcolor=parsecolor("#FFFFFF");
 	DOM::Element table_body=e.firstChild();
 	if (!table_body.getAttribute("bgcolor").string().isEmpty())
 	       bgcolor=parsecolor(table_body.getAttribute("bgcolor").string());
 	if ((e.getAttribute("border").string().toInt() > 0))
 		has_borders=true;

 	// fixme rewrite this proper
 	//(maybe using computed sizes from khtml if thats once exported)
 	for (DOM::Element rows=table_body.firstChild();!rows.isNull();rows=rows.nextSibling()) {
 	if (rows.tagName().string().lower() == "tr") {

 	    QColor obgcolor=bgcolor;
 	    if (!rows.getAttribute("bgcolor").string().isEmpty())
 	       	bgcolor=parsecolor(rows.getAttribute("bgcolor").string());

 		ncol=0;
 		for (DOM::Element cols=rows.firstChild();!cols.isNull();cols=cols.nextSibling()) {
 		        if (cols.tagName().string().lower() == "td") {
 		             QColor bbgcolor=bgcolor;
		 	    if (!cols.getAttribute("bgcolor").string().isEmpty())
 	       			bgcolor=parsecolor(cols.getAttribute("bgcolor").string());

			    	pushNewState();
	 	    	    	QRect colrect=cols.getRect();
	 	    	    	state()->frameset=_writer->createTableCell(tableno,nrow,ncol,1,colrect);
	 	    	     	state()->frameset.firstChild().toElement().setAttribute("bkRed",bgcolor.red());
	 	    	     	state()->frameset.firstChild().toElement().setAttribute("bkGreen",bgcolor.green());
	 	    	     	state()->frameset.firstChild().toElement().setAttribute("bkBlue",bgcolor.blue());
	 	    	     	if (has_borders) {
	 	    	     	    state()->frameset.firstChild().toElement().setAttribute("lWidth",1);
	 	    	     	    state()->frameset.firstChild().toElement().setAttribute("rWidth",1);
	 	    	     	    state()->frameset.firstChild().toElement().setAttribute("bWidth",1);
	 	    	     	    state()->frameset.firstChild().toElement().setAttribute("tWidth",1);
	 	    	     	}

				// fixme don't guess. get it right.
	 	    	    	state()->paragraph=_writer->addParagraph(state()->frameset);
	 	    	    	parseNode(cols);
				_writer->cleanUpParagraph(state()->paragraph);
 			    	popState();
 				ncol++;
 				bgcolor=bbgcolor;
 			}
 		}
 		nrow++;
 	  	bgcolor=obgcolor;
          }
 	}
   	_writer->finishTable(tableno/*,0,0,r.right()-r.left(),r.bottom()-r.top()*/); // FIXME find something better.
 	startNewParagraph(false,false);
 	_writer->createInline(state()->paragraph,_writer->fetchTableCell(tableno,0,0));
	startNewParagraph(false,false);
	return false; // we do our own recursion
}

bool KHTMLReader::parse_img(DOM::Element e) {
     	//QRect e=e.getRect();
    return true;
}


bool KHTMLReader::parse_pre(DOM::Element e) {
	DOM::Text prething=e.firstChild();
	if (prething.isNull()) return false;

	QStringList k=QStringList::split("\n",prething.data().string());

	startNewParagraph();

	for (QStringList::Iterator b=k.begin();b!=k.end();++b) {
		_writer->addText(state()->paragraph,*b);
		startNewParagraph();
	}
	return false; // FIXME no support for tags in <PRE> sections ATM.
}

bool KHTMLReader::parse_ol(DOM::Element e) {
	return parse_ul(e);
}

bool KHTMLReader::parse_font(DOM::Element e) {
	// fixme don't hardcode 12 font size ...
	QString face=e.getAttribute("face").string();
        QColor color=parsecolor("#000000");
        if (!e.getAttribute("color").string().isEmpty())
        	color=parsecolor(e.getAttribute("color").string());
        QString size=e.getAttribute("size").string();
        int isize=-1;
        if (size.startsWith("+"))
        	isize=12+size.right(size.length()-1).toInt();
        else if (size.startsWith("-"))
        	isize=12-size.right(size.length()-1).toInt();
        else
	        isize=12+size.toInt();

        _writer->formatAttribute(state()->paragraph,"FONT","name",face);
        if ((isize>=0) && (isize != 12))
        	_writer->formatAttribute(state()->paragraph,"SIZE","value",QString("%1").arg(isize));

        _writer->formatAttribute(state()->paragraph,"COLOR","red",QString("%1").arg(color.red()));
        _writer->formatAttribute(state()->paragraph,"COLOR","green",QString("%1").arg(color.green()));
        _writer->formatAttribute(state()->paragraph,"COLOR","blue",QString("%1").arg(color.blue()));
        return true;
}

bool KHTMLReader::parse_ul(DOM::Element e) {
        _list_depth++;
        for (DOM::Element items=e.firstChild();!items.isNull();items=items.nextSibling()) {
                  if (items.tagName().string().lower() == "li") {
                  	pushNewState();
                  		startNewLayout();
                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","numberingtype","1");
                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","righttext",".");
                  		if (e.tagName().string().lower() == "ol")
	                  		{
	                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","type","1");
	                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","numberingtype","1");
        	          		_writer->layoutAttribute(state()->paragraph,"COUNTER","righttext",".");
	                  		}
	                  	else
	                  		{
	                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","type","10");
	                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","numberingtype","");
        	          		_writer->layoutAttribute(state()->paragraph,"COUNTER","righttext","");
	                  		}
                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","depth",QString("%1").arg(_list_depth-1));
                  		parseNode(items);
                  	popState();

                  }
        }
        _list_depth--;
	return false;
}

