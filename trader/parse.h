#ifndef __parse_h__
#define __parse_h__

/**
 * Funktions Definitionen fuer den YACC.
 */
void setConstraintsParseTree( void *_ptr1 );
void setPreferencesParseTree( void *_ptr1 );
void* newOR( void *_ptr1, void *_ptr2 );
void* newAND( void *_ptr1, void *_ptr2 );
void* newCMP( void *_ptr1, void *_ptr2, int _i );
void* newIN( void *_ptr1, void *_ptr2 );
void* newMATCH( void *_ptr1, void *_ptr2 );
void* newCALC( void *_ptr1, void *_ptr2, int _i );
void* newBRACKETS( void *_ptr1 );
void* newNOT( void *_ptr1 );
void* newEXIST( void *_ptr1 );
void* newID( char *_ptr1 );
void* newSTRING( char *_ptr1 );
void* newNUM( int _i );
void* newFLOAT( float _f );
void* newBOOL( char _b );

void* newWITH( void *_ptr1 );
void* newMAX( void *_ptr1 );
void* newMIN( void *_ptr1 );
void* newFIRST();
void* newRANDOM();

#endif











