/* This file is part of the KDE project
 * Copyright (C) 2006-2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007,2008 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007-2011 Pierre Ducroquet <pinaraf@gmail.com>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KOPARAGRAPHSTYLE_H
#define KOPARAGRAPHSTYLE_H

#include "KoCharacterStyle.h"
#include "KoText.h"
#include "kotext_export.h"

#include <KoXmlReaderForward.h>
#include <KoBorder.h>

#include <QVariant>
#include <QTextFormat>

extern QVariant val;
class KoShadowStyle;
class KoListStyle;
class QTextBlock;
class KoGenStyle;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoList;

/**
 * A container for all properties for the paragraph wide style.
 * Each paragraph in the main text either is based on a parag style, or its not. Where
 * it is based on a paragraph style this is indicated that it has a property 'StyleId'
 * with an integer as value.  The integer value corresponds to the styleId() output of
 * a specific KoParagraphStyle.
 * @see KoStyleManager
 */
class KOTEXT_EXPORT KoParagraphStyle : public KoCharacterStyle
{
    Q_OBJECT
public:
    enum Property {
        // Every 10 properties, the decimal number shown indicates the decimal offset over the QTextFormat::UserProperty enum value
        StyleId = QTextFormat::UserProperty + 1,
        // Linespacing properties
        PercentLineHeight,  ///< this property is used for a percentage of the highest character on that line
        FixedLineHeight,    ///< this property is used to use a non-default line height
        MinimumLineHeight,  ///< this property is used to have a minimum line spacing
        LineSpacing,        ///< Hard leader height.
        LineSpacingFromFont,  ///< if false, use fontsize (in pt) solely, otherwise respect font settings
        AlignLastLine,      ///< When the paragraph is justified, what to do with the last word line
        WidowThreshold,     ///< If 'keep together'=false, amount of lines to keep it anyway.
        OrphanThreshold,   ///< If 'keep together'=false, amount of lines to keep it anyway.
        DropCaps, /*10*/   ///< defines if a paragraph renders its first char(s) with drop-caps
        DropCapsLength, ///< Number of glyphs to show as drop-caps
        DropCapsLines,  ///< Number of lines that the drop-caps span
        DropCapsDistance,   ///< Distance between drop caps and text
        DropCapsTextStyle,  ///< Text style of dropped chars.
        FollowDocBaseline,  ///< If true the baselines will be aligned with the doc-wide grid

        // border stuff
        HasLeftBorder,  ///< If true, paint a border on the left
        HasTopBorder,   ///< If true, paint a border on the top
        HasRightBorder, ///< If true, paint a border on the right
        HasBottomBorder,///< If true, paint a border on the bottom
        BorderLineWidth, /*20*/ ///< Thickness of inner-border
        SecondBorderLineWidth,  ///< Thickness of outer-border
        DistanceToSecondBorder, ///< Distance between inner and outer border
        LeftPadding,    ///< distance between text and border
        TopPadding,     ///< distance between text and border
        RightPadding,   ///< distance between text and border
        BottomPadding,   ///< distance between text and border
        LeftBorderWidth,        ///< The thickness of the border, or 0 if there is no border
        LeftInnerBorderWidth,   ///< In case of style being 'double' the thickness of the inner border line
        LeftBorderSpacing,      ///< In case of style being 'double' the space between the inner and outer border lines
        LeftBorderStyle, /*30*/ ///< The border style. (see BorderStyle)
        LeftBorderColor,        ///< The border Color
        TopBorderWidth,         ///< The thickness of the border, or 0 if there is no border
        TopInnerBorderWidth,    ///< In case of style being 'double' the thickness of the inner border line
        TopBorderSpacing,       ///< In case of style being 'double' the space between the inner and outer border lines
        TopBorderStyle,         ///< The border style. (see BorderStyle)
        TopBorderColor,         ///< The border Color
        RightBorderWidth,       ///< The thickness of the border, or 0 if there is no border
        RightInnerBorderWidth,  ///< In case of style being 'double' the thickness of the inner border line
        RightBorderSpacing,     ///< In case of style being 'double' the space between the inner and outer border lines
        RightBorderStyle, /*40*/ ///< The border style. (see BorderStyle)
        RightBorderColor,       ///< The border Color
        BottomBorderWidth,      ///< The thickness of the border, or 0 if there is no border
        BottomInnerBorderWidth, ///< In case of style being 'double' the thickness of the inner border line
        BottomBorderSpacing,    ///< In case of style being 'double' the space between the inner and outer border lines
        BottomBorderStyle,      ///< The border style. (see BorderStyle)
        BottomBorderColor,      ///< The border Color

        // lists
        ListStyleId,            ///< Style Id of associated list style
        ListStartValue,         ///< Int with the list-value that that parag will have. Ignored if this is not a list.
        RestartListNumbering,   ///< boolean to indicate that this paragraph will have numbering restart at the list-start. Ignored if this is not a list.
        ListLevel, /*50*/       ///< int with the list-level that the paragraph will get when this is a list (numbered paragraphs)
        IsListHeader,           ///< bool, if true the paragraph shows up as a list item, but w/o a list label.
        UnnumberedListItem,     ///< bool. if true this paragraph is part of a list but is not numbered

        AutoTextIndent,         ///< bool, says whether the paragraph is auto-indented or not

        TabStopDistance,        ///< Double, Length. specifies that there's a tab stop every n inches
        ///< (after the last of the TabPositions, if any)
        TabPositions,           ///< A list of tab positions
        TextProgressionDirection,

        MasterPageName,         ///< Optional name of the master-page

        OutlineLevel,            ///< Outline level for headings
        DefaultOutlineLevel,

        // numbering
        LineNumbering,         /*60*/   ///< bool, specifies whether lines should be numbered in this paragraph
        LineNumberStartValue, ///< integer value that specifies the number for the first line in the paragraph
        SectionStartings,            ///< list of section definitions
        SectionEndings,               ///< list <end of a named section>
// do 15.5.24
// continue at 15.5.28
        ForceDisablingList,       ///< bool, for compatibility with the weird text:enable-numbering attribute not used anymore by OpenOffice.org

        // other properties
        BackgroundTransparency,   ///< qreal between 0 and 1, background transparency
        SnapToLayoutGrid,         ///< bool, snap the paragraph to the layout grid of the page
        JoinBorder,               ///< bool, whether a border for one paragraph is to be extended around the following paragraph
        RegisterTrue,             ///< bool, align lines on both sides of a printed text
        StrictLineBreak,          ///< bool, if true, line breaks are forbidden between some characters
        JustifySingleWord,        ///< bool, if true, a single word will be justified
        BreakBefore,              ///< KoText::TextBreakProperty, whether there is a page/column break before the paragraphs
        BreakAfter,               ///< KoText::TextBreakProperty, whether there is a page/column break after the paragraphs
        AutomaticWritingMode,     ///< bool
        PageNumber,               ///< int, 0 means auto (ie. previous page number + 1), N sets up a new page number
        TextAutoSpace,            ///< AutoSpace, indicating whether to add space between portions of Asian, Western and complex texts
        KeepWithNext,             ///< Try to keep this block with its following block on the same page
        KeepHyphenation,          ///< bool, whether both parts of a hyphenated word shall lie within a single page
        HyphenationLadderCount,   ///< int, 0 means no limit, else limit the number of successive hyphenated line areas in a block
        PunctuationWrap,          ///< bool, whether a punctuation mark can be at the end of a full line (false) or not (true)
        VerticalAlignment,        ///< KoParagraphStyle::VerticalAlign, the alignment of this paragraph text
        HiddenByTable,        ///< don't let this paragraph have any height

        NormalLineHeight,         ///< bool, internal property for reserved usage
        BibliographyData,

        TableOfContentsData,      // set when block is inside a TableOfContents
        GeneratedDocument,  // set when block is inside a generated document
        Shadow,                    //< KoShadowStyle, the shadow of this paragraph
        NextStyle,                  ///< holds the styleId of the style to be used on a new paragraph
        ParagraphListStyleId,        ///< this holds the listStyleId of the list got from style:list-style-name property from ODF 1.2
        EndCharStyle           // QSharedPointer<KoCharacterStyle>  used when final line is empty
    };

    enum AutoSpace {
        NoAutoSpace,              ///< space should not be added between portions of Asian, Western and complex texts
        IdeographAlpha            ///< space should be added between portions of Asian, Western and complex texts
    };

    enum VerticalAlign {
        VAlignAuto,
        VAlignBaseline,
        VAlignBottom,
        VAlignMiddle,
        VAlignTop
    };

    /// Constructor
    KoParagraphStyle(QObject *parent = 0);
    /// Creates a KoParagrahStyle with the given block format, the block character format and \a parent
    KoParagraphStyle(const QTextBlockFormat &blockFormat, const QTextCharFormat &blockCharFormat, QObject *parent = 0);
    /// Destructor
    ~KoParagraphStyle() override;

    KoCharacterStyle::Type styleType() const override;

    /// Creates a KoParagraphStyle that represents the formatting of \a block.
    static KoParagraphStyle *fromBlock(const QTextBlock &block, QObject *parent = 0);

    /// creates a clone of this style with the specified parent
    KoParagraphStyle *clone(QObject *parent = 0) const;

    //  ***** Linespacing
    /**
     * Sets the line height as a percentage of the highest character on that line.
     * A good typographically correct value would be 120%
     * Note that lineSpacing() is added to this.
     * You should consider doing a remove(KoParagraphStyle::LineSpacing); because if set, it will
     *  be used instead of this value.
     * @see setLineSpacingFromFont
     */
    void setLineHeightPercent(qreal lineHeight);
    /// @see setLineHeightPercent
    qreal lineHeightPercent() const;

    /**
     * Sets the line height to a specific pt-based height, ignoring the font size.
     * Setting this will ignore the lineHeightPercent() and lineSpacing() values.
     */
    void setLineHeightAbsolute(qreal height);
    /// @see setLineHeightAbsolute
    qreal lineHeightAbsolute() const;

    /**
     * Sets the line height to have a minimum height in pt.
     * You should consider doing a remove(KoParagraphStyle::FixedLineHeight); because if set, it will
     *  be used instead of this value.
     */
    void setMinimumLineHeight(const QTextLength &height);
    /// @see setMinimumLineHeight
    qreal minimumLineHeight() const;

    /**
     * Sets the space between two lines to be a specific height. The total linespacing will become
     * the line height + this height.  Where the line height is dependent on the font.
     * You should consider doing a remove(KoParagraphStyle::FixedLineHeight) and a
     * remove(KoParagraphStyle::PercentLineHeight); because if set, they will be used instead of this value.
     */
    void setLineSpacing(qreal spacing);
    /// @see setLineSpacing
    qreal lineSpacing() const;

    /**
     * Set the line-height to "normal". This overwrites a line-height set before either
     * with \a setLineHeightAbsolute or \a setMinimumLineHeight . If set then a value
     * set with \a setLineSpacing will be ignored.
     */
    void setNormalLineHeight();
    /// @see setNormalLineHeight
    bool hasNormalLineHeight() const;

    /**
     * If set to true the font-encoded height will be used instead of the font-size property
     * This property influences setLineHeightPercent() behavior.
     * When off (default) a font of 12pt will always have a linespacing of 12pt times the
     * current linespacing percentage.  When on the linespacing embedded in the font
     * is used which can differ for various fonts, even if they are the same pt-size.
     */
    void setLineSpacingFromFont(bool on);
    /**
     * @see setLineSpacingFromFont
     */
    bool lineSpacingFromFont() const;


    /**
     * For paragraphs that are justified the last line alignment is specified here.
     * There are only 3 valid options, Left, Center and Justified. (where Left will
     * be right aligned for RTL text).
     */
    void setAlignLastLine(Qt::Alignment alignment);
    /**
     * @see setAlignLastLine
     */
    Qt::Alignment alignLastLine() const;
    /**
     * Paragraphs that are broken across two frames are normally broken at the bottom
     * of the frame.  Using this property we can set the minimum number of lines that should
     * appear in the second frame to avoid really short paragraphs standing alone (also called
     * widows).  So, if a 10 line parag is broken in a way that only one line is in the second
     * frame, setting a widowThreshold of 4 will break at 6 lines instead to leave the
     * requested 4 lines.
     */

    void setWidowThreshold(int lines);
    /**
     * @see setWidowThreshold
     */
    int widowThreshold() const;
    /**
     * Paragraphs that are broken across two frames are normally broken at the bottom
     * of the frame.  Using this property we can set the minimum number of lines that should
     * appear in the first frame to avoid really short paragraphs standing alone (also called
     * orphans).  So, if a paragraph is broken so only 2 line is left in the first frame
     * setting the orphanThreshold to something greater than 2 will move the whole paragraph
     * to the second frame.
     */

    void setOrphanThreshold(int lines);
    /**
     * @see setOrphanThreshold
     */
    int orphanThreshold() const;
    /**
     * If true, make the first character span multiple lines.
     * @see setDropCapsLength
     * @see setDropCapsLines
     * @see dropCapsDistance
     */

    void setDropCaps(bool on);
    /**
     * @see setDropCaps
     */
    bool dropCaps() const;
    /**
     * Set the number of glyphs to show as drop-caps
     * @see setDropCaps
     * @see setDropCapsLines
     * @see dropCapsDistance
     */

    void setDropCapsLength(int characters);
    /**
     * set dropCaps Length in characters
     * @see setDropCapsLength
     */
    int dropCapsLength() const;
    /**
     * Set the number of lines that the drop-caps span
     * @see setDropCapsLength
     * @see setDropCaps
     * @see dropCapsDistance
     */

    void setDropCapsLines(int lines);
    /**
     * The dropCapsLines
     * @see setDropCapsLines
     */
    int dropCapsLines() const;
    /**
     * set the distance between drop caps and text in pt
     * @see setDropCapsLength
     * @see setDropCaps
     * @see setDropCapsLines
     */

    void setDropCapsDistance(qreal distance);
    /**
     * The dropCaps distance
     * @see setDropCapsDistance
     */
    qreal dropCapsDistance() const;

    /**
     * Set the style id of the text style used for dropcaps
     * @see setDropCapsDistance
     */
    void setDropCapsTextStyleId(int id);

    /**
     * The style id of the text style used for dropcaps
     * @see setDropCapsTextStyleId
     */
    int dropCapsTextStyleId() const;

    /**
     * If true the baselines will be aligned with the doc-wide grid
     */
    void setFollowDocBaseline(bool on);
    /**
     * return if baseline alignment is used
     * @see setFollowDocBaseline
     */
    bool followDocBaseline() const;

    /// See similar named method on QTextBlockFormat
    void setBackground(const QBrush &brush);
    /// See similar named method on QTextBlockFormat
    QBrush background() const;
    /// See similar named method on QTextBlockFormat
    void clearBackground();

    qreal backgroundTransparency() const;
    void setBackgroundTransparency(qreal transparency);

    bool snapToLayoutGrid() const;
    void setSnapToLayoutGrid(bool value);

    bool registerTrue() const;
    void setRegisterTrue(bool value);

    bool strictLineBreak() const;
    void setStrictLineBreak(bool value);

    bool justifySingleWord() const;
    void setJustifySingleWord(bool value);

    bool automaticWritingMode() const;
    void setAutomaticWritingMode(bool value);

    void setPageNumber(int pageNumber);
    int pageNumber() const;

    void setKeepWithNext(bool value);
    bool keepWithNext() const;

    void setPunctuationWrap(bool value);
    bool punctuationWrap() const;

    void setTextAutoSpace(AutoSpace value);
    AutoSpace textAutoSpace() const;

    void setKeepHyphenation(bool value);
    bool keepHyphenation() const;

    void setHyphenationLadderCount(int value);
    int hyphenationLadderCount() const;

    VerticalAlign verticalAlignment() const;
    void setVerticalAlignment(VerticalAlign value);

    void setBreakBefore(KoText::KoTextBreakProperty value);
    KoText::KoTextBreakProperty breakBefore() const;
    void setBreakAfter(KoText::KoTextBreakProperty value);
    KoText::KoTextBreakProperty breakAfter() const;
    void setLeftPadding(qreal padding);
    qreal leftPadding() const;
    void setTopPadding(qreal padding);
    qreal topPadding() const;
    void setRightPadding(qreal padding);
    qreal rightPadding() const;
    void setBottomPadding(qreal padding);
    qreal bottomPadding() const;
    void setPadding(qreal padding);

    void setLeftBorderWidth(qreal width);
    qreal leftBorderWidth() const;
    void setLeftInnerBorderWidth(qreal width);
    qreal leftInnerBorderWidth() const;
    void setLeftBorderSpacing(qreal width);
    qreal leftBorderSpacing() const;
    void setLeftBorderStyle(KoBorder::BorderStyle style);
    KoBorder::BorderStyle leftBorderStyle() const;
    void setLeftBorderColor(const QColor &color);
    QColor leftBorderColor() const;
    void setTopBorderWidth(qreal width);
    qreal topBorderWidth() const;
    void setTopInnerBorderWidth(qreal width);
    qreal topInnerBorderWidth() const;
    void setTopBorderSpacing(qreal width);
    qreal topBorderSpacing() const;
    void setTopBorderStyle(KoBorder::BorderStyle style);
    KoBorder::BorderStyle topBorderStyle() const;
    void setTopBorderColor(const QColor &color);
    QColor topBorderColor() const;
    void setRightBorderWidth(qreal width);
    qreal rightBorderWidth() const;
    void setRightInnerBorderWidth(qreal width);
    qreal rightInnerBorderWidth() const;
    void setRightBorderSpacing(qreal width);
    qreal rightBorderSpacing() const;
    void setRightBorderStyle(KoBorder::BorderStyle style);
    KoBorder::BorderStyle rightBorderStyle() const;
    void setRightBorderColor(const QColor &color);
    QColor rightBorderColor() const;
    void setBottomBorderWidth(qreal width);
    qreal bottomBorderWidth() const;
    void setBottomInnerBorderWidth(qreal width);
    qreal bottomInnerBorderWidth() const;
    void setBottomBorderSpacing(qreal width);
    qreal bottomBorderSpacing() const;
    void setBottomBorderStyle(KoBorder::BorderStyle style);
    KoBorder::BorderStyle bottomBorderStyle() const;
    void setBottomBorderColor(const QColor &color);
    QColor bottomBorderColor() const;

    bool joinBorder() const;
    void setJoinBorder(bool value);

    KoText::Direction textProgressionDirection() const;
    void setTextProgressionDirection(KoText::Direction dir);

    // ************ properties from QTextBlockFormat
    /// duplicated property from QTextBlockFormat
    void setTopMargin(QTextLength topMargin);
    /// duplicated property from QTextBlockFormat
    qreal topMargin() const;
    /// duplicated property from QTextBlockFormat
    void setBottomMargin(QTextLength margin);
    /// duplicated property from QTextBlockFormat
    qreal bottomMargin() const;
    /// duplicated property from QTextBlockFormat
    void setLeftMargin(QTextLength margin);
    /// duplicated property from QTextBlockFormat
    qreal leftMargin() const;
    /// duplicated property from QTextBlockFormat
    void setRightMargin(QTextLength margin);
    /// duplicated property from QTextBlockFormat
    qreal rightMargin() const;
    /// set the margin around the paragraph, making the margin on all sides equal.
    void setMargin(QTextLength margin);

    void setIsListHeader(bool on);
    bool isListHeader() const;

    /// duplicated property from QTextBlockFormat
    void setAlignment(Qt::Alignment alignment);
    /// duplicated property from QTextBlockFormat
    Qt::Alignment alignment() const;
    /// duplicated property from QTextBlockFormat
    void setTextIndent(QTextLength margin);
    /// duplicated property from QTextBlockFormat
    qreal textIndent() const;
    /// Custom KoParagraphStyle property for auto-text-indent
    void setAutoTextIndent(bool on);
    bool autoTextIndent() const;

    /// duplicated property from QTextBlockFormat
    void setNonBreakableLines(bool on);
    /// duplicated property from QTextBlockFormat
    bool nonBreakableLines() const;

    /// set the default style this one inherits its unset properties from if no parent style.
    void setDefaultStyle(KoParagraphStyle *parent);

    /// set the parent style this one inherits its unset properties from.
    void setParentStyle(KoParagraphStyle *parent);

    /// return the parent style
    KoParagraphStyle *parentStyle() const;

    /// the 'next' style is the one used when the user creates a new paragraph after this one.
    void setNextStyle(int next);

    /// the 'next' style is the one used when the user creates a new paragraph after this one.
    int nextStyle() const;

    /// return the name of the style.
    QString name() const;

    /// set a user-visible name on the style.
    void setName(const QString &name);

    /// each style has a unique ID (non persistent) given out by the styleManager
    int styleId() const;

    /// each style has a unique ID (non persistent) given out by the styleManager
    void setStyleId(int id);

    /// return the optional name of the master-page or a QString() if this paragraph isn't attached to a master-page.
    QString masterPageName() const;
    /// Set the name of the master-page.
    void setMasterPageName(const QString &name);


    /// Set the list start value
    void setListStartValue(int value);
    /// Returns the list start value
    int listStartValue() const;

    /// Set to true if this paragraph is marked to start the list numbering from the first entry.
    void setRestartListNumbering(bool on);
    /// return if this paragraph is marked to start the list numbering from the first entry.
    bool restartListNumbering();

    /// Set the tab stop distance for this paragraph style.
    void setTabStopDistance(qreal value);
    /// return the tab stop distance for this paragraph style
    qreal tabStopDistance() const;
    /// Set the tab data for this paragraph style.
    void setTabPositions(const QVector<KoText::Tab> &tabs);
    /// return the tabs data for this paragraph style
    QVector<KoText::Tab> tabPositions() const;

    /// If this style is a list, then this sets the nested-ness (aka level) of this paragraph.  A H2 has level 2.
    void setListLevel(int value);
    /// return the list level.
    int listLevel() const;

    /**
     * Return the outline level of this block, or 0 if it's not a heading.
     * This information is here and not in the styles because the OpenDocument specification says so.
     * See ODF Spec 1.1, §14.1, Outline Numbering Level, but also other parts of the specification.
     */
    int outlineLevel() const;

    /**
     * Change this block outline level
     */
    void setOutlineLevel(int outline);

    /**
     * Return the default outline level of this style, or 0 if there is none.
     */
    int defaultOutlineLevel() const;

    /**
     * Change the default outline level for this style.
     */
    void setDefaultOutlineLevel(int outline);


    /**
     * 15.5.30: The text:number-lines attribute controls whether or not lines are numbered
     */
    bool lineNumbering() const;
    void setLineNumbering(bool lineNumbering);

    /**
     * 15.5.31:
     * The text:line-number property specifies a new start value for line numbering. The attribute is
     * only recognized if there is also a text:number-lines attribute with a value of true in the
     * same properties element.
     */
    int lineNumberStartValue() const;
    void setLineNumberStartValue(int lineNumberStartValue);

    /**
     * 20.349 style:shadow
     * The style:shadow attribute specifies a shadow effect.
     * The defined values for this attribute are those defined in §7.16.5 of [XSL], except the value
     * inherit.
     * The shadow effect is not applied to the text content of an element, but depending on the element
     * where the attribute appears, to a paragraph, a text box, a page body, a header, a footer, a table
     * or a table cell.
     */
    KoShadowStyle shadow() const;
    void setShadow (const KoShadowStyle &shadow);

    /// copy all the properties from the other style to this style, effectively duplicating it.
    void copyProperties(const KoParagraphStyle *style);

    void unapplyStyle(QTextBlock &block) const;

    /**
     * Apply this style to a blockFormat by copying all properties from this, and parent
     * styles to the target block format.  Note that the character format will not be applied
     * using this method, use the other applyStyle() method for that.
     */
    void applyStyle(QTextBlockFormat &format) const;

    /**
     * Apply this style to the textBlock by copying all properties from this, parent and
     * the character style (where relevant) to the target block formats.
     */
    void applyStyle(QTextBlock &block, bool applyListStyle = true) const;
/*
    /// return the character "properties" for this paragraph style, Note it does not inherit
    KoCharacterStyle *characterStyle();
    /// return the character "properties" for this paragraph style, Note it does not inherit
    const KoCharacterStyle *characterStyle() const;
    /// set the character "properties" for this paragraph style
    void setCharacterStyle(KoCharacterStyle *style);
*/
    /**
     * Returns the list style for this paragraph style.
     * @see KoListStyle::isValid()
     * @see setListStyle()
     * @see removeListStyle()
     */
    KoListStyle *listStyle() const;
    /**
     * Set a new liststyle on this paragraph style, making all paragraphs that use this style
     *  automatically be part of the list.
     * @see setListStyle()
     * @see removeListStyle()
     */
    void setListStyle(KoListStyle *style);

    void remove(int key);

    /// Compare the paragraph, character and list properties of this style with the other
    bool operator==(const KoParagraphStyle &other) const;
    /// Compare the paragraph properties of this style with other
    bool compareParagraphProperties(const KoParagraphStyle &other) const;

    void removeDuplicates(const KoParagraphStyle &other);

    /**
     * Load the style form the element
     *
     * @param context the odf loading context
     * @param element the element containing the style
     * @param loadParents true = use the stylestack, false = use just the element
     */
    void loadOdf(const KoXmlElement *element, KoShapeLoadingContext &context,
                bool loadParents = false);

    void saveOdf(KoGenStyle &style, KoShapeSavingContext &context) const;

    /**
     * Returns true if this paragraph style has the property set.
     * Note that this method does not delegate to the parent style.
     * @param key the key as found in the Property enum
     */
    bool hasProperty(int key) const;

    /**
     * Set a property with key to a certain value, overriding the value from the parent style.
     * If the value set is equal to the value of the parent style, the key will be removed instead.
     * @param key the Property to set.
     * @param value the new value to set on this style.
     * @see hasProperty(), value()
     */
    void setProperty(int key, const QVariant &value);
    /**
     * Return the value of key as represented on this style, taking into account parent styles.
     * You should consider using the direct accessors for individual properties instead.
     * @param key the Property to request.
     * @returns a QVariant which holds the property value.
     */
    QVariant value(int key) const;
    /**
     * Returns true if this pragraph style has default properties
     * Note that the value of StyleId property is not considered
     */
    bool hasDefaults() const;

    KoList *list() const;

    void applyParagraphListStyle(QTextBlock &block, const QTextBlockFormat &blockFormat) const;

    /** Returns true if the style is in use.
     */
    bool isApplied() const;


Q_SIGNALS:
    void styleApplied(const KoParagraphStyle*);

private:
    /**
     * Load the style from the \a KoStyleStack style stack using the
     * OpenDocument format.
     */
    void loadOdfProperties(KoShapeLoadingContext &scontext);
    qreal propertyDouble(int key) const;
    QTextLength propertyLength(int key) const;
    int propertyInt(int key) const;
    bool propertyBoolean(int key) const;
    QColor propertyColor(int key) const;

    class Private;
    Private * const d;
};
Q_DECLARE_METATYPE(KoParagraphStyle *)
Q_DECLARE_METATYPE(const KoParagraphStyle *)
Q_DECLARE_METATYPE(QSharedPointer<KoParagraphStyle>)
#endif
