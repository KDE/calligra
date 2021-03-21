/* This file is part of the KDE project
   Copyright (C) 2005-2006 Ariya Hidayat <ariya@kde.org>

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

#include "KoXmlReader.h"
#include "KoXmlNS.h"

/*
  This is a memory-efficient DOM implementation for Calligra. See the API
  documentation for details.

  IMPORTANT !

  * When you change this stuff, make sure it DOES NOT BREAK the test suite.
    Build tests/koxmlreadertest.cpp and verify it. Many sleepless nights
    have been sacrificed for this piece of code, do not let those precious
    hours wasted!

  * Run koxmlreadertest.cpp WITH Valgrind and make sure NO illegal
    memory read/write and any type of leak occurs. If you are not familiar
    with Valgrind then RTFM first and come back again later on.

  * The public API shall remain as compatible as QDom.

  * All QDom-compatible methods should behave the same. All QDom-compatible
    functions should return the same result. In case of doubt, run
    koxmlreadertest.cpp but uncomment KOXML_USE_QDOM in koxmlreader.h
    so that the tests are performed with standard QDom.

  Some differences compared to QDom:

  - DOM tree in KoXmlDocument is read-only, you can not modify it. This is
    sufficient for Calligra since the tree is only accessed when loading
    a document to the application. For saving the document to XML file,
    use KoXmlWriter.

  - Because the dynamic loading and unloading, you have to use the
    nodes (and therefore also elements) carefully since the whole API
 (just like QDom) is reference-based, not pointer-based. If the
 parent node is unloaded from memory, the reference is not valid
 anymore and may give unpredictable result.
 The easiest way: use the node/element in very short time only.

  - Comment node (like QDomComment) is not implemented as comments are
    simply ignored.

  - DTD, entity and entity reference are not handled. Thus, the associated
    nodes (like QDomDocumentType, QDomEntity, QDomEntityReference) are also
    not implemented.

  - Attribute mapping node is not implemented. But of course, functions to
    query attributes of an element are available.


 */

#include <QTextCodec>
#include <QTextDecoder>

#ifndef KOXML_USE_QDOM

#include <QtXml>
#include <QDomDocument>
#include <QXmlStreamReader>
#include <QXmlStreamEntityResolver>

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QVector>

/*
 Use more compact representation of in-memory nodes.

 Advantages: faster iteration, can facilitate real-time compression.
 Disadvantages: still buggy, eat slightly more memory.
*/
#define KOXML_COMPACT

/*
 Use real-time compression. Only works in conjuction with KOXML_COMPACT
 above because otherwise the non-compact layout will slow down everything.
*/
#define KOXML_COMPRESS


// prevent mistake, see above
#ifdef KOXML_COMPRESS
#ifndef KOXML_COMPACT
#error Please enable also KOXML_COMPACT
#endif
#endif

// this is used to quickly get namespaced attribute(s)
typedef QPair<QString, QString> KoXmlStringPair;

class KoQName {
public:
    QString nsURI;
    QString name;

    explicit KoQName(const QString& nsURI_, const QString& name_)
        : nsURI(nsURI_), name(name_) {}
    bool operator==(const KoQName& qname) const {
        // local name is more likely to differ, so compare that first
        return name == qname.name && nsURI == qname.nsURI;
    }
};

uint qHash(const KoQName& qname)
{
    // possibly add a faster hash function that only includes some trailing
    // part of the nsURI

    // in case of doubt, use this:
    // return qHash(qname.nsURI)^qHash(qname.name);
    return qHash(qname.nsURI)^qHash(qname.name);
}

// this simplistic hash is rather fast-and-furious. it works because
// likely there is very few namespaced attributes per element
static inline uint qHash(const KoXmlStringPair &p, uint /*seed*/ = 0)
{
    return qHash(p.second[0].unicode()) ^ 0x1477;

    // in case of doubt, use this:
    // return qHash(p.first)^qHash(p.second);
}

static inline bool operator==(const KoXmlStringPair &a, const KoXmlStringPair &b)
{
    return a.second == b.second && a.first == b.first;
}

// Older versions of OpenOffice.org used different namespaces. This function
// does translate the old namespaces into the new ones.
static QString fixNamespace(const QString &nsURI)
{
    static QString office = QStringLiteral("http://openoffice.org/2000/office");
    static QString text = QStringLiteral("http://openoffice.org/2000/text");
    static QString style = QStringLiteral("http://openoffice.org/2000/style");
    static QString fo = QStringLiteral("http://www.w3.org/1999/XSL/Format");
    static QString table = QStringLiteral("http://openoffice.org/2000/table");
    static QString drawing = QStringLiteral("http://openoffice.org/2000/drawing");
    static QString datastyle = QStringLiteral("http://openoffice.org/2000/datastyle");
    static QString svg = QStringLiteral("http://www.w3.org/2000/svg");
    static QString chart = QStringLiteral("http://openoffice.org/2000/chart");
    static QString dr3d = QStringLiteral("http://openoffice.org/2000/dr3d");
    static QString form = QStringLiteral("http://openoffice.org/2000/form");
    static QString script = QStringLiteral("http://openoffice.org/2000/script");
    static QString meta = QStringLiteral("http://openoffice.org/2000/meta");
    static QString config = QStringLiteral("http://openoffice.org/2001/config");
    static QString pres = QStringLiteral("http://openoffice.org/2000/presentation");
    static QString manifest = QStringLiteral("http://openoffice.org/2001/manifest");
    if (nsURI == text)
        return KoXmlNS::text;
    if (nsURI == style)
        return KoXmlNS::style;
    if (nsURI == office)
        return KoXmlNS::office;
    if (nsURI == fo)
        return KoXmlNS::fo;
    if (nsURI == table)
        return KoXmlNS::table;
    if (nsURI == drawing)
        return KoXmlNS::draw;
    if (nsURI == datastyle)
        return KoXmlNS::number;
    if (nsURI == svg)
        return KoXmlNS::svg;
    if (nsURI == chart)
        return KoXmlNS::chart;
    if (nsURI == dr3d)
        return KoXmlNS::dr3d;
    if (nsURI == form)
        return KoXmlNS::form;
    if (nsURI == script)
        return KoXmlNS::script;
    if (nsURI == meta)
        return KoXmlNS::meta;
    if (nsURI == config)
        return KoXmlNS::config;
    if (nsURI == pres)
        return KoXmlNS::presentation;
    if (nsURI == manifest)
        return KoXmlNS::manifest;
    return nsURI;
}

// ==================================================================
//
//         KoXmlPackedItem
//
// ==================================================================

// 12 bytes on most system 32 bit systems, 16 bytes on 64 bit systems
class KoXmlPackedItem
{
public:
bool attr: 1;
KoXmlNode::NodeType type: 3;

#ifdef KOXML_COMPACT
quint32 childStart: 28;
#else
unsigned depth: 28;
#endif

    unsigned qnameIndex;
    QString value;

    // it is important NOT to have a copy constructor, so that growth is optimal
    // see https://doc.qt.io/qt-5/containers.html#growth-strategies
#if 0
    KoXmlPackedItem(): attr(false), type(KoXmlNode::NullNode), childStart(0), depth(0) {}
#endif
};

Q_DECLARE_TYPEINFO(KoXmlPackedItem, Q_MOVABLE_TYPE);

#ifdef KOXML_COMPRESS
static QDataStream& operator<<(QDataStream& s, const KoXmlPackedItem& item)
{
    quint8 flag = item.attr ? 1 : 0;

    s << flag;
    s << (quint8) item.type;
    s << item.childStart;
    s << item.qnameIndex;
    s << item.value;

    return s;
}

static QDataStream& operator>>(QDataStream& s, KoXmlPackedItem& item)
{
    quint8 flag;
    quint8 type;
    quint32 child;
    QString value;

    s >> flag;
    s >> type;
    s >> child;
    s >> item.qnameIndex;
    s >> value;

    item.attr = (flag != 0);
    item.type = (KoXmlNode::NodeType) type;
    item.childStart = child;
    item.value = value;

    return s;
}
#endif

// ==================================================================
//
//         KoXmlPackedDocument
//
// ==================================================================

#ifdef KOXML_COMPRESS

#include "KoXmlVector.h"

// when number of buffered items reach this, compression will start
// small value will give better memory usage at the cost of speed
// bigger value will be better in term of speed, but use more memory
#define ITEMS_FULL  (1*256)

typedef KoXmlVector<KoXmlPackedItem, ITEMS_FULL> KoXmlPackedGroup;
#else
typedef QVector<KoXmlPackedItem> KoXmlPackedGroup;
#endif

// growth strategy: increase every GROUP_GROW_SIZE items
// this will override standard QVector's growth strategy
#define GROUP_GROW_SHIFT 3
#define GROUP_GROW_SIZE (1 << GROUP_GROW_SHIFT)

class KoXmlPackedDocument
{
public:
    bool processNamespace;
#ifdef KOXML_COMPACT
    // map given depth to the list of items
    QHash<int, KoXmlPackedGroup> groups;
#else
    QVector<KoXmlPackedItem> items;
#endif

    QList<KoQName> qnameList;
    QString docType;

private:
    QHash<KoQName, unsigned> qnameHash;

    unsigned cacheQName(const QString& name, const QString& nsURI) {
        KoQName qname(nsURI, name);

        const unsigned ii = qnameHash.value(qname, (unsigned)-1);
        if (ii != (unsigned)-1)
            return ii;

        // not yet declared, so we add it
        unsigned i = qnameList.count();
        qnameList.append(qname);
        qnameHash.insert(qname, i);

        return i;
    }

    QHash<QString, unsigned> valueHash;
    QStringList valueList;

    QString cacheValue(const QString& value) {
        if (value.isEmpty())
            return 0;

        const unsigned& ii = valueHash[value];
        if (ii > 0)
            return valueList[ii];

        // not yet declared, so we add it
        unsigned i = valueList.count();
        valueList.append(value);
        valueHash.insert(value, i);

        return valueList[i];
    }

#ifdef KOXML_COMPACT
public:
    const KoXmlPackedItem& itemAt(unsigned depth, unsigned index) {
        const KoXmlPackedGroup& group = groups[depth];
        return group[index];
    }

    unsigned itemCount(unsigned depth) {
        const KoXmlPackedGroup& group = groups[depth];
        return group.count();
    }

    /*
       NOTE:
          Function clear, newItem, addElement, addAttribute, addText,
          addCData, addProcessing are all related. These are all necessary
          for stateful manipulation of the document. See also the calls
          to these function from parseDocument().

          The state itself is defined by the member variables
          currentDepth and the groups (see above).
     */

    unsigned currentDepth;

    KoXmlPackedItem& newItem(unsigned depth) {
        KoXmlPackedGroup& group = groups[depth];

#ifdef KOXML_COMPRESS
        KoXmlPackedItem& item = group.newItem();
#else
        // reserve up front
        if ((groups.size() % GROUP_GROW_SIZE) == 0)
            group.reserve(GROUP_GROW_SIZE * (1 + (groups.size() >> GROUP_GROW_SHIFT)));
        group.resize(group.count() + 1);

        KoXmlPackedItem& item = group[group.count()-1];
#endif

        // this is necessary, because intentionally we don't want to have
        // a constructor for KoXmlPackedItem
        item.attr = false;
        item.type = KoXmlNode::NullNode;
        item.qnameIndex = 0;
        item.childStart = itemCount(depth + 1);
        item.value.clear();

        return item;
    }

    void clear() {
        currentDepth = 0;
        qnameHash.clear();
        qnameList.clear();
        valueHash.clear();
        valueList.clear();
        groups.clear();
        docType.clear();

        // first node is root
        KoXmlPackedItem& rootItem = newItem(0);
        rootItem.type = KoXmlNode::DocumentNode;
    }

    void finish() {
        // won't be needed anymore
        qnameHash.clear();
        valueHash.clear();
        valueList.clear();

        // optimize, see documentation on QVector::squeeze
        for (int d = 0; d < groups.count(); ++d) {
            KoXmlPackedGroup& group = groups[d];
            group.squeeze();
        }
    }

    // in case namespace processing, 'name' contains the prefix already
    void addElement(const QString& name, const QString& nsURI) {
        KoXmlPackedItem& item = newItem(currentDepth + 1);
        item.type = KoXmlNode::ElementNode;
        item.qnameIndex = cacheQName(name, nsURI);

        ++currentDepth;
    }

    void closeElement() {
        --currentDepth;
    }

    void addDTD(const QString& dt) {
        docType = dt;
    }

    void addAttribute(const QString& name, const QString& nsURI, const QString& value) {
        KoXmlPackedItem& item = newItem(currentDepth + 1);
        item.attr = true;
        item.qnameIndex = cacheQName(name, nsURI);
        //item.value = cacheValue( value );
        item.value = value;
    }

    void addText(const QString& text) {
        KoXmlPackedItem& item = newItem(currentDepth + 1);
        item.type = KoXmlNode::TextNode;
        item.value = text;
    }

    void addCData(const QString& text) {
        KoXmlPackedItem& item = newItem(currentDepth + 1);
        item.type = KoXmlNode::CDATASectionNode;
        item.value = text;
    }

    void addProcessingInstruction() {
        KoXmlPackedItem& item = newItem(currentDepth + 1);
        item.type = KoXmlNode::ProcessingInstructionNode;
    }

public:
    KoXmlPackedDocument(): processNamespace(false), currentDepth(0) {
        clear();
    }

#else

private:
    unsigned elementDepth;

public:

    KoXmlPackedItem& newItem() {
        unsigned count = items.count() + 512;
        count = 1024 * (count >> 10);
        items.reserve(count);

        items.resize(items.count() + 1);

        // this is necessary, because intentionally we don't want to have
        // a constructor for KoXmlPackedItem
        KoXmlPackedItem& item = items[items.count()-1];
        item.attr = false;
        item.type = KoXmlNode::NullNode;
        item.qnameIndex = 0;
        item.depth = 0;

        return item;
    }

    void addElement(const QString& name, const QString& nsURI) {
        // we are going one level deeper
        ++elementDepth;

        KoXmlPackedItem& item = newItem();

        item.attr = false;
        item.type = KoXmlNode::ElementNode;
        item.depth = elementDepth;
        item.qnameIndex = cacheQName(name, nsURI);
    }

    void closeElement() {
        // we are going up one level
        --elementDepth;
    }

    void addDTD(const QString& dt) {
        docType = dt;
    }

    void addAttribute(const QString& name, const QString& nsURI, const QString& value) {
        KoXmlPackedItem& item = newItem();

        item.attr = true;
        item.type = KoXmlNode::NullNode;
        item.depth = elementDepth;
        item.qnameIndex = cacheQName(name, nsURI);
        //item.value = cacheValue( value );
        item.value = value;
    }

    void addText(const QString& str) {
        KoXmlPackedItem& item = newItem();

        item.attr = false;
        item.type = KoXmlNode::TextNode;
        item.depth = elementDepth + 1;
        item.qnameIndex = 0;
        item.value = str;
    }

    void addCData(const QString& str) {
        KoXmlPackedItem& item = newItem();

        item.attr = false;
        item.type = KoXmlNode::CDATASectionNode;
        item.depth = elementDepth + 1;
        item.qnameIndex = 0;
        item.value = str;
    }

    void addProcessingInstruction() {
        KoXmlPackedItem& item = newItem();

        item.attr = false;
        item.type = KoXmlNode::ProcessingInstructionNode;
        item.depth = elementDepth + 1;
        item.qnameIndex = 0;
        item.value.clear();
    }

    void clear() {
        qnameHash.clear();
        qnameList.clear();
        valueHash.clear();
        valueList.clear();
        items.clear();
        elementDepth = 0;

        KoXmlPackedItem& rootItem = newItem();
        rootItem.attr = false;
        rootItem.type = KoXmlNode::DocumentNode;
        rootItem.depth = 0;
        rootItem.qnameIndex = 0;
    }

    void finish() {
        qnameHash.clear();
        valueList.clear();
        valueHash.clear();
        items.squeeze();
    }

    KoXmlPackedDocument(): processNamespace(false), elementDepth(0) {
    }

#endif

};

namespace {

    class ParseError {
    public:
        QString errorMsg;
        int errorLine;
        int errorColumn;
        bool error;

        ParseError() :errorLine(-1), errorColumn(-1), error(false) {}
    };

    void parseElement(QXmlStreamReader &xml, KoXmlPackedDocument &doc, bool stripSpaces = true);

    // parse one element as if this were a standalone xml document
    ParseError parseDocument(QXmlStreamReader &xml, KoXmlPackedDocument &doc, bool stripSpaces = true)
    {
        doc.clear();
        ParseError error;
        xml.readNext();
        while (!xml.atEnd() && xml.tokenType() != QXmlStreamReader::EndDocument && !xml.hasError()) {
            switch (xml.tokenType()) {
            case QXmlStreamReader::StartElement:
                parseElement(xml, doc, stripSpaces);
                break;
            case QXmlStreamReader::DTD:
                doc.addDTD(xml.dtdName().toString());
                break;
            case QXmlStreamReader::StartDocument:
                if (!xml.documentEncoding().isEmpty() || !xml.documentVersion().isEmpty()) {
                    doc.addProcessingInstruction();
                }
                break;
            case QXmlStreamReader::ProcessingInstruction:
                doc.addProcessingInstruction();
                break;
            default:
                break;
            }
            xml.readNext();
        }
        if (xml.hasError()) {
            error.error = true;
            error.errorMsg = xml.errorString();
            error.errorColumn = xml.columnNumber();
            error.errorLine = xml.lineNumber();
        } else {
            doc.finish();
        }
        return error;
    }

    void parseElementContents(QXmlStreamReader &xml, KoXmlPackedDocument &doc)
    {
        xml.readNext();
        QString ws;
        while (!xml.atEnd()) {
            switch (xml.tokenType()) {
            case QXmlStreamReader::EndElement:
                // if an element contains only whitespace, put it in the dom
                if (!ws.isEmpty()) {
                    doc.addText(ws);
                }
                return;
            case QXmlStreamReader::StartElement:
                // The whitespaces between > and < are also a text element
                if (!ws.isEmpty()) {
                    doc.addText(ws);
                    ws.clear();
                }
                // Do not strip spaces
                parseElement(xml, doc, false);
                break;
            case QXmlStreamReader::Characters:
                if (xml.isCDATA()) {
                    doc.addCData(xml.text().toString());
                } else if (!xml.isWhitespace()) {
                    doc.addText(xml.text().toString());
                } else {
                    ws += xml.text();
                }
                break;
            case QXmlStreamReader::ProcessingInstruction:
                doc.addProcessingInstruction();
                break;
            default:
                break;
            }
            xml.readNext();
        }
    }

    void parseElementContentsStripSpaces(QXmlStreamReader &xml, KoXmlPackedDocument &doc)
    {
        xml.readNext();
        QString ws;
        bool sawElement = false;
        while (!xml.atEnd()) {
            switch (xml.tokenType()) {
            case QXmlStreamReader::EndElement:
                // if an element contains only whitespace, put it in the dom
                if (!ws.isEmpty() && !sawElement) {
                    doc.addText(ws);
                }
                return;
            case QXmlStreamReader::StartElement:
                sawElement = true;
                // Do strip spaces
                parseElement(xml, doc, true);
                break;
            case QXmlStreamReader::Characters:
                if (xml.isCDATA()) {
                    doc.addCData(xml.text().toString());
                } else if (!xml.isWhitespace()) {
                    doc.addText(xml.text().toString());
                } else if (!sawElement) {
                    ws += xml.text();
                }
                break;
            case QXmlStreamReader::ProcessingInstruction:
                doc.addProcessingInstruction();
                break;
            default:
                break;
            }
            xml.readNext();
        }
    }

    void parseElement(QXmlStreamReader &xml, KoXmlPackedDocument &doc, bool stripSpaces)
    {
        // Unfortunately MSVC fails using QXmlStreamReader::const_iterator
        // so we apply a for loop instead. https://bugreports.qt.io/browse/QTBUG-45368
        doc.addElement(xml.qualifiedName().toString(),
                       fixNamespace(xml.namespaceUri().toString()));
        QXmlStreamAttributes attr = xml.attributes();
        for  (int a = 0; a < attr.count(); a++) {
            doc.addAttribute(attr[a].qualifiedName().toString(),
                             attr[a].namespaceUri().toString(),
                             attr[a].value().toString());
        }
        if (stripSpaces)
          parseElementContentsStripSpaces(xml, doc);
        else
          parseElementContents(xml, doc);
        // reader.tokenType() is now QXmlStreamReader::EndElement
        doc.closeElement();
    }
}


// ==================================================================
//
//         KoXmlNodePrivate
//
// ==================================================================

class KoXmlNodePrivate
{
public:

    explicit KoXmlNodePrivate(unsigned long initialRefCount = 1);
    ~KoXmlNodePrivate();

    // generic properties
    KoXmlNode::NodeType nodeType;
    bool loaded;

#ifdef KOXML_COMPACT
    unsigned nodeDepth;
#endif

    QString tagName;
    QString namespaceURI;
    QString prefix;
    QString localName;

    void ref() {
        ++refCount;
    }
    void unref() {
        if (!--refCount) {
            delete this;
        }
    }

    // type information
    QString nodeName() const;

    // for tree and linked-list
    KoXmlNodePrivate* parent;
    KoXmlNodePrivate* prev;
    KoXmlNodePrivate* next;
    KoXmlNodePrivate* first;
    KoXmlNodePrivate* last;

    QString text();

    // node manipulation
    void clear();

    // attributes
    inline void setAttribute(const QString& name, const QString& value);
    inline QString attribute(const QString& name, const QString& def) const;
    inline bool hasAttribute(const QString& name) const;
    inline void setAttributeNS(const QString& nsURI, const QString& name, const QString& value);
    inline QString attributeNS(const QString& nsURI, const QString& name, const QString& def) const;
    inline bool hasAttributeNS(const QString& nsURI, const QString& name) const;
    inline void clearAttributes();
    inline QStringList attributeNames() const;
    inline QList< QPair<QString, QString> > attributeFullNames() const;


    // for text and CDATA
    QString data() const;

    // reference from within the packed doc
    KoXmlPackedDocument* packedDoc;
    unsigned long nodeIndex;

    // used when doing on-demand (re)parse
    void loadChildren(int depth = 1);
    void unloadChildren();

    void dump();

    static KoXmlNodePrivate null;

    // compatibility
    void asQDomNode(QDomDocument& ownerDoc) const;

private:
    QHash<QString, QString> attr;
    QHash<KoXmlStringPair, QString> attrNS;
    QString textData;
    // reference counting
    unsigned long refCount;
    friend class KoXmlElement;
};

KoXmlNodePrivate KoXmlNodePrivate::null;


KoXmlNodePrivate::KoXmlNodePrivate(unsigned long initialRefCount)
    : nodeType(KoXmlNode::NullNode)
    , loaded(false)
#ifdef KOXML_COMPACT
    , nodeDepth(0)
#endif
    , parent(0), prev(0), next(0), first(0), last(0)
    , packedDoc(0), nodeIndex(0)
    , refCount(initialRefCount)
{
}

KoXmlNodePrivate::~KoXmlNodePrivate()
{
    clear();
}

void KoXmlNodePrivate::clear()
{
    if (first)
        for (KoXmlNodePrivate* node = first; node ;) {
            KoXmlNodePrivate* next = node->next;
            node->unref();
            node = next;
        }

    // only document can delete these
    // normal nodes don't "own" them
    if (nodeType == KoXmlNode::DocumentNode)
        delete packedDoc;

    nodeType = KoXmlNode::NullNode;
    tagName.clear();
    prefix.clear();
    namespaceURI.clear();
    textData.clear();
    packedDoc = 0;

    attr.clear();
    attrNS.clear();

    parent = 0;
    prev = next = 0;
    first = last = 0;

    loaded = false;
}

QString KoXmlNodePrivate::text()
{
    QString t;

    loadChildren();

    KoXmlNodePrivate* node = first;
    while (node) {
        switch (node->nodeType) {
        case KoXmlNode::ElementNode:
            t += node->text(); break;
        case KoXmlNode::TextNode:
            t += node->data(); break;
        case KoXmlNode::CDATASectionNode:
            t += node->data(); break;
        default: break;
        }
        node = node->next;
    }

    return t;
}

QString KoXmlNodePrivate::nodeName() const
{
    switch (nodeType) {
    case KoXmlNode::ElementNode: {
        QString n(tagName);
        if (!prefix.isEmpty())
            n.prepend(':').prepend(prefix);
        return n;
    }
    break;

    case KoXmlNode::TextNode:         return QStringLiteral("#text");
    case KoXmlNode::CDATASectionNode: return QStringLiteral("#cdata-section");
    case KoXmlNode::DocumentNode:     return QStringLiteral("#document");
    case KoXmlNode::DocumentTypeNode: return tagName;

    default: return QString(); break;
    }

    // should not happen
    return QString();
}

void KoXmlNodePrivate::setAttribute(const QString& name, const QString& value)
{
    attr.insert(name, value);
}

QString KoXmlNodePrivate::attribute(const QString& name, const QString& def) const
{
    return attr.value(name, def);
}

bool KoXmlNodePrivate::hasAttribute(const QString& name) const
{
    return attr.contains(name);
}

void KoXmlNodePrivate::setAttributeNS(const QString& nsURI,
                                   const QString& name, const QString& value)
{
    int i = name.indexOf(':');
    if (i != -1) {
        QString localName(name.mid(i + 1));
        KoXmlStringPair key(nsURI, localName);
        attrNS.insert(key, value);
    }
}

QString KoXmlNodePrivate::attributeNS(const QString& nsURI, const QString& name,
                                   const QString& def) const
{
    KoXmlStringPair key(nsURI, name);
    return attrNS.value(key, def);
}

bool KoXmlNodePrivate::hasAttributeNS(const QString& nsURI, const QString& name) const
{
    KoXmlStringPair key(nsURI, name);
    return attrNS.contains(key);
}

void KoXmlNodePrivate::clearAttributes()
{
    attr.clear();
    attrNS.clear();
}

// FIXME how about namespaced attributes ?
QStringList KoXmlNodePrivate::attributeNames() const
{
    QStringList result;
    result = attr.keys();

    return result;
}


QList< QPair<QString, QString> > KoXmlNodePrivate::attributeFullNames() const
{
    QList< QPair<QString, QString> > result;
    result = attrNS.keys();

    return result;
}

QString KoXmlNodePrivate::data() const
{
    return textData;
}

#ifdef KOXML_COMPACT

void KoXmlNodePrivate::loadChildren(int depth)
{
    // sanity check
    if (!packedDoc) return;

    // already loaded ?
    if (loaded && (depth <= 1)) return;

    // in case depth is different
    unloadChildren();


    KoXmlNodePrivate* lastDat = 0;

    unsigned childStop = 0;
    if (nodeIndex == packedDoc->itemCount(nodeDepth) - 1)
        childStop = packedDoc->itemCount(nodeDepth + 1);
    else {
        const KoXmlPackedItem& next = packedDoc->itemAt(nodeDepth, nodeIndex + 1);
        childStop = next.childStart;
    }

    const KoXmlPackedItem& self = packedDoc->itemAt(nodeDepth, nodeIndex);

    for (unsigned i = self.childStart; i < childStop; ++i) {
        const KoXmlPackedItem& item = packedDoc->itemAt(nodeDepth + 1, i);
        bool textItem = (item.type == KoXmlNode::TextNode);
        textItem |= (item.type == KoXmlNode::CDATASectionNode);

        // attribute belongs to this node
        if (item.attr) {
            KoQName qname = packedDoc->qnameList[item.qnameIndex];
            QString value = item.value;

            QString prefix;

            QString qName; // with prefix
            QString localName;  // without prefix, i.e. local name

            localName = qName = qname.name;
            int i = qName.indexOf(':');
            if (i != -1) prefix = qName.left(i);
            if (i != -1) localName = qName.mid(i + 1);

            if (packedDoc->processNamespace) {
                setAttributeNS(qname.nsURI, qName, value);
                setAttribute(localName, value);
            } else
                setAttribute(qName, value);
        } else {
            KoQName qname = packedDoc->qnameList[item.qnameIndex];
            QString value = item.value;

            QString nodeName = qname.name;
            QString localName;
            QString prefix;

            if (packedDoc->processNamespace) {
                localName = qname.name;
                int di = qname.name.indexOf(':');
                if (di != -1) {
                    localName = qname.name.mid(di + 1);
                    prefix = qname.name.left(di);
                }
                nodeName = localName;
            }

            // make a node out of this item
            KoXmlNodePrivate* dat = new KoXmlNodePrivate;
            dat->nodeIndex = i;
            dat->packedDoc = packedDoc;
            dat->nodeDepth = nodeDepth + 1;
            dat->nodeType = item.type;
            dat->tagName = nodeName;
            dat->localName = localName;
            dat->prefix = prefix;
            dat->namespaceURI = qname.nsURI;
            dat->parent = this;
            dat->prev = lastDat;
            dat->next = 0;
            dat->first = 0;
            dat->last = 0;
            dat->loaded = false;
            dat->textData = (textItem) ? value : QString();

            // adjust our linked-list
            first = (first) ? first : dat;
            last = dat;
            if (lastDat)
                lastDat->next = dat;
            lastDat = dat;

            // recursive
            if (depth > 1)
                dat->loadChildren(depth - 1);
        }
    }

    loaded = true;
}

#else

void KoXmlNodePrivate::loadChildren(int depth)
{
    // sanity check
    if (!packedDoc) return;

    // already loaded ?
    if (loaded && (depth <= 1)) return;

    // cause we don't know how deep this node's children already loaded are
    unloadChildren();

    KoXmlNodePrivate* lastDat = 0;
    int nodeDepth = packedDoc->items[nodeIndex].depth;

    for (int i = nodeIndex + 1; i < packedDoc->items.count(); ++i) {
        KoXmlPackedItem& item = packedDoc->items[i];
        bool textItem = (item.type == KoXmlNode::TextNode);
        textItem |= (item.type == KoXmlNode::CDATASectionNode);

        // element already outside our depth
        if (!item.attr && (item.type == KoXmlNode::ElementNode))
            if (item.depth <= (unsigned)nodeDepth)
                break;

        // attribute belongs to this node
        if (item.attr && (item.depth == (unsigned)nodeDepth)) {
            KoQName qname = packedDoc->qnameList[item.qnameIndex];
            QString value = item.value;

            QString prefix;

            QString qName; // with prefix
            QString localName;  // without prefix, i.e. local name

            localName = qName = qname.name;
            int i = qName.indexOf(':');
            if (i != -1) prefix = qName.left(i);
            if (i != -1) localName = qName.mid(i + 1);

            if (packedDoc->processNamespace) {
                setAttributeNS(qname.nsURI, qName, value);
                setAttribute(localName, value);
            } else
                setAttribute(qname.name, value);
        }

        // the child node
        if (!item.attr) {
            bool instruction = (item.type == KoXmlNode::ProcessingInstructionNode);
            bool ok = (textItem || instruction)  ? (item.depth == (unsigned)nodeDepth) : (item.depth == (unsigned)nodeDepth + 1);

            ok = (item.depth == (unsigned)nodeDepth + 1);

            if (ok) {
                KoQName qname = packedDoc->qnameList[item.qnameIndex];
                QString value = item.value;

                QString nodeName = qname.name;
                QString localName;
                QString prefix;

                if (packedDoc->processNamespace) {
                    localName = qname.name;
                    int di = qname.name.indexOf(':');
                    if (di != -1) {
                        localName = qname.name.mid(di + 1);
                        prefix = qname.name.left(di);
                    }
                    nodeName = localName;
                }

                // make a node out of this item
                KoXmlNodePrivate* dat = new KoXmlNodePrivate;
                dat->nodeIndex = i;
                dat->packedDoc = packedDoc;
                dat->nodeType = item.type;
                dat->tagName = nodeName;
                dat->localName = localName;
                dat->prefix = prefix;
                dat->namespaceURI = qname.nsURI;
                dat->count = 1;
                dat->parent = this;
                dat->prev = lastDat;
                dat->next = 0;
                dat->first = 0;
                dat->last = 0;
                dat->loaded = false;
                dat->textData = (textItem) ? value : QString();

                // adjust our linked-list
                first = (first) ? first : dat;
                last = dat;
                if (lastDat)
                    lastDat->next = dat;
                lastDat = dat;

                // recursive
                if (depth > 1)
                    dat->loadChildren(depth - 1);
            }
        }
    }

    loaded = true;
}
#endif

void KoXmlNodePrivate::unloadChildren()
{
    // sanity check
    if (!packedDoc) return;

    if (!loaded) return;

    if (first)
        for (KoXmlNodePrivate* node = first; node ;) {
            KoXmlNodePrivate* next = node->next;
            node->unloadChildren();
            node->unref();
            node = next;
        }

    clearAttributes();
    loaded = false;
    first = last = 0;
}

#ifdef KOXML_COMPACT


static void itemAsQDomNode(QDomDocument& ownerDoc, KoXmlPackedDocument* packedDoc,
                               unsigned nodeDepth, unsigned nodeIndex, QDomNode parentNode = QDomNode())
{
    // sanity check
    if (!packedDoc)
        return;

    const KoXmlPackedItem& self = packedDoc->itemAt(nodeDepth, nodeIndex);

    unsigned childStop = 0;
    if (nodeIndex == packedDoc->itemCount(nodeDepth) - 1)
        childStop = packedDoc->itemCount(nodeDepth + 1);
    else {
        const KoXmlPackedItem& next = packedDoc->itemAt(nodeDepth, nodeIndex + 1);
        childStop = next.childStart;
    }

    // nothing to do here
    if (self.type == KoXmlNode::NullNode)
        return;

    // create the element properly
    if (self.type == KoXmlNode::ElementNode) {
        QDomElement element;

        KoQName qname = packedDoc->qnameList[self.qnameIndex];
        qname.nsURI = fixNamespace(qname.nsURI);

        if (packedDoc->processNamespace)
            element = ownerDoc.createElementNS(qname.nsURI, qname.name);
        else
            element = ownerDoc.createElement(qname.name);

        if ( parentNode.isNull() ) {
            ownerDoc.appendChild( element );
        } else {
            parentNode.appendChild( element );
        }
        // check all subnodes for attributes
        for (unsigned i = self.childStart; i < childStop; ++i) {
            const KoXmlPackedItem& item = packedDoc->itemAt(nodeDepth + 1, i);
            bool textItem = (item.type == KoXmlNode::TextNode);
            textItem |= (item.type == KoXmlNode::CDATASectionNode);

            // attribute belongs to this node
            if (item.attr) {
                KoQName qname = packedDoc->qnameList[item.qnameIndex];
                qname.nsURI = fixNamespace(qname.nsURI );
                QString value = item.value;

                QString prefix;

                QString qName; // with prefix
                QString localName;  // without prefix, i.e. local name

                localName = qName = qname.name;
                int i = qName.indexOf(':');
                if (i != -1) prefix = qName.left(i);
                if (i != -1) localName = qName.mid(i + 1);

                if (packedDoc->processNamespace) {
                    element.setAttributeNS(qname.nsURI, qName, value);
                    element.setAttribute(localName, value);
                } else
                    element.setAttribute(qname.name, value);
            } else {
                // add it recursively
                itemAsQDomNode(ownerDoc, packedDoc, nodeDepth + 1, i, element);
            }
        }
        return;
    }

    // create the text node
    if (self.type == KoXmlNode::TextNode) {
        QString text = self.value;

        // FIXME: choose CDATA when the value contains special characters
        QDomText textNode = ownerDoc.createTextNode(text);
        if ( parentNode.isNull() ) {
            ownerDoc.appendChild( textNode );
        } else {
            parentNode.appendChild( textNode );
        }
        return;
    }
    // nothing matches? strange...
}

void KoXmlNodePrivate::asQDomNode(QDomDocument& ownerDoc) const
{
    itemAsQDomNode(ownerDoc, packedDoc, nodeDepth, nodeIndex);
}

#else

static void itemAsQDomNode(QDomDocument& ownerDoc, KoXmlPackedDocument* packedDoc,
                               unsigned nodeIndex, QDomNode parentNode = QDomNode())
{
    // sanity check
    if (!packedDoc)
        return;

    KoXmlPackedItem& item = packedDoc->items[nodeIndex];

    // nothing to do here
    if (item.type == KoXmlNode::NullNode)
        return;

    // create the element properly
    if (item.type == KoXmlNode::ElementNode) {
        QDomElement element;

        KoQName qname = packedDoc->qnameList[item.qnameIndex];
        qname.nsURI = fixNamespace(qname.nsURI);

        if (packedDoc->processNamespace)
            element = ownerDoc.createElementNS(qname.nsURI, qname.name);
        else
            element = ownerDoc.createElement(qname.name);

        if ( parentNode.isNull() ) {
            ownerDoc.appendChild( element );
        } else {
            parentNode.appendChild( element );
        }
        // check all subnodes for attributes
        int nodeDepth = item.depth;
        for (int i = nodeIndex + 1; i < packedDoc->items.count(); ++i) {
            KoXmlPackedItem& item = packedDoc->items[i];
            bool textItem = (item.type == KoXmlNode::TextNode);
            textItem |= (item.type == KoXmlNode::CDATASectionNode);

            // element already outside our depth
            if (!item.attr && (item.type == KoXmlNode::ElementNode))
                if (item.depth <= (unsigned)nodeDepth)
                    break;

            // attribute belongs to this node
            if (item.attr && (item.depth == (unsigned)nodeDepth)) {
                KoQName qname = packedDoc->qnameList[item.qnameIndex];
                qname.nsURI = fixNamespace(qname.nsURI);
                QString value = item.value;
                QString prefix;

                QString qName; // with prefix
                QString localName;  // without prefix, i.e. local name

                localName = qName = qname.name;
                int i = qName.indexOf(':');
                if (i != -1) prefix = qName.left(i);
                if (i != -1) localName = qName.mid(i + 1);

                if (packedDoc->processNamespace) {
                    element.setAttributeNS(qname.nsURI, qName, value);
                    element.setAttribute(localName, value);
                } else
                    element.setAttribute(qname.name, value);
            }

            // direct child of this node
            if (!item.attr && (item.depth == (unsigned)nodeDepth + 1)) {
                // add it recursively
                itemAsQDomNode(ownerDoc, packedDoc, i, element);
            }
        }
        return;
    }

    // create the text node
    if (item.type == KoXmlNode::TextNode) {
        QString text = item.value;
        // FIXME: choose CDATA when the value contains special characters
        QDomText textNode = ownerDoc.createTextNode(text);
        if ( parentNode.isNull() ) {
            ownerDoc.appendChild( textNode );
        } else {
            parentNode.appendChild( textNode );
        }
        return;
    }

    // nothing matches? strange...
}

void KoXmlNodePrivate::asQDomNode(QDomDocument& ownerDoc) const
{
    itemAsQDomNode(ownerDoc, packedDoc, nodeIndex);
}

#endif

void KoXmlNodePrivate::dump()
{
    printf("NodeData %p\n", (void*)this);

    printf("  nodeIndex: %d\n", (int)nodeIndex);
    printf("  packedDoc: %p\n", (void*)packedDoc);

    printf("  nodeType : %d\n", (int)nodeType);
    printf("  tagName: %s\n", qPrintable(tagName));
    printf("  namespaceURI: %s\n", qPrintable(namespaceURI));
    printf("  prefix: %s\n", qPrintable(prefix));
    printf("  localName: %s\n", qPrintable(localName));

    printf("  parent : %p\n", (void*)parent);
    printf("  prev : %p\n", (void*)prev);
    printf("  next : %p\n", (void*)next);
    printf("  first : %p\n", (void*)first);
    printf("  last : %p\n", (void*)last);

    printf("  refCount: %ld\n", refCount);

    if (loaded)
        printf("  loaded: TRUE\n");
    else
        printf("  loaded: FALSE\n");
}


// ==================================================================
//
//         KoXmlNodePrivate
//
// ==================================================================

class KoXmlDocumentData : public KoXmlNodePrivate
{
public:

    KoXmlDocumentData(unsigned long initialRefCount = 1);
    ~KoXmlDocumentData();

    bool setContent(QXmlStreamReader *reader,
                    QString* errorMsg = 0, int* errorLine = 0, int* errorColumn = 0);

    KoXmlDocumentType dt;

    bool emptyDocument :1;
    // to read the xml with or without spaces
    bool stripSpaces :1;
};

#define KOXMLDOCDATA(d)  static_cast<KoXmlDocumentData*>(d)


KoXmlDocumentData::KoXmlDocumentData(unsigned long initialRefCount)
    : KoXmlNodePrivate(initialRefCount)
    , emptyDocument(true)
    , stripSpaces(true)
{
}

KoXmlDocumentData::~KoXmlDocumentData()
{
}

bool KoXmlDocumentData::setContent(QXmlStreamReader* reader, QString* errorMsg, int* errorLine, int* errorColumn)
{
    // sanity checks
    if (!reader) return false;

    if (nodeType != KoXmlNode::DocumentNode)
        return false;

    clear();
    nodeType = KoXmlNode::DocumentNode;

    packedDoc = new KoXmlPackedDocument;
    packedDoc->processNamespace = reader->namespaceProcessing();

    ParseError error = parseDocument(*reader, *packedDoc, stripSpaces);
    if (error.error) {
        // parsing error has occurred
        if (errorMsg) *errorMsg = error.errorMsg;
        if (errorLine) *errorLine = error.errorLine;
        if (errorColumn)  *errorColumn = error.errorColumn;
        return false;
    }

    // initially load
    loadChildren();

    KoXmlNodePrivate *typeData = new KoXmlNodePrivate(0);
    typeData->nodeType = KoXmlNode::DocumentTypeNode;
    typeData->tagName = packedDoc->docType;
    typeData->parent = this;
    dt = KoXmlDocumentType(typeData);

    return true;
}

// ==================================================================
//
//         KoXmlNode
//
// ==================================================================

// Creates a null node
KoXmlNode::KoXmlNode()
{
    d = &KoXmlNodePrivate::null;
    d->ref();
}

// Destroys this node
KoXmlNode::~KoXmlNode()
{
    d->unref();
}

// Creates a copy of another node
KoXmlNode::KoXmlNode(const KoXmlNode& node)
{
    d = node.d;
    d->ref();
}

// Creates a node for specific implementation
KoXmlNode::KoXmlNode(KoXmlNodePrivate* data)
{
    d = data;
    data->ref();
}

// Creates a shallow copy of another node
KoXmlNode& KoXmlNode::operator=(const KoXmlNode & node)
{
    if (this != &node) {
        d->unref();
        d = node.d;
        d->ref();
    }
    return *this;
}

// Note: two null nodes are always equal
bool KoXmlNode::operator==(const KoXmlNode& node) const
{
    if (isNull() && node.isNull()) return true;
    return(d == node.d);
}

// Note: two null nodes are always equal
bool KoXmlNode::operator!=(const KoXmlNode& node) const
{
    if (isNull() && !node.isNull()) return true;
    if (!isNull() && node.isNull()) return true;
    if (isNull() && node.isNull()) return false;
    return(d != node.d);
}

KoXmlNode::NodeType KoXmlNode::nodeType() const
{
    return d->nodeType;
}

bool KoXmlNode::isNull() const
{
    return d->nodeType == NullNode;
}

bool KoXmlNode::isElement() const
{
    return d->nodeType == ElementNode;
}

bool KoXmlNode::isText() const
{
    return (d->nodeType == TextNode) || isCDATASection();
}

bool KoXmlNode::isCDATASection() const
{
    return d->nodeType == CDATASectionNode;
}

bool KoXmlNode::isDocument() const
{
    return d->nodeType == DocumentNode;
}

bool KoXmlNode::isDocumentType() const
{
    return d->nodeType == DocumentTypeNode;
}

void KoXmlNode::clear()
{
    d->unref();
    d = new KoXmlNodePrivate;
}

QString KoXmlNode::nodeName() const
{
    return d->nodeName();
}

QString KoXmlNode::prefix() const
{
    return isElement() ? d->prefix : QString();
}

QString KoXmlNode::namespaceURI() const
{
    return isElement() ? d->namespaceURI : QString();
}

QString KoXmlNode::localName() const
{
    return isElement() ? d->localName : QString();
}

KoXmlDocument KoXmlNode::ownerDocument() const
{
    KoXmlNodePrivate* node = d;
    while (node->parent) node = node->parent;

    if (node->nodeType == DocumentNode) {
        return KoXmlDocument(static_cast<KoXmlDocumentData*>(node));
    }
    return KoXmlDocument();
}

KoXmlNode KoXmlNode::parentNode() const
{
    return d->parent ? KoXmlNode(d->parent) : KoXmlNode();
}

bool KoXmlNode::hasChildNodes() const
{
    if (isText())
        return false;

    if (!d->loaded)
        d->loadChildren();

    return d->first != 0 ;
}

int KoXmlNode::childNodesCount() const
{
    if (isText())
        return 0;

    if (!d->loaded)
        d->loadChildren();

    KoXmlNodePrivate* node = d->first;
    int count = 0;
    while (node) {
        ++count;
        node = node->next;
    }

    return count;
}

QStringList KoXmlNode::attributeNames() const
{
    if (!d->loaded)
        d->loadChildren();

    return d->attributeNames();
}

QList< QPair<QString, QString> > KoXmlNode::attributeFullNames() const
{
    if (!d->loaded)
        d->loadChildren();

    return d->attributeFullNames();
}

KoXmlNode KoXmlNode::firstChild() const
{
    if (!d->loaded)
        d->loadChildren();
    return d->first ? KoXmlNode(d->first) : KoXmlNode();
}

KoXmlElement KoXmlNode::firstChildElement() const
{
    KoXmlElement element;
    forEachElement (element, (*this)) {
        return element;
    }
    return KoXmlElement();
}

KoXmlNode KoXmlNode::lastChild() const
{
    if (!d->loaded)
        d->loadChildren();
    return d->last ? KoXmlNode(d->last) : KoXmlNode();
}

KoXmlNode KoXmlNode::nextSibling() const
{
    return d->next ? KoXmlNode(d->next) : KoXmlNode();
}

KoXmlNode KoXmlNode::previousSibling() const
{
    return d->prev ? KoXmlNode(d->prev) : KoXmlNode();
}

KoXmlNode KoXmlNode::namedItem(const QString& name) const
{
    if (!d->loaded)
        d->loadChildren();

    for (KoXmlNodePrivate* node = d->first; node; node = node->next) {
        if (node->nodeName() == name)
            return KoXmlNode(node);
    }

    // not found
    return KoXmlNode();
}

KoXmlNode KoXmlNode::namedItemNS(const QString& nsURI, const QString& name) const
{
    if (!d->loaded)
        d->loadChildren();

    for (KoXmlNodePrivate* node = d->first; node; node = node->next) {
        if (node->nodeType == KoXmlNode::ElementNode
                 && node->localName == name
                 && node->namespaceURI == nsURI
                 ) {
            return KoXmlNode(node);
        }
    }

    // not found
    return KoXmlNode();
}

KoXmlNode KoXmlNode::namedItemNS(const QString& nsURI, const QString& name, KoXmlNamedItemType type) const
{
    if (!d->loaded)
        d->loadChildren();

    for (KoXmlNodePrivate* node = d->first; node; node = node->next) {
        if (node->nodeType != KoXmlNode::ElementNode)
            continue;
        if (node->localName == name && node->namespaceURI == nsURI) {
            return KoXmlNode(node);
        }
        bool isPrelude = false;
        switch (type) {
            case KoXmlTextContentPrelude:
                isPrelude =
                    (node->localName == QLatin1String("tracked-changes") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("variable-decls") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("user-field-decls") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("user-field-decl") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("sequence-decls") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("sequence-decl") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("dde-connection-decls") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("alphabetical-index-auto-mark-file") && node->namespaceURI == KoXmlNS::text) ||
                    (node->localName == QLatin1String("forms") && node->namespaceURI == KoXmlNS::office);
                break;
        }
        if (!isPrelude) {
            return KoXmlNode(); // no TextContentPrelude means it follows TextContentMain, so stop here.
        }
    }

    // not found
    return KoXmlNode();
}

KoXmlElement KoXmlNode::toElement() const
{
    return isElement() ? KoXmlElement(d) : KoXmlElement();
}

KoXmlText KoXmlNode::toText() const
{
    return isText() ? KoXmlText(d) : KoXmlText();
}

KoXmlCDATASection KoXmlNode::toCDATASection() const
{
    return isCDATASection() ? KoXmlCDATASection(d) : KoXmlCDATASection();
}

KoXmlDocument KoXmlNode::toDocument() const
{
    if (isDocument()) {
        return KoXmlDocument(static_cast<KoXmlDocumentData*>(d));
    }
    return KoXmlDocument();
}

void KoXmlNode::load(int depth)
{
    d->loadChildren(depth);
}

void KoXmlNode::unload()
{
    d->unloadChildren();
}

void KoXmlNode::asQDomNode(QDomDocument& ownerDoc) const
{
    Q_ASSERT(!isDocument());
    d->asQDomNode(ownerDoc);
}

// ==================================================================
//
//         KoXmlElement
//
// ==================================================================

// Creates an empty element
KoXmlElement::KoXmlElement(): KoXmlNode()
{
}

KoXmlElement::~KoXmlElement()
{
}

// Creates a shallow copy of another element
KoXmlElement::KoXmlElement(const KoXmlElement& element): KoXmlNode(element.d)
{
}

KoXmlElement::KoXmlElement(KoXmlNodePrivate* data): KoXmlNode(data)
{
}

// Copies another element
KoXmlElement& KoXmlElement::operator=(const KoXmlElement & element)
{
    KoXmlNode::operator=(element);
    return *this;
}

bool KoXmlElement::operator== (const KoXmlElement& element) const
{
    if (isNull() || element.isNull()) return false;
    return (d == element.d);
}

bool KoXmlElement::operator!= (const KoXmlElement& element) const
{
    if (isNull() && element.isNull()) return false;
    if (isNull() || element.isNull()) return true;
    return (d != element.d);
}

QString KoXmlElement::tagName() const
{
    return isElement() ? d->tagName : QString();
}

QString KoXmlElement::text() const
{
    return d->text();
}

QString KoXmlElement::attribute(const QString& name) const
{
    if (!isElement())
        return QString();

    if (!d->loaded)
        d->loadChildren();

    return d->attribute(name, QString());
}

QString KoXmlElement::attribute(const QString& name,
                                const QString& defaultValue) const
{
    if (!isElement())
        return defaultValue;

    if (!d->loaded)
        d->loadChildren();

    return d->attribute(name, defaultValue);
}

QString KoXmlElement::attributeNS(const QString& namespaceURI,
                                  const QString& localName, const QString& defaultValue) const
{
    if (!isElement())
        return defaultValue;

    if (!d->loaded)
        d->loadChildren();

    KoXmlStringPair key(namespaceURI, localName);
    return d->attrNS.value(key, defaultValue);

//  return d->attributeNS( namespaceURI, localName, defaultValue );
}

bool KoXmlElement::hasAttribute(const QString& name) const
{
    if (!d->loaded)
        d->loadChildren();

    return isElement() ? d->hasAttribute(name) : false;
}

bool KoXmlElement::hasAttributeNS(const QString& namespaceURI,
                                  const QString& localName) const
{
    if (!d->loaded)
        d->loadChildren();

    return isElement() ? d->hasAttributeNS(namespaceURI, localName) : false;
}

// ==================================================================
//
//         KoXmlText
//
// ==================================================================

KoXmlText::KoXmlText(): KoXmlNode()
{
}

KoXmlText::~KoXmlText()
{
}

KoXmlText::KoXmlText(const KoXmlText& text): KoXmlNode(text.d)
{
}

KoXmlText::KoXmlText(KoXmlNodePrivate* data): KoXmlNode(data)
{
}

bool KoXmlText::isText() const
{
    return true;
}

QString KoXmlText::data() const
{
    return d->data();
}

KoXmlText& KoXmlText::operator=(const KoXmlText & element)
{
    KoXmlNode::operator=(element);
    return *this;
}

// ==================================================================
//
//         KoXmlCDATASection
//
// ==================================================================

KoXmlCDATASection::KoXmlCDATASection(): KoXmlText()
{
}

KoXmlCDATASection::KoXmlCDATASection(const KoXmlCDATASection& cdata)
        : KoXmlText(cdata)
{
}

KoXmlCDATASection::~KoXmlCDATASection()
{
}

KoXmlCDATASection::KoXmlCDATASection(KoXmlNodePrivate* cdata):
        KoXmlText(cdata)
{
}

bool KoXmlCDATASection::isCDATASection() const
{
    return true;
}

KoXmlCDATASection& KoXmlCDATASection::operator=(const KoXmlCDATASection & cdata)
{
    KoXmlNode::operator=(cdata);
    return *this;
}

// ==================================================================
//
//         KoXmlDocumentType
//
// ==================================================================

KoXmlDocumentType::KoXmlDocumentType(): KoXmlNode()
{
}

KoXmlDocumentType::~KoXmlDocumentType()
{
}

KoXmlDocumentType::KoXmlDocumentType(const KoXmlDocumentType& dt):
        KoXmlNode(dt.d)
{
}

QString KoXmlDocumentType::name() const
{
    return nodeName();
}

KoXmlDocumentType::KoXmlDocumentType(KoXmlNodePrivate* dt): KoXmlNode(dt)
{
}

KoXmlDocumentType& KoXmlDocumentType::operator=(const KoXmlDocumentType & dt)
{
    KoXmlNode::operator=(dt);
    return *this;
}

// ==================================================================
//
//         KoXmlDocument
//
// ==================================================================

KoXmlDocument::KoXmlDocument(bool stripSpaces): KoXmlNode(new KoXmlDocumentData(0))
{
    KOXMLDOCDATA(d)->emptyDocument = false;
    KOXMLDOCDATA(d)->stripSpaces = stripSpaces;
}

KoXmlDocument::~KoXmlDocument()
{
}

KoXmlDocument::KoXmlDocument(KoXmlDocumentData* data): KoXmlNode(data)
{
    KOXMLDOCDATA(d)->emptyDocument = true;
}

// Creates a copy of another document
KoXmlDocument::KoXmlDocument(const KoXmlDocument& doc): KoXmlNode(doc.d)
{
}

// Creates a shallow copy of another document
KoXmlDocument& KoXmlDocument::operator=(const KoXmlDocument & doc)
{
    KoXmlNode::operator=(doc);
    return *this;
}

// Checks if this document and doc are equals
bool KoXmlDocument::operator==(const KoXmlDocument& doc) const
{
    return(d == doc.d);
}

// Checks if this document and doc are not equals
bool KoXmlDocument::operator!=(const KoXmlDocument& doc) const
{
    return(d != doc.d);
}

KoXmlElement KoXmlDocument::documentElement() const
{
    if (!d->loaded)
        d->loadChildren();

    for (KoXmlNodePrivate* node = d->first; node; node = node->next) {
        if (node->nodeType == KoXmlNode::ElementNode) {
            return KoXmlElement(node);
        }
    }

    return KoXmlElement();
}

KoXmlDocumentType KoXmlDocument::doctype() const
{
    return KOXMLDOCDATA(d)->dt;
}

QString KoXmlDocument::nodeName() const
{
    return (KOXMLDOCDATA(d)->emptyDocument) ? QStringLiteral("#document") : QString();
}

void KoXmlDocument::clear()
{
    d->unref();
    KoXmlDocumentData *dat = new KoXmlDocumentData;
    dat->emptyDocument = false;
    d = dat;
}

namespace {
    /* Use an entity resolver that ignores undefined entities and simply
       returns an empty string for them.
       */
    class DumbEntityResolver : public QXmlStreamEntityResolver {
    public:
	DumbEntityResolver() = default;
        QString resolveUndeclaredEntity ( const QString &) override { return QLatin1String(""); }
    	Q_DISABLE_COPY(DumbEntityResolver)
};

}

bool KoXmlDocument::setContent(QXmlStreamReader *reader,
                               QString* errorMsg, int* errorLine, int* errorColumn)
{
    if (d->nodeType != KoXmlNode::DocumentNode) {
        const bool stripSpaces = KOXMLDOCDATA(d)->stripSpaces;
        d->unref();
        KoXmlDocumentData *dat = new KoXmlDocumentData;
        dat->nodeType = KoXmlNode::DocumentNode;
        dat->stripSpaces = stripSpaces;
        d = dat;
    }

    const bool result = KOXMLDOCDATA(d)->setContent(reader, errorMsg, errorLine, errorColumn);

    return result;
}

// no namespace processing
bool KoXmlDocument::setContent(QIODevice* device, QString* errorMsg,
                               int* errorLine, int* errorColumn)
{
    return setContent(device, false, errorMsg, errorLine, errorColumn);
}

bool KoXmlDocument::setContent(QIODevice* device, bool namespaceProcessing,
                               QString* errorMsg, int* errorLine, int* errorColumn)
{
    if (d->nodeType != KoXmlNode::DocumentNode) {
        const bool stripSpaces = KOXMLDOCDATA(d)->stripSpaces;
        d->unref();
        KoXmlDocumentData *dat = new KoXmlDocumentData;
        dat->nodeType = KoXmlNode::DocumentNode;
        dat->stripSpaces = stripSpaces;
        d = dat;
    }

    if (!device->isOpen()) device->open(QIODevice::ReadOnly);
    QXmlStreamReader reader(device);
    reader.setNamespaceProcessing(namespaceProcessing);
    DumbEntityResolver entityResolver;
    reader.setEntityResolver(&entityResolver);

    const bool result = KOXMLDOCDATA(d)->setContent(&reader, errorMsg, errorLine, errorColumn);

    return result;
}

bool KoXmlDocument::setContent(const QByteArray& text, bool namespaceProcessing,
                               QString *errorMsg, int *errorLine, int *errorColumn)
{
    QBuffer buffer;
    buffer.setData(text);
    return setContent(&buffer, namespaceProcessing, errorMsg, errorLine, errorColumn);
}

bool KoXmlDocument::setContent(const QString& text, bool namespaceProcessing,
                               QString *errorMsg, int *errorLine, int *errorColumn)
{
    if (d->nodeType != KoXmlNode::DocumentNode) {
        const bool stripSpaces = KOXMLDOCDATA(d)->stripSpaces;
        d->unref();
        KoXmlDocumentData *dat = new KoXmlDocumentData;
        dat->nodeType = KoXmlNode::DocumentNode;
        dat->stripSpaces = stripSpaces;
        d = dat;
    }

    QXmlStreamReader reader(text);
    reader.setNamespaceProcessing(namespaceProcessing);
    DumbEntityResolver entityResolver;
    reader.setEntityResolver(&entityResolver);

    const bool result = KOXMLDOCDATA(d)->setContent(&reader, errorMsg, errorLine, errorColumn);

    return result;
}

bool KoXmlDocument::setContent(const QString& text,
                               QString *errorMsg, int *errorLine, int *errorColumn)
{
    return setContent(text, false, errorMsg, errorLine, errorColumn);
}

void  KoXmlDocument::setWhitespaceStripping(bool stripSpaces)
{
    KOXMLDOCDATA(d)->stripSpaces = stripSpaces;
}


#endif

// ==================================================================
//
//         functions in KoXml namespace
//
// ==================================================================

KoXmlElement KoXml::namedItemNS(const KoXmlNode& node, const QString& nsURI,
                                const QString& localName)
{
#ifdef KOXML_USE_QDOM
    // David's solution for namedItemNS, only for QDom stuff
    KoXmlNode n = node.firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (n.isElement() && n.localName() == localName &&
                n.namespaceURI() == nsURI)
            return n.toElement();
    }
    return KoXmlElement();
#else
    return node.namedItemNS(nsURI, localName).toElement();
#endif
}

KoXmlElement KoXml::namedItemNS(const KoXmlNode& node, const QString& nsURI,
                                const QString& localName, KoXmlNamedItemType type)
{
#ifdef KOXML_USE_QDOM
Q_ASSERT(false);
    return namedItemNS(node, nsURI, localName);
#else
    return node.namedItemNS(nsURI, localName, type).toElement();
#endif
}

void KoXml::load(KoXmlNode& node, int depth)
{
#ifdef KOXML_USE_QDOM
    // do nothing, QDom has no on-demand loading
    Q_UNUSED(node);
    Q_UNUSED(depth);
#else
    node.load(depth);
#endif
}


void KoXml::unload(KoXmlNode& node)
{
#ifdef KOXML_USE_QDOM
    // do nothing, QDom has no on-demand unloading
    Q_UNUSED(node);
#else
    node.unload();
#endif
}

int KoXml::childNodesCount(const KoXmlNode& node)
{
#ifdef KOXML_USE_QDOM
    return node.childNodes().count();
#else
    // compatibility function, because no need to implement
    // a class like QDomNodeList
    return node.childNodesCount();
#endif
}

QStringList KoXml::attributeNames(const KoXmlNode& node)
{
#ifdef KOXML_USE_QDOM
    QStringList result;

    QDomNamedNodeMap attrMap = node.attributes();
    for (int i = 0; i < attrMap.count(); ++i)
        result += attrMap.item(i).toAttr().name();

    return result;
#else
    // compatibility function, because no need to implement
    // a class like QDomNamedNodeMap
    return node.attributeNames();
#endif
}

void KoXml::asQDomNode(QDomDocument& ownerDoc, const KoXmlNode& node)
{
    Q_ASSERT(!node.isDocument());
#ifdef KOXML_USE_QDOM
    ownerDoc.appendChild(ownerDoc.importNode(node));
#else
    node.asQDomNode(ownerDoc);
#endif
}

void KoXml::asQDomElement(QDomDocument &ownerDoc, const KoXmlElement& element)
{
    KoXml::asQDomNode(ownerDoc, element);
}

QDomDocument KoXml::asQDomDocument(const KoXmlDocument& document)
{
#ifdef KOXML_USE_QDOM
    return document;
#else
    QDomDocument qdoc( document.nodeName() );
    if ( document.hasChildNodes() ) {
        for ( KoXmlNode n = document.firstChild(); ! n.isNull(); n = n.nextSibling() ) {
            KoXml::asQDomNode(qdoc, n);
        }
    }
    return qdoc;
#endif
}

bool KoXml::setDocument(KoXmlDocument& doc, QIODevice* device,
                        bool namespaceProcessing, QString* errorMsg, int* errorLine,
                        int* errorColumn)
{
    QXmlStreamReader reader(device);
    reader.setNamespaceProcessing(namespaceProcessing);
    bool result = doc.setContent(&reader, errorMsg, errorLine, errorColumn);
    return result;
}
