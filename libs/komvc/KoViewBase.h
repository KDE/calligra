/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

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
#ifndef __koView_h__
#define __koView_h__

#include <QWidget>

#include "komvc_export.h"

class KoDocumentBase;

/**
 * This class is used to display a @ref KoDocumentBase.
 *
 * Multiple views can be attached to one document at a time.
 */
class KOMVC_EXPORT KoViewBase : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a new view for the document. Usually you don't create views yourself
     * since the Calligra components come with their own view classes which inherit
     * KoView.
     *
     * @param document is the document which should be displayed in this view. This pointer
     *                 must not be zero.
     * @param parent   parent widget for this view.
     */
    KoViewBase(KoDocumentBase *document, QWidget *parent = 0);

    /**
     * Destroys the view and unregisters at the document.
     */
    virtual ~KoViewBase();

    KoDocumentBase *document() const;

public:

    class Private;
    Private * const d;
};

class KoViewFactory {
public:
    virtual ~KoViewFactory() {}
    virtual KoViewBase *create(KoDocumentBase *document, QWidget *parent = 0) = 0;
};

#endif
