/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_PARA
#define _TREEBUILD_PARA

#include <qlist.h>

class Font;
class Unique;
class Pgf;
class PgfTag;
class PgfNumString;
class ParaLine;
class FNote;
class String;
class Char;
class Marker;
class TextRectID;
class ATbl;
class AFrame;
class Variable;
class XRef;
class XRefEnd;

class ParaElement
{
public:
	enum ParaElementType { T_Unique, T_Pgf, T_PgfTag, T_PgfNumString, 
						   T_ParaLine };

	ParaElement( Unique* element );
	ParaElement( Pgf* element );
	ParaElement( PgfTag* element );
	ParaElement( PgfNumString* element );
	ParaElement( ParaLine* element );

	ParaElementType type() const { return _type; }
	Unique* unique() const { return _type == T_Unique ? _unique : 0; }
	Pgf* pgf() const { return _type == T_Pgf ? _pgf : 0; }
	PgfTag* pgftag() const { return _type == T_PgfTag ? _pgftag : 0; }
	PgfNumString* pgfnumstring() const { return _type == T_PgfNumString ? 
											 _pgfnumstring : 0; }
	ParaLine* paraline() const { return _type == T_ParaLine ? _paraline : 0; }

private:
	ParaElementType _type;
	union {
		Unique* _unique;
		Pgf* _pgf;
		PgfTag* _pgftag;
		PgfNumString* _pgfnumstring;
		ParaLine* _paraline;
	};
};

typedef QList<ParaElement> ParaElementList;

class Para
{
public:
	Para( ParaElementList* elements );

	ParaElementList* elements() const { return _elements; }

private:
	ParaElementList* _elements;
};


class ParaLineElement
{
public:
	enum ParaLineElementType { T_Font, T_XRef, T_FNote, T_String, T_Char,
							   T_TextRectID, T_Variable, T_ATbl, T_AFrame,
							   T_Marker, T_XRefEnd, T_NIL };

	ParaLineElement();
	ParaLineElement( Font* element );
	ParaLineElement( XRef* element );
	ParaLineElement( FNote* element );
	ParaLineElement( String* element );
	ParaLineElement( Char* element );
	ParaLineElement( TextRectID* element );
	ParaLineElement( Variable* element );
	ParaLineElement( ATbl* element );
	ParaLineElement( AFrame* element );
	ParaLineElement( Marker* element );
	ParaLineElement( XRefEnd* element );

	ParaLineElementType type() const { return _type; }
	Font* font() const { return _type == T_Font ? _font : 0; }
	XRef* xref() const { return _type == T_XRef ? _xref : 0; }
	FNote* fnote() const { return _type == T_FNote ? _fnote : 0; }
	String* plestring() const { return _type == T_String ? _string : 0; }
	Char* plechar() const { return _type == T_Char ? _char : 0; }
	TextRectID* textrectid() const { return _type == T_TextRectID ? _textrectid : 0; }
	Variable* variable() const { return _type == T_Variable ? _variable : 0; }
	ATbl* atbl() const { return _type == T_ATbl ? _atbl : 0; }
	AFrame* aframe() const { return _type == T_AFrame ? _aframe : 0; }
	Marker* marker() const { return _type == T_Marker ? _marker : 0; }
	XRefEnd* xrefend() const { return _type == T_XRefEnd ? _xrefend : 0; }


private:
	ParaLineElementType _type;
	union {
		Font* _font;
		XRef* _xref;
		FNote* _fnote;
		String* _string;
		Char* _char;
		TextRectID* _textrectid;
		Variable* _variable;
		ATbl* _atbl;
		AFrame* _aframe;
		Marker* _marker;
		XRefEnd* _xrefend;
	};
};

typedef QList<ParaLineElement> ParaLineElementList;

class ParaLine
{
public:
	ParaLine( ParaLineElementList* elements );

	ParaLineElementList* elements() const { return _elements; }

private:
	ParaLineElementList* _elements;
};


#endif
