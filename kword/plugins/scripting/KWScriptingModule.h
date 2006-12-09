/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KWSCRIPTINGMODULE_H
#define KWSCRIPTINGMODULE_H

#include <QString>
#include <QStringList>
#include <QObject>

class KWDocument;
class KWView;

/**
* The ScriptingModule class enables access to the KWord
* functionality from within the scripting backends.
*/
class KWScriptingModule : public QObject
{
        Q_OBJECT
    public:
        KWScriptingModule();
        virtual ~KWScriptingModule();

        KWDocument* doc();
        void setView(KWView* view = 0);

    public slots:

        /**
        * Return the \a KoApplicationAdaptor object.
        */
        QObject* application();

        /**
        * Return the \a KoDocumentAdaptor object.
        */
        QObject* document();

        /**
        * Return total number of pages the document has.
        */
        int pageCount();

        /**
        * Return the \a KWPageAdaptor of the specific page number.
        */
        QObject* page(int pageNumber);

    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;
};

#endif
