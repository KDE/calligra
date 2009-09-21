/* This file is part of the KOffice project
   Copyright (C) 2009 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#include "TestKWDocument.h"

#include <KoPluginLoader.h>

#include <kdebug.h>

void TestKWDocument::init()
{
}

void TestKWDocument::testCreateSimple()
{
    QObject* parent = new QObject();
    KWDocument* doc = new KWDocument(0, parent);
    qDebug() << doc->dataCenterMap().keys();
    KoDataCenter* dc = doc->dataCenterMap()["InlineTextObjectManager"];
    qDebug() << dc;
    delete parent;
}
void TestKWDocument::testCreateNoParentWidget()
{
    KWDocument* doc1 = new KWDocument(0, 0);
    delete KoPluginLoader::instance();
    delete doc1;
}

QTEST_KDEMAIN(TestKWDocument, GUI)
#include "TestKWDocument.moc"
