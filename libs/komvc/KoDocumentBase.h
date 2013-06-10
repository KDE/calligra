/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>

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
#ifndef KODOCUMENTBASE_H
#define KODOCUMENTBASE_H

#include <QObject>
#include <QUrl>

#include "komvc_export.h"

#include <kundo2stack.h>

/**
 *  The %Calligra document class
 *
 *  This class provides some functionality each %Calligra document should have.
 *
 *  @short The %Calligra document class
 */
class KOMVC_EXPORT KoDocumentBase : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @param undoStack accepts the stack for the document. You can create any type of stack if you need.
     *        The stack objects will become owned by the document. This is used by Krita's KisDoc2. The default value for this
     *        parameter is a usual Qt's stack.
     */
    explicit KoDocumentBase(KUndo2Stack *undoStack, QObject *parent);

    /**
     *  Destructor.
     *
     * The destructor does not delete any attached KoView objects and it does not
     * delete the attached widget as returned by widget().
     */
    virtual ~KoDocumentBase();

    virtual bool open(const QString &fileName) = 0;
    virtual bool saveTo(const QString &fileName) = 0;

    void setFileName(const QString &fileName);
    QString &fileName();

    void setModified(bool modified = true);
    bool modified();

private:

    class Private;
    Private *const d;
};

class KoDocumentFactory {
public:
    virtual ~KoDocumentFactory() {}
    virtual KoDocumentBase *create(KUndo2Stack *undoStack = new KUndo2Stack(), QObject *parent = 0) = 0;
};

#endif
