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
	QObject::connect(_html,SIGNAL(completed()),this,SLOT(completed()));
	_state.clear();
	_html->begin();
	_list_depth=0;
	_html->view()->resize(700,530); //FIX?
	_html->setAutoloadImages(false);
	_html->openURL(url);
	_html->setAutoloadImages(false);
	QWidget dummy(0,0,WType_Dialog | WShowModal);
	qt_enter_modal(&dummy);
	qApp->enter_loop();
	qt_leave_modal(&dummy);
	return _it_worked;
}

HTMLReader_state *KHTMLReader::state() {
	if (_state.count() == 0) {
		HTMLReader_state *s=new HTMLReader_state;
		s->frameset=_writer->currentFrameset();
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
	state()->paragraph=s->paragraph;
	state()->format=_writer->startFormat(state()->paragraph,state()->format);
	delete(s);
}


void KHTMLReader::completed() {
        qApp->exit_loop();
	DOM::Document doc=_html->document(); // FIXME parse <HEAD> too
	DOM::Node docbody=doc.getElementsByTagName("BODY").item(0);
	parseNode(docbody);
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

	state()->format=_writer->currentFormat(state()->paragraph,true);
	state()->layout=_writer->currentLayout(state()->paragraph);

	pushNewState();

	 //FIXME fix this

	DOM::Element e=node;
	bool go_recursive=true;
	if (!e.isNull()) {
                // get the CSS information
                parseStyle(e);
	        // get the tag information
	        go_recursive=parseTag(e);
	}
	if (go_recursive)
		for (DOM::Node q=node.firstChild(); !q.isNull(); q=q.nextSibling()) {

			parseNode(q);
		}
	popState();

}

#define _PP(x) {if (e.tagName() == #x) return parse_##x(e);}
#define _PF(x,a,b,c) {if (e.tagName() == #x) { _writer->formatAttribute(state()->paragraph, #a,#b,#c); return true;}}
#define _PL(x,a,b,c) {if (e.tagName() == #x) { _writer->layoutAttribute(state()->paragraph, #a,#b,#c); return true;}}

bool KHTMLReader::parseTag(DOM::Element e) {
	_PP(P);
	_PP(BR);
	_PP(TABLE);
	_PP(PRE);
	_PP(UL);
	_PP(OL);
	_PP(FONT);
	_PP(HR);

	// FIXME we can get rid of these, make things tons more simple
	// when khtml finally implements getComputedStyle
	_PF(B,WEIGHT,value,75);
	_PF(STRONG,WEIGHT,value,75);
	_PF(U,UNDERLINE,value,1);
	_PF(I,ITALIC,value,1);

	_PL(CENTER,FLOW,align,center);
	_PL(RIGHT,FLOW,align,right);
	_PL(LEFT,FLOW,align,left);
}



void KHTMLReader::parseStyle(DOM::Element e) {
#if 0  // styles are broken broken broken broken broken broken.
     DOM::CSSStyleDeclaration s1=e.style();
     DOM::Document doc=_html->document();
     DOM::CSSStyleDeclaration s2=doc.defaultView().getComputedStyle(e,"");
     				//FIXME: wait until getComputedStyle is more than
     				// 'return 0' in khtml

     if (PROPV("font-weight") == "bolder")
	_writer->formatAttribute(state()->paragraph,"WEIGHT","value","75");

     // debugging code.
     qWarning("e.style()");
     for (unsigned int i=0;i<s1.length();i++) {
        qWarning(QString("%1: %2").arg(s1.item(i).string()).arg(s1.getPropertyValue(s1.item(i)).string()));
     }
     qWarning("override style");
     for (unsigned int i=0;i<s2.length();i++) {
        qWarning(QString("%1: %2").arg(s2.item(i).string()).arg(s2.getPropertyValue(s2.item(i)).string()));
     }
#endif
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

	QString ct=_writer->getLayoutAttribute(state()->paragraph,"COUNTER","type");
	if ((ct != QString::null) && (ct != "0")) {
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
        if (s != "") {
              _writer->formatAttribute(state()->paragraph,"FLOW","align",s);
        }
        return true;
}



bool KHTMLReader::parse_P(DOM::Element e) {

	startNewParagraph();
	parse_CommonAttributes(e);

	return true;
}

bool KHTMLReader::parse_HR(DOM::Element e) {
	startNewParagraph();
	_writer->addText(state()->paragraph,"--------------------------------FIXME:HR-------------");
	startNewParagraph();
	return true;
}

bool KHTMLReader::parse_BR(DOM::Element e) {
	//HTMLReader_state *s=_state.pop();
		// a BR tag affects something 'up' in the hierarchy

	startNewParagraph(false,false);

	//_state.push(s);
	return false; // a BR tag has no childs.
}

QColor parsecolor(QString colorstring) {
      QColor dummy;
      if (colorstring.upper() == "RED") return QColor(255,0,0);
      if (colorstring.upper() == "BLUE") return QColor(0,0,255);
      if (colorstring.upper() == "GREEN") return QColor(0,255,0);
      if (colorstring.startsWith("#")) {
      return QColor(
            colorstring.mid(1,2).toInt(0,16),
            colorstring.mid(3,2).toInt(0,16),
            colorstring.mid(5,2).toInt(0,16)
            );
      }
      else return dummy;

}


bool KHTMLReader::parse_TABLE(DOM::Element e) {
	int tableno=_writer->createTable();
 	int nrow=0;
 	int ncol=0;
 	int has_borders=false;
	QColor bgcolor=parsecolor("#FFFFFF");
 	DOM::Element table_body=e.firstChild();
 	if (table_body.getAttribute("BGCOLOR").string() != "")
 	       bgcolor=parsecolor(table_body.getAttribute("BGCOLOR").string());
 	if ((e.getAttribute("BORDER").string().toInt() > 0))
 		has_borders=true;

 	// fixme rewrite this proper
 	//(maybe using computed sizes from khtml if thats once exported)
 	for (DOM::Element rows=table_body.firstChild();!rows.isNull();rows=rows.nextSibling()) {
 	if (rows.tagName().string() == "TR") {

 	    QColor obgcolor=bgcolor;
 	    if (rows.getAttribute("BGCOLOR").string() != "")
 	       	bgcolor=parsecolor(rows.getAttribute("BGCOLOR").string());

 		ncol=0;
 		for (DOM::Element cols=rows.firstChild();!cols.isNull();cols=cols.nextSibling()) {
 		        if (cols.tagName().string() == "TD") {
 		             QColor bbgcolor=bgcolor;
		 	    if (cols.getAttribute("BGCOLOR").string() != "")
 	       			bgcolor=parsecolor(cols.getAttribute("BGCOLOR").string());

			    	pushNewState();
	 	    	    	QRect colrect=cols.getRect();
	 	    	    	state()->frameset=_writer->createTableCell(tableno,nrow,ncol,1,0,0,
	 	    	     		colrect.right()-colrect.left(),(colrect.bottom()-colrect.top())/2);
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

bool KHTMLReader::parse_IMG(DOM::Element e) {
     	//QRect e=e.getRect();
    return true;
}


bool KHTMLReader::parse_PRE(DOM::Element e) {
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

bool KHTMLReader::parse_OL(DOM::Element e) {
	return parse_UL(e);
}

bool KHTMLReader::parse_FONT(DOM::Element e) {
	// fixme don't hardcode 12 font size ...
	QString face=e.getAttribute("face").string();
        QColor color=parsecolor("#000000");
        if (e.getAttribute("color").string() != "")
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

bool KHTMLReader::parse_UL(DOM::Element e) {
        _list_depth++;
        for (DOM::Element items=e.firstChild();!items.isNull();items=items.nextSibling()) {
                  if (items.tagName().string() == "LI") {
                  	pushNewState();
                  		startNewParagraph();
                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","numberingtype","1");
                  		_writer->layoutAttribute(state()->paragraph,"COUNTER","righttext",".");
                  		if (e.tagName().string() == "OL")
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

