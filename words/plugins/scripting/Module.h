/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
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

#include <KoScriptingModule.h>

#include <KWord.h>
#include <QWeakPointer>

class KWDocument;

namespace Scripting
{

/**
* The Module class enables access to the KWord functionality
* from within the scripting backends.
* It will be shown as 'KWord' entity in scripting code.
*
* KWord provides as top-level containers the \a FrameSet
* objects. Each such frameset is then able to contain
* multiple \a Frame objects. The frameset also allows to
* access the \a TextDocument object to deals with the actual
* content within a text document.
*
* Python example to set the content of the main text document;
* \code
* import KWord
* doc = KWord.mainFrameSet().document()
* doc.setHtml("<b>Hello World</b>")
* \endcode
*
* Python example to append content to the main text document
* and set the page header and footer;
* \code
* import KWord
* doc = KWord.mainFrameSet().document()
* doc.lastCursor().insertHtml("Even more <b>Hello World</b>")
* KWord.firstPageHeaderFrameSet().document().setText("Header")
* KWord.firstPageFooterFrameSet().document().setText("Footer")
* \endcode
*
* Python example that prints the documents Url and some other
* meta information;
* \code
* import KWord
* print KWord.document().url()
* print KWord.document().documentInfoTitle()
* print KWord.document().documentInfoAuthorName()
* \endcode
*/
class Module : public KoScriptingModule
{
    Q_OBJECT
public:
    explicit Module(QObject* parent = 0);
    virtual ~Module();

    KWDocument* kwDoc();
    virtual KoDocument* doc();
    QObject* findFrameSet(KWord::TextFrameSetType type);

public slots:

    /***** Page *****/

    /** Return total number of pages the document has. */
    int pageCount();

    /** Return the \a Page of the specific page number.
    *
    * Python example that iterates over all pages;
    * \code
    * import KWord
    * for i in range( KWord.pageCount() ):
    *     page = KWord.page(i)
    *     print "width=%s height=%s" % (page.width(),page.height())
    * \endcode
    */
    QObject* page(int pageNumber);

    /** Insert a new page and returns the new \a Page object.
    *
    * The new page is inserted after the page which has the pagenumber
    * \p afterPageNum . If \p afterPageNum is 0, the new page is
    * inserted before page 1.  In all cases, the new page will have
    * the number afterPageNum+1. To append a new page use
    * insertPage( pageCount() ).
    */
    QObject* insertPage(int afterPageNum);
    /** Remove the page with the pagenumber \p pageNumber . */
    void removePage(int pageNumber);

    /***** FrameSet *****/

    /** Return the amount of framesets this document holds. */
    int frameSetCount();
    /** Return the \a FrameSet object identified by the index frameSetNr. */
    QObject* frameSet(int frameSetNr);
    /** Return the \a FrameSet object which has the name \p name . */
    QObject* frameSetByName(const QString& name);

    /** Return the \a FrameSet that holds the headers for the odd pages */
    QObject* oddPagesHeaderFrameSet() {
        return findFrameSet(KWord::OddPagesHeaderTextFrameSet);
    }
    /** Return the \a FrameSet that holds the headers for the even pages */
    QObject* evenPagesHeaderFrameSet() {
        return findFrameSet(KWord::EvenPagesHeaderTextFrameSet);
    }
    /** Return the \a FrameSet that holds the footers for the odd pages */
    QObject* oddPagesFooterFrameSet() {
        return findFrameSet(KWord::OddPagesFooterTextFrameSet);
    }
    /** Return the \a FrameSet that holds the footers for the even pages */
    QObject* evenPagesFooterFrameSet() {
        return findFrameSet(KWord::EvenPagesFooterTextFrameSet);
    }

    /** Return the \a FrameSet that holds all the frames for the main text area */
    QObject* mainFrameSet() {
        return findFrameSet(KWord::MainTextFrameSet);
    }

    /** Return any other \a FrameSet not managed by the auto-frame layout */
    QObject* otherFrameSet() {
        return findFrameSet(KWord::OtherTextFrameSet);
    }

    /** Add and return a new \a FrameSet object for text handled with \a TextDocument . */
    QObject* addTextFrameSet(const QString& framesetname);
    /** Add and return a new \a FrameSet object. */
    QObject* addFrameSet(const QString& framesetname);

    /***** Frame *****/

    /** Return a list of shapeId names. Such a shapeId name could then be
    used for example with the \a addFrame() method to create a new frame. */
    QStringList shapeKeys();

    /** Return the amount of frames this document holds. */
    int frameCount();
    /** Return a the \a Frame object identified by the index frameNr. */
    QObject* frame(int frameNr);

    /***** Page Layout *****/

    /** Return the standard page layout. */
    QObject* standardPageLayout();

    /***** Style *****/

    /** Return the default paragraph \a ParagraphStyle that will always be present in each document. */
    QObject* defaultParagraphStyle();
    /** Return the \a ParagraphStyle with the defined user-visible-name \p name . */
    QObject* paragraphStyle(const QString& name);
    /** Add and return a new \a ParagraphStyle object. */
    QObject* addParagraphStyle(const QString& name);

    /** Return the \a CharacterStyle with the defined user-visible-name \p name . */
    QObject* characterStyle(const QString& name);
    /** Add and return a new \a CharacterStyle object. */
    QObject* addCharacterStyle(const QString& name);

    /***** Active selection *****/

    /** Return a \a Tool object that provides access to functionality
    like handling for example current/active selections. */
    QObject* tool();

private:
    QWeakPointer<KWDocument> m_doc;
};

}

#endif
