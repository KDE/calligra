/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres nandres @web.de

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_STYLE_MANAGER
#define CALLIGRA_SHEETS_STYLE_MANAGER

#include "sheets_core_export.h"
#include <QMap>
#include <QString>

class QStringList;

namespace Calligra
{
namespace Sheets
{
class Conditions;
class Map;
class ValueParser;
class CustomStyle;

// needs to be ordered (QMap) for the style dialog
typedef QMap<QString, CustomStyle*> CustomStyles;

/**
 * \class StyleManager
 * \brief Manages cell styles
 * \ingroup Style
 * The StyleManager takes care of named styles. It also provides some static
 * methods for the preloading of OpenDocument autostyles.
 */
class CALLIGRA_SHEETS_CORE_EXPORT StyleManager
{
    friend class StyleManagerDialog;

public:
    StyleManager();
    ~StyleManager();

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
    bool validateStyleName(QString const & name);
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
    QMap<QString, QString>  m_oasisStyles;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STYLE_MANAGER
