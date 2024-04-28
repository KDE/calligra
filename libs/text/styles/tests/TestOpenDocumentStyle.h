/*
    SPDX-FileCopyrightText: 2011 Pierre Ducroquet <pinaraf@pinaraf.info>

    SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef TESTOPENDOCUMENTSTYLE_H
#define TESTOPENDOCUMENTSTYLE_H

#include <QDomElement>
#include <QList>
#include <QMultiHash>
#include <QObject>
#include <QStringList>

#include <KoGenStyle.h>

class Attribute : public QObject
{
    Q_OBJECT
public:
    Attribute(const QDomElement &element);
    QString name();
    QStringList listValues();
    bool compare(const QString &initialValue, const QString &outputValue);
    bool hasReference(const QString &reference);
    QMap<QString, QString> requiredExtraAttributes() const;

private:
    QStringList listValuesFromNode(const QDomElement &element);
    QStringList m_references;
    QString m_name;
    QStringList m_values;
    QList<QStringList> m_equivalences;
    QMap<QString, QString> m_requiredExtraAttributes;
};

Q_DECLARE_METATYPE(Attribute *);

class TestOpenDocumentStyle : public QObject
{
    Q_OBJECT
public:
    TestOpenDocumentStyle();

private:
    QByteArray
    generateStyleNodeWithAttribute(const QString &styleFamily, const QString &attributeName, const QString &attributeValue, const Attribute &attribute);
    QByteArray generateStyleProperties(const KoGenStyle &genStyle, const QString &styleFamily);
    QList<Attribute *> listAttributesFromRNGName(const QString &name);

    QMultiHash<QString, QDomElement> m_rngRules;

    template<class T>
    bool basicTestFunction(KoGenStyle::Type family, const QString &familyName, Attribute *attribute, const QString &value);

private Q_SLOTS:
    void initTestCase();
    void testTableColumnStyle();
    void testTableColumnStyle_data();
    void testTableStyle();
    void testTableStyle_data();
    void testTableRowStyle();
    void testTableRowStyle_data();
    void testTableCellStyle();
    void testTableCellStyle_data();
    void testParagraphStyle();
    void testParagraphStyle_data();
    void testCharacterStyle();
    void testCharacterStyle_data();
};

#endif // TESTOPENDOCUMENTSTYLE_H
