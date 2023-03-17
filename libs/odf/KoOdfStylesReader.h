/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KOODFSTYLESREADER_H
#define KOODFSTYLESREADER_H

#include <QHash>
#include <QList>

#include <KoXmlReader.h>

#include "koodf_export.h"
#include "KoOdfNumberStyles.h"
#include "KoOdfNotesConfiguration.h"

class KoOdfLineNumberingConfiguration;
class KoOdfBibliographyConfiguration;

/**
 * Repository of styles used during loading of OASIS/OOo file
 */
class KOODF_EXPORT KoOdfStylesReader
{
public:
    /// constructor
    KoOdfStylesReader();
    /// destructor
    ~KoOdfStylesReader();

    /// Look into @p doc for styles and remember them
    /// @param doc document to look into
    /// @param stylesDotXml true when loading styles.xml, false otherwise
    void createStyleMap(const KoXmlDocument &doc, bool stylesDotXml);

    /**
     * Look up a style by name.
     *  This method can find styles defined by the tags "style:page-layout",
     *   "style:presentation-page-layout", or "style:font-face".
     * Do NOT use this method for style:style styles.
     *
     * @param name the style name
     * @return the dom element representing the style, or an empty QString if it wasn't found.
     */
    const KoXmlElement* findStyle(const QString &name) const;

    /**
     * Looks up a style:style by name.
     * Searches in the list of custom styles first and then in the lists of automatic styles.
     * @param name the style name
     * @param family the style family (for a style:style, use 0 otherwise)
     * @return the dom element representing the style, or an empty QString if it wasn't found.
     */
    const KoXmlElement* findStyle(const QString &name, const QString &family) const;

    /**
     * Looks up a style:style by name.
     *
     * Searches in the list of custom styles first and then in the lists of automatic styles.
     *
     * @param name the style name
     * @param family the style family (for a style:style, use 0 otherwise)
     * @param stylesDotXml if true search the styles.xml auto-styles otherwise the content.xml ones
     *
     * @return the dom element representing the style, or an empty QString if it wasn't found.
     */
    const KoXmlElement* findStyle(const QString &name, const QString &family, bool stylesDotXml) const;

    /// Similar to findStyle but for custom styles only.
    const KoXmlElement *findStyleCustomStyle(const QString &name, const QString &family) const;

    /**
     * Similar to findStyle but for auto-styles only.
     * \note Searches in styles.xml only!
     * \see findStyle()
     */
    const KoXmlElement *findAutoStyleStyle(const QString &name, const QString &family) const;

    /**
     * Similar to findStyle but for auto-styles only.
     * \note Searches in content.xml only!
     * \see findStyle()
     */
    const KoXmlElement *findContentAutoStyle(const QString &name, const QString &family) const;

    /// @return the default style for a given family ("graphic","paragraph","table" etc.)
    /// Returns 0 if no default style for this family is available
    KoXmlElement *defaultStyle(const QString &family) const;

    /// @return the office:style element
    KoXmlElement officeStyle() const;

    /// @return the draw:layer-set element
    KoXmlElement layerSet() const;

    /// @return master pages ("style:master-page" elements), hashed by name
    QHash<QString, KoXmlElement*> masterPages() const;

    /// @return all presentation page layouts ("presentation-page-layout" elements), hashed by name
    QHash<QString, KoXmlElement*> presentationPageLayouts() const;

    /// @return all table templates("table-template" elements), template names may be duplicated
    QList<KoXmlElement *> tableTemplates() const;

    /**
     * Get the draw styles for a specified type. 
     *
     * @param drawType The type of the wanted drawStyles
     *                 Available types: gradient(returns gradient, linearGradient, radialGradient and conicalGradient styles),
     *                 hatch, fill-image, marker, stroke-dash, opacity
     * @return draw styles of the specified type, hashed by name
     */
    QHash<QString, KoXmlElement*> drawStyles(const QString &drawType) const;

    /// @return all custom styles ("style:style" elements) for a given family, hashed by name
    QHash<QString, KoXmlElement*> customStyles(const QString& family) const;

    /**
     * Returns all auto-styles defined in styles.xml, if \p stylesDotXml is \c true ,
     * or all in content.xml, if \p stylesDotXml is \c false .
     * \return all auto-styles ("style:style" elements) for a given family, hashed by name
     */
    QHash<QString, KoXmlElement*> autoStyles(const QString& family, bool stylesDotXml = false) const;

    typedef QHash<QString, QPair<KoOdfNumberStyles::NumericStyleData, KoXmlElement*> > DataFormatsMap;
    /// Value (date/time/number...) formats found while parsing styles. Used e.g. for fields.
    /// Key: format name. Value:
    DataFormatsMap dataFormats() const;

    /**
     * Return the notes configuration for the given note class (footnote or endnote).
     *
     * Note that ODF supports different notes configurations for sections, but we don't
     * support that yet.
     */
    KoOdfNotesConfiguration globalNotesConfiguration(KoOdfNotesConfiguration::NoteClass noteClass) const;

    /**
     * return the line numbering configuration for this document.
     */
    KoOdfLineNumberingConfiguration lineNumberingConfiguration() const;

    /**
     * return the bibliography configuration for this document.
     */
    KoOdfBibliographyConfiguration globalBibliographyConfiguration() const;

private:
    enum TypeAndLocation {
        CustomInStyles,     ///< custom style located in styles.xml
        AutomaticInContent, ///< auto-style located in content.xml
        AutomaticInStyles   ///< auto-style located in styles.xml
    };
    /// Add styles to styles map
    void insertStyles(const KoXmlElement &styles, TypeAndLocation typeAndLocation = CustomInStyles);

    void insertOfficeStyles(const KoXmlElement &styles);
    void insertStyle(const KoXmlElement &style, TypeAndLocation typeAndLocation);

    KoOdfStylesReader(const KoOdfStylesReader &);   // forbidden
    KoOdfStylesReader& operator=(const KoOdfStylesReader &);   // forbidden

    class Private;
    Private * const d;
};

#endif /* KOODFSTYLESREADER_H */
