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

#ifndef SCRIPTING_MODULE_H
#define SCRIPTING_MODULE_H

#include <QString>
#include <QStringList>
#include <QObject>

class KWDocument;
class KWView;

namespace Scripting {

    /**
    * The ScriptingModule class enables access to the KWord
    * functionality from within the scripting backends.
    */
    class Module : public QObject
    {
            Q_OBJECT
        public:
            explicit Module();
            virtual ~Module();

            KWDocument* doc();
            void setView(KWView* view = 0);

        public Q_SLOTS:

            /***** Adaptor *****/

            /** Return the \a KoApplicationAdaptor object. */
            QObject* application();
            /** Return the \a KoMainWindow object. */
            QObject* shell();
            /** Return the \a KMainWindow object. */
            QObject* mainWindow();
            /** Return the \a KoDocumentAdaptor object. */
            QObject* document();

            /***** Page *****/

            /** Return total number of pages the document has. */
            int pageCount();
            /** Return the \a Page of the specific page number. */
            QObject* page(int pageNumber);

            /** Insert a new page and returns the new \a Page object.
            The new page is inserted after the page which has the pagenumber
            \p afterPageNum . If \p afterPageNum is 0, the new page is
            inserted before page 1.  In all cases, the new page will have
            the number afterPageNum+1. To append a new page use
            insertPage( pageCount() ). */
            QObject* insertPage( int afterPageNum );
            /** Remove the page with the pagenumber \p pageNumber . */
            void removePage( int pageNumber );

            /** Return the first pagenumber of the document. */
            int startPage();
            /** Set a new startpage for this document.  */
            void setStartPage(int pageNumber);

            /***** Shapes *****/

            /** Return a list of shapeId names. Such a shapeId name could then be
            used for example with the \a addFrame() method to create a new frame. */
            QStringList shapeKeys();

            /***** FrameSet *****/

            /** Return the amount of framesets this document holds. */
            int frameSetCount();
            /** Return the \a FrameSet object identified by the index frameSetNr. */
            QObject* frameSet(int frameSetNr);
            /** Add and return a new \a FrameSet object for text handled with \a TextDocument . */
            QObject* addTextFrameSet(const QString& framesetname);
            /** Add and return a new \a FrameSet object. */
            QObject* addFrameSet(const QString& framesetname);

            /***** Frame *****/

            /** Return the amount of frames this document holds. */
            int frameCount();
            /** Return a the \a Frame object identified by the index frameNr. */
            QObject* frame(int frameNr);
            /** Add and return a new \a FrameSet object for text handled with \a TextDocument . */
            QObject* addTextFrame(const QString& framesetname);
            /** Add and return a new \a FrameSet object. */
            QObject* addFrame(const QString& framesetname, const QString& shapeId);

            /***** Layout *****/

            /** Return the standard page layout. */
            QObject* standardPageLayout();
            /** Return the default page layout. */
            QObject* defaultPageLayout();
            /** Set the default page layout. */
            void setDefaultPageLayout(QObject* pagelayout);

            /***** Style *****/

            /** Return the default paragraph \a Style that will always be present in each document. */
            QObject* defaultParagraphStyle();
            /** Return the first paragraph \a Style with the param user-visible-name. */
            QObject* paragraphStyle(const QString& name);
            /** Add and return a new paragraph \a Style object. */
            QObject* addParagraphStyle(const QString& name);

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private* const d;
    };

}

#endif
