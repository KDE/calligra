/* This file is part of the KDE project
 * Copyright (C) 2006-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C) 2010 Nandita Suri <suri.nandita@gmail.com>
 * Copyright (C) 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 * Copyright (C) 2011-2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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
#ifndef KOLISTSTYLE_H
#define KOLISTSTYLE_H

#include "kotext_export.h"

#include <QTextFormat>
#include <QTextListFormat>

#include <KoXmlReader.h>

class KoListLevelProperties;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoGenStyle;

class QTextBlock;

/**
 * This class groups all styling-options for lists.
 * See KoParagraphStyle::setListStyle()
 * The list-style represents several list-levels, where each level is represented by the
 * KoListLevelProperties class. The top most list level is 1.
 *
 * list level1
 *   list level2
 *   list level2
 *     list level3
 * list level1
 *
 * A list-style as such represents cross paragraph relations. The most obvious evidence of this
 * is with a numbered list where a counter is automatically increased from one paragraph to the next.
 *
 * If the list is interrupted by a praragraph with another list-style the counting will start from
 * fresh when the list resumes. However you can set the list to continue if you like.
 *
 * Following from the above you can use the same paragraph style for several paragraphs and the
 * the counter will increase. If you want a paragraph to be on a sub level you do however need to
 * create a new paragraph-style when another listLevel set.
 */
class  KOTEXT_EXPORT KoListStyle : public QObject
{
    Q_OBJECT
    Q_ENUMS(Style Property)
public:
    // ListIdType will be 32-bit in 32 bit machines and 64 bit in 64 bit machines
    typedef quintptr ListIdType;

    /// This is used to specify what type of list label to use
    enum LabelType {
        /// List style with no numbering
        None = 1,
        /// an unicode char for the bullet
        BulletCharLabelType,
        /// an image for the bullet
        ImageLabelType,
        NumberLabelType
    };

    /// further properties
    enum Property {
        // Housekeeping
        Level = QTextFormat::UserProperty + 1000,          ///< list nesting level, is 1 or higher, or zero when implied
        ListId,         ///< A group of lists together are called 1 (user intended) list in ODF. Store the listId here
        StyleId,        ///< The id stored in the listFormat to link the list to this style.
        IsOutline,      ///< If true then this list is an outline list (for header paragraphs)

        // Label
        NumberFormat, ///< The KoOdfNumberFormat::FormattingSpecification
        ListItemPrefix, ///< The text to be printed before the listItem
        ListItemSuffix, ///< The text to be printed after the listItem
        StartValue,     ///< First value to use
        DisplayLevel,   ///< show this many parent levels. Is always lower than the (implied) level.
        CharacterStyleId,///< CharacterStyle used for markup of the counter
        CharacterProperties, ///< This stores the character properties of the list style
        BulletCharacter,///< an int with the unicode value of the character (for CustomCharItem)
        RelativeBulletSize,     ///< size in percent relative to the height of the text
        LetterSynchronization, ///< If letters are used for numbering, when true increment all at the same time. (aa, bb)
        Width,          ///< The width, in pt, of  a picture bullet.
        Height,         ///< The height, in pt, of a picture bullet.
        BulletImage,    ///< Bullet image stored as a key for lookup in the imageCollection

        // Geometry mode
        AlignmentMode,   ///< Is true if list-level-position-and-space-mode=label-alignment

        // Geometry properties for old mode
        Indent,         ///< The space (margin) to include for all paragraphs
        MinimumWidth,   ///< The minimum width, in pt, of the listItem including the prefix/suffix.
        MinimumDistance, ///< The minimum distance, in pt, between the counter and the text

        // Geometry properties for label-alignment mode
        Alignment,      ///< Alignment of the counter
        Margin,         ///< Stores the margin of the list - not ODF but convenience when auto generating new levels
        MarginIncrease, ///< Stores the margin increase of the list
        TextIndent,     ///< Stores the text indent of list item
        LabelFollowedBy,  ///< Label followed by one of the enums ListLabelFollowedBy
        TabStopPosition   ///< Specifies the additional tab stops
    };

    enum ListLabelFollowedBy
    {
        ListTab,  ///< Label is followed by a list tab
        Space,    ///< Label followed by a Space
        Nothing      ///< Nothing is present between label and the text
    };

    /**
     * Constructor
     * Create a new list style which uses numbered (KoListStyle::ListDecimal) listitems.
     */
    explicit KoListStyle(QObject *parent = 0);

    /// Destructor
    ~KoListStyle() override;

    /// creates a clone of this style with the specified parent
    KoListStyle *clone(QObject *parent = 0);

    /// each style has a unique ID (non persistent) given out by the styleManager
    int styleId() const;

    /// each style has a unique ID (non persistent) given out by the styleManager
    void setStyleId(int id);

    /**
     * Return the properties for the specified list level.
     * A list style can contain properties for more than one level, when a paragraph is added to this list
     * it will be added at a certain level and it will then be using the properties of that level.
     * The gain from using one list style for multiple levels is in allowing a way to format the list label.
     * A list item which is of level '4' will be able to have a display level of up to 4, which means that not
     * only is the counter of the current level displayed, the counters of the higher levels can be displayed as
     * well.
     * Adding level properties for lower levels will have the effect that the counter of that level will be displayed
     * in the specified format instead of being inherited from the list style at the higher level.
     */
    KoListLevelProperties levelProperties(int level) const;

    /**
     * Set the properties for a level.
     * @param properties the new properties for the level, including the level number.
     * @see level()
     */
    void setLevelProperties(const KoListLevelProperties &properties);

    /**
     * @return if there are the properties for a level set.
     * @param level the level for which to check
     * @see level()
     */
    bool hasLevelProperties(int level) const;

    /**
     * Remove any properties that were set for a level.
     * @param level the level for which to remove
     * @see level()
     */
    void removeLevelProperties(int level);

    /// return a QTextListFormat that contains formatting information for the level.
    QTextListFormat listFormat(int level) const;

    /// return the configured list levels that hasLevelProperties would return true to.
    QList<int> listLevels() const;

    /// return the name of the style.
    QString name() const;

    /// set a user-visible name on the style.
    void setName(const QString &name);

    /**
     * Apply this style to a block by adding the block to the proper list.
     */
    void applyStyle(const QTextBlock &block, int level = 0);

    bool operator==(const KoListStyle &other) const;
    bool operator!=(const KoListStyle &other) const;

    /**
     * Load the style from the \a KoStyleStack style stack using the
     * OpenDocument format.
     */
    void loadOdf(KoShapeLoadingContext& context, const KoXmlElement& style = KoXmlElement());

    /**
     * Save the style to a KoGenStyle object using the OpenDocument format
     */
    void saveOdf(KoGenStyle &style, KoShapeSavingContext &context) const;

    /// copy all the properties from the other style to this style, effectively duplicating it.
    void copyProperties(KoListStyle *other);

    /**
     * Check if list has numbering in one of it's list levels
     */
    bool isNumberingStyle() const;

    /**
     * Returns true if this list style is a outline style
     */
    bool isOulineStyle() const;

    /// returns true if style is a numbering style
    static bool isNumberingStyle(int style);

Q_SIGNALS:
    void nameChanged(const QString &newName);
    void styleChanged(int level);

private:
    friend class ChangeListCommand;
    friend class ChangeListLevelCommand;

    void refreshLevelProperties(const KoListLevelProperties &properties);

    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KoListStyle *)

#endif
