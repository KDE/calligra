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

#ifndef __KoApplicationIface_h__
#define __KoApplicationIface_h__

#include <dcopobject.h>
#include <qvaluelist.h>
#include <dcopref.h>

/**
 * DCOP interface for any KOffice application (entry point)
 */
class KoApplicationIface : public DCOPObject
{
  K_DCOP
public:

  KoApplicationIface();
  ~KoApplicationIface();

k_dcop:
  /**
   * Creates a new document for the given native mimetype
   * Use it to create a shell and to load an existing file, if any
   */
  DCOPRef createDocument( const QString &nativeFormat );

  /**
   * @return a list of references to all the documents
   * (see KoDocumentIface)
   */
  QValueList<DCOPRef> getDocuments();

  /**
   * @return a list of references to all the views
   * (see KoViewIface)
   * Convenience method to avoid iterating over all documents to get all the views.
   */
  QValueList<DCOPRef> getViews();

  /**
   * @return a list of references to all the windows
   * (see KoMainWindowIface)
   */
  QValueList<DCOPRef> getWindows();
};

#endif

