/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_XREF
#define _TREEBUILD_XREF

#include <list>
#include <string>

class Unique;

class XRefSrcText
{
public:
	XRefSrcText ( const char* );

private:
	string _value;
};


class XRefSrcFile
{
public:
	XRefSrcFile ( const char* );

private:
	string _value;
};


class XRefSrcIsElem
{
public:
	XRefSrcIsElem ( const char* );

private:
	string _value;
};


class XRefLastUpdate
{
public:
	XRefLastUpdate ( int );

private:
	int _value;
};


class XRefName
{
public:
	XRefName( const char* );

private:
	string _value;
};

class XRefElement
{
public:
	enum XRefElementType { T_XRefName, T_XRefSrcText, T_XRefSrcFile,
						   T_XRefSrcIsElem, T_XRefLastUpdate,
						   T_Unique };
	
	XRefElement( XRefName* element );
	XRefElement( XRefSrcText* element );
	XRefElement( XRefSrcFile* element );
	XRefElement( XRefSrcIsElem* element );
	XRefElement( XRefLastUpdate* element );
	XRefElement( Unique* element );

private:
	XRefElementType _type;

	union {
		XRefName* _xrefname;
		XRefSrcText* _xrefsrctext;
		XRefSrcIsElem* _xrefsrciselem;
		XRefSrcFile* _xrefsrcfile;
		XRefLastUpdate* _xreflastupdate;
		Unique* _unique;
	};
};

typedef list<XRefElement*> XRefElementList;

class XRef
{
public:
	XRef( XRefElementList* elements );

private:
	XRefElementList* _elements;
};


class XRefEnd
{
public:

};

#endif
