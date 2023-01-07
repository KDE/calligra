/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#ifndef CALLIGRA_SHEETS_GENVALIDATIONSTYLE
#define CALLIGRA_SHEETS_GENVALIDATIONSTYLE

#include "sheets_core_export.h"

#include <QMap>
#include <QString>

class KoXmlWriter;

namespace Calligra
{
namespace Sheets
{
class Validity;
class CalculationSettings;

namespace Odf {

class GenValidationStyles;

/**
 * \class GenValidationStyle
 * \ingroup OpenDocument
 */
class GenValidationStyle
{
public:
    explicit GenValidationStyle(Validity *_val = 0, CalculationSettings *cs = nullptr) {
        initVal(_val, cs);
    }


    bool operator<(const GenValidationStyle &other) const {
        if (allowEmptyCell != other.allowEmptyCell) return (allowEmptyCell < other.allowEmptyCell);
        if (condition != other.condition) return (condition < other.condition);
        if (titleInfo != other.titleInfo) return (titleInfo < other.titleInfo);
        if (displayValidationInformation != other.displayValidationInformation) return (displayValidationInformation < other.displayValidationInformation);
        if (messageInfo != other.messageInfo) return (messageInfo < other.messageInfo);
        if (messageType != other.messageType) return (messageType < other.messageType);
        if (displayMessage != other.displayMessage) return (displayMessage < other.displayMessage);
        if (message != other.message) return (message < other.message);
        if (title != other.title) return (title < other.title);

        return false;
    }
private:
    QString createValidationCondition(Validity* _val, CalculationSettings *cs);
    QString createTextValidationCondition(Validity* _val);
    QString createTimeValidationCondition(Validity* _val);
    QString createDateValidationCondition(Validity* _val, CalculationSettings *cs);
    QString createNumberValidationCondition(Validity* _val);
    QString createListValidationCondition(Validity* _val);

    void initVal(Validity *_val, CalculationSettings *cs);

    QString allowEmptyCell;
    QString condition;
    QString titleInfo;
    QString displayValidationInformation;
    QString messageInfo;
    QString messageType;
    QString displayMessage;
    QString message;
    QString title;
    friend class GenValidationStyles;
};

/**
 * \class GenValidationStyles
 * \ingroup OpenDocument
 */
class CALLIGRA_SHEETS_CORE_EXPORT GenValidationStyles
{
public:
    GenValidationStyles();
    ~GenValidationStyles();
    QString insert(const GenValidationStyle& style);

    typedef QMap<GenValidationStyle, QString> StyleMap;
    void writeStyle(KoXmlWriter& writer) const;

private:
    QString makeUniqueName(const QString& base) const;

    /// style definition -> name
    StyleMap m_styles;
    /// name -> style   (used to check for name uniqueness)
    typedef QMap<QString, bool> NameMap;
    NameMap m_names;

};

} // namespace Odf


} // namespace Sheets
} // namespace Calligra

#endif
