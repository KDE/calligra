/*
   This file is part of the KDE project
   Copyright (C) 2001, 2004 Nicolas GOUTTE <goutte@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef PROCESS_DOCUMENT_H
#define PROCESS_DOCUMENT_H

#include <qdom.h>

class KWEFKWordLeader;

/**
 * @internal
 */
void ProcessLayoutTag       ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader );
/**
 * @internal
 */
void ProcessFormatsTag      ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader );
/**
 * @internal
 */
void ProcessTextTag         ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader );
/**
 * @internal
 */
void ProcessDocumentInfoTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader );
/**
 * @internal
 * @since 1.4 (made public)
 */
void ProcessImageTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader );

#endif /* PROCESS_DOCUMENT_H */

