/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Nandita Suri <suri.nandita@gmail.com>
 * SPDX-FileCopyrightText: 2011-2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOLISTLEVELPROPERTIES_H
#define KOLISTLEVELPROPERTIES_H

#include "KoListStyle.h"

#include <KoOdfNumberDefinition.h>
#include <KoXmlReader.h>

class KoCharacterStyle;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoXmlWriter;
class KoImageData;

class QTextList;

/**
 * Properties per list level.
 */
class KOTEXT_EXPORT KoListLevelProperties : public QObject
{
    Q_OBJECT
public:
    /// Constructor
    explicit KoListLevelProperties();
    /// Copy constructor
    KoListLevelProperties(const KoListLevelProperties &other);
    /// Destructor
    ~KoListLevelProperties() override;

    /// each style has a unique ID (non persistent) given out by the styleManager
    int styleId() const;
    /// each style has a unique ID (non persistent) given out by the styleManager
    void setStyleId(int id);

    /// set the label type to be used.
    void setLabelType(KoListStyle::LabelType labelType);
    /// return the used label type
    KoListStyle::LabelType labelType() const;
    /// set the number format to be used.
    void setNumberFormat(KoOdfNumberDefinition::FormatSpecification numberFormat);
    /// return the used style
    KoOdfNumberDefinition::FormatSpecification numberFormat() const;
    /// set the string that will be shown before the counter in the list label
    void setListItemPrefix(const QString &prefix);
    /// return the string that will be shown before the counter in the list label
    QString listItemPrefix() const;
    /// set the string that will be shown after the counter in the list label
    void setListItemSuffix(const QString &suffix);
    /// return the string that will be shown after the counter in the list label
    QString listItemSuffix() const;
    /// set the index of the first value of this whole list.
    void setStartValue(int value);
    /// return the index of the first value of this whole list.
    int startValue() const;
    /// set the list level which is how deep the counter is nested below other lists (should be >=1)
    void setLevel(int level);
    /// return the list level which is how deep the counter is nested below other lists
    int level() const;
    /// set the amount of levels that will be shown in list items of this list.
    void setDisplayLevel(int level);
    /// return the amount of levels that will be shown in list items of this list.
    int displayLevel() const;
    /// set the styleId of the KoCharacterStyle to be used to layout the listitem
    void setCharacterStyleId(int id);
    /// return the styleId of the KoCharacterStyle to be used to layout the listitem
    int characterStyleId() const;
    /// set the style for the bullet or the number of the list
    void setCharacterProperties(QSharedPointer<KoCharacterStyle> style);
    /// return the KoCharacterStyle for the bullet or the number of the list
    QSharedPointer<KoCharacterStyle> characterProperties() const;
    /// set the character to be used as the counter of the listitem
    void setBulletCharacter(QChar character);
    /// return the character to be used as the counter of the listitem
    QChar bulletCharacter() const;
    /// set the size, in percent, of the bullet counter relative to the fontsize of the counter
    void setRelativeBulletSize(int percent);
    /// return the size, in percent, of the bullet counter relative to the fontsize of the counter
    int relativeBulletSize() const;
    /// set how the list label should be aligned in the width this list reserves for the listitems
    void setAlignment(Qt::Alignment align);
    /// return how the list label should be aligned in the width this list reserves for the listitems
    Qt::Alignment alignment() const;
    /// set the minimum width (in pt) of the list label for all items in this list
    void setMinimumWidth(qreal width);
    /// return the minimum width (in pt) of the list label for all items in this list
    qreal minimumWidth() const;
    /// set the width (in pt) of the image bullet
    void setWidth(qreal width);
    /// return the width (in pt) of the image bullet
    qreal width() const;
    /// set the height (in pt) of the image bullet
    void setHeight(qreal height);
    /// return the height (in pt) of the image bullet
    qreal height() const;
    /// set the bullet image key (as from the KoImageData)
    void setBulletImage(KoImageData *imageData);
    /// return the bullet image that is used in the list(as KoImageData)
    KoImageData *bulletImage() const;

    /// set the listId used by all list-styles that together make 1 user defined list in an ODF file.
    void setListId(KoListStyle::ListIdType listId);
    /// return the listId used by all list-styles that together make 1 user defined list in an ODF file.
    KoListStyle::ListIdType listId() const;
    /**
     * For alpha-based lists numbers above the 'z' will increase the value of all characters at the same time.
     * If true; we get the sequence 'aa', 'bb', 'cc'. If false; 'aa', 'ab', 'ac'.
     * @return if letterSynchronization should be applied.
     */
    bool letterSynchronization() const;
    /**
     * For alpha-based lists numbers above the 'z' will increase the value of all characters at the same time.
     * If true; we get the sequence 'aa', 'bb', 'cc'. If false; 'aa', 'ab', 'ac'.
     * @param on if letterSynchronization should be applied.
     */
    void setLetterSynchronization(bool on);

    /// sets the indentation of paragraph
    void setIndent(qreal value);
    /// returns the indentation of paragraphs
    qreal indent() const;

    /// sets the minimum distance between the counter and the text
    void setMinimumDistance(qreal value);
    /// returns the minimum distance between the counter and text
    qreal minimumDistance() const;

    /// sets the margin of the list
    void setMargin(qreal vlaue);
    /// returns the margin of the list
    qreal margin() const;
    /// sets how much the margin should increase with each level when autogenerating lists - default of 18
    void setMarginIncrease(qreal vlaue);
    /// returns how much the margin should increase with each level when autogenerating lists - default of 18
    qreal marginIncrease() const;

    /// sets the text indent of the list item
    void setTextIndent(qreal value);
    /// returns the text indent of the list item
    qreal textIndent() const;

    /// set the item that follows the label; this is used if alignmentMode() is true
    void setLabelFollowedBy(KoListStyle::ListLabelFollowedBy value);
    /// returns the item that follows the label; this is used if alignmentMode() is true
    KoListStyle::ListLabelFollowedBy labelFollowedBy() const;

    /// sets the value of tab stop that follows the label, it is used only if ListLabelFollowedBy is ListTab
    void setTabStopPosition(qreal value);
    /// returns the value of tab stop that follows the label, it is used only if ListLabelFollowedBy is ListTab
    qreal tabStopPosition() const;
    /// returns if the value of tab stop that follows the label is set, it is used only if ListLabelFollowedBy is ListTab
    bool hasTabStopPosition() const;
    /// clears (unsets) the value of tab stop that follows the label, it is used only if ListLabelFollowedBy is ListTab
    void clearTabStopPosition();

    /// sets the alignment mode of the list isLabelAlignmentMode=true if ist-level-position-and-space-mode=label-alignment
    void setAlignmentMode(bool isLabelAlignmentMode);
    /// return the alignment mode of the list isLabelAlignmentMode=true if ist-level-position-and-space-mode=label-alignment
    bool alignmentMode() const;

    void setOutlineList(bool isOutline);
    bool isOutlineList() const;

    bool operator==(const KoListLevelProperties &other) const;
    bool operator!=(const KoListLevelProperties &other) const;
    KoListLevelProperties &operator=(const KoListLevelProperties &other);

    /**
     * Create a KoListLevelProperties object from a QTextList instance.
     */
    static KoListLevelProperties fromTextList(QTextList *list);

    /**
     * Apply this style to a QTextListFormat by copying all properties from this style
     * to the target list format.
     */
    void applyStyle(QTextListFormat &format) const;

    /**
     * Load the properties from the \p style using the OpenDocument format.
     */
    void loadOdf(KoShapeLoadingContext &scontext, const KoXmlElement &style);

    /**
     * Save the properties of the style using the OpenDocument format
     */
    void saveOdf(KoXmlWriter *writer, KoShapeSavingContext &context) const;

public Q_SLOTS:
    void onStyleChanged(int key);

Q_SIGNALS:
    void styleChanged(int key);

private:
    void setProperty(int key, const QVariant &value);
    int propertyInt(int key) const;
    uint propertyUInt(int key) const;
    qulonglong propertyULongLong(int key) const;
    bool propertyBoolean(int key) const;
    qreal propertyDouble(int key) const;
    QString propertyString(int key) const;
    QColor propertyColor(int key) const;
    QVariant property(int key) const;

    class Private;
    Private *const d;
};

#endif
