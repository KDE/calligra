/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KoDocumentIface_h__
#define __KoDocumentIface_h__

#include <dcopobject.h>
#include <qvaluelist.h>
#include <dcopref.h>

class KoDocument;
class KDCOPActionProxy;

/**
 * DCOP interface for any KOffice document
 * Use KoApplicationIface to get hold of an existing document's interface,
 * or to create a document.
 *
 * Note: KOffice Applications may (and should) reimplement KoDocument::dcopObject()
 * In this case, don't look here... (unless the DCOP interface for the document
 * inherits KoDocumentIface, which is a good thing to do)
 */
class KoDocumentIface : public DCOPObject
{
    K_DCOP
public:

    KoDocumentIface( KoDocument * doc, const char * name = 0 );
    ~KoDocumentIface();

    // Generate a name for this interface. Automatically used if name=0 is
    // passed to the constructor
    static QCString newIfaceName();

k_dcop:
    /**
     * Returns the URL for this document (empty, real URL, or internal one)
     */
    QString url();

    bool isModified();

    int viewCount();
    DCOPRef view( int idx );

    // DCOP-action proxy
    DCOPRef action( const QCString &name );
    QCStringList actions();
    QMap<QCString,DCOPRef> actionMap();

    void saveAs( const QString & url );
    void setOutputMimeType( const QCString & mimetype );
protected:
    KoDocument * m_pDoc;
    KDCOPActionProxy *m_actionProxy;
};

#endif

