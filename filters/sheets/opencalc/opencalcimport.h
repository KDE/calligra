/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres <nandres@web.de>
   Copyright (C) 2004 Montel Laurent <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef OpenCalc_IMPORT_H__
#define OpenCalc_IMPORT_H__

#include "Format.h"

#include <KoFilter.h>

#include <QHash>
#include <QDomDocument>
#include <QByteArray>
#include <KoXmlReader.h>
#include <QVariantList>

class KoStyleStack;
class KoStore;

namespace Calligra
{
namespace Sheets
{
class Cell;
class Conditional;
class Doc;
class Sheet;
class Style;
class Validity;
class ValueParser;
}
}

class OpenCalcImport : public KoFilter
{
    Q_OBJECT
public:
    OpenCalcImport(QObject * parent, const QVariantList &);
    ~OpenCalcImport() override;

    KoFilter::ConversionStatus convert(QByteArray const & from, QByteArray const & to) override;


private:

    class OpenCalcPoint
    {
    public:
        explicit OpenCalcPoint(const QString &str);

        QString table;
        QString translation;
        QPoint  topLeft;
        QPoint  botRight;
        bool    isRange;
    };

    enum bPos { Left, Top, Right, Bottom, Fall, GoUp, Border };

    Calligra::Sheets::Doc *    m_doc;
    Calligra::Sheets::Style *  m_defaultStyle;

    KoXmlDocument   m_content;
    KoXmlDocument   m_meta;
    KoXmlDocument   m_settings;

    QHash<QString, KoXmlElement*>   m_styles;
    QHash<QString, Calligra::Sheets::Style*> m_defaultStyles;
    QHash<QString, QString*>        m_formats;
    QMap<QString, KoXmlElement> m_validationList;

    QStringList          m_namedAreas;

    int  readMetaData();
    bool parseBody(int numOfTables);
    void insertStyles(KoXmlElement const & element);
    bool createStyleMap(KoXmlDocument const & styles);
    bool readRowFormat(KoXmlElement & rowNode, KoXmlElement * rowStyle,
                       Calligra::Sheets::Sheet * table, int & row, int & number, bool last);
    bool readColLayouts(KoXmlElement & content, Calligra::Sheets::Sheet * table);
    bool readRowsAndCells(KoXmlElement & content, Calligra::Sheets::Sheet * table);
    bool readCells(KoXmlElement & rowNode, Calligra::Sheets::Sheet  * table, int row, int & columns);
    void convertFormula(QString & text, QString const & f) const;
    void loadFontStyle(Calligra::Sheets::Style * layout, KoXmlElement const * font) const;
    void readInStyle(Calligra::Sheets::Style * layout, KoXmlElement const & style);
    void loadStyleProperties(Calligra::Sheets::Style * layout, KoXmlElement const & property) const;
    void loadBorder(Calligra::Sheets::Style * layout, QString const & borderDef, bPos pos) const;
    void loadTableMasterStyle(Calligra::Sheets::Sheet * table, QString const & stylename);
    QString * loadFormat(KoXmlElement * element,
                         Calligra::Sheets::Format::Type & formatType,
                         QString name);
    void checkForNamedAreas(QString & formula) const;
    void loadOasisCellValidation(const KoXmlElement&body, const Calligra::Sheets::ValueParser *parser);
    void loadOasisValidation(Calligra::Sheets::Validity val, const QString& validationName, const Calligra::Sheets::ValueParser *parser);
    void loadOasisValidationCondition(Calligra::Sheets::Validity val, QString &valExpression, const Calligra::Sheets::ValueParser *parser);
    void loadOasisAreaName(const KoXmlElement&body);
    void loadOasisMasterLayoutPage(Calligra::Sheets::Sheet * table, KoStyleStack &styleStack);
    void loadOasisValidationValue(Calligra::Sheets::Validity val, const QStringList &listVal, const Calligra::Sheets::ValueParser *parser);
    QString translatePar(QString & par) const;
    void loadCondition(const Calligra::Sheets::Cell& cell, const KoXmlElement &property);
    void loadOasisCondition(const Calligra::Sheets::Cell& cell, const KoXmlElement &property);
    void loadOasisConditionValue(const QString &styleCondition, Calligra::Sheets::Conditional &newCondition, const Calligra::Sheets::ValueParser *parser);
    void loadOasisCondition(QString &valExpression, Calligra::Sheets::Conditional &newCondition, const Calligra::Sheets::ValueParser *parser);
    KoFilter::ConversionStatus loadAndParse(KoXmlDocument& doc, const QString& fileName, KoStore *m_store);

    KoFilter::ConversionStatus openFile();
};

#endif // OpenCalc_IMPORT_H__

