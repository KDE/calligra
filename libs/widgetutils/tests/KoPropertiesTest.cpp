/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KoPropertiesTest.h"

#include <KoProperties.h>
#include <QTest>

void KoPropertiesTest::testDeserialization()
{
    QString test;
    KoProperties props;
    props.setProperty(QStringLiteral("bla"), "bla");

    QVERIFY(!props.load(test));
    QVERIFY(!props.isEmpty());
    QVERIFY(props.stringProperty(QStringLiteral("bla")) == QStringLiteral("bla"));

    test = QStringLiteral("<bla>asdsadasjk</bla>");
    QVERIFY(props.load(test));
    QVERIFY(props.isEmpty());

    props.setProperty(QStringLiteral("bla"), "bla");
    test = QStringLiteral("<bla>asdsadasjk</");
    QVERIFY(!props.load(test));
    QVERIFY(!props.isEmpty());
    QVERIFY(props.stringProperty(QStringLiteral("bla")) == QStringLiteral("bla"));
}

void KoPropertiesTest::testRoundTrip()
{
    KoProperties props;
    props.setProperty(QStringLiteral("string"), "string");
    props.setProperty(QStringLiteral("xmlstring"), "<xml>bla</xml>");
    props.setProperty(QStringLiteral("xmlstring2"), "<xml>&adsa</xml>");
    props.setProperty(QStringLiteral("cdata"), "<![CDATA[blabla]]>");
    props.setProperty(QStringLiteral("int"), 10);
    props.setProperty(QStringLiteral("bool"), false);
    props.setProperty(QStringLiteral("qreal"), 1.38);

    QString stored = props.store(QStringLiteral("KoPropertiesTest"));
    KoProperties restored;
    restored.load(stored);

    QVERIFY(restored.stringProperty(QStringLiteral("string")) == QStringLiteral("string"));
    QVERIFY(restored.stringProperty(QStringLiteral("xmlstring")) == QStringLiteral("<xml>bla</xml>"));
    QVERIFY(restored.stringProperty(QStringLiteral("xmlstring2")) == QStringLiteral("<xml>&adsa</xml>"));
    QVERIFY(restored.stringProperty(QStringLiteral("cdata")) == QStringLiteral("<![CDATA[blabla]]>"));
    QVERIFY(restored.intProperty(QStringLiteral("int")) == 10);
    QVERIFY(restored.boolProperty(QStringLiteral("bool")) == false);
    QVERIFY(restored.doubleProperty(QStringLiteral("qreal")) == 1.38);
}

void KoPropertiesTest::testProperties()
{
    KoProperties props;
    QVERIFY(props.isEmpty());

    QString visible = QStringLiteral("visible");
    QVERIFY(!props.value(visible).isValid());

    props.setProperty(QStringLiteral("visible"), "bla");
    QVERIFY(props.value(QStringLiteral("visible")) == QStringLiteral("bla"));
    QVERIFY(props.stringProperty(QStringLiteral("visible"), QStringLiteral("blabla")) == QStringLiteral("bla"));

    props.setProperty(QStringLiteral("bool"), true);
    QVERIFY(props.boolProperty(QStringLiteral("bool"), false) == true);
    props.setProperty(QStringLiteral("bool"), false);
    QVERIFY(props.boolProperty(QStringLiteral("bool"), true) == false);

    props.setProperty(QStringLiteral("qreal"), 1.0);
    QVERIFY(props.doubleProperty(QStringLiteral("qreal"), 2.0) == 1.0);
    props.setProperty(QStringLiteral("qreal"), 2.0);
    QVERIFY(props.doubleProperty(QStringLiteral("qreal"), 1.0) == 2.0);

    props.setProperty(QStringLiteral("int"), 1);
    QVERIFY(props.intProperty(QStringLiteral("int"), 2) == 1);
    props.setProperty(QStringLiteral("int"), 2);
    QVERIFY(props.intProperty(QStringLiteral("int"), 1) == 2);

    QVariant v;
    QVERIFY(props.property(QStringLiteral("sdsadsakldjsajd"), v) == false);
    QVERIFY(!v.isValid());
    QVERIFY(props.property(QStringLiteral("visible"), v) == true);
    QVERIFY(v.isValid());
    QVERIFY(v == "bla");

    QVERIFY(!props.isEmpty());
    QVERIFY(props.contains(QStringLiteral("visible")));
    QVERIFY(!props.contains(QStringLiteral("adsajkdsakj dsaieqwewqoie")));
    QVERIFY(props.contains(visible));

    int count = 0;
    QMapIterator<QString, QVariant> iter = props.propertyIterator();
    while (iter.hasNext()) {
        iter.next();
        count++;
    }
    QVERIFY(count == 4);
}

bool checkProps(const KoProperties &props)
{
    return (props.value(QStringLiteral("bla")) == 1);
}

void KoPropertiesTest::testPassAround()
{
    KoProperties props;
    props.setProperty(QStringLiteral("bla"), 1);
    QVERIFY(checkProps(props));

    KoProperties props2 = props;
    QVERIFY(checkProps(props2));

    KoProperties props3(props);
    checkProps(props3);
    props3.setProperty(QStringLiteral("bla"), 3);
    QVERIFY(props3.value(QStringLiteral("bla")) == 3);

    QVERIFY(checkProps(props));
    QVERIFY(checkProps(props2));
}

QTEST_GUILESS_MAIN(KoPropertiesTest)
