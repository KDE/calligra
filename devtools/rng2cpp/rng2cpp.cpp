#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QMap>
#include <QtGlobal>
#include <QSharedPointer>

//#define FULL
//#define SUB

#define FAKE_KOXML

#ifdef FULL
#include "out.h"
void test() {
}

#elif defined(SUB)
#include "odf.h"

void test() {
}
#else

void test() {
}

#endif

#define assert(cond, what) (Q_ASSERT_X(cond,"",(const char*)QString(what).toAscii()))

static const QString ns = "writeodf";

class RNGItem;
//typedef QSharedPointer<RNGItem> RNGItemPtr;
typedef RNGItem* RNGItemPtr;
typedef QSet<RNGItemPtr> RNGItems;
typedef QVector<RNGItemPtr> RNGItemList;

class Datatype {
public:
    bool operator==(const Datatype& a) const;
    bool operator!=(const Datatype& a) const {
        return !(*this == a);
    }
    QString constant;
    QString type;
};
bool
Datatype::operator==(const Datatype& a) const {
    return constant == a.constant
            && type == a.type;
}
uint qHash(const Datatype& d) {
    return qHash(d.constant) + qHash(d.type);
}

class RNGItem {
private:
    RNGItem(const RNGItem&);
    void operator=(const RNGItem&);
protected:
    bool iselement;
    bool isattribute;
public:
    bool isStart;
    bool mixedContent;
    QString defineName;
    QString elementName;
    QString attributeName;
    QString cppName;
    RNGItems allowedItems;
    RNGItems requiredItems;
    QSet<QString> referencedDeclares;
    QSet<QString> requiredReferencedDeclares;
    QSet<Datatype> datatype;

    RNGItem() :iselement(false), isattribute(false), isStart(false), mixedContent(false) {
    }
    bool isElement() const { return iselement; }
    bool isAttribute() const { return isattribute; }
    QString singleConstant() const {
        return datatype.size() == 1 ?datatype.constBegin()->constant :QString();
    }
    QString singleType() const {
        return datatype.size() == 1 ?datatype.constBegin()->type :QString();
    }
    bool isDefine() const {
        bool isdefine = !iselement && !isattribute;
        assert(!isdefine || isStart || defineName.length(), elementName + " " + attributeName);
        return isdefine;
    }
    bool operator==(const RNGItem& a) const;
};
class Element : public RNGItem {
public:
    Element(const QString& name) {
        iselement = true;
        elementName = name;
    }
};
class Attribute : public RNGItem {
public:
    Attribute(const QString& name) {
        isattribute = true;
        attributeName = name;
    }
};
bool
isMixed(const RNGItemPtr& item, RNGItems& seen) {
    bool mixed = false;
    if (item->mixedContent || seen.contains(item)) {
        return item->mixedContent;
    }
    seen.insert(item);
    RNGItems::ConstIterator i = item->allowedItems.constBegin();
    RNGItems::ConstIterator end = item->allowedItems.constEnd();
    while (!mixed && i != end) {
        mixed = isMixed(*i, seen);
        ++i;
    }
    return mixed;
}
bool
isMixed(const RNGItemPtr& item) {
    RNGItems seen;
    return isMixed(item, seen);
}

// merge b in a
void
merge(RNGItemPtr& a, const RNGItemPtr& b) {
    if (isMixed(b)) {
        a->mixedContent = true;
    }
    assert(a->allowedItems.contains(b), "");
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

bool rngItemPtrLessThan2(const RNGItemPtr &a, const RNGItemPtr &b) {
    return a->cppName < b->cppName;
}

bool rngItemPtrLessThan(const RNGItemPtr &a, const RNGItemPtr &b) {
    if (a->isDefine()) {
        if (b->isDefine()) {
            return a->defineName < b->defineName;
        }
        return true;
    }
    if (b->isDefine()) {
        return false;
    }
    if (a->isElement()) {
        if (b->isElement()) {
            if (a->elementName == b->elementName) {
                return a->cppName < b->cppName;
            }
            return a->elementName < b->elementName;
        }
        return true;
    }
    if (b->isElement()) {
        return false;
    }
    if (a->attributeName == b->attributeName) {
        return a->cppName < b->cppName;
    }
    return a->attributeName < b->attributeName;
}

bool
hasChild(const RNGItemPtr &parent, const RNGItemPtr &child) {
    foreach (const RNGItemPtr& i, parent->allowedItems) {
        if (!i->isElement() && !i->isAttribute()) {
            if (child == i || hasChild(i, child)) {
                return true;
            }
        }
    }
    return false;
}
class fatal {
private:
    QString msg;
public:
    fatal() {}
    ~fatal() {
        qFatal("%s", (const char*)msg.toAscii());
    }
    fatal& operator<<(const QString& s) {
        msg += s;
        return *this;
    }
};

class Files {
    QMap<QString,QMap<QString,QTextStream*> > files;
    const QString outdir;
public:
    Files(const QString& outdir_) :outdir(outdir_) {}
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

QMap<QString, QString>
createTypeMap() {
    QMap<QString, QString> map;
    map.insert("string", "const QString&");
    map.insert("date", "const QDate&");
    map.insert("time", "const QTime&");
    map.insert("dateTime", "const QDateTime&");
    map.insert("duration", "qint64");
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
QString
mapType(const QString& type) {
    static const QMap<QString, QString> map = createTypeMap();
    if (!map.contains(type)) {
        fatal() << "Unknown data type " << type;
    }
    return map.value(type);
}

void parseContent(QDomElement content, RNGItem& item, bool required);

RNGItemPtr
parseElement(QDomElement e) {
    RNGItemPtr element= RNGItemPtr(new Element(e.attribute("name")));
    parseContent(e, *element, true);
    return element;
}
RNGItemPtr
parseAttribute(QDomElement e) {
    RNGItemPtr attribute = RNGItemPtr(new Attribute(e.attribute("name")));
    parseContent(e, *attribute, true);
    return attribute;
}
void
parseContent(QDomElement content, RNGItem& item, bool required) {
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
            RNGItemPtr i = parseAttribute(e);
            if (i->attributeName.length()) {
                item.allowedItems.insert(i);
                if (required) {
                    item.requiredItems.insert(i);
                }
            }
        } else if (type == "element") {
            RNGItemPtr i = parseElement(e);
            if (i->elementName.length()) {
                item.allowedItems.insert(i);
                if (required) {
                    item.requiredItems.insert(i);
                }
            }
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

RNGItemPtr
parseDefine(QDomElement defineElement, RNGItems& items) {
    RNGItemPtr item;
    item = RNGItemPtr(new RNGItem());
    parseContent(defineElement, *item, true);
    item->defineName = defineElement.attribute("name");
    items.insert(item);
    return item;
}

RNGItemPtr
getDefines(QDomElement e, RNGItems& items) {
    RNGItemPtr start = RNGItemPtr(0);
    e = e.firstChildElement();
    while (!e.isNull()) {
        if (e.localName() == "define") {
            parseDefine(e, items);
        } else if (e.localName() == "start") {
            assert(!start, "Multiple start elements.");
            start = parseDefine(e, items);
            start->isStart = true;
        } else {
            fatal() << "Unknown element " << e.localName();
        }
        e = e.nextSiblingElement();
    }
    return start;
}

QDomDocument
loadDOM(QString url) {
    QFile f(url);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();

    QDomDocument dom;
    QString err;
    if (!dom.setContent(data, true, &err)) {
        qFatal(err.toAscii());
    }
    return dom;
}
RNGItemPtr
findEqualItem(const RNGItemPtr&i, const RNGItems& items) {
    foreach (const RNGItemPtr& j, items) {
        if (*i == *j) {
            return j;
        }
    }
    return RNGItemPtr();
}
bool RNGItem::operator==(const RNGItem& a) const {
    bool unequal = iselement != a.iselement
            || isattribute != a.isattribute
            || isStart != a.isStart
            || mixedContent != a.mixedContent
            || defineName != a.defineName
            || elementName != a.elementName
            || attributeName != a.attributeName
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
void
collect(RNGItem& item, RNGItems& collected) {
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
void
collect(const RNGItems& items, RNGItems& collected) {
    foreach (const RNGItemPtr& item, items) {
        collect(*item, collected);
    }
}
void
countUsage(RNGItem& item, QMap<RNGItemPtr,int>& usageCount) {
    foreach (const RNGItemPtr& i, item.allowedItems) {
        if (usageCount.contains(i)) {
            usageCount[i]++;
        } else {
            usageCount[i] = 1;
        }
    }
}
int
reduce(RNGItems& items) {
    QMap<RNGItemPtr,int> usageCount;
    foreach (const RNGItemPtr& item, items) {
        countUsage(*item, usageCount);
    }
    RNGItems toRemove;
    foreach (RNGItemPtr item, items) {
        if (usageCount[item] <= 1 && !item->isStart && item->isDefine()) {
            RNGItemPtr user = RNGItemPtr(0);
            foreach (const RNGItemPtr& i, items) {
                if (i->allowedItems.contains(item)) {
                    assert(!user, "");
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
int
expand(RNGItems& items) {
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
RNGItemPtr
getDefine(const QString& name, const RNGItems& items) {
    RNGItemPtr item = RNGItemPtr(0);
    foreach (RNGItemPtr i, items) {
        if (i->defineName == name) {
            assert(!item, "Doubly defined element " + name + ".");
            item = i;
        }
    }
    assert(item, "Define not found " + name);
    return item;
}

void
resolveDefines(RNGItemPtr start, const RNGItems& items, RNGItems& resolved) {
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

    typedef QPair<RNGItemPtr,RNGItemPtr> Pair;
    foreach (RNGItemPtr item, start->allowedItems) {
        resolveDefines(item, items, resolved);
    }
}

QString
makeCppName(const RNGItemPtr&item) {
    QString name;
    if (item->isElement()) {
        name = item->elementName;
    } else if (item->isAttribute()) {
        name = item->attributeName;
    } else {
        name = "group_" + item->defineName;
    }
    name.replace(":", "_");
    name.replace("-", "_");
    return name;
}

QString
makeUniqueCppName(const RNGItemPtr&item, QSet<QString>& takenNames) {
    QString n = makeCppName(item);
    QString name = n;
    int i = 0;
    while (!name.isEmpty() && takenNames.contains(name)) {
        name = n + "_" + QString::number(++i);
    }
    takenNames.insert(name);
    return name;
}

void
makeCppNames(RNGItemList& items) {
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

RNGItemList
getBasesList(RNGItemPtr item) {
    RNGItems list;
    RNGItems antilist;
    foreach (RNGItemPtr i, item->allowedItems) {
        if (i->isDefine()) {
            list.insert(i);
            foreach (RNGItemPtr j, i->allowedItems) {
                if (j->isDefine() && j != i) {
                    antilist.insert(j);
                }
            }
        }
    }
    list.subtract(antilist);
    RNGItemList l = list.toList().toVector();
    qStableSort(l.begin(), l.end(), rngItemPtrLessThan);
    return l;
}
// put items in list sorted by cppname
RNGItemList
list(const RNGItems& items) {
    RNGItemList list = items.toList().toVector();
    qStableSort(list.begin(), list.end(), rngItemPtrLessThan);
    return list;
}
RNGItemList
getMissingDeps(const RNGItemPtr& i, const RNGItemList& defined) {
    RNGItemList missing;
    foreach (const RNGItemPtr& j, list(i->allowedItems)) {
        if (!defined.contains(j)) {
            missing.append(i);
        }
    }
    return missing;
}
void
resolveType(const RNGItemPtr& item, QSet<Datatype>& type) {
    type.unite(item->datatype);
    foreach (const RNGItemPtr& i, item->allowedItems) {
        resolveType(i, type);
    }
}
void
resolveType(const RNGItemPtr& item) {
    resolveType(item, item->datatype);
}
void
resolveAttributes(const RNGItems& items) {
    foreach (const RNGItemPtr& i, items) {
        if (i->isAttribute()) {
            resolveType(i);
        }
    }
}
void
matchName(const QString& name, const RNGItemList& list) {
    foreach (RNGItemPtr i, list) {
        if (i->defineName == name) {
            qDebug() << "found " << i->defineName;
        }
    }
}

void
addInOrder(RNGItemList& undefined, RNGItemList& defined) {
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
                    } else if (j->defineName.isEmpty()) {
                        ii->allowedItems.remove(j);
                        ii->requiredItems.remove(j);
                    } else {
                        qDebug() << j->defineName << " (" << j->cppName << ") " << " not found for " << ii->cppName;
                        matchName(j->defineName, undefined);
                        qDebug() << j->isAttribute() << " " << j->isElement() << " " << j->isDefine();
                        matchName(j->defineName, defined);
                        undefined.append(j);
                        //Q_ASSERT(false);
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
        // HACK
        qDebug() << undefined.size() << " missing dependencies";
        foreach (RNGItemPtr i, undefined) {
            qDebug() << "No dep for " << i->cppName;
            foreach (RNGItemPtr j, getMissingDeps(i, defined)) {
                qDebug() << "Missing: " << j->cppName;
            }
            defined.append(i);
        }
        undefined.clear();
    }
    // Q_ASSERT(undefined.size() == 0);
}
struct RequiredArgsList {
    int length;
    QString args;
    QString vals;
};
RequiredArgsList
makeRequiredArgsList(const RNGItemPtr& item) {
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
    r.args = r.args.left(r.args.length() - 2);
    return r;
}
RNGItemList
getAllRequiredAttributes(const RNGItemPtr& item, RNGItemList& list, int depth = 0) {
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
RequiredArgsList
makeFullRequiredArgsList(const RNGItemPtr& item) {
    RequiredArgsList r;
    r.length = 0;
    RNGItemList list;
    getAllRequiredAttributes(item, list);
    qStableSort(list.begin(), list.end(), rngItemPtrLessThan);
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
    r.args = r.args.left(r.args.length() - 2);
    return r;
}
void
setRequiredAttributes(QTextStream& out, const RNGItemPtr& item) {
    QString o;
    if (!item->isElement()) {
        o = "xml.";
    }
    foreach (RNGItemPtr i, list(item->requiredItems)) {
        if (i->isAttribute()) {
            out << "        " << o << "addAttribute(\"" + i->attributeName + "\", ";
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
void
defineElement(QTextStream& out, const RNGItemPtr& item) {
    RNGItemList bases = getBasesList(item);
    out << "class " << item->cppName << " : public OdfWriter";
    RNGItemList::const_iterator i = bases.begin();
    while (i != bases.end()) {
        out << ", public " << (*i)->cppName;
        ++i;
    }
    out << " {\n";
    out << "public:" << "\n";
    RequiredArgsList r = makeFullRequiredArgsList(item);
    if (r.args.length()) {
        r.args = ", " + r.args;
    }
    out << "    " << item->cppName << "(OdfWriter* x" + r.args + ") :OdfWriter(x)";
    i = bases.begin();
    while (i != bases.end()) {
        RequiredArgsList r;
        if (item->requiredItems.contains(*i)) {
            r = makeFullRequiredArgsList(*i);
        }
        out << ", " << (*i)->cppName << "(*static_cast<OdfWriter*>(this)" + r.vals + ")";
        ++i;
    }
    out << " {\n";
    out << "        OdfWriter::xml->startElement(\"" << item->elementName << "\");\n";
    setRequiredAttributes(out, item);
    out << "    }\n";
    out << "    " << item->cppName << "(KoXmlWriter* x" + r.args + ") :OdfWriter(x)";
    i = bases.begin();
    while (i != bases.end()) {
        RequiredArgsList r;
        if (item->requiredItems.contains(*i)) {
            r = makeFullRequiredArgsList(*i);
        }
        out << ", " << (*i)->cppName << "(*static_cast<OdfWriter*>(this)" + r.vals + ")";
        ++i;
    }
    out << " {\n";
    out << "        OdfWriter::xml->startElement(\"" << item->elementName << "\");\n";
    setRequiredAttributes(out, item);
    out << "    }\n";
    QSet<QString> doneA;
    QSet<QString> doneE;
    foreach (RNGItemPtr i, list(item->allowedItems)) {
        QString name = makeCppName(i);
        if (i->isAttribute() && !item->requiredItems.contains(i) && !doneA.contains(name)) {
            QString type = i->singleType();
            type = (type.isNull()) ?"const QString&" :type;
            out << "    void set_" << name << "(" + type + " value) {\n";
            out << "        addAttribute(\"" + i->attributeName + "\", value);\n";
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
void
defineGroup(QTextStream& out, const RNGItemPtr& item) {
    RNGItemList bases = getBasesList(item);
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
        if (i->isAttribute() && !item->requiredItems.contains(i) && !done.contains(name)) {
            QString type = i->singleType();
            type = (type.isNull()) ?"const QString&" :type;
            out << "    void set_" << name << "(" + type + " value) {\n";
            out << "        xml.addAttribute(\"" + i->attributeName + "\", value);\n";
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
void
writeAdderDefinition(const RNGItemPtr& item, const RNGItemPtr& i, QTextStream& out) {
    QString name = makeCppName(i);
    RequiredArgsList r = makeFullRequiredArgsList(i);
    out << "inline ";
    if (!ns.isEmpty()) {
        out << ns << "::";
    }
    out << i->cppName << "\n";
    if (!ns.isEmpty()) {
        out << ns << "::";
    }
    out << item->cppName << "::add_" << name << "(";
    out << r.args << ") {\n";
    out << "    return " << ns << "::" << i->cppName << "(";
    if (item->isElement()) {
        out << "this";
    } else {
        out << "&xml";
    }
    out << r.vals << ");\n";
    out << "}\n";
}
void
writeAdderDefinitions(const RNGItemPtr& item, Files& files) {
    QSet<QString> done;
    foreach (RNGItemPtr i, list(item->allowedItems)) {
        QString name = makeCppName(i);
        if (i->isElement() && !done.contains(name)) {
            QTextStream& out = files.getFile(item->elementName, i->elementName);
            writeAdderDefinition(item, i, out);
            done.insert(name);
        }
    }
}
void
writeAdderDefinitions(const RNGItemList& items, Files& files) {
    foreach (RNGItemPtr item, items) {
        writeAdderDefinitions(item, files);
    }
}

QString
getPrefix(const QString& tag) {
    QString prefix = tag.left(tag.indexOf(":"));
    if (prefix.isNull()) {
        prefix = "";
    }
    return prefix;
}

QTextStream&
Files::getFile(const QString& tag1, const QString& tag2 = QString()) {
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
        qDebug() << file->errorString();
        ::exit(1);
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
        *out << "#include \"odfwriter" + prefix + ".h\"\n";
        *out << "#include \"odfwriter" + prefix2 + ".h\"\n";
    } else {
        *out << "namespace " << ns << " {\n";
    }
    files[prefix][prefix2] = out;
    return *out;
}
void
Files::closeNamespace() {
    typedef const QMap<QString,QTextStream*> map;
    foreach (map& m, files) {
        map::ConstIterator i = m.begin();
        while (i != m.end()) {
            if (!i.key().isNull() && !ns.isEmpty()) {
                *i.value() << "}\n";
            }
            ++i;
        }
    }
}

void
write(const RNGItemList& items, QString outdir) {
    Files files(outdir);

    QTextStream& out = files.getFile("", "");
    RNGItemList undefined = items;
    RNGItemList defined;
    addInOrder(undefined, defined);
    // declare all classes
    foreach (RNGItemPtr item, defined) {
        if (!item->isAttribute()) {
            out << "class " << item->cppName << ";\n";
        }
    }
    foreach (RNGItemPtr item, defined) {
        if (item->isElement()) {
            defineElement(files.getFile(item->elementName), item);
        } else if (item->isDefine()) {
            defineGroup(out, item);
        }
    }
    files.closeNamespace();
    writeAdderDefinitions(defined, files);
}
void
convert(const QString& rngfile, const QString& outdir) {
    QDomDocument dom = loadDOM(rngfile);
    RNGItems items;
    RNGItemPtr start = getDefines(dom.documentElement(), items);
    RNGItems collected;
    qDebug() << "define " << items.size();
    //collect(items, collected);
    collected = items;
    qDebug() << "collect " << collected.size();
    RNGItems resolved;
    resolveDefines(start, collected, resolved);
    qDebug() << "resolve " << resolved.size();
    //while (expand(resolved)) {}
    resolved.remove(start);
    qDebug() << "expand " << resolved.size();
    resolveAttributes(resolved);
    while (reduce(resolved)) {}
    qDebug() << "reduce " << resolved.size();
    RNGItemList list = resolved.toList().toVector();
    qDebug() << "filteredItems " << list.size();
    qStableSort(list.begin(), list.end(), rngItemPtrLessThan);
    makeCppNames(list);
    write(list, outdir);
    qDebug() << list.size();
}

int
main(int argc, char *argv[]) {
    QString rngfile;
    QString outdir;
    if (argc != 3) {
        rngfile = "/home/oever/src/rng2cpp/test.rng";
        outdir = "/home/oever/src/rng2cpp/";
    } else {
        rngfile = argv[1];
        outdir = argv[2];
    }
    convert(rngfile, outdir);
    test();
    return 0;
}
