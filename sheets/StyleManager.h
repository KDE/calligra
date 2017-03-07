/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_STYLE_MANAGER
#define CALLIGRA_SHEETS_STYLE_MANAGER

#include <KoXmlReader.h>

#include "sheets_odf_export.h"
#include <Style.h>

class QDomElement;
class QDomDocument;
class QStringList;

namespace Calligra
{
namespace Sheets
{
class Conditions;
class Map;
class ValueParser;

/**
 * \class StyleManager
 * \brief Manages cell styles
 * \ingroup Style
 * The StyleManager takes care of named styles. It also provides some static
 * methods for the preloading of OpenDocument autostyles.
 */
class CALLIGRA_SHEETS_ODF_EXPORT StyleManager
{
    friend class StyleManagerDialog;

public:
    StyleManager();
    ~StyleManager();

    QDomElement save(QDomDocument & doc);
    bool loadXML(KoXmlElement const & styles);

    CustomStyle * defaultStyle() const {
        return m_defaultStyle;
    }

    /**
     * Searches for a style named \p name in the map of styles.
     * On OpenDocument loading, it searches the name in the map sorted
     * by the OpenDocument internal name .
     * \return the custom style named \p name
     */
    CustomStyle * style(QString const & name) const;

    void resetDefaultStyle();

    bool checkCircle(QString const & name, QString const & parent);
    bool validateStyleName(QString const & name, CustomStyle * style);
    void changeName(QString const & oldName, QString const & newName);

    void insertStyle(CustomStyle *style);

    void takeStyle(CustomStyle * style);
    void createBuiltinStyles();

    QStringList styleNames(bool includeDefault = true) const;
    int count() const {
        return m_styles.count();
    }

    // Defines a temporary Oasis style alias.
    void defineOasisStyle(const QString &oasisName, const QString &styleName);

    void clearOasisStyles();

   /// OpenDocument name to internal name (on loading) or vice versa (on saving)
    QString openDocumentName(const QString&) const;

private:
    void dump() const;

    CustomStyle * m_defaultStyle;
    CustomStyles  m_styles; // builtin and custom made styles

    // OpenDocument name to internal name (on loading) or vice versa (on saving)
    // NOTE: Temporary! Only valid while loading or saving OpenDocument files.
    QHash<QString, QString>  m_oasisStyles;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STYLE_MANAGER
