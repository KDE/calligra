#ifndef __ko_stream_h__
#define __ko_stream_h__

#include <unistd.h>
#include <config.h>
#include <iostream.h>
#ifdef HAVE_MINI_STL
#include <ministl/vector.h>
#else
#include <vector.h>
#endif

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
