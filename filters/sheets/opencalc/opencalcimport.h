/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2004 Montel Laurent <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OpenCalc_IMPORT_H__
#define OpenCalc_IMPORT_H__

#include "sheets/engine/Format.h"

#include <KoFilter.h>

#include <KoXmlReader.h>
#include <QByteArray>
#include <QDomDocument>
#include <QHash>
#include <QPoint>
#include <QVariantList>

class KoStyleStack;
class KoStore;

namespace Calligra
{
namespace Sheets
{
class Cell;
class Conditional;
class DocBase;
class Map;
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
    OpenCalcImport(QObject *parent, const QVariantList &);
    ~OpenCalcImport() override;

    KoFilter::ConversionStatus convert(QByteArray const &from, QByteArray const &to) override;

private:
    class OpenCalcPoint
    {
    public:
        explicit OpenCalcPoint(const QString &str, Calligra::Sheets::Map *map);

        QString table;
        QString translation;
        QPoint topLeft;
        QPoint botRight;
        bool isRange;
    };

    enum bPos { Left, Top, Right, Bottom, Fall, GoUp, Border };

    Calligra::Sheets::DocBase *m_doc;
    Calligra::Sheets::Style *m_defaultStyle;

    KoXmlDocument m_content;
    KoXmlDocument m_meta;
    KoXmlDocument m_settings;

    QHash<QString, KoXmlElement *> m_styles;
    QHash<QString, Calligra::Sheets::Style *> m_defaultStyles;
    QHash<QString, QString *> m_formats;
    QMap<QString, KoXmlElement> m_validationList;

    QStringList m_namedAreas;

    int readMetaData();
    bool parseBody(int numOfTables);
    void insertStyles(KoXmlElement const &element);
    bool createStyleMap(KoXmlDocument const &styles);
    bool readRowFormat(KoXmlElement &rowNode, KoXmlElement *rowStyle, Calligra::Sheets::Sheet *table, int &row, int &number, bool last);
    bool readColLayouts(KoXmlElement &content, Calligra::Sheets::Sheet *table);
    bool readRowsAndCells(KoXmlElement &content, Calligra::Sheets::Sheet *table);
    bool readCells(KoXmlElement &rowNode, Calligra::Sheets::Sheet *table, int row, int &columns);
    void convertFormula(QString &text, QString const &f) const;
    void loadFontStyle(Calligra::Sheets::Style *layout, KoXmlElement const *font) const;
    void readInStyle(Calligra::Sheets::Style *layout, KoXmlElement const &style);
    void loadStyleProperties(Calligra::Sheets::Style *layout, KoXmlElement const &property) const;
    void loadBorder(Calligra::Sheets::Style *layout, QString const &borderDef, bPos pos) const;
    void loadTableMasterStyle(Calligra::Sheets::Sheet *table, QString const &stylename);
    QString *loadFormat(KoXmlElement *element, Calligra::Sheets::Format::Type &formatType, QString name);
    void checkForNamedAreas(QString &formula) const;
    void loadOasisCellValidation(const KoXmlElement &body, const Calligra::Sheets::ValueParser *parser);
    void loadOasisValidation(Calligra::Sheets::Validity val, const QString &validationName, const Calligra::Sheets::ValueParser *parser);
    void loadOasisValidationCondition(Calligra::Sheets::Validity val, QString &valExpression, const Calligra::Sheets::ValueParser *parser);
    void loadOasisAreaName(const KoXmlElement &body);
    void loadOasisMasterLayoutPage(Calligra::Sheets::Sheet *table, KoStyleStack &styleStack);
    void loadOasisValidationValue(Calligra::Sheets::Validity val, const QStringList &listVal, const Calligra::Sheets::ValueParser *parser);
    QString translatePar(QString &par) const;
    void loadCondition(const Calligra::Sheets::Cell &cell, const KoXmlElement &property);
    void loadOasisCondition(const Calligra::Sheets::Cell &cell, const KoXmlElement &property);
    void loadOasisConditionValue(const QString &styleCondition, Calligra::Sheets::Conditional &newCondition, const Calligra::Sheets::ValueParser *parser);
    void loadOasisCondition(QString &valExpression, Calligra::Sheets::Conditional &newCondition, const Calligra::Sheets::ValueParser *parser);
    KoFilter::ConversionStatus loadAndParse(KoXmlDocument &doc, const QString &fileName, KoStore *m_store);

    KoFilter::ConversionStatus openFile();
};

#endif // OpenCalc_IMPORT_H__
