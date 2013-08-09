/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __parse_h__
#define __parse_h__

/*
 * Functions definition for yacc
 */
void KTraderParse_mainParse( const char *_code );
void KTraderParse_setParseTree( void *_ptr1 );
void KTraderParse_error( const char* err );
void* KTraderParse_newOR( void *_ptr1, void *_ptr2 );
void* KTraderParse_newAND( void *_ptr1, void *_ptr2 );
void* KTraderParse_newCMP( void *_ptr1, void *_ptr2, int _i );
void* KTraderParse_newIN( void *_ptr1, void *_ptr2, int _cs );
void* KTraderParse_newSubstringIN( void *_ptr1, void *_ptr2, int _cs );
void* KTraderParse_newMATCH( void *_ptr1, void *_ptr2, int _cs );
void* KTraderParse_newCALC( void *_ptr1, void *_ptr2, int _i );
void* KTraderParse_newBRACKETS( void *_ptr1 );
void* KTraderParse_newNOT( void *_ptr1 );
void* KTraderParse_newEXIST( char *_ptr1 );
void* KTraderParse_newID( char *_ptr1 );
void* KTraderParse_newSTRING( char *_ptr1 );
void* KTraderParse_newNUM( int _i );
void* KTraderParse_newFLOAT( float _f );
void* KTraderParse_newBOOL( char _b );

void* KTraderParse_newWITH( void *_ptr1 );
void* KTraderParse_newMAX( void *_ptr1 );
void* KTraderParse_newMIN( void *_ptr1 );
void* KTraderParse_newMAX2( char *_id );
void* KTraderParse_newMIN2( char *_id );
void* KTraderParse_newCI( void *_ptr1 );
void* KTraderParse_newFIRST();
void* KTraderParse_newRANDOM();

void KTraderParse_destroy(void *);

#endif
