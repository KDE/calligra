/* This file is part of the wvWare 2 project
   Copyright (C) 2002-2003 Werner Trobin <trobin@kde.org>

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

#ifndef HANDLERS_H
#define HANDLERS_H

#include <vector>

#include "global.h"
#include "sharedptr.h"
#include "functordata.h"
#include "wv2_export.h"
#include "ms_odraw.h"

namespace wvWare {

    class Parser9x;
    struct UChar;
    class UString;
    template<class ParserT, typename Data> class Functor;
    typedef Functor<Parser9x, HeaderData> HeaderFunctor;
    typedef Functor<Parser9x, AnnotationData> AnnotationFunctor;
    typedef Functor<Parser9x, TableRowData> TableRowFunctor;
    typedef Functor<Parser9x, FootnoteData> FootnoteFunctor;
    typedef Functor<Parser9x, PictureData> PictureFunctor;

    /**
     * This class allows to replace the character values of some
     * characters in a Word document, like non-breaking hyphens,...
     * during the processing. As we have to look at every single
     * character anyway, we can as well do single-character replacement
     * in case it's necessary. That way you don't have to scan all the
     * text again in the consumer filter.
     *
     * We use U8 instead of U16 as all interesting Word characters are
     * within 7bit ASCII range, the names of the methods should be
     * descriptive enough.
     */
    class WV2_EXPORT InlineReplacementHandler
    {
    public:
        virtual ~InlineReplacementHandler();

        virtual U8 tab();
        virtual U8 hardLineBreak();
        virtual U8 columnBreak();
        virtual U8 nonBreakingHyphen();
        virtual U8 nonRequiredHyphen();
        virtual U8 nonBreakingSpace();
    };


    /**
     * The SubDocumentHandler is more or less a state-callback class. It informs
     * the consumer filter about changes of subdocuments like body, header,
     * footnote,...
     * This class is needed as we reuse one @ref TextHandler object for all the
     * text we find, no matter whether it's inside the main body or within a textbox.
     */
    class WV2_EXPORT SubDocumentHandler
    {
    public:
        virtual ~SubDocumentHandler();

        /**
         * This method is called as soon as you call @ref Parser::parse. It indicates
         * the start of the body text (the main document text-flow).
         */
        virtual void bodyStart();
        /**
         * This method is called when all characters of the main body text
         * are processed.
         */
        virtual void bodyEnd();

        /**
         * Every time you invoke a @ref FootnoteFunctor this method will be called.
         * Note that @ref FoonoteFunctor is also emitted when we find endnotes.
         */
        virtual void footnoteStart();
        /**
         * Once the footnote characters are processed this method is called.
         */
        virtual void footnoteEnd();

        /**
         * 2.3.4
         *
         * Comments/Annotations (Microsoft isn't consistent)
         *
         * The comment document contains all of the content in the comments/annotation. It begins at the CP
         * immediately following the Header Document and is FibRgLw97.ccpAtn characters long.
         * The locations of individual comments within the comment document are specified by a PlcfandTxt
         * whose location is specified by the fcPlcfandTxt member of FibRgFcLcb97. The locations of the
         * comment reference characters in the Main Document are specified by a PlcfandRef whose location is
         * specified by the fcPlcfandRef member of FibRgFcLcb97.
         *
         * Let's see whether it makes sense when I continue implementing this...
         */
        /**
         * Every time you invoke a @ref AnnotationFunctor this method will be called.
         */
        virtual void annotationStart();
        /**
         * Once the annotation characters are processed this method is called.
         */
        virtual void annotationEnd();

        /**
         * For every section in a Word document a @ref HeaderFunctor is emitted.
         * Invoking the functor triggers the parsing of all headers inside the
         * section, this method is called to indicate that.
         */
        virtual void headersStart();
        /**
         * This method is called once all headers and footers of the current
         * section are processed.
         */
        virtual void headersEnd();
        /**
         * There are up to 6 headers/footers for every section. This method informs
         * you about the begin of a new header or footer of the given type.
         *
         * The default header/footer is the *Odd one. If you also get any *Even
         * header or footer the document's left and right pages have different
         * headers/footers. In case you get a *First header or footer the first
         * page of this section has a different header or footer.
         *
         * @param type The type of the header or footer. Always emitted in
         *             the order of definition in the enum
         */
        virtual void headerStart( HeaderData::Type type );
        /**
         * The end of the current header or footer.
         */
        virtual void headerEnd();

    };


    namespace Word97
    {
        struct TAP;
    }

    /**
     * The TableHandler class is the interface for all table related callbacks.
     * It informs about about starts and ends of table cells and rows.
     * Only invoking a table row functor triggers the parsing of the given table
     * row, and results in table(Row|Cell)(Start|End) callbacks.
     *
     * Word doesn't store information about table boundaries internally, but
     * it's quite easy to find out the start/end of a table, as all table
     * row functors are emitted in a row (pun intended). This means that if
     * you get a sequence of table row functors and then suddenly a
     * paragraphStart callback, you know that the table has ended.
     */
    class WV2_EXPORT TableHandler
    {
    public:
        virtual ~TableHandler();

        /**
         * Indicates the start of a table row with the passed properties.
         */
        virtual void tableRowStart( SharedPtr<const Word97::TAP> tap );
        /**
         * Indicates the end of a table row.
         */
        virtual void tableRowEnd();
        /**
         * This method is invoked every time we start processing a new cell.
         */
        virtual void tableCellStart();
        /**
         * This method is invoked every time we reach a cell end.
         */
        virtual void tableCellEnd();
    };


    class OLEImageReader;

    /**
     * The PictureHandler class is the interface for all image related
     * callbacks. All the image data is passed to the consumer via this
     * interface.
     */
    //NOTE: OBSOLETE, graphics are handled by the GraphicsHandler and we are
    //using msoscheme to parse Office Drawing Binary File Format structures.
    class WV2_EXPORT PictureHandler
    {
    public:
        /**
         * A small helper struct to express the dimensions of the passed
         * .wmf. A dimension of 0 indicates an invalid dimension.
         */
        // ###### FIXME: Do we really need that?
        /*
        struct WMFDimensions
        {
            WMFDimensions( const U8* rcWinMF )
            {
                left = readS16( rcWinMF );
                top = readS16( rcWinMF + 2 );
                width = readU16( rcWinMF + 4 );
                height = readU16( rcWinMF + 6 );
            }
            S16 left;
            S16 top;
            U16 width;
            U16 height;
        };
        */

        virtual ~PictureHandler();

        /**
         * This method is called when you invoke a PictureFunctor and the embedded
         * image is a bitmap. The bitmap data can be accessed using the OLEImageReader.
         * Note: The reader will only be valid until you return form that method, and
         * don't forget that you're directly accessing little-endian image data!
         */
        virtual void bitmapData( OLEImageReader& reader, SharedPtr<const Word97::PICF> picf );
        /**
         * This method is called when the image is escher data.
         */
        virtual void escherData( OLEImageReader& reader, SharedPtr<const Word97::PICF> picf, int type, const U8* rgbUid);
        virtual void escherData( std::vector<U8> data, SharedPtr<const Word97::PICF> picf, int type, const U8* rgbUid);
        /**
         * This method is called when you invoke a PictureFunctor and the embedded
         * image is a .wmf file. The data can be accessed using the OLEImageReader.
         * Note: The reader will only be valid until you return form that method, and
         * don't forget that you're directly accessing little-endian data!
         */
        virtual void wmfData( OLEImageReader& reader, SharedPtr<const Word97::PICF> picf );
        /**
         * Word allows to store .tif, .bmp, or .gif images externally.
         */
        virtual void externalImage( const UString& name, SharedPtr<const Word97::PICF> picf );

        /**
         * For the output of officeArt.
         */
        virtual void officeArt(wvWare::OfficeArtProperties *artProperties);
    };

    /**
     * The GraphicsHandler class is the interface for MS-ODRAW objects related
     * callbacks.  Office Drawing Binary File Format structures are parsed by
     * msoscheme.
     */
    class WV2_EXPORT GraphicsHandler
    {
    public:

        virtual ~GraphicsHandler();

        /**
         * This method gets called when a floating object is found.
	 * @param globalCP (character position)
         */
        virtual void handleFloatingObject(unsigned int globalCP);

	/**
	 * This method gets called when an inline object is found.  @param data
         * the picture properties and offset into data stream.
	 */
	virtual void handleInlineObject(const PictureData& data);
    };


    class ParagraphProperties;
    struct FLD;
    namespace Word97
    {
        struct CHP;
        struct SEP;
    }

    /**
     * The TextHandler class is the main worker in this filter. It tells you
     * about sections, paragraphs, and characters inside the document. If
     * you want to have a fast filter, try to make those methods fast, they
     * will get called very oftern.
     */
    class WV2_EXPORT TextHandler
    {
    public:
        virtual ~TextHandler();

        //////////////////////////////////////////////////////////////////////
        // Section related callbacks...
        /**
         * Denotes the start of a section.
         * The section properties are passed in the @p sep argument.
         */
        virtual void sectionStart( SharedPtr<const Word97::SEP> sep );
        virtual void sectionEnd();
        /**
         * A page break within a section.
         */
        virtual void pageBreak();
        /**
         * Emitted when we found headers or footers. The default implementation
         * simply invokes the functor. This function is called right after the
         * start of a new section.
         */
        virtual void headersFound( const HeaderFunctor& parseHeaders );

        //////////////////////////////////////////////////////////////////////
        // Paragraph related callbacks...
        /**
         * Denotes the start of a paragraph.
         * The paragraph properties are passed in the @p paragraphProperties argument.
         */
        virtual void paragraphStart( SharedPtr<const ParagraphProperties> paragraphProperties );
        virtual void paragraphEnd();

        /**
         * runOfText is the main worker in this API. It provides a chunk of text with
         * the properties passed. A run of text inside a Word document may encompass
         * an entire document, but in our implementation we chop those runs of text into
         * smaller pieces.
         * A run of text will never cross paragraph boundaries, even if the paragraph
         * properties of the two or more containing paragraphs are the same. It would
         * probably be slightly faster to do that, but IMHO it's just not worth the
         * pain this would cause.
         * @param text The text of this run, UString holds it as UCS-2 host order string.
         * @param chp The character properties attached to this run of text
         */
        virtual void runOfText( const UString& text, SharedPtr<const Word97::CHP> chp );

        //////////////////////////////////////////////////////////////////////
        // Special characters...
        /**
         * The enum only contains characters which are "easy" to handle (as they don't
         * need any further information). All the other fSpec characters are/will be
         * handled via functors. The values match the ones found in the specification.
         *
         * The current list is just a first draft, I have no idea how many of them will
         * be moved out because they're more complex as expected.
         */
        enum SpecialCharacter { CurrentPageNumber = 0, LineNumber = 6,
                                AbbreviatedDate = 10, TimeHMS = 11,
                                CurrentSectionNumber = 12, AbbreviatedDayOfWeek = 14,
                                DayOfWeek = 15, DayShort = 16, HourCurrentTime = 22,
                                HourCurrentTimeTwoDigits = 23, MinuteCurrentTime = 24,
                                MinuteCurrentTimeTwoDigits = 25, SecondsCurrentTime = 26,
                                AMPMCurrentTime = 27, CurrentTimeHMSOld = 28,
                                DateM = 29, DateShort = 30, MonthShort = 33,
                                YearLong = 34, YearShort = 35,
                                AbbreviatedMonth = 36, MonthLong = 37,
                                CurrentTimeHMS = 38, DateLong = 39 };

        /**
         * Very special characters (bad, bad name) are the ones which need additional
         * information from the file (i.e. the plain "put the current date there" isn't sufficent).
         */
        enum VerySpecialCharacter { Picture = 1, FootnoteAuto = 2, AnnotationRef = 5,
                                    DrawnObject = 8, FieldBegin = 19,
                                    FieldSeparator = 20, FieldEnd = 21, FieldEscapeChar = 92 };

        /**
         * special charachters that were dfined in parser9x.h  (fSpec = 1) but that weren't used.
         */
        enum UnusedSpecialCharacter {FootnoteSeparator = 3, FootnodeContinuation = 4, HandAnnotationPic = 7,
                                 AbbrevDate = 10, MergeHelper = 41};

        /**
         * This method passes the simple cases of special characters we find. More complex ones
         * will get their own callback.
         */
        virtual void specialCharacter( SpecialCharacter character, SharedPtr<const Word97::CHP> chp );

        /**
         * The parser found a footnote. The passed functor will trigger the parsing of this
         * footnote/endnote, the default implementation just emits the passed character
         * (unless it's an auto-numbered footnote, we won't emit ASCII 2 by default) with
         * runOfText (that it doesn't get lost if someone doesn't override this method) and
         * invokes the functor.
         */
        virtual void footnoteFound( FootnoteData::Type type, UString characters,
                                    SharedPtr<const Word97::CHP> chp, const FootnoteFunctor& parseFootnote);

        /**
         * The parser found an annotation. The passed functor will trigger the parsing of this
         * annotation, the default implementation just emits the passed character with
         * runOfText (that it doesn't get lost if someone doesn't override this method) and
         * invokes the functor.
         */
        virtual void annotationFound( UString characters,
                                      SharedPtr<const Word97::CHP> chp, const AnnotationFunctor& parseAnnotation);

        /**
         * This callback will get triggered when parsing a auto-numbered footnote.
         * The passed CHP is the character formatting information provided for the
         * "number"
         */
        virtual void footnoteAutoNumber( SharedPtr<const Word97::CHP> chp );

        /**
         * This callback indicates the start of a field. Fields consist of two parts, separated
         * by a field separator.
         * @param fld Describes the type of the field for live fields. May be 0!
         * @param chp The character properties of the field start (ASCII 19)
         */
        virtual void fieldStart( const FLD* fld, SharedPtr<const Word97::CHP> chp );
        /**
         * This callback separates the two parts of a field. The first part contains control
         * codes and keywords, the second part is the field result.
         * @param fld Describes the type of the field for live fields. May be 0!
         * @param chp The character properties of the field separator (ASCII 20)
         */
        virtual void fieldSeparator( const FLD* fld, SharedPtr<const Word97::CHP> chp );
        /**
         * The end of the field result is indicated by this callback, fields may be nested up
         * to 20 levels, so take care :-)
         * @param fld Describes the type of the field for live fields. May be 0!
         * @param chp The character properties of the field end (ASCII 21)
         */
        virtual void fieldEnd( const FLD* fld, SharedPtr<const Word97::CHP> chp );

        /**
         * This method is called every time we find a table row. The default
         * implementation invokes the functor, which triggers the parsing
         * process for the given table row.
         * @param tap the table row properties. Those are the same as the
         * ones you'll get when invoking the functor, but by having them here,
         * you can do some preprocessing on the whole table first.
         */
        virtual void tableRowFound( const TableRowFunctor& tableRow, SharedPtr<const Word97::TAP> tap );

        /**
         * This method is called every time we find an inline object.
         * @param data the picture data as defined by functordata.
	 * @param chp the character properties.
         */
        virtual void inlineObjectFound(const PictureData& data, SharedPtr<const Word97::CHP> chp);

        /**
         * This method is called every time we find a floating object.
	 * @param cp of a drawing
         */
        virtual void floatingObjectFound( unsigned int globalCP );

        /**
         * Denotes the start of a bookmark.
         */
        virtual void bookmarkStart( const BookmarkData& data );
        virtual void bookmarkEnd( const BookmarkData& data );
    };

} // namespace wvWare

#endif // HANDLERS_H
