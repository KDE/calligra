#ifndef __ko_stream_h__
#define __ko_stream_h__

#include <iostream.h>
#include <vector.h>

#include <komlParser.h>
#include <komlWriter.h>

#include <qrect.h>
#include <qcolor.h>
#include <qpen.h>
#include <qfont.h>

ostream& operator<< ( ostream& outs, const QRect &_rect );
QRect tagToRect( vector<KOMLAttrib>& _attribs );

ostream& operator<< ( ostream& outs, const QColor &_rect );
istream& operator>> ( istream& outs, QColor &_rect );
QColor strToColor( const char *_buffer );

ostream& operator<< ( ostream& outs, const QPen &_pen );
QPen tagToPen( vector<KOMLAttrib>& _attribs );

ostream& operator<< ( ostream& outs, const QFont &_font );
QFont tagToFont( vector<KOMLAttrib>& _attribs );

#endif
