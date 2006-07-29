/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <kunittest/tester.h>

#include <KWFrameSet.h>
#include <KWTextFrameSet.h>

class KWDocument;
class KoXmlWriter;

class KWFrameTester : public KUnitTest::Tester {
    public:
        void allTests();
    private:
        void testSimpleFrame();
        void testFrameList();
        void testDeleteAllCopies();
        void testNestedFrames();
};

class TestFrameSet : public KWFrameSet {
public:
    TestFrameSet( KWDocument* doc = 0 ) : KWFrameSet(doc) {
    }
    QDomElement save(QDomElement&, bool) { return QDomElement(); }
    void saveOasis( KoXmlWriter&, KoSavingContext&, bool) const { };
    void setProtectContent(bool) { }
    bool protectContent() const { return true; }
};

class TestTextFrameSet : public KWTextFrameSet {
public:
    TestTextFrameSet(KWDocument *doc, const QString name) : KWTextFrameSet(name) {
        m_doc = doc;
    }
    QDomElement save(QDomElement&, bool) { return QDomElement(); }
    void saveOasis( KoXmlWriter&, KoSavingContext&, bool) const { };
    void setProtectContent(bool) { }
    bool protectContent() const { return true; }
    void updateFrames( int flags = 0xff ) {
        flags = UpdateFramesInPage;
        KWFrameSet::updateFrames(flags);
    }
};
