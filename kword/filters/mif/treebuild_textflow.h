/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_TEXTFLOW
#define _TREEBUILD_TEXTFLOW

#include <list>
#include <string>

class Notes;
class Para;

class TFTag
{
public:
	TFTag( const char* );

private:
	string _value;
};

class TFAutoConnect
{
public:
	TFAutoConnect( const char* );

private:
	string _value;
};


class TFLineSpacing
{
public:
	TFLineSpacing( double, const char* unit );

private:
	double _value;
};


class TFSynchronized
{
public:
	TFSynchronized( const char* );

private:
	bool _value;
};


class TFMinHangHeight
{
public:
	TFMinHangHeight( double, const char* unit );

private:
	double _value;
};


class TextFlowElement
{
public:
	enum TextFlowElementType { T_Notes, T_Para, T_TFTag, T_TFAutoConnect,
							   T_TFSynchronized, T_TFLineSpacing,
							   T_TFMinHangHeight };

	TextFlowElement( Notes* element );
	TextFlowElement( Para* element );
	TextFlowElement( TFTag* element );
	TextFlowElement( TFAutoConnect* element );
	TextFlowElement( TFSynchronized* element );
	TextFlowElement( TFLineSpacing* element );
	TextFlowElement( TFMinHangHeight* element );

	TextFlowElementType type() const { return _type; }
	Notes* notes() const { return _type == T_Notes ? _notes : 0; }
	Para* para() const { return _type == T_Para ? _para : 0; }
	TFTag* tftag() const { return _type == T_TFTag ? _tftag : 0; }
	TFAutoConnect* tfautoconnect() const { return _type == T_TFAutoConnect ?
											   _tfautoconnect : 0; }
	TFSynchronized* tfsynchronized() const { return _type == T_TFSynchronized
												 ? _tfsynchronized : 0; }
	TFLineSpacing* tflinespacing() const { return _type == T_TFLineSpacing ?
											   _tflinespacing : 0; }
	TFMinHangHeight* tfminhangheight() const { return _type ==
												   T_TFMinHangHeight ?
												   _tfminhangheight : 0; }

private:
	TextFlowElementType _type;
	union {
		Notes* _notes;
		Para* _para;
		TFTag* _tftag;
		TFAutoConnect* _tfautoconnect;
		TFSynchronized* _tfsynchronized;
		TFLineSpacing* _tflinespacing;
		TFMinHangHeight* _tfminhangheight;
	};
};

typedef list<TextFlowElement*> TextFlowElementList;

class TextFlow
{
public:
	TextFlow( TextFlowElementList* elements );
	
	TextFlowElementList* elements() const { return _elements; }

private:
	TextFlowElementList* _elements;
};

typedef list<TextFlow*> TextFlowList;

extern TextFlowList textflows;

#endif
