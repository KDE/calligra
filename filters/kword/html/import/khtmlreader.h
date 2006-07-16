/***************************************************************************
                          khtmlreader.h  -  description
                             -------------------
    begin                : Sun Sep 9 2001
    copyright            : (C) 2001 by Frank Dekervel
    email                : Frank.Dekervel@student.kuleuven.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by                                                          *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KHTMLREADER_H
#define KHTMLREADER_H

#include <khtml_part.h> // fixme speedup compile time by moving these into the cpp file.

#include <qdom.h>
#include <q3ptrstack.h>

#include <kwdwriter.h>

//#include <qstack.h>

/**
  *@author Frank Dekervel
  */

namespace DOM {
 	class Node;
 	class Element;
}

struct HTMLReader_state {
  QDomElement format;
  QDomElement frameset;
  QDomElement paragraph;
  QDomElement layout;
  bool in_pre_mode; // keep formatting as is.
};

class KHTMLReader: public QObject {

Q_OBJECT

public:
	KHTMLReader(KWDWriter *writer);
	~KHTMLReader();
	
	bool filter(KUrl url);

private slots:
	void completed();

private:	
	void parseNode(DOM::Node n);
        bool parseTag(DOM::Element e);
        void parseStyle(DOM::Element e);
	void parse_head(DOM::Element e);
        HTMLReader_state *state();
        HTMLReader_state *pushNewState();
        void popState();


  /*      void stateSetFormat(QDomElement f);
        void stateSetFrameset(QDomElement f);
        void stateSetLayout(QDomElement l);
        void stateSetParagraph(QDomElement p);*/

        Q3PtrStack<HTMLReader_state> _state;
        KHTMLPart *_html;
        KWDWriter *_writer;

	void startNewLayout(bool startNewFormat=false);
	void startNewLayout(bool startNewFormat, QDomElement layout);
        void startNewParagraph(bool startnewformat=true, bool startnewlayout=true);
        bool _it_worked;
// -----------------------------------------------------
        int _list_depth;
// -----------------------------------------------------
	bool parse_CommonAttributes(DOM::Element e);
	bool parse_a(DOM::Element e);
	bool parse_p(DOM::Element e);
	bool parse_br(DOM::Element e);
	bool parse_table(DOM::Element e);
	bool parse_pre(DOM::Element e);
	bool parse_font(DOM::Element e);
	bool parse_ol(DOM::Element e);
	bool parse_ul(DOM::Element e);
	bool parse_hr(DOM::Element e);
	bool parse_img(DOM::Element e);


// -------------------------------------------------------

};

#endif
