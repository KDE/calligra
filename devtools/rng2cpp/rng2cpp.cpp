/* This file is part of the KDE project
   Copyright (C) 2013 Jos van den Oever <jos@vandenoever.info>

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
#include <algorithm>

#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QMap>
#include <QtGlobal>
#include <QSharedPointer>
#include <QStringList>
#include <QSet>
#include <QVector>

static QString ns() {
    return QStringLiteral("writeodf");
}

class RNGItem;
//typedef QSharedPointer<RNGItem> RNGItemPtr;
typedef RNGItem* RNGItemPtr;
typedef QSet<RNGItemPtr> RNGItems;
typedef QVector<RNGItemPtr> RNGItemList;

/**
 * Helper class for writing fatal messages of the form fatal() << "error!";
 */
class fatal
{
private:
    QString msg;
public:
    fatal() {}
    ~fatal()
    {
        qFatal("%s", qPrintable(msg));
    }
    template<class T>
    fatal& operator<<(T s)
    {
        msg += s;
        return *this;
    }
};

/**
 * RNG Datatype has either a constant value or a type.
 */
class Datatype
{
public:
    /**
     * Standard comparator for Datatype.
     */
    bool operator==(const Datatype& a) const {
        return constant == a.constant
                && type == a.type;
    }
    /**
     * Standard comparator for Datatype.
     */
    bool operator!=(const Datatype& a) const {
        return !(*this == a);
    }
    /**
     * The data type is a constant string, this string may be "".
     * To check if this value has been set, call constant.isNull().
     */
    QString constant;
    /**
     * The data type name, this string may be "".
     * To check if this value has been set, call constant.isNull().
     */
    QString type;
};

/**
 * Define a qHash so Datatype can be put into a QSet.
 */
uint qHash(const Datatype& d)
{
    return qHash(d.constant) ^ qHash(d.type);
}

/**
 * @brief The RNGItem class
 * Generic class that describes any of a number of concepts from an RNG file.
 * Any <define/>, <element/> or <attribute/> class is parsed into an instance
 * of RNGItem.
 */
class RNGItem
{
public:
    enum ItemType { Define, Start, Element, Attribute };
private:
    RNGItem(const RNGItem&);
    void operator=(const RNGItem&);
protected:
    /**
     * @brief type
     */
    const ItemType m_type;
    const QString m_name;
    /**
     * Internal constructor.
     */
    RNGItem(ItemType type, const QString &name = QString()) :m_type(type),
        m_name(name), mixedContent(false)
    {
        if (type != Start && name.isEmpty()) {
            fatal() << "name is empty";
        }
    }
public:
    /**
     * true if this item may contain text nodes
     */
    bool mixedContent;
    /**
     * name attribute of the <define/> element
     */
    const QString& name() const { return m_name; }
    /**
     * transformed name that is used in generated C++ code
     */
    QString cppName;
    /**
     * items that are allowed to be used in this item
     */
    RNGItems allowedItems;
    /**
     * items that must to be used in this item
     */
    RNGItems requiredItems;
    /**
     * names of items that are used in this item
     * This list is resolved into allowedItems after parsing.
     */
    QSet<QString> referencedDeclares;
    /**
     * names of items that must be used in this item
     * This list is resolved into allowedItems after parsing.
     */
    QSet<QString> requiredReferencedDeclares;
    /**
     * Collection of possible datatypes for this item.
     */
    QSet<Datatype> datatype;
    /**
     * true if this is item corresponds to a <element/>
     */
    bool isElement() const { return m_type == Element; }
    /**
     * true if this is item corresponds to a <attribute/>
     */
    bool isAttribute() const { return m_type == Attribute; }
    /**
     * Return a string value if this item can only contain a single constant value.
     * For example "1.2" is the only allowed, but required value for the
     * office:version attribute.
     */
    QString singleConstant() const
    {
        return datatype.size() == 1 ?datatype.constBegin()->constant :QString();
    }
    /**
     * Return a string with the datatype if only one datatype is possible for
     * this item.
     */
    QString singleType() const
    {
        return datatype.size() == 1 ?datatype.constBegin()->type :QString();
    }
    /**
     * true if this is item corresponds to a <define/>
     */
    bool isDefine() const
    {
        bool isdefine = m_type == Define || m_type == Start;
        return isdefine;
    }
    /**
     * true if this is item corresponds to a <start/>
     */
    bool isStart() const
    {
        return m_type == Start;
    }
    bool operator==(const RNGItem& a) const;
};

/**
 * Specialization of RNGItem that is an element.
 */
class Element : public RNGItem
{
public:
    Element(const QString& name) :RNGItem(RNGItem::Element, name)
    {
    }
};

/**
 * Specialization of RNGItem that is an attribute.
 */
class Attribute : public RNGItem
{
public:
    Attribute(const QString& name) :RNGItem(RNGItem::Attribute, name)
    {
    }
};

/**
 * Specialization of RNGItem that is a a define.
 */
class Start : public RNGItem
{
public:
    Start() :RNGItem(RNGItem::Start)
    {
    }
};

/**
 * Specialization of RNGItem that is a a define.
 */
class Define : public RNGItem
{
public:
    Define(const QString& name) :RNGItem(RNGItem::Define, name)
    {
    }
};

/**
 * Simple helper class for collecting information about whether an RNGItem
 * may contain a mix of text nodes and elements.
 */
class MixedCheck
{
public:
    int elementCount;
    bool mixed;
    MixedCheck() :elementCount(0), mixed(false) {}
};

/**
 * Determine if the RNGItem item may contain a mix of text nodes and elements.
 * @param item item to be investigated
 * @param seen items that were already seen, needed to avoid infinite recursion
 * @param mc data that is being collected
 */
void isMixed(const RNGItemPtr& item, RNGItems& seen, MixedCheck& mc)
{
    if (item->isAttribute() || seen.contains(item)) {
        return;
    }
    seen.insert(item);
    mc.mixed = mc.mixed || item->mixedContent;
    RNGItems::ConstIterator i = item->allowedItems.constBegin();
    RNGItems::ConstIterator end = item->allowedItems.constEnd();
    while (i != end) {
        if ((*i)->isDefine()) {
            isMixed(*i, seen, mc);
        } else if ((*i)->isElement()) {
            ++mc.elementCount;
        }
        ++i;
    }
}

/**
 * Determine if the RNGItem item may contain a mix of text nodes and elements.
 * This function call a helper function that inspects the item recursively.
 * @param item item to be investigated
 * @return true if item may contain a mix of text nodes and elements
 */
bool isMixed(const RNGItemPtr& item)
{
    RNGItems seen;
    MixedCheck mc;
    isMixed(item, seen, mc);
    return mc.mixed || mc.elementCount == 0;
}

/**
 * Merge item b in to item a.
 */
void merge(RNGItemPtr& a, const RNGItemPtr& b)
{
    if (b->mixedContent) {
        a->mixedContent = true;
    }
    Q_ASSERT(a->allowedItems.contains(b));
    if (a->requiredItems.contains(b)) {
        foreach(RNGItemPtr i, b->requiredItems) {
            a->requiredItems.insert(i);
        }
        a->requiredItems.remove(b);
    }
    foreach(RNGItemPtr i, b->allowedItems) {
        a->allowedItems.insert(i);
    }
    a->allowedItems.remove(b);
}

/**
 * Sort function to sort the items in a nice way.
 * <define/> items (including <start/> go first.
 * <element/> items come next.
 * <attribute/> items go last.
 * Items of similar type are compared by their names.
 */
bool rngItemPtrLessThan(const RNGItemPtr &a, const RNGItemPtr &b)
{
    if (a->isDefine()) {
        if (b->isDefine()) {
            return a->name() < b->name();
        }
        return true;
    }
    if (b->isDefine()) {
        return false;
    }
    if (a->isElement()) {
        if (b->isElement()) {
            if (a->name() == b->name()) {
                // cppname maybe different, it can have e.g. a number appended
                return a->cppName < b->cppName;
            }
            return a->name() < b->name();
        }
        return true;
    }
    if (b->isElement()) {
        return false;
    }
    if (a->name() == b->name()) {
        return a->cppName < b->cppName;
    }
    return a->name() < b->name();
}

/**
 * Class that has a separate open header file for each namespace and each
 * combination of namespaces.
 * This object is passed around the code generating functions instead of
 * a single output stream for a single header file.
 */
class Files
{
    /**
     * List of open files.
     */
    QMap<QString,QMap<QString,QTextStream*> > files;
    /**
     * Directory into which to write all the header files.
     */
    const QString outdir;
public:
    Files(const QString& outdir_) :outdir(outdir_) {}
    /**
     * Close all open files after writing the closing '#endif'
     */
    ~Files() {
        typedef const QMap<QString,QTextStream*> map;
        foreach (map& m, files) {
            foreach (QTextStream* out, m) {
                *out << "#endif\n";
                out->device()->close();
                delete out->device();
                delete out;
            }
        }
    }
    QTextStream& getFile(const QString& tag, const QString& tag2);
    void closeNamespace();
};

/**
 * Create a map that maps each Relax NG type to a Qt/C++ type.
 */
QMap<QString, QString> createTypeMap()
{
    QMap<QString, QString> map;
    map.insert("string", "const QString&");
    map.insert("date", "const QDate&");
    map.insert("time", "const QTime&");
    map.insert("dateTime", "const QDateTime&");
    map.insert("duration", "Duration");
    map.insert("integer", "qint64");
    map.insert("nonNegativeInteger", "quint64");
    map.insert("positiveInteger", "quint64");
    map.insert("double", "double");
    map.insert("anyURI", "const QUrl&");
    map.insert("base64Binary", "const QByteArray&");
    map.insert("ID", "const QString&");
    map.insert("IDREF", "const QString&");
    map.insert("IDREFS", "const QStringList&");
    map.insert("NCName", "const QString&");
    map.insert("language", "const QString&");
    map.insert("token", "const QString&");
    map.insert("QName", "const QString&");
    map.insert("decimal", "double");
    return map;
}

/**
 * Return a Qt/C++ type for each Relax NG type.
 */
QString mapType(const QString& type)
{
    static const QMap<QString, QString> map = createTypeMap();
    if (!map.contains(type)) {
        fatal() << "Unknown data type " << type;
    }
    return map.value(type);
}

/**
 * see below
 */
void parseContent(const QDomElement& content, RNGItem& item, bool required);

/**
 * Get a list of names for the attribute or element.
 */
QStringList getNames(const QDomElement& e)
{
    QStringList names;
    QString name = e.attribute("name");
    if (name.isEmpty()) {
        QDomElement ce = e.firstChildElement();
        if (ce.localName() == "choice") {
            ce = ce.firstChildElement();
            while (!ce.isNull()) {
                if (ce.localName() == "name") {
                    names << ce.text();
                } else {
                    fatal() << "Found element without comprehensible name.";
                }
                ce = ce.nextSiblingElement();
            }
        } else if (ce.localName() != "anyName") {
            fatal() << "Found element without comprehensible name.";
        }
    } else {
        names << name;
    }
    return names;
}

/**
 * Parse an <element/> element.
 */
void parseElement(const QDomElement& e, RNGItem& parent, bool required)
{
    QStringList names = getNames(e);
    foreach (const QString& name, names) {
        RNGItemPtr element = RNGItemPtr(new Element(name));
        parseContent(e, *element, true);
        parent.allowedItems.insert(element);
        if (required) {
            parent.requiredItems.insert(element);
        }
    }
}

/**
 * Parse an <attribute/> element.
 */
void parseAttribute(const QDomElement& e, RNGItem& parent, bool required)
{
    QStringList names = getNames(e);
    foreach (const QString& name, names) {
        RNGItemPtr attribute = RNGItemPtr(new Attribute(name));
        parseContent(e, *attribute, true);
        parent.allowedItems.insert(attribute);
        if (required) {
            parent.requiredItems.insert(attribute);
        }
    }
}

/**
 * Parse the contents of any Relax NG element.
 */
void parseContent(const QDomElement& content, RNGItem& item, bool required)
{
    QDomElement e = content.firstChildElement();
    while (!e.isNull()) {
        QString type = e.localName();
        QString name = e.attribute("name");
        if (type == "interleave" || type == "oneOrMore" || type == "group") {
            parseContent(e, item, required);
        } else if (type == "optional" || type == "choice"
                   || type == "zeroOrMore") {
            parseContent(e, item, false);
        } else if (type == "ref") {
            item.referencedDeclares.insert(name);
            if (required) {
                item.requiredReferencedDeclares.insert(name);
            }
        } else if (type == "empty") {
        } else if (type == "data") {
            Datatype d;
            d.type = mapType(e.attribute("type"));
            item.datatype.insert(d);
        } else if (type == "list") {
        } else if (type == "description") {
        } else if (type == "attribute") {
            parseAttribute(e, item, required);
        } else if (type == "element") {
            parseElement(e, item, required);
        } else if (type == "text") {
            item.mixedContent = true;
        } else if (type == "value") {
            Datatype d;
            d.constant = e.text();
            item.datatype.insert(d);
        } else if (type == "name") {
        } else if (type == "anyName") {
        } else if (type == "mixed") {
        } else {
            fatal() << "Unknown element " << type;
        }
        e = e.nextSiblingElement();
    }
}

/**
 * Parse the contents of a <define/> or <start/> element.
 */
RNGItemPtr parseDefine(const QDomElement& defineElement, RNGItems& items, bool isstart)
{
    RNGItemPtr item;
    if (isstart) {
        item = RNGItemPtr(new Start());
    } else {
        item = RNGItemPtr(new Define(defineElement.attribute("name")));
    }
    parseContent(defineElement, *item, true);
    items.insert(item);
    return item;
}

/**
 * Parse all top level Relax NG elements.
 */
RNGItemPtr getDefines(QDomElement e, RNGItems& items)
{
    RNGItemPtr start = RNGItemPtr(0);
    e = e.firstChildElement();
    while (!e.isNull()) {
        if (e.localName() == "define") {
            parseDefine(e, items, false);
        } else if (e.localName() == "start") {
            Q_ASSERT_X(!start, "getDefines", "Multiple start elements.");
            start = parseDefine(e, items, true);
        } else {
            fatal() << "Unknown element " << e.localName();
        }
        e = e.nextSiblingElement();
    }
    return start;
}

/**
 * Load an XML from disk into a DOMDocument instance.
 */
QDomDocument loadDOM(const QString& url)
{
    QFile f(url);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();

    QDomDocument dom;
    QString err;
    if (!dom.setContent(data, true, &err)) {
        fatal() << err;
    }
    return dom;
}

/**
 * Look through a set of RNGitems to find one that is the same.
 * This can be used after parsing to find definitions that are the same.
 * Such deduplication can reduce the size of the generated code.
 */
RNGItemPtr findEqualItem(const RNGItemPtr&i, const RNGItems& items)
{
    foreach (const RNGItemPtr& j, items) {
        if (*i == *j) {
            return j;
        }
    }
    return RNGItemPtr();
}

/**
 * Compare two RNGItem instances.
 */
bool RNGItem::operator==(const RNGItem& a) const
{
    bool unequal = m_type != a.m_type
            || m_name != a.m_name
            || mixedContent != a.mixedContent
            || cppName != a.cppName
            || referencedDeclares != a.referencedDeclares
            || requiredReferencedDeclares != a.requiredReferencedDeclares
            || allowedItems.size() != a.allowedItems.size()
            || requiredItems.size() != a.requiredItems.size()
            || datatype != a.datatype;
    if (unequal) {
        return false;
    }
    foreach (const RNGItemPtr& i, allowedItems) {
        RNGItemPtr j = findEqualItem(i, a.allowedItems);
        if (!j) {
            return false;
        }
    }
    foreach (const RNGItemPtr& i, requiredItems) {
        RNGItemPtr j = findEqualItem(i, a.requiredItems);
        if (!j) {
            return false;
        }
    }
    return true;
}

/**
 * Move all member items in the global list.
 * If there is already a global member that is equal, use that in the item.
 */
void collect(RNGItem& item, RNGItems& collected)
{
    typedef QPair<RNGItemPtr,RNGItemPtr> Pair;
    QList<Pair> toSwap;
    foreach (const RNGItemPtr& i, item.allowedItems) {
        RNGItemPtr j = findEqualItem(i, collected);
        if (!j) {
            collected.insert(i);
            collect(*i, collected);
        } else if (i != j) {
            toSwap.append(qMakePair(i, j));
        }
    }
    foreach (const Pair& i, toSwap) {
        RNGItemPtr toRemove = i.first;
        RNGItemPtr toAdd = i.second;
        if (item.requiredItems.contains(toRemove)) {
            item.requiredItems.remove(toRemove);
            item.requiredItems.insert(toAdd);
        }
        item.allowedItems.remove(toRemove);
        item.allowedItems.insert(toAdd);
    }
}

/**
 * Move all member items in the global list.
 * If there is already a global member that is equal, use that in the item.
 */
void collect(const RNGItems& items, RNGItems& collected)
{
    foreach (const RNGItemPtr& item, items) {
        collect(*item, collected);
    }
}

/**
 * Count how often a particular item is used by other items or itself.
 */
void countUsage(RNGItem& item, QHash<RNGItemPtr,int>& usageCount)
{
    foreach (const RNGItemPtr& i, item.allowedItems) {
        if (usageCount.contains(i)) {
            usageCount[i]++;
        } else {
            usageCount[i] = 1;
        }
    }
}

/**
 * Remove items that are not used and merge items that are used in only one
 * place into their parent if possible.
 * This reduces the number of classes in the generated headers.
 */
int reduce(RNGItems& items)
{
    QHash<RNGItemPtr,int> usageCount;
    foreach (const RNGItemPtr& item, items) {
        countUsage(*item, usageCount);
    }
    RNGItems toRemove;
    foreach (RNGItemPtr item, items) {
        if (usageCount[item] <= 1 && !item->isStart() && item->isDefine()) {
            RNGItemPtr user = RNGItemPtr(0);
            foreach (const RNGItemPtr& i, items) {
                if (i->allowedItems.contains(item)) {
                    Q_ASSERT(!user);
                    user = i;
                }
            }
            if (user) {
                merge(user, item);
            }
            toRemove.insert(item);
            break;
        }
    }
    foreach (const RNGItemPtr& item, toRemove) {
        items.remove(item);
    }
    return toRemove.size();
}

/**
 * Collect items that are contained in other items into a list with
 * all the items.
 * Relax NG is a hierarchical file format and this function creates a flat list
 * with all items.
 */
int expand(RNGItems& items)
{
    RNGItems toAdd;
    foreach (RNGItemPtr item, items) {
        foreach (const RNGItemPtr& i, item->allowedItems) {
            if (!items.contains(i)) {
                toAdd.insert(i);
            }
        }
    }
    foreach (RNGItemPtr item, toAdd) {
        items.insert(item);
    }
    return toAdd.size();
}

/**
 * Find the <define/> item by name.
 */
RNGItemPtr getDefine(const QString& name, const RNGItems& items)
{
    RNGItemPtr item = RNGItemPtr(0);
    foreach (RNGItemPtr i, items) {
        if (i->name() == name) {
            Q_ASSERT_X(!item, "getDefine", qPrintable("Doubly defined element" + name + "."));
            item = i;
        }
    }
    Q_ASSERT_X(item, "getDefine", qPrintable("Define not found " + name));
    return item;
}

/**
 * Resolve all <define/> references.
 * After parsing, the <ref/> instances should be replaced by the actual
 * items.
 */
void resolveDefines(RNGItemPtr start, const RNGItems& items, RNGItems& resolved)
{
    if (resolved.contains(start)) {
        return;
    }
    resolved.insert(start);
    foreach (const QString& name, start->referencedDeclares) {
        RNGItemPtr i = getDefine(name, items);
        if (start->requiredReferencedDeclares.contains(name)) {
            start->requiredItems.insert(i);
        }
        start->allowedItems.insert(i);
    }
    start->referencedDeclares.clear();
    start->requiredReferencedDeclares.clear();

    foreach (RNGItemPtr item, start->allowedItems) {
        resolveDefines(item, items, resolved);
    }
}

/**
 * Create a C++ name from the item name.
 */
QString makeCppName(const RNGItemPtr&item)
{
    QString name;
    if (item->isElement() || item->isAttribute()) {
        name = item->name();
    } else {
        name = "group_" + item->name();
    }
    name.replace(':', '_');
    name.replace('-', '_');
    return name;
}

/**
 * Create a new name from the item name.
 * The new name will not clash with the names from takenNames.
 */
QString makeUniqueCppName(const RNGItemPtr&item, QSet<QString>& takenNames)
{
    QString n = makeCppName(item);
    QString name = n;
    int i = 0;
    while (!name.isEmpty() && takenNames.contains(name)) {
        name = n + "_" + QString::number(++i);
    }
    takenNames.insert(name);
    return name;
}

/**
 * Create all the C++ names corresponding with the Relax NG items.
 */
void makeCppNames(RNGItemList& items)
{
    QSet<QString> cppnames;
    // handle elements first so they have the nicest names
    foreach (RNGItemPtr item, items) {
        if (item->isElement()) {
            item->cppName = makeUniqueCppName(item, cppnames);
        }
    }
    // next handle the attributes
    foreach (RNGItemPtr item, items) {
        if (item->isAttribute()) {
            item->cppName = makeUniqueCppName(item, cppnames);
        }
    }
    // give the remaining declares names
    foreach (RNGItemPtr item, items) {
        if (item->isDefine()) {
            item->cppName = makeUniqueCppName(item, cppnames);
        }
    }
}

/**
 * Check if an element or attribute is defined below this item.
 */
bool hasElementOrAttribute(const RNGItemPtr& item, RNGItems& items)
{
    if (items.contains(item)) {
        return false;
    }
    foreach (const RNGItemPtr& i, item->allowedItems) {
        if (!i->isDefine() || hasElementOrAttribute(i, items)) {
            return true;
        }
    }
    return false;
}

/**
 * Check if an element or attribute is defined below this item.
 */
bool hasElementOrAttribute(const RNGItemPtr& item)
{
    RNGItems items;
    return hasElementOrAttribute(item, items);
}

static RNGItemList toVector(const RNGItems& list) {
    RNGItemList l;
    l.reserve(list.size());
    std::copy(list.constBegin(), list.constEnd(), std::back_inserter(l));
    return l;
}

/**
 * Find all the items that are used in this item but are not element or
 * attributes.
 * These items will be base classes to a class that corresponds to an element.
 */
RNGItemList getBasesList(RNGItemPtr item)
{
    RNGItems list;
    RNGItems antilist;
    foreach (RNGItemPtr i, item->allowedItems) {
        if (i->isDefine() && hasElementOrAttribute(i)) {
            list.insert(i);
            foreach (RNGItemPtr j, i->allowedItems) {
                if (j->isDefine() && j != i) {
                    antilist.insert(j);
                }
            }
        }
    }
    list.subtract(antilist);
    RNGItemList l = toVector(list);
    std::stable_sort(l.begin(), l.end(), rngItemPtrLessThan);
    return l;
}

/**
 * Sort items in the set.
 * This is helpful in making the output reproducible.
 */
RNGItemList list(const RNGItems& items)
{
    RNGItemList list = toVector(items);
    std::stable_sort(list.begin(), list.end(), rngItemPtrLessThan);
    return list;
}

/**
 * Collect the data types of the attribute item.
 */
void resolveType(const RNGItemPtr& item, QSet<Datatype>& type)
{
    type.unite(item->datatype);
    foreach (const RNGItemPtr& i, item->allowedItems) {
        resolveType(i, type);
    }
}

/**
 * Collect the data types of the attributes.
 */
void resolveAttributeDataTypes(const RNGItems& items)
{
    foreach (const RNGItemPtr& i, items) {
        if (i->isAttribute()) {
            resolveType(i, i->datatype);
        }
    }
}

/**
 * Create a ordered list of items.
 * The order is such that dependencies of an item precede the item in the list.
 */
void addInOrder(RNGItemList& undefined, RNGItemList& defined)
{
    int last = -1;
    while (last != undefined.size()) {
        last = undefined.size();
        for (int i = 0; i < undefined.size(); ++i) {
            const RNGItemPtr& ii = undefined[i];
            bool missingDependency = false;
            foreach (const RNGItemPtr& j, list(ii->allowedItems)) {
                if (j->isDefine() && !defined.contains(j) && j != ii) {
                    if (undefined.contains(j)) {
                        missingDependency = true;
                    } else if (j->name().isEmpty()) {
                        ii->allowedItems.remove(j);
                        ii->requiredItems.remove(j);
                    }
                }
            }
            if (!missingDependency) {
                defined.append(ii);
                undefined.remove(i);
            } else {
                ++i;
            }
        }
    }
    if (undefined.size()) {
        fatal() << undefined.size() << " missing dependencies";
        undefined.clear();
    }
    // Q_ASSERT(undefined.size() == 0);
}

/**
 * Helper structure to collect required arguments.
 */
struct RequiredArgsList
{
    int length;
    QString args;
    QString vals;
};

/**
 * Write lists of required arguments that can be used in generated code.
 * This list only covers the required attributes, not required elements.
 */
RequiredArgsList makeRequiredArgsList(const RNGItemPtr& item)
{
    RequiredArgsList r;
    r.length = 0;
    foreach (RNGItemPtr i, list(item->requiredItems)) {
        if (i->isAttribute() && i->singleConstant().isNull()) {
            QString name = makeCppName(i);
            QString type = i->singleType();
            if (type.isNull()) {
                type = "const QString&";
            }
            r.args += type + " " + name + ", ";
            r.vals += ", " + name;
            ++r.length;
        }
    }
    r.args.chop(2);
    return r;
}

/**
 * Recursively find the items that are required for the given item.
 */
RNGItemList getAllRequiredAttributes(const RNGItemPtr& item, RNGItemList& list, int depth = 0)
{
    if (depth > 10) {
        return list;
    }
    foreach (RNGItemPtr i, item->allowedItems) {
        if (item->requiredItems.contains(i)) {
            if (i->isAttribute() && i->singleConstant().isNull()) {
                list.append(i);
            } else if (i->isDefine()) {
                getAllRequiredAttributes(i, list, depth + 1);
            }
        }
    }
    return list;
}

/**
 * Write full lists of required arguments that can be used in generated code.
 */
RequiredArgsList makeFullRequiredArgsList(const RNGItemPtr& item)
{
    RequiredArgsList r;
    r.length = 0;
    RNGItemList list;
    getAllRequiredAttributes(item, list);
    std::stable_sort(list.begin(), list.end(), rngItemPtrLessThan);
    foreach (RNGItemPtr i, list) {
        QString name = makeCppName(i);
        QString type = i->singleType();
        if (type.isNull()) {
            type = "const QString&";
        }
        r.args += type + " " + name + ", ";
        r.vals += ", " + name;
        ++r.length;
    }
    r.args.chop(2);
    return r;
}

/**
 * Write C++ code to set the required attribute values.
 */
void setRequiredAttributes(QTextStream& out, const RNGItemPtr& item)
{
    QString o;
    if (!item->isElement()) {
        o = "xml.";
    }
    foreach (RNGItemPtr i, list(item->requiredItems)) {
        if (i->isAttribute()) {
            out << "        " << o << "addAttribute(\"" + i->name() + "\", ";
            QString constant = i->singleConstant();
            if (constant.isNull()) {
                out << makeCppName(i);
            } else {
                out << "\"" << constant << "\"";
            }
            out << ");\n";
        }
    }
}

/**
 * Write the class definition for a class that corresponds to an xml element.
 */
void defineElement(QTextStream& out, const RNGItemPtr& item)
{
    const RNGItemList bases = getBasesList(item);
    out << "class " << item->cppName << " : public OdfWriter";
    for (auto i = bases.begin(); i != bases.end(); ++i) {
        out << ", public " << (*i)->cppName;
    }
    out << " {\n";
    out << "public:" << "\n";
    RequiredArgsList r = makeFullRequiredArgsList(item);
    if (r.args.length()) {
        r.args = ", " + r.args;
    }
    out << "    " << item->cppName << "(OdfWriter* x" << r.args
        << ") :OdfWriter(x, \"" << item->name() << "\", "
        << (isMixed(item) ?"false" :"true") << ")";
    for (auto i = bases.begin(); i != bases.end(); ++i) {
        RequiredArgsList r;
        if (item->requiredItems.contains(*i)) {
            r = makeFullRequiredArgsList(*i);
        }
        out << ", " << (*i)->cppName << "(*static_cast<OdfWriter*>(this)" << r.vals << ")";
    }
    out << " {\n";
    setRequiredAttributes(out, item);
    out << "    }\n";
    out << "    " << item->cppName << "(KoXmlWriter* x" << r.args
        << ") :OdfWriter(x, \"" << item->name() << "\", "
        << (isMixed(item) ?"false" :"true") << ")";
    for (auto i = bases.begin(); i != bases.end(); ++i) {
        RequiredArgsList r;
        if (item->requiredItems.contains(*i)) {
            r = makeFullRequiredArgsList(*i);
        }
        out << ", " << (*i)->cppName << "(*static_cast<OdfWriter*>(this)" << r.vals << ")";
    }
    out << " {\n";
    setRequiredAttributes(out, item);
    out << "    }\n";
    QSet<QString> doneA;
    QSet<QString> doneE;
    foreach (RNGItemPtr i, list(item->allowedItems)) {
        QString name = makeCppName(i);
        if (i->isAttribute() && !item->requiredItems.contains(i) && !doneA.contains(name)) {
            QString type = i->singleType();
            if (type.isNull()) {
                out << "    template<class T>\n";
                type = "const T&";
            }
            out << "    void set_" << name << "(" + type + " value) {\n";
            out << "        addAttribute(\"" + i->name() + "\", value);\n";
            out << "    }\n";
            doneA.insert(name);
        } else if (i->isElement() && !doneE.contains(name)) {
            RequiredArgsList r = makeFullRequiredArgsList(i);
            out << "    " << i->cppName << " add_" << name << "(" + r.args + ");\n";
            doneE.insert(name);
        }
    }
    if (isMixed(item)) {
        out << "    void addTextNode(const QString& data) {\n";
        out << "        OdfWriter::addTextNode(data);\n";
        out << "    }\n";
    }
    out << "};\n";
}

/**
 * Write the class definition for a class that corresponds to a Relax NG group.
 * These groups are bases to classes that correspond to elements.
 */
void defineGroup(QTextStream& out, const RNGItemPtr& item)
{
    if (!hasElementOrAttribute(item)) {
        return;
    }
    const RNGItemList bases = getBasesList(item);
    out << "class " << item->cppName;
    if (bases.size()) {
        RNGItemList::const_iterator i = bases.begin();
        out << " : public " << (*i)->cppName;
        while (++i != bases.end()) {
            out << ", public " << (*i)->cppName;
        }
    }
    out << " {\n";
    out << "private:\n";
    out << "    OdfWriter& xml;\n";
    out << "public:\n";
    RequiredArgsList r = makeFullRequiredArgsList(item);
    if (r.args.length()) {
        r.args = ", " + r.args;
    }
    out << "    " << item->cppName << "(OdfWriter& x" + r.args + ") :";
    foreach (const RNGItemPtr& i, bases) {
        RequiredArgsList r;
        if (item->requiredItems.contains(i)) {
            r = makeFullRequiredArgsList(i);
        }
        out << i->cppName << "(x" + r.vals + "), ";
    }
    out << "xml(x) {\n";
    setRequiredAttributes(out, item);
    out << "    }\n";
    if (r.length) {
        out << "    " << item->cppName << "(OdfWriter& x) :";
        foreach (const RNGItemPtr& i, bases) {
            out << i->cppName << "(x), ";
        }
        out << "xml(x) {}\n";
    }
    QSet<QString> done;
    foreach (RNGItemPtr i, list(item->allowedItems)) {
        QString name = makeCppName(i);
        // also allow setting of required elements, because the might need to be
        // set in elements where the group is optional
        if (i->isAttribute() && !done.contains(name)) {
            QString type = i->singleType();
            if (type.isNull()) {
                out << "    template<class T>\n";
                type = "const T&";
            }
            out << "    void set_" << name << "(" << type << " value) {\n";
            out << "        xml.addAttribute(\"" + i->name() + "\", value);\n";
            out << "    }\n";
            done.insert(name);
        } else if (i->isElement()) {
            RequiredArgsList r = makeFullRequiredArgsList(i);
            out << "    " << i->cppName << " add_" << name << "(" + r.args + ");\n";
        }
    }
    if (isMixed(item)) {
        out << "    void addTextNode(const QString& data) {\n";
        out << "        xml.addTextNode(data);\n";
        out << "    }\n";
    }
    out << "};\n";
}

/**
 * Write the definition for a member function to add an element to another
 * element.
 */
void writeAdderDefinition(const RNGItemPtr& item, const RNGItemPtr& i, QTextStream& out)
{
    QString name = makeCppName(i);
    RequiredArgsList r = makeFullRequiredArgsList(i);
    out << "inline ";
    if (!ns().isEmpty()) {
        out << ns() << "::";
    }
    out << i->cppName << "\n";
    if (!ns().isEmpty()) {
        out << ns() << "::";
    }
    out << item->cppName << "::add_" << name << "(";
    out << r.args << ") {\n";
    out << "    return " << ns() << "::" << i->cppName << "(";
    if (item->isElement()) {
        out << "this";
    } else {
        out << "&xml";
    }
    out << r.vals << ");\n";
    out << "}\n";
}

/**
 * Write the definitions for member functions to add elements to other
 * element.
 */
void writeAdderDefinitions(const RNGItemPtr& item, Files& files)
{
    QSet<QString> done;
    foreach (RNGItemPtr i, list(item->allowedItems)) {
        QString name = makeCppName(i);
        if (i->isElement() && !done.contains(name)) {
            QString tag1 = (item->isElement()) ?item->name() :QString();
            QTextStream& out = files.getFile(tag1, i->name());
            writeAdderDefinition(item, i, out);
            done.insert(name);
        }
    }
}

/**
 * Write the definitions for member functions to add elements to other
 * element.
 */
void writeAdderDefinitions(const RNGItemList& items, Files& files)
{
    foreach (RNGItemPtr item, items) {
        writeAdderDefinitions(item, files);
    }
}

/**
 * Retrieve the namespace prefix from the tag name.
 * @param tag string with a tag, may be null.
 */
QString getPrefix(const QString& tag)
{
    QString prefix = tag.left(tag.indexOf(":"));
    if (prefix.isNull()) {
        prefix = "";
    }
    return prefix;
}

/**
 * Get the stream for the combination of the two tags.
 * For tag1 = "office:text" and tag2 = "text:p", this returns a stream to a file
 * "writeodfofficetext.h".
 */
QTextStream& Files::getFile(const QString& tag1, const QString& tag2 = QString())
{
    // each tag can result in either no prefix or a prefix
    // if one if the prefixes is empty and the other is not, then the first
    // prefix is given a value
    QString prefix = getPrefix(tag1);
    QString prefix2 = getPrefix(tag2);
    if (prefix.isEmpty() || prefix == prefix2) {
        prefix = prefix2;
        prefix2 = "";
    }
    if (files.contains(prefix) && files[prefix].contains(prefix2)) {
        return *files[prefix][prefix2];
    }
    QString name = "writeodf" + prefix + prefix2 + ".h";
    QString path = outdir + "/" + name;
    QFile* file = new QFile(path);
    if (!file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        fatal() << file->errorString();
    }
    QTextStream* out = new QTextStream(file);
    name = name.replace(".", "_").toUpper();

    *out << "#ifndef " + name + "\n";
    *out << "#define " + name + "\n";
    if (name == "WRITEODF_H") {
        *out << "#include \"writeodf/odfwriter.h\"\n";
    } else {
        *out << "#include \"writeodf.h\"\n";
    }
    if (!prefix2.isEmpty() && prefix2 != prefix) {
        *out << "#include \"writeodf" + prefix + ".h\"\n";
        *out << "#include \"writeodf" + prefix2 + ".h\"\n";
    } else {
        *out << "namespace " << ns() << " {\n";
    }
    files[prefix][prefix2] = out;
    return *out;
}

/**
 * Close the namespace if it was opened previously.
 */
void Files::closeNamespace()
{
    typedef const QMap<QString,QTextStream*> map;
    foreach (map& m, files) {
        map::ConstIterator i = m.begin();
        while (i != m.end()) {
            if (!i.key().isNull() && !ns().isEmpty()) {
                *i.value() << "}\n";
            }
            ++i;
        }
    }
}

/**
 * Write the header files.
 */
void write(const RNGItemList& items, const QString &outdir)
{
    Files files(outdir);

    QTextStream& out = files.getFile("", "");
    RNGItemList undefined = items;
    RNGItemList defined;
    addInOrder(undefined, defined);
    // declare all classes
    foreach (RNGItemPtr item, defined) {
        if (item->isElement() || (item->isDefine() && hasElementOrAttribute(item))) {
            out << "class " << item->cppName << ";\n";
        }
    }
    foreach (RNGItemPtr item, defined) {
        if (item->isElement()) {
            defineElement(files.getFile(item->name()), item);
        } else if (item->isDefine()) {
            defineGroup(out, item);
        }
    }
    files.closeNamespace();
    writeAdderDefinitions(defined, files);
}

/**
 * Convert the given rng file to a collection of header files.
 */
void convert(const QString& rngfile, const QString& outdir)
{
    QDomDocument dom = loadDOM(rngfile);
    RNGItems items;
    RNGItemPtr start = getDefines(dom.documentElement(), items);
    RNGItems collected;
    //qDebug() << "define " << items.size();
    //collect(items, collected);
    collected = items;
    //qDebug() << "collect " << collected.size();
    RNGItems resolved;
    resolveDefines(start, collected, resolved);
    //qDebug() << "resolve " << resolved.size();
    //while (expand(resolved)) {}
    resolved.remove(start);
    //qDebug() << "expand " << resolved.size();
    resolveAttributeDataTypes(resolved);
    while (reduce(resolved)) {}
    //qDebug() << "reduce " << resolved.size();
    RNGItemList list = toVector(resolved);
    //qDebug() << "filteredItems " << list.size();
    std::stable_sort(list.begin(), list.end(), rngItemPtrLessThan);
    makeCppNames(list);
    write(list, outdir);
    //qDebug() << list.size();
}

int main(int argc, char *argv[])
{
    QString rngfile;
    QString outdir;
    if (argc != 3) {
        fatal() << "Usage " << argv[0] << " rngfile outputdir";
    } else {
        rngfile = argv[1];
        outdir = argv[2];
    }
    convert(rngfile, outdir);
    return 0;
}
