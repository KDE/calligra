/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef PARSER_H
#define PARSER_H

#include "sharedptr.h"
#include "global.h"
#include "wv2_export.h"

namespace wvWare
{

// This ensures that we return the correct FIB :)
// Note: The guy implementing Word95 support also has to return such
// a Word97 FIB (as for most of the other important structures)!
// We will create the conversion code with a script (at least those
// parts of it which are easy to map)
namespace Word97
{
struct FIB;
struct DOP;
struct SEP;
struct FFN;
struct STTBF;
}

class InlineReplacementHandler;
class SubDocumentHandler;
class TableHandler;
class PictureHandler;
class TextHandler;
class GraphicsHandler;
class OLEStorage;
class OLEStreamReader;
class StyleSheet;
class AssociatedStrings;
class Drawings;
class Bookmark;

/**
 * This class is the heart of the filter:
 *<ul>
 *  <li>
 *    The layers above it consume the output, primarily using the handlers passed in
 *    using the setXXXHandler() methods.
 *  </li>
 *  <li>
 *    The layers below handle the specifics of different versions of Word.
 *  </li>
 *  <li>
 *    This layer:
 *  <ul>
 *    <li>
 *        Mostly hides visibility of structures that are specific to any given
 *        version of Word (a few things like the FIB from Word97 *are* generic enough
 *        to be used here).
 *    </li>
 *    <li>
 *        Does all the processing during the call to parse(), by calling the provided
 *        handlers as needed.
 *    </li>
 *  </ul>
 *</li>
 *</ul>
 */
class WV2_EXPORT Parser : public Shared
{
public:
    /**
     * This enum is needed to keep track of the parsing state. We e.g. have to
     * handle footnote references differently while parsing footnotes. It's a bit
     * hacky, but needed.
     */
    enum SubDocument { None, Main, Footnote, Header, Macro, Annotation, Endnote, TextBox, HeaderTextBox, Bookmark };

    /**
     * Construct a parser. The reason that we get the "open" storage
     * and the document reader passed (ugly :}) is that "someone" has
     * to determine the nFib to select the proper parser... oh well
     * At least we take ownership ;)
     */
    Parser( OLEStorage* storage, OLEStreamReader* wordDocument );
    virtual ~Parser();  // Don't forget to close everything properly here

    /**
     * Is everything allright?
     */
    bool isOk() const { return m_okay; }

    /**
     * Invokes the parsing process.
     */
    virtual bool parse() = 0;

    /**
     * Get the FIB (read only!)
     */
    virtual const Word97::FIB& fib() const = 0;
    /**
     * Get the DOP (read only!)
     */
    virtual const Word97::DOP& dop() const = 0;

    /**
     * Get the font family name structure for a given ftc.
     */
    virtual const Word97::FFN& font( S16 ftc ) const = 0;

    /**
     * Get the associated strings (author, title,...).
     * Not cached, so don't use it like:
     * parser->associatedStrings().author(); parser->associatedStrings().title()...
     */
    virtual AssociatedStrings associatedStrings() = 0;

    /**
     * This stylesheet holds all the styles inside the Word file
     */
    virtual const StyleSheet& styleSheet() const = 0;

    /**
     * This Drawings holds all the information about drawings the Word file
     */
    virtual Drawings * getDrawings() = 0;

    /**
     * This table holds actuald MS-Doc stream
     */
    virtual OLEStreamReader* getTable() = 0;

    /**
     * Looks for textbox text data and process them
     */
    virtual void parseTextBox( uint lid) =0;

    /**
     * The inline replacement handler is used to replace certain characters on the fly.
     * We don't take ownership of the handler!
     */
    void setInlineReplacementHandler( InlineReplacementHandler* handler );
    /**
     * The sub-document handler gets all callbacks related to the document structure.
     * We don't take ownership of the handler!
     */
    void setSubDocumentHandler( SubDocumentHandler* handler );
    /**
     * The table handler is used to tell the consumer about table structures.
     * We don't take ownership of the handler!
     */
    void setTableHandler( TableHandler* handler );
    /**
     * The picture handler passes the image/drawing data to the consumer.
     * We don't take ownership of the handler!
     */
    void setPictureHandler( PictureHandler* handler );
    /**
     * The text handler is the main worker among all handlers. It's used to forward
     * the formatted text to the document, make sure that it's fast.
     * We don't take ownership of the handler!
     */
    void setTextHandler( TextHandler* handler );
    /**
     * The graphics handler is used to tell the consumer about MS-ODRAW structures.
     * We don't take ownership of the handler!
     */
    void setGraphicsHandler( GraphicsHandler* handler );

    // Do we need public access to parts of the OLEStorage interface?
    // If we add public accessors we should make m_storage private.

protected:
    InlineReplacementHandler* m_inlineHandler;
    SubDocumentHandler* m_subDocumentHandler;
    TableHandler* m_tableHandler;
    PictureHandler* m_pictureHandler;
    TextHandler* m_textHandler;
    GraphicsHandler* m_graphicsHandler;
    bool m_ourInlineHandler;
    bool m_ourSubDocumentHandler;
    bool m_ourTableHandler;
    bool m_ourPictureHandler;
    bool m_ourTextHandler;
    bool m_ourGraphicsHandler;

    OLEStorage* m_storage;           // The storage representing the file
    OLEStreamReader* m_wordDocument; // document stream ('WordDocument')

    bool m_okay;                     // Still allright?

private:
    Parser( const Parser& rhs );
    Parser& operator=( const Parser& rhs );

    template<typename Handler> void setHandler( Handler* newHandler, Handler** handler, bool& ourHandler )
    {
        if ( ourHandler ) {
            ourHandler = false;
            delete *handler;
        }
        *handler = newHandler;
    }
};

} // namespace wvWare

#endif // PARSER_H
