/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_EXTERNAL_H
#define _TREEBUILD_EXTERNAL_H

class ExternalData;
class ImportObFile;
class BitmapDPI;
class FlipLR;
class NativeOrigin;
class Unique;
class ShapeRect;
class BRect;
class RunAroundType;
class RunAroundGap;

class BitmapDPI
{
public:
	BitmapDPI( int value );

private:
	int _value;
};


class ImportObFile
{
public:
	ImportObFile( const char* value );

private:
	string _value;
};


class FlipLR
{
public:
	FlipLR( const char* value );

private:
	bool _value;
};


class NativeOrigin
{
public:
	NativeOrigin( double x, const char* unitx,
				  double y, const char* unity );

private:
	double _x;
	double _y;
};




class ImportObjectElement
{
public:
	enum ImportObjectElementType { T_Unique, T_ImportObFile,
								   T_ShapeRect, T_BRect, T_FlipLR,
								   T_NativeOrigin, T_BitmapDPI,
								   T_RunAroundType, T_RunAroundGap,
								   T_ExternalData };

	ImportObjectElement( Unique* element );
	ImportObjectElement( ImportObFile* element );
	ImportObjectElement( ShapeRect* element );
	ImportObjectElement( BRect* element );
	ImportObjectElement( FlipLR* element );
	ImportObjectElement( NativeOrigin* element );
	ImportObjectElement( BitmapDPI* element );
	ImportObjectElement( RunAroundType* element );
	ImportObjectElement( RunAroundGap* element );
	ImportObjectElement( ExternalData* element );

	ImportObjectElementType type() const { return _type; }

private:
	ImportObjectElementType _type;
	union {
		Unique* _unique;
		ImportObFile* _importobfile;
		ShapeRect* _shaperect;
		BRect* _brect;
		FlipLR* _fliplr;
		ExternalData* _externaldata;
		NativeOrigin* _nativeorigin;
		BitmapDPI* _bitmapdpi;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
	};
};



typedef list<ImportObjectElement*> ImportObjectElementList;

class ImportObject
{
public:
	ImportObject( ImportObjectElementList* elements );

	ImportObjectElementList* elements() const { return _elements; }

private:
	ImportObjectElementList* _elements;
};

typedef list<ImportObject*> ImportObjectList;


#endif
