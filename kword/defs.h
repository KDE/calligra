#ifndef defs_h
#define defs_h

#include <iostream.h>
#include <qstring.h>

#define MIME_TYPE "application/x-kword"
#define KWordRepoID "IDL:KWord/KWordDocument:1.0"

// #define ZOOM( vx ) ( ( vx )*ZoomZ/ZoomN )
#define ZOOM( vx ) ( vx )
// #define UNZOOM( px ) ( ( px )*ZoomN/ZoomZ )
#define UNZOOM( px ) ( px )

#define POINT_TO_MM( px ) ( ( float )px/2.83465 )
#define MM_TO_POINT( mm ) ( int( ( float )mm*2.83465 ) )
#define POINT_TO_INCH( px ) ( ( float )px/72.0 )
#define INCH_TO_POINT( inch ) ( int( ( float )inch*72.0 ) )
#define MM_TO_INCH( mm ) ( mm/25.4 )
#define INCH_TO_MM( inch ) ( inch*25.4 )

#define A4_WIDTH 210.0
#define A4_HEIGHT 297.0

#define A5_WIDTH 155.0
#define A5_HEIGHT 290.0

#define LETTER_WIDTH 216.0
#define LETTER_HEIGHT 280.0

#define LEGAL_WIDTH 216.0
#define LEGAL_HEIGHT 355.0

#define EXECUTIVE_WIDTH 184.0
#define EXECUTIVE_HEIGHT 266.0

#define DEFAULT_PAPER_WIDTH  210.0
#define DEFAULT_PAPER_HEIGHT 297.0

#define DEFAULT_TOP_BORDER 15.0
#define DEFAULT_BOTTOM_BORDER 15.0
#define DEFAULT_LEFT_BORDER 10.0
#define DEFAULT_RIGHT_BORDER 10.0

#define STANDARD_COLUMNS 2
#define STANDARD_COLUMN_SPACING 3

enum MouseMode {MM_EDIT = 0, MM_EDIT_FRAME = 1, MM_CREATE_TEXT = 2, MM_CREATE_PIX = 3, MM_CREATE_CLIPART = 4, MM_CREATE_TABLE = 5, \
				MM_CREATE_FORMULA = 6, MM_CREATE_PART = 7, MM_CREATE_KSPREAD_TABLE = 8}; 

enum EditMode {EM_INSERT, EM_DELETE, EM_BACKSPACE, EM_CMOVE, EM_NONE, EM_RETURN}; 

enum InsertPos {I_BEFORE, I_AFTER}; 

enum KWUnits {U_MM, U_PT, U_INCH}; 

class KWUnit
{
public:
	KWUnit() { _pt = 0; _mm = 0.0; _inch = 0.0; }
	KWUnit( unsigned int __pt, float ___mm, float __inch )
    { _pt = __pt; _mm = ___mm; _inch = __inch; }
	KWUnit( float ___mm )
    { setMM( ___mm ); }
	KWUnit( const KWUnit &unit ) {
		_pt = unit._pt; 
		_mm = unit._mm; 
		_inch = unit._inch; 
	}

	inline void setPT( unsigned int __pt )
    { _pt = __pt; _mm = POINT_TO_MM( _pt ); _inch = POINT_TO_INCH( _pt ); }
	inline void setMM( float ___mm )
    { _mm = ___mm; _pt = MM_TO_POINT( _mm ); _inch = MM_TO_INCH( _mm ); }
	inline void setINCH( float __inch )
    { _inch = __inch; _mm = INCH_TO_MM( _inch ); _pt = INCH_TO_POINT( _inch ); }
	inline void setPT_MM( unsigned int __pt, float ___mm )
    { _pt = __pt; _mm = ___mm; _inch = MM_TO_INCH( _mm ); }
	inline void setPT_INCH( unsigned int __pt, float __inch )
    { _pt = __pt; _inch = __inch; _mm = INCH_TO_MM( _inch ); }
	inline void setMM_INCH( float ___mm, float __inch )
    { _mm = ___mm; _inch = __inch; _pt = MM_TO_POINT( _mm ); }
	inline void setPT_MM_INCH( unsigned int __pt, float ___mm, float __inch )
    { _pt = __pt; _mm = ___mm; _inch = __inch; }

	inline const unsigned int pt() { return _pt; }
	inline const float mm() { return _mm; }
	inline const float inch() { return _inch; }

	KWUnit &operator=( const KWUnit &unit ); 

	static const KWUnits unitType( const QString _unit ); 

protected:
	unsigned int _pt; 
	float _mm; 
	float _inch; 

}; 

inline KWUnit &KWUnit::operator=( const KWUnit &unit )
{
	_pt = unit._pt; 
	_mm = unit._mm; 
	_inch = unit._inch; 
	return *this; 
}

inline ostream& operator<<( ostream &out, KWUnit &unit )
{
	out << "pt=\"" << unit.pt() << "\" mm=\"" << unit.mm() << "\" inch=\"" << unit.inch() << "\""; 
	return out; 
}

inline const KWUnits KWUnit::unitType( const QString _unit )
{
	if ( _unit == "mm" ) return U_MM; 
	if ( _unit == "inch" ) return U_INCH; 
	return U_PT; 
}

#endif
