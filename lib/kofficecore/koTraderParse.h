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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#ifndef __parse_h__
#define __parse_h__

/**
 * Functions definition for yacc
 */
void setParseTree( void *_ptr1 );
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
void* newMAX2( const char *_id );
void* newMIN2( const char *_id );
void* newFIRST();
void* newRANDOM();

#endif











