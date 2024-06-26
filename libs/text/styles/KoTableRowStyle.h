/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTABLEROWSTYLE_H
#define KOTABLEROWSTYLE_H

#include "KoText.h"
#include "kotext_export.h"

#include <KoXmlReaderForward.h>

#include <QTextTableFormat>

class KoStyleStack;
class KoGenStyle;
class KoOdfLoadingContext;
class QString;
class QVariant;

/**
 * A container for all properties for the table row style.
 * Each table column in the main text is based on a table row style.
 * Row styles are stored (besides in the KoStyleManager) in the KoTableColumnAndRowStyleManager.
 * The style has a property 'StyleId' with an integer as value.  The integer value corresponds to the styleId() output of
 * a specific KoTableRowStyle.
 * @see KoStyleManager
 * @see KoTableRowAndColumnStyleManager
 */
class KOTEXT_EXPORT KoTableRowStyle
{
public:
    enum Property {
        StyleId = QTextTableFormat::UserProperty + 1,
        // Linespacing properties
        KeepTogether, ///< If true, the row is not allowed to break
        MinimumRowHeight, ///< a qreal specifying the minimum row height in pt
        RowHeight, ///< a qreal specifying the exact row height in pt
        BreakBefore, ///< If true, insert a frame break before this table row
        BreakAfter, ///< If true, insert a frame break after this table row
        MasterPageName, ///< Optional name of the master-page
        UseOptimalHeight ///< If true, the row height should fit the content
    };

    /// Constructor
    KoTableRowStyle();
    /// Constructor
    KoTableRowStyle(const KoTableRowStyle &rhs);
    /// assign operator
    KoTableRowStyle &operator=(const KoTableRowStyle &rhs);

    /// Destructor
    ~KoTableRowStyle();

    void copyProperties(const KoTableRowStyle *style);

    KoTableRowStyle *clone() const;

    void setBackground(const QBrush &brush);
    /// See similar named method on QTextBlockFormat
    QBrush background() const;
    /// See similar named method on QTextBlockFormat
    void clearBackground();

    void setBreakBefore(KoText::KoTextBreakProperty state);
    KoText::KoTextBreakProperty breakBefore() const;
    void setBreakAfter(KoText::KoTextBreakProperty state);
    KoText::KoTextBreakProperty breakAfter() const;

    void setUseOptimalHeight(bool on);
    bool useOptimalHeight() const;

    /// Set minimum height of row
    void setMinimumRowHeight(const qreal height);
    qreal minimumRowHeight() const;

    /// Set exact and fixed height of row in a table, even if the row could be smaller because of less content
    void setRowHeight(qreal height);
    /// Get the exact and fixed height of row in a table
    qreal rowHeight() const;

    void setKeepTogether(bool on);
    bool keepTogether() const;

    /// set the parent style this one inherits its unset properties from.
    void setParentStyle(KoTableRowStyle *parent);

    /// return the parent style
    KoTableRowStyle *parentStyle() const;

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

    void remove(int key);

    /// Compare the properties of this style with the other
    bool operator==(const KoTableRowStyle &other) const;

    void removeDuplicates(const KoTableRowStyle &other);

    /**
     * Load the style from the element
     *
     * @param context the odf loading context
     * @param element the element containing the
     */
    void loadOdf(const KoXmlElement *element, KoOdfLoadingContext &context);

    void saveOdf(KoGenStyle &style) const;

    /**
     * Returns true if this table column style has the property set.
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
     * Returns true if this table row style is empty.
     */
    bool isEmpty() const;

private:
    /**
     * Load the style from the \a KoStyleStack style stack using the
     * OpenDocument format.
     */
    void loadOdfProperties(KoStyleStack &styleStack);
    qreal propertyDouble(int key) const;
    int propertyInt(int key) const;
    bool propertyBoolean(int key) const;
    QColor propertyColor(int key) const;

    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_TYPEINFO(KoTableRowStyle, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(KoTableRowStyle *)

#endif
