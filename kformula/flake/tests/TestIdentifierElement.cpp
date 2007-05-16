/* This file is part of the KDE project
   Copyright 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TestIdentifierElement.h"

#include <QtTest/QtTest>
#include <QtCore/QBuffer>
#include <QtXml/QDomDocument>

#include <KoXmlWriter.h>

#include "IdentifierElement.h"

#include <kdebug.h>

static QString loadAndSave(const QString& input)
{
    QDomDocument doc;
    doc.setContent( input );
    IdentifierElement element;
    element.readMathML(doc.documentElement());
    QBuffer device;
    device.open(QBuffer::ReadWrite);
    KoXmlWriter writer( &device );
    element.writeMathML( &writer );
    device.seek( 0 );
    return device.readAll();
}

void TestIdentifierElement::loading_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    
    QString content = "<mi>x</mi>";
    QTest::newRow("mi") << content << content;
}

void TestIdentifierElement::loading()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    kWarning() << "input: " << input << endl;
    kWarning() << "output: " << loadAndSave(output) << endl;
    QCOMPARE(input, loadAndSave(output));
}

QTEST_MAIN(TestIdentifierElement)
#include "TestIdentifierElement.moc"
