/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#ifndef STYLES_H
#define STYLES_H

#include "../../msdoc.h"
#include "../../exceptions.h"

#include "word97_generated.h"
#include "wv2_export.h"

namespace wvWare
{

class OLEStreamReader;
class OLEStreamWriter;


namespace Word97
{

/**
 * STyle Definition (STD)
 */
struct STD
{
    // The Stdf structure contains an StdfBase structure that is followed by a
    // StdfPost2000OrNone structure which contains an optional StdfPost2000
    // structure.  This value MUST be 0x000A when the Stdf structure does not
    // contain an StdfPost2000 structure and MUST be 0x0012 when the Stdf
    // structure does contain an StdfPost2000 structure.  [MS-DOC] — v20101219
    enum StdfPost2000OrNone
    {
        StdfPost2000 = 0x0012,
        StdfPost2000None = 0x000A
    };

    /**
     * Creates an empty STD structure and sets the defaults
     */
    STD();
    /**
     * Simply calls read(...)
     */
    STD( U16 stdfSize, U16 totalSize, OLEStreamReader* stream, bool preservePos = false ) throw(InvalidFormatException);
    /**
     * Attention: This struct allocates memory on the heap
     */
    STD( const STD& rhs );
    ~STD();

    STD& operator=( const STD& rhs );

    /**
     * This method reads the STD structure from the stream.  If preservePos is
     * true we push/pop the position of the stream to save the state.  If it's
     * false the state of stream will be changed!
     *
     * @return true - success, false - failed
     */
    bool read( const U16 cbStd, const U16 stdfSize, OLEStreamReader* stream, bool preservePos = false ) throw(InvalidFormatException);

    /**
     * Same as reading :)
     */
    bool write( U16 baseSize, OLEStreamWriter* stream, bool preservePos = false ) const;

    /**
     * Set all the fields to the initial value (default is 0)
     */
    void clear();

    /**
     * Parts of the style sheet may be invalid.
     */
    bool isInvalid() const { return m_invalid; }

    // Data

    //[StdfBase] - BEGIN
    // ----------------------------------------
    /**
     * invariant style identifier
     */
    U16 sti:12;

    /**
     * spare field for any temporary use, always reset back to zero!
     */
    U16 fScratch:1;

    /**
     * PHEs of all text with this style are wrong
     */
    U16 fInvalHeight:1;

    /**
     * UPEs have been generated
     */
    U16 fHasUpe:1;

    /**
     * std has been mass-copied; if unused at save time, style should be deleted
     */
    U16 fMassCopy:1;

    /**
     * style type code
     */
    U16 sgc:4;

    /**
     * base style
     */
    U16 istdBase:12;

    /**
     * # of UPXs (and UPEs)
     */
    U16 cupx:4;

    /**
     * next style
     */
    U16 istdNext:12;

    /**
     * offset to end of upx's, start of upe's
     */
    U16 bchUpe;

    //[GRFSTD] - BEGIN
    // --------------------
    /**
     * auto redefine style when appropriate
     */
    U16 fAutoRedef:1;

    /**
     * hidden from UI?
     */
    U16 fHidden:1;

    /**
     * unused bits
     */
    U16 unused8_3:14;
    // --------------------
    //[GRFSTD] - END

    // ----------------------------------------
    //[StdfBase] - END

    //TODO: This info is not used at the moment.

    //[StdfPost2000OrNone] - BEGIN

    //StdfPost2000OrNone - structure specifies general information about a
    //style - This field is optional; Stshif.cbSTDBaseInFile defines whether it
    //is included or not.

    //[StdfPost2000OrNone] - END

    /**
     * sub-names are separated by chDelimStyle
     */
    UString xstzName;

    /**
     * A GrLPUpxSw structure that specifies the formatting for the style.
     */
    U8* grupx;

    // -------------------------
    // Internal, for bookkeeping
    // -------------------------
    U16 grupxLen;

    /**
     * Stores the information that this STD is invalid.  This helps us to
     * ignore any invalid parts of the style sheet.
     */
    bool m_invalid;

private:
    void clearInternal();

    /**
     * Read the name of the style.
     * @return true - success, false - fail
     */
    bool readStyleName( const U16 stdfSize, const U16 stdBytesLeft, OLEStreamReader* stream );
}; // STD

bool operator==( const STD& lhs, const STD& rhs );
bool operator!=( const STD& lhs, const STD& rhs );

}  // namespace Word97


class StyleSheet;
class ParagraphProperties;

/**
 * The structure to hold the UPE for character styles.
 */
struct UPECHPX
{
    UPECHPX() : istd( 0 ), cb( 0 ), grpprl( 0 ) {}
    ~UPECHPX() { delete [] grpprl; }

    U16 istd;
    U8 cb;
    U8* grpprl;

private:
    UPECHPX( const UPECHPX& rhs );
    UPECHPX& operator=( const UPECHPX& rhs );
};

/**
 * This class represents one single style.
 */
// TODO: It would maybe be worth to optimize the construction a bit, like creating
// the PAP and the CHP on copying from the base style, and not before
class WV2_EXPORT Style
{
public:

    Style( const U16 stdfSize, OLEStreamReader* tableStream, U16* ftc );

    /**
     * A special purpose constructor which creates an invalid Style
     * class which stores a copy of the provided CHPs.
     */
    explicit Style(const Word97::CHP& chp);

    ~Style();

    /**
     * Additional validation of the style, which requires the whole
     * stylesheet to be loaded to check the STD structure.  You MUST
     * call this one after the stylesheet is loaded, else the isValid
     * method informs only about the parsing errors.
     *
     * At the moment a number of selected repair procedures is
     * included.  Remember that the filter is NOT prepared to process
     * corrupt and invalid files.  If you relax the validation process
     * a number of corrupt files will squeeze through.
     */
    void validate(const U16 istd, const U16 rglpstd_cnt,
                  const std::vector<Style*>& styles, U16& udsNum);

    /**
     * Check if the style is valid.
     */
    bool isInvalid() const { return m_invalid; }

    /**
     * The stylesheet can have "empty" slots.
     */
    bool isEmpty() const { return m_isEmpty; }
    /**
     * Did we already unwrap the style?
     */
    bool isWrapped() const { return m_isWrapped; }

    /**
     * Unwrap the style and create a valid PAP/CHP.
     */
    void unwrapStyle( const StyleSheet& stylesheet, WordVersion version );

    /**
     * @return the (unique?) sti of that style
     */
    U16 sti() const;

    /**
     * @return the type of the style (paragraph/character style)
     */
    ST_StyleType type() const;

    /**
     * @return the style name
     */
    UString name() const;

    /**
     * @return ID of following style
     */
    U16 followingStyle() const;

    /**
     * For paragraph styles only.
     * @return a reference to Paragraph Properties (PAP)
     */
    const ParagraphProperties& paragraphProperties() const;

    /**
     * For paragraph styles only.
     * @return a reference to Character Properties (CHP)
     */
    const Word97::CHP& chp() const;

    /**
     * For character styles only.
     * @return a reference to the UPE array
     */
    const UPECHPX& upechpx() const;

private:
    Style( const Style& rhs );
    Style& operator=( const Style& rhs );

    /**
     * This helper method merges two CHPX structures to one (needed for
     * character styles).
     */
    void mergeUpechpx( const Style* parentStyle, WordVersion version );

    bool m_isEmpty;
    bool m_isWrapped;

    /**
     * Stores the information that this Style is invalid.  This helps us to
     * ignore any invalid parts of the style sheet.
     */
    bool m_invalid;
public:
    Word97::STD* m_std;
private:

    /**
     * UPEs are not stored in the file, they are a cache of the based-on chain.
     */
    // "mutable" in case someone goes mad with the styles.  We have to create a
    // default style to avoid crashes and still have to keep ownership!
    mutable ParagraphProperties *m_properties;
    mutable Word97::CHP *m_chp;
    mutable UPECHPX *m_upechpx;
}; // Style


/**
 * This class holds all the styles this Word document
 * defines. You can query styles by ID (sti) or index (istd)
 */
class WV2_EXPORT StyleSheet
{
public:
    StyleSheet( OLEStreamReader* tableStream, U32 fcStshf, U32 lcbStshf ) throw(InvalidFormatException);
    ~StyleSheet();

    /**
     * Check if the stylesheet information is valid.  At the moment only the
     * number of styles is checked against the allowed number.
     */
    bool valid() const;

    /**
     * Check the beginning of the rglpstd array, it is reserved for specific
     * fixed-index application-defined styles.  A particular fixed-index,
     * application-defined style always has the same istd value in every
     * stylesheet.
     */
    bool fixed_index_valid() const;

    /**
     * Return the number of styles.
     */
    unsigned int size() const;

    /**
     * @return 0 in case the style sheet does not contain the requested style.
     */
    const Style* styleByIndex( U16 istd ) const;

    /**
     * @return 0 in case the style sheet does not contain the requested style.
     */
    const Style* styleByID( U16 sti ) const;

    U16 indexByID( U16 sti, bool& ok ) const;

private:
    StyleSheet( const StyleSheet& rhs );
    StyleSheet& operator=( const StyleSheet& rhs );

    Word97::STSHI m_stsh;
    std::vector<Style*> m_styles;

    //Number of user defined styles with empty style name.
    U16 m_udsNum;

}; // StyleSheet

}  // namespace wvWare

#endif // STYLES_H
