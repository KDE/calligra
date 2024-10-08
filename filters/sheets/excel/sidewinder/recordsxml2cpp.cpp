/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QCoreApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

static inline QString ucFirst(const QString &s)
{
    return s[0].toUpper() + s.mid(1);
}

static inline QString lcFirst(const QString &s)
{
    return s[0].toLower() + s.mid(1);
}

struct Field {
    QString name;
    QString type;
    bool isArray;
    bool isArrayLength;
    bool isStringLength;
    bool isEnum;
    QList<Field> arrayFields;
    QString defaultValue;
    QString lengthFor;

    Field(const QString &name = QString(), const QString &type = QString())
        : name(name)
        , type(type)
        , isArray(false)
        , isArrayLength(false)
        , isStringLength(false)
        , isEnum(false)
    {
    }

    QString getterName() const
    {
        if (type == "bool" && !(name.startsWith("has") || name.startsWith("is"))) {
            return "is" + ucFirst(name);
        } else {
            return name;
        }
    }

    QString setterName() const
    {
        return "set" + ucFirst(name);
    }
};

static QString getFieldType(const QString &xmlType, unsigned bits, const QString &otherType, const QMap<QString, QString> &extraTypes)
{
    Q_UNUSED(bits);
    Q_UNUSED(otherType);
    if (xmlType == "unsigned")
        return "unsigned";
    else if (xmlType == "signed")
        return "int";
    else if (xmlType == "float" || xmlType == "fixed")
        return "double";
    else if (xmlType == "bool")
        return "bool";
    else if (xmlType == "bytestring" || xmlType == "unicodestring" || xmlType == "unicodechars")
        return "QString";
    else if (xmlType == "blob")
        return "QByteArray";
    else if (xmlType == "uuid")
        return "QUuid";
    else if (extraTypes.contains(xmlType))
        return getFieldType(extraTypes[xmlType], bits, otherType, extraTypes);
    return "ERROR";
}

bool hasParentNode(const QDomNode &n, const QString &name)
{
    QDomNode p = n.parentNode();
    if (p.isNull())
        return false;
    if (p.nodeName() == name)
        return true;
    return hasParentNode(p, name);
}

static QMap<QString, Field> getFields(QDomElement record, bool *foundStrings = nullptr)
{
    QDomNodeList types = record.elementsByTagName("type");
    QMap<QString, QString> extraTypes;
    QMap<QString, QString> extraTypesDefaults;
    for (int i = 0; i < types.size(); i++) {
        QDomElement e = types.at(i).toElement();
        extraTypes[e.attribute("name")] = e.attribute("type");
        if (e.elementsByTagName("enum").size() > 0)
            extraTypesDefaults[e.attribute("name")] = e.elementsByTagName("enum").at(0).toElement().attribute("name");
    }

    QDomNodeList fields = record.elementsByTagName("field");
    QMap<QString, Field> map;
    for (int i = 0; i < fields.size(); i++) {
        QDomElement e = fields.at(i).toElement();
        QString name = e.attribute("name");
        if (!name.startsWith("reserved")) {
            map[name] = Field(name, getFieldType(e.attribute("type"), e.attribute("size").toUInt(), map[name].type, extraTypes));
            if (foundStrings && map[name].type == "QString")
                *foundStrings = true;
            if (hasParentNode(e, "array")) {
                map[name].isArray = true;
            }
            if (e.elementsByTagName("enum").size() > 0) {
                map[name].isEnum = true;
                map[name].type = ucFirst(name);
                map[name].defaultValue = e.elementsByTagName("enum").at(0).toElement().attribute("name");
            }
            if (extraTypes.contains(e.attribute("type"))) {
                map[name].isEnum = true;
                map[name].type = e.attribute("type");
                map[name].defaultValue = extraTypesDefaults[e.attribute("type")];
            }
            if (e.hasAttribute("default"))
                map[name].defaultValue = e.attribute("default");
        }
    }
    for (int i = 0; i < fields.size(); i++) {
        QDomElement e = fields.at(i).toElement();
        if (e.hasAttribute("length")) {
            QString name = e.attribute("length");
            if (map.contains(name)) {
                map[name].isStringLength = true;
                map[name].lengthFor = e.attribute("name");
            }
        }
    }
    QDomNodeList arrays = record.elementsByTagName("array");
    for (int i = 0; i < arrays.size(); i++) {
        QDomElement e = arrays.at(i).toElement();
        QString name = e.attribute("length");
        if (map.contains(name)) {
            Field &field = map[name];
            field.isArrayLength = true;
            QDomNodeList afields = e.elementsByTagName("field");
            for (int j = 0; j < afields.size(); j++) {
                QDomElement af = afields.at(j).toElement();
                QString fname = af.attribute("name");
                if (!fname.startsWith("reserved"))
                    field.arrayFields.append(map[fname]);
            }
        }
    }
    return map;
}

void processEnumsForHeader(QDomNodeList fieldList, QTextStream &out)
{
    for (int i = 0; i < fieldList.size(); i++) {
        QDomElement f = fieldList.at(i).toElement();
        QDomNodeList enumNodes = f.elementsByTagName("enum");
        if (enumNodes.size()) {
            QString name = ucFirst(f.attribute("name"));
            out << "    enum " << name << " {\n";
            for (int j = 0; j < enumNodes.size(); j++) {
                QDomElement en = enumNodes.at(j).toElement();
                out << "        " << en.attribute("name");
                if (en.hasAttribute("value"))
                    out << " = " << en.attribute("value");
                if (j != enumNodes.size() - 1)
                    out << ",";
                out << "\n";
            }
            out << "    };\n\n"
                << "    static QString " << lcFirst(f.attribute("name")) << "ToString(" << name << " " << f.attribute("name") << ");\n\n";
        }
    }
}

void processRecordForHeader(QDomElement e, QTextStream &out)
{
    QString className = e.attribute("name") + "Record";
    QList<Field> fields = getFields(e).values();

    out << "class " << className << " : public Record\n{\npublic:\n";

    // add id field and rtti method
    out << "    static const unsigned id;\n\n"
        << "    virtual unsigned rtti() const override { return this->id; }\n\n";

    // constructor and destructor
    out << "    " << className << "(Swinder::Workbook *book);\n    virtual ~" << className << "() override;\n\n";

    // copy and assignment
    out << "    " << className << "( const " << className << "& record );\n"
        << "    " << className << "& operator=( const " << className << "& record );\n\n";

    // enums
    QDomNodeList fieldNodes = e.elementsByTagName("field");
    processEnumsForHeader(fieldNodes, out);
    QDomNodeList cfieldNodes = e.elementsByTagName("computedField");
    processEnumsForHeader(cfieldNodes, out);
    QDomNodeList typeNodes = e.elementsByTagName("type");
    processEnumsForHeader(typeNodes, out);

    // getters and setters
    foreach (const Field &f, fields) {
        out << "    " << f.type << " " << f.getterName() << "(";
        if (f.isArray)
            out << " unsigned index ";
        out << ") const;\n"
            << "    void " << f.setterName() << "(";
        if (f.isArray)
            out << " unsigned index,";
        out << " " << f.type << " " << f.name << " );\n\n";
    }

    // computed fields
    for (int i = 0; i < cfieldNodes.size(); i++) {
        QDomElement f = cfieldNodes.at(i).toElement();
        QString type = f.attribute("ctype");
        if (type.isEmpty())
            type = ucFirst(f.attribute("name"));
        out << "    " << type << " " << f.attribute("name") << "() const;\n\n";
    }

    // array lengths
    QDomNodeList arrayNodes = e.elementsByTagName("array");
    for (int i = 0; i < arrayNodes.size(); i++) {
        QDomElement f = arrayNodes.at(i).toElement();
        if (f.hasAttribute("lengthField")) {
            QString name = f.attribute("lengthField");
            out << "    unsigned " << lcFirst(name) << "() const;\n";
            out << "    void set" << ucFirst(name) << "( unsigned " << name << " );\n\n";
        }
    }

    // optional groups
    QDomNodeList optionalNodes = e.elementsByTagName("optional");
    for (int i = 0; i < optionalNodes.size(); i++) {
        QDomElement f = optionalNodes.at(i).toElement();
        out << "    bool has" << ucFirst(f.attribute("name")) << "() const;\n\n";
        out << "    void setHas" << ucFirst(f.attribute("name")) << "( bool value );\n\n";
    }

    // setData method
    out << "    virtual void setData( unsigned size, const unsigned char* data, const unsigned* continuePositions ) override;\n\n";

    // writeData method
    out << "    virtual void writeData( XlsRecordOutputStream& out ) const override;\n\n";

    // name method
    out << "    virtual const char* name() const override { return \"" << e.attribute("name") << "\"; }\n\n";

    // dump method
    out << "    virtual void dump( std::ostream& out ) const override;\n\n";

    // private stuff
    out << "private:\n    class Private;\n    Private * const d;\n};\n\n";
}

static void invalidFound(const QString &indent, QTextStream &out, const QString &currentOptional)
{
    if (currentOptional.isEmpty()) {
        out << indent << "setIsValid(false);\n";
        out << indent << "return;\n";
    } else {
        out << indent << "d->has" << ucFirst(currentOptional) << " = false;\n";
        out << indent << "goto " << lcFirst(currentOptional) << "Failed;\n";
    }
}

static void sizeCheck(const QString &indent, QTextStream &out, QDomElement firstField, unsigned offset, bool dynamicOffset, const QString &currentOptional)
{
    // find size of all fields until first unsized field or first if/array
    unsigned size = 0;
    for (QDomElement e = firstField; !e.isNull(); e = e.nextSiblingElement()) {
        if (e.tagName() != "field")
            break;
        unsigned bits = e.attribute("size", "0").toUInt();
        if (bits == 0 && e.attribute("type") == "uuid") {
            bits = 128;
            e.setAttribute("size", "128");
        }
        if (bits == 0)
            break;
        size += bits;
    }
    if (size % 8 != 0)
        qFatal("Invalid non-byte-sized chunk of fields found");
    if (offset % 8 != 0)
        qFatal("Invalid non-byte-aligned chunk of fields found");
    if (size != 0) {
        out << indent << "if (size < ";
        if (dynamicOffset)
            out << "curOffset + ";
        if (offset)
            out << (offset / 8) << " + ";
        out << (size / 8) << ") {\n";
        invalidFound(indent + "    ", out, currentOptional);
        out << indent << "}\n";
    }
}

static void processFieldElement(const QString &indent,
                                QTextStream &out,
                                QDomElement field,
                                unsigned &offset,
                                bool &dynamicOffset,
                                QMap<QString, Field> &fieldsMap,
                                const QString &setterArgs = QString(),
                                const QString &currentOptional = QString())
{
    if (field.tagName() == "fail") {
        invalidFound(indent, out, currentOptional);
    } else if (field.tagName() == "field") {
        unsigned bits = field.attribute("size").toUInt();
        QString name = field.attribute("name");
        if (!name.startsWith("reserved")) {
            // if (bits >= 8 && offset % 8 != 0)
            //     qFatal("Unaligned byte-or-larger field");
            if (bits >= 16 && bits % 8 != 0)
                qFatal("Fields of 16 bits and larger must always be an exact number of bytes");

            Field &f = fieldsMap[name];
            if (f.type == "QString") {
                if (offset % 8 != 0)
                    qFatal("Unaligned string");

                if (!dynamicOffset)
                    out << indent << "curOffset = " << (offset / 8) << ";\n";
                else if (offset)
                    out << indent << "curOffset += " << (offset / 8) << ";\n";
                out << indent << f.setterName() << "(" << setterArgs;
                dynamicOffset = true;
                offset = 0;

                if (field.attribute("type") == "unicodestring")
                    out << "readUnicodeString(";
                else if (field.attribute("type") == "unicodechars")
                    out << "readUnicodeCharArray(";
                else
                    out << "readByteString(";

                out << "data + curOffset, ";
                if (field.hasAttribute("length") || field.attribute("type") != "unicodechars")
                    out << field.attribute("length");
                else
                    out << "-1";
                out << ", size - curOffset"
                    << ", &stringLengthError, &stringSize));\n";
                out << indent << "if (stringLengthError) {\n";
                invalidFound(indent + "    ", out, currentOptional);
                out << indent << "}\n";
                out << indent << "curOffset += stringSize;\n";
                sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
            } else if (f.type == "QByteArray") {
                if (offset % 8 != 0)
                    qFatal("Unaligned string");

                if (field.hasAttribute("length")) {
                    out << indent << "if (";
                    if (dynamicOffset)
                        out << "curOffset + ";
                    if (offset)
                        out << (offset / 8) << " + ";
                    out << "(" << field.attribute("length") << ") > size) {\n";
                    invalidFound(indent + "    ", out, currentOptional);
                    out << indent << "}\n";
                }
                out << indent << f.setterName() << "(" << setterArgs;
                out << "QByteArray(reinterpret_cast<const char*>(";
                out << "data";
                if (dynamicOffset)
                    out << " + curOffset";
                if (offset)
                    out << " + " << (offset / 8);
                out << "), ";
                if (field.hasAttribute("length"))
                    out << field.attribute("length");
                else
                    out << (bits / 8);
                out << "));\n";
                if (field.hasAttribute("length")) {
                    if (!dynamicOffset)
                        out << indent << "curOffset = " << (offset / 8);
                    else
                        out << indent << "curOffset += " << (offset / 8);
                    out << " + " << field.attribute("length") << ";\n";
                    dynamicOffset = true;
                    offset = 0;
                    sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
                }
            } else if (f.type == "QUuid") {
                out << indent << f.setterName() << "(" << setterArgs;
                out << "readUuid(data";
                if (dynamicOffset)
                    out << " + curOffset";
                if (offset)
                    out << " + " << (offset / 8);
                out << "));\n";
            } else if (bits % 8 == 0) {
                if (f.isStringLength)
                    out << indent << "unsigned " << name << " = ";
                else
                    out << indent << f.setterName() << "(" << setterArgs;

                if (f.isEnum)
                    out << "static_cast<" << f.type << ">(";

                if (field.attribute("type") == "unsigned" || field.attribute("type") == "bool")
                    out << "readU" << bits;
                else if (field.attribute("type") == "signed")
                    out << "readS" << bits;
                else if (field.attribute("type") == "float")
                    out << "readFloat" << bits;
                else if (field.attribute("type") == "fixed")
                    out << "readFixed" << bits;
                else
                    qFatal("Unsupported type %s", qPrintable(field.attribute("type")));

                out << "(data";
                if (dynamicOffset)
                    out << " + curOffset";
                if (offset)
                    out << " + " << (offset / 8);
                out << ")";
                if (field.attribute("type") == "bool")
                    out << " != 0";
                if (f.isEnum)
                    out << ")";
                if (!f.isStringLength)
                    out << ")";
                out << ";\n";
            } else {
                if (f.isStringLength)
                    out << indent << "unsigned " << name << " = ";
                else
                    out << indent << f.setterName() << "(" << setterArgs;
                if (f.isEnum)
                    out << "static_cast<" << f.type << ">(";
                unsigned firstByte = offset / 8;
                unsigned lastByte = (offset + bits - 1) / 8;
                unsigned bitOffset = offset % 8;
                unsigned mask = (1 << bits) - 1;
                if (firstByte == lastByte) {
                    out << "((readU8(data";
                } else {
                    out << "((readU16(data";
                }
                if (dynamicOffset)
                    out << " + curOffset";
                if (firstByte)
                    out << " + " << firstByte;
                out << ")";
                if (bitOffset)
                    out << " >> " << bitOffset;
                out << ") & 0x" << Qt::hex << mask << Qt::dec << ")";
                if (field.attribute("type") == "bool")
                    out << " != 0";
                if (f.isEnum)
                    out << ")";
                if (!f.isStringLength)
                    out << ")";
                out << ";\n";
            }
            // evaluate constraints
            for (QDomElement child = field.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
                if (child.tagName() == "constraint") {
                    out << indent << "if (!(" << f.getterName() << "(" << setterArgs << ") " << child.attribute("expr") << ")) {\n";
                    invalidFound(indent + "    ", out, currentOptional);
                    out << indent << "}\n";
                }
            }
        }
        offset += bits;
    } else if (field.tagName() == "if") {
        if (offset % 8 != 0)
            qFatal("Ifs should always be byte-aligned");
        if (!dynamicOffset)
            out << indent << "curOffset = " << (offset / 8) << ";\n";
        else if (offset)
            out << indent << "curOffset += " << (offset / 8) << ";\n";

        out << indent << "if (" << field.attribute("predicate") << ") {\n";

        offset = 0;
        dynamicOffset = true;
        sizeCheck(indent + "    ", out, field.firstChildElement(), offset, dynamicOffset, currentOptional);

        for (QDomElement child = field.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
            processFieldElement(indent + "    ", out, child, offset, dynamicOffset, fieldsMap, setterArgs, currentOptional);
        }

        if (offset % 8 != 0)
            qFatal("Ifs should contain an integer number of bytes");

        if (offset)
            out << indent << "    curOffset += " << (offset / 8) << ";\n";
        out << indent << "}\n";
        offset = 0;
        sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
    } else if (field.tagName() == "optional") {
        if (offset % 8 != 0)
            qFatal("Optionals should always be byte-aligned");
        if (!dynamicOffset)
            out << indent << "curOffset = " << (offset / 8) << ";\n";
        else if (offset)
            out << indent << "curOffset += " << (offset / 8) << ";\n";
        offset = 0;
        dynamicOffset = true;

        QString name = ucFirst(field.attribute("name"));
        QString label = field.attribute("name") + "Failed";
        out << indent << "d->has" << name << " = true;\n";
        out << indent << "unsigned savedOffset" << name << " = curOffset;\n";
        out << indent << "{\n";

        sizeCheck(indent + "    ", out, field.firstChildElement(), offset, dynamicOffset, name);
        for (QDomElement child = field.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
            processFieldElement(indent + "    ", out, child, offset, dynamicOffset, fieldsMap, setterArgs, name);
        }

        if (offset % 8 != 0)
            qFatal("Optionals should contain an integer number of bytes");

        if (offset)
            out << indent << "    curOffset += " << (offset / 8) << ";\n";

        out << indent << "}\n";
        out << label << ":\n";
        out << indent << "if (!d->has" << name << ") {\n";
        out << indent << "    curOffset = savedOffset" << name << ";\n";
        out << indent << "}\n";
        offset = 0;
        sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
    } else if (field.tagName() == "choose") {
        if (offset % 8 != 0)
            qFatal("Choose tags should always be byte-aligned");
        if (!dynamicOffset)
            out << indent << "curOffset = " << (offset / 8) << ";\n";
        else if (offset)
            out << indent << "curOffset += " << (offset / 8) << ";\n";
        offset = 0;
        dynamicOffset = true;

        bool isFirst = true;

        for (QDomElement childField = field.firstChildElement(); !childField.isNull(); childField = childField.nextSiblingElement()) {
            if (childField.tagName() != "option") {
                qFatal("only option tags are allowed inside a choose tag");
            }

            if (isFirst)
                out << indent;
            else
                out << " ";
            if (!isFirst)
                out << "else ";
            isFirst = false;
            if (childField.hasAttribute("predicate"))
                out << "if (" << childField.attribute("predicate") << ") ";
            out << "{\n";

            sizeCheck(indent + "    ", out, childField.firstChildElement(), offset, dynamicOffset, currentOptional);

            for (QDomElement child = childField.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
                processFieldElement(indent + "    ", out, child, offset, dynamicOffset, fieldsMap, setterArgs, currentOptional);
            }

            if (offset % 8 != 0)
                qFatal("options should contain an integer number of bytes");

            if (offset)
                out << indent << "    curOffset += " << (offset / 8) << ";\n";
            out << indent << "}";
            offset = 0;
        }
        out << "\n";
        sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
    } else if (field.tagName() == "array") {
        if (offset % 8 != 0)
            qFatal("Arrays should always be byte-aligned");

        if (!dynamicOffset)
            out << indent << "curOffset = " << (offset / 8) << ";\n";
        else if (offset)
            out << indent << "curOffset += " << (offset / 8) << ";\n";

        QString length = field.attribute("length");
        if (fieldsMap.contains(length))
            length = fieldsMap[length].getterName() + "()";
        else {
            QDomNodeList fields = field.elementsByTagName("field");
            for (int i = 0; i < fields.size(); i++) {
                QDomElement e = fields.at(i).toElement();
                QString name = e.attribute("name");
                if (!name.startsWith("reserved")) {
                    out << indent << "d->" << name << ".resize(" << length << ");\n";
                }
            }
        }

        out << indent << "for (unsigned i = 0, endi = " << length << "; i < endi; ++i) {\n";
        offset = 0;
        dynamicOffset = true;
        sizeCheck(indent + "    ", out, field.firstChildElement(), offset, dynamicOffset, currentOptional);

        for (QDomElement child = field.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
            processFieldElement(indent + "    ", out, child, offset, dynamicOffset, fieldsMap, setterArgs + "i, ", currentOptional);
        }

        if (offset % 8 != 0)
            qFatal("Arrays should contain an integer number of bytes");

        if (offset)
            out << indent << "    curOffset += " << (offset / 8) << ";\n";
        out << indent << "}\n";
        offset = 0;
        sizeCheck(indent, out, field.nextSiblingElement(), offset, dynamicOffset, currentOptional);
    }
}

static void processFieldElementForWrite(const QString &indent,
                                        QTextStream &out,
                                        QDomElement field,
                                        const QMap<QString, Field> &fieldsMap,
                                        const QString &getterArgs = QString())
{
    if (field.tagName() == "field") {
        QString name = field.attribute("name");
        unsigned bits = field.attribute("size").toUInt();
        if (!name.startsWith("reserved")) {
            const Field &f = fieldsMap[name];
            if (field.attribute("type") == "bytestring") {
                out << indent << "out.writeByteString(";
            } else if (field.attribute("type") == "unicodestring") {
                out << indent << "out.writeUnicodeStringWithFlags(";
            } else if (field.attribute("type") == "unicodechars") {
                out << indent << "out.writeUnicodeString(";
            } else if (f.type == "QByteArray") {
                out << indent << "out.writeBlob(";
            } else if (f.type == "QUuid") {
                out << indent << "// TODO ";
            } else if (field.attribute("type") == "bool" || field.attribute("type") == "unsigned" || f.isEnum) {
                out << indent << "out.writeUnsigned(" << bits << ", ";
            } else if (field.attribute("type") == "signed") {
                out << indent << "out.writeSigned(" << bits << ", ";
            } else if (field.attribute("type") == "float") {
                out << indent << "out.writeFloat(" << bits << ", ";
            } else if (field.attribute("type") == "fixed") {
                out << indent << "// TODO ";
            } else {
                out << f.type;
            }
            if (f.isStringLength) {
                // TODO: figure out length from string
                const Field &f2 = fieldsMap[f.lengthFor];
                out << f2.getterName() << "(" << getterArgs << ").length()";
            } else {
                out << f.getterName() << "(" << getterArgs << ")";
            }
            out << ");\n";
        } else {
            out << indent << "out.writeUnsigned(" << bits << ", ";
            if (field.hasAttribute("default")) {
                out << field.attribute("default");
            } else {
                out << "0";
            }
            out << ");\n";
        }
    } else if (field.tagName() == "if") {
        out << indent << "if (" << field.attribute("predicate") << ") {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForWrite(indent + "    ", out, e, fieldsMap, getterArgs);
        out << indent << "}\n";
    } else if (field.tagName() == "choose") {
        bool isFirst = true;
        for (QDomElement childField = field.firstChildElement(); !childField.isNull(); childField = childField.nextSiblingElement()) {
            if (isFirst)
                out << indent;
            else
                out << " ";
            if (!isFirst)
                out << "else ";
            isFirst = false;
            if (childField.hasAttribute("predicate"))
                out << "if (" << childField.attribute("predicate") << ") ";
            out << "{\n";

            for (QDomElement child = childField.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
                processFieldElementForWrite(indent + "    ", out, child, fieldsMap, getterArgs);
            }

            out << indent << "}";
        }
        out << "\n";
    } else if (field.tagName() == "array") {
        QString length = field.attribute("length");
        if (fieldsMap.contains(length))
            length = fieldsMap[length].getterName() + "()";
        else if (field.firstChildElement().hasAttribute("name"))
            length = "d->" + field.firstChildElement().attribute("name") + ".size()";

        out << indent << "for (unsigned i = 0, endi = " << length << "; i < endi; ++i) {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForWrite(indent + "    ", out, e, fieldsMap, "i");
        out << indent << "}\n";
    } else if (field.tagName() == "optional") {
        out << indent << "if (has" << ucFirst(field.attribute("name")) << "()) {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForWrite(indent + "    ", out, e, fieldsMap, getterArgs);
        out << indent << "}\n";
    }
}

static void processFieldElementForDump(const QString &indent,
                                       QTextStream &out,
                                       QDomElement field,
                                       const QMap<QString, Field> &fieldsMap,
                                       const QString &getterArgs = QString())
{
    if (field.tagName() == "field") {
        QString name = field.attribute("name");
        if (!name.startsWith("reserved")) {
            const Field &f = fieldsMap[name];
            if (!f.isStringLength) {
                out << indent << "out << \"";
                if (getterArgs.length() == 0) {
                    out << QString(19 - name.length(), ' ');
                    out << ucFirst(name) << " : \" << ";
                } else {
                    out << QString(15 - name.length(), ' ');
                    out << ucFirst(name);
                    out << " \" << std::setw(3) << " << getterArgs << " <<\" : \" << ";
                }
                if (f.isEnum)
                    out << lcFirst(f.type) << "ToString(" << f.getterName() << "(" << getterArgs << "))";
                else
                    out << f.getterName() << "(" << getterArgs << ")";
                out << " << std::endl;\n";
            }
        }
    } else if (field.tagName() == "if") {
        out << indent << "if (" << field.attribute("predicate") << ") {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForDump(indent + "    ", out, e, fieldsMap, getterArgs);
        out << indent << "}\n";
    } else if (field.tagName() == "choose") {
        bool isFirst = true;
        for (QDomElement childField = field.firstChildElement(); !childField.isNull(); childField = childField.nextSiblingElement()) {
            if (isFirst)
                out << indent;
            else
                out << " ";
            if (!isFirst)
                out << "else ";
            isFirst = false;
            if (childField.hasAttribute("predicate"))
                out << "if (" << childField.attribute("predicate") << ") ";
            out << "{\n";

            for (QDomElement child = childField.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
                processFieldElementForDump(indent + "    ", out, child, fieldsMap, getterArgs);
            }

            out << indent << "}";
        }
        out << "\n";
    } else if (field.tagName() == "array") {
        QString length = field.attribute("length");
        if (fieldsMap.contains(length))
            length = fieldsMap[length].getterName() + "()";
        else if (field.firstChildElement().hasAttribute("name"))
            length = "d->" + field.firstChildElement().attribute("name") + ".size()";

        out << indent << "for (unsigned i = 0, endi = " << length << "; i < endi; ++i) {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForDump(indent + "    ", out, e, fieldsMap, "i");
        out << indent << "}\n";
    } else if (field.tagName() == "optional") {
        out << indent << "if (has" << ucFirst(field.attribute("name")) << "()) {\n";
        for (QDomElement e = field.firstChildElement(); !e.isNull(); e = e.nextSiblingElement())
            processFieldElementForDump(indent + "    ", out, e, fieldsMap, getterArgs);
        out << indent << "}\n";
    }
}

void processEnumsForImplementation(QDomNodeList fieldList, const QString &className, QTextStream &out)
{
    for (int i = 0; i < fieldList.size(); i++) {
        QDomElement f = fieldList.at(i).toElement();
        QDomNodeList enumNodes = f.elementsByTagName("enum");
        if (enumNodes.size()) {
            QString name = ucFirst(f.attribute("name"));
            out << "QString " << className << "::" << lcFirst(f.attribute("name")) << "ToString(" << name << " " << f.attribute("name") << ")\n{\n"
                << "    switch (" << f.attribute("name") << ") {\n";
            for (int j = 0; j < enumNodes.size(); j++) {
                QDomElement en = enumNodes.at(j).toElement();
                out << "        case " << en.attribute("name") << ": return QString(\"" << en.attribute("name") << "\");\n";
            }
            out << "        default: return QString(\"Unknown: %1\").arg(" << f.attribute("name") << ");\n    }\n}\n\n";
        }
    }
}

void processRecordForImplementation(QDomElement e, QTextStream &out)
{
    QString className = e.attribute("name") + "Record";
    bool containsStrings = false;
    QMap<QString, Field> fieldsMap = getFields(e, &containsStrings);
    QList<Field> fields = fieldsMap.values();

    out << "// ========== " << className << " ==========\n\n";

    // id field
    out << "const unsigned " << className << "::id = " << e.attribute("id") << ";\n\n";

    // private class
    out << "class " << className << "::Private\n{\n"
        << "public:\n";
    foreach (const Field &f, fields) {
        if (f.isArray)
            out << "    std::vector<" << f.type << "> " << f.name << ";\n";
        else if (!f.isStringLength)
            out << "    " << f.type << " " << f.name << ";\n";
    }
    // optional groups
    QDomNodeList optionalNodes = e.elementsByTagName("optional");
    for (int i = 0; i < optionalNodes.size(); i++) {
        QDomElement f = optionalNodes.at(i).toElement();
        out << "    bool has" << ucFirst(f.attribute("name")) << ";\n";
    }
    out << "};\n\n";

    // constructor
    out << className << "::" << className << "(Swinder::Workbook *book)\n";
    out << "    : Record(book), d(new Private)\n{\n";
    foreach (const Field &f, fields) {
        if (f.isArray || f.isStringLength)
            continue;
        QString val;
        if (!f.defaultValue.isEmpty()) {
            val = f.defaultValue;
        } else if (f.type == "unsigned" || f.type == "int") {
            val = "0";
        } else if (f.type == "bool") {
            val = "false";
        }
        if (!val.isEmpty())
            out << "    " << f.setterName() << "(" << val << ");\n";
    }
    for (int i = 0; i < optionalNodes.size(); i++) {
        QDomElement f = optionalNodes.at(i).toElement();
        out << "    d->has" << ucFirst(f.attribute("name")) << " = false;\n";
    }
    out << "}\n\n";

    // destructor
    out << className << "::~" << className << "()\n{\n    delete d;\n}\n\n";

    // copy constructor
    out << className << "::" << className << "( const " << className << "& record )\n"
        << "    : Record(record), d(new Private)\n{\n"
        << "    *this = record;\n}\n\n";

    // assignment operator
    out << className << "& " << className << "::operator=( const " << className << "& record )\n{\n"
        << "    *d = *record.d;\n"
        << "    return *this;\n}\n\n";

    // enums
    QDomNodeList fieldNodes = e.elementsByTagName("field");
    processEnumsForImplementation(fieldNodes, className, out);
    QDomNodeList cfieldNodes = e.elementsByTagName("computedField");
    processEnumsForImplementation(cfieldNodes, className, out);
    QDomNodeList typeNodes = e.elementsByTagName("type");
    processEnumsForImplementation(typeNodes, className, out);

    // getters and setters
    foreach (const Field &f, fields) {
        if (f.isStringLength)
            continue;

        if (f.isEnum)
            out << className << "::";
        out << f.type << " " << className << "::" << f.getterName() << "(";
        if (f.isArray)
            out << " unsigned index ";
        out << ") const\n{\n    return d->" << f.name;
        if (f.isArray)
            out << "[index]";
        out << ";\n}\n\n";

        out << "void " << className << "::" << f.setterName() << "(";
        if (f.isArray)
            out << " unsigned index, ";
        out << f.type << " " << f.name << " )\n{\n    d->" << f.name;
        if (f.isArray)
            out << "[index]";
        out << " = " << f.name << ";\n";

        if (f.isArrayLength) {
            foreach (const Field &af, f.arrayFields) {
                out << "    d->" << af.name << ".resize(" << f.name << ");\n";
            }
        }

        out << "}\n\n";
    }

    // computed fields
    for (int i = 0; i < cfieldNodes.size(); i++) {
        QDomElement f = cfieldNodes.at(i).toElement();
        QString type = f.attribute("ctype");
        if (type.isEmpty())
            type = className + "::" + ucFirst(f.attribute("name"));
        out << type << " " << className << "::" << f.attribute("name") << "() const\n{\n"
            << "    return " << f.attribute("value") << ";\n}\n\n";
    }

    // array lengths
    QDomNodeList arrayNodes = e.elementsByTagName("array");
    for (int i = 0; i < arrayNodes.size(); i++) {
        QDomElement f = arrayNodes.at(i).toElement();
        if (f.hasAttribute("lengthField")) {
            QString name = f.attribute("lengthField");
            QDomNodeList afields = f.elementsByTagName("field");
            QDomElement af1 = afields.at(0).toElement();

            out << "unsigned " << className << "::" << lcFirst(name) << "() const\n{\n";
            out << "    return d->" << af1.attribute("name") << ".size();\n}\n\n";

            out << "void " << className << "::set" << ucFirst(name) << "( unsigned " << name << " )\n{\n";
            for (int j = 0; j < afields.size(); j++) {
                QDomElement af = afields.at(j).toElement();
                if (af.attribute("name").startsWith("reserved"))
                    continue;
                out << "    d->" << af.attribute("name") << ".resize(" << name << ");\n";
            }
            out << "}\n\n";
        }
    }

    // optional groups
    for (int i = 0; i < optionalNodes.size(); i++) {
        QDomElement f = optionalNodes.at(i).toElement();
        out << "bool " << className << "::has" << ucFirst(f.attribute("name")) << "() const\n{\n";
        out << "    return d->has" << ucFirst(f.attribute("name")) << ";\n}\n\n";
        out << "void " << className << "::setHas" << ucFirst(f.attribute("name")) << "( bool value )\n{\n";
        out << "    d->has" << ucFirst(f.attribute("name")) << " = value;\n}\n\n";
    }

    // setData method
    bool hasFields = !fieldNodes.isEmpty();
    if (hasFields) {
        out << "void " << className << "::setData( unsigned size, const unsigned char* data, const unsigned int* )\n{\n";
    } else {
        out << "void " << className << "::setData( unsigned size, const unsigned char*, const unsigned int* )\n{\n";
    }
    out << "    setRecordSize(size);\n\n";
    if (e.elementsByTagName("if").size() > 0 || e.elementsByTagName("array").size() > 0 || containsStrings || e.elementsByTagName("optional").size() > 0)
        out << "    unsigned curOffset;\n";
    if (containsStrings) {
        out << "    bool stringLengthError = false;\n"
            << "    unsigned stringSize;\n";
    }
    unsigned offset = 0;
    bool dynamicOffset = false;
    sizeCheck("    ", out, e.firstChildElement(), offset, dynamicOffset, "");
    for (QDomElement child = e.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        processFieldElement("    ", out, child, offset, dynamicOffset, fieldsMap);
    out << "}\n\n";

    // writeData method
    if (hasFields) {
        out << "void " << className << "::writeData( XlsRecordOutputStream& out ) const\n{\n";
    } else {
        out << "void " << className << "::writeData( XlsRecordOutputStream& ) const\n{\n";
    }
    for (QDomElement child = e.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        processFieldElementForWrite("    ", out, child, fieldsMap);
    out << "}\n\n";

    // dump method
    out << "void " << className << "::dump( std::ostream& out ) const\n{\n"
        << "    out << \"" << e.attribute("name") << "\" << std::endl;\n";
    for (QDomElement child = e.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        processFieldElementForDump("    ", out, child, fieldsMap);
    out << "}\n\n";

    // creator function
    out << "static Record* create" << className << "(Swinder::Workbook *book)\n{\n    return new " << className << "(book);\n}\n\n";
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QDomDocument doc("records");
    QFile f;
    if (argc <= 1) {
        f.setFileName("records.xml");
    } else {
        f.setFileName(argv[1]);
    }
    if (!f.open(QIODevice::ReadOnly))
        qFatal("Error opening file");
    QString errorMsg;
    int errorLine;
    int errorCol;
    if (!doc.setContent(&f, &errorMsg, &errorLine, &errorCol)) {
        f.close();
        errorMsg = "Error parsing file: " + errorMsg + "\n";
        errorMsg += QLatin1String("In line ") + QString::number(errorLine) + QLatin1String(", column ") + QString::number(errorCol);
        qFatal("%s", errorMsg.toLatin1().constData());
    }
    f.close();

    QFile hFile("records.h");
    hFile.open(QIODevice::WriteOnly);
    QTextStream hOut(&hFile);

    QFile cppFile("records.cpp");
    cppFile.open(QIODevice::WriteOnly);
    QTextStream cppOut(&cppFile);

    hOut << "// This file was automatically generated from records.xml\n"
         << "#ifndef SWINDER_RECORDS_H\n"
         << "#define SWINDER_RECORDS_H\n\n"
         << "#include \"utils.h\"\n\n"
         << "namespace Swinder {\n\n"
         << "void registerRecordClasses();\n\n";

    cppOut << "// This file was automatically generated from records.xml\n"
           << "#include \"records.h\"\n"
           << "#include <vector>\n"
           << "#include <iomanip>\n"
           << "#include \"XlsRecordOutputStream.h\"\n\n"
           << "namespace Swinder {\n\n";

    QDomNodeList records = doc.elementsByTagName("record");
    for (int i = 0; i < records.size(); i++) {
        QDomElement e = records.at(i).toElement();
        processRecordForHeader(e, hOut);
        processRecordForImplementation(e, cppOut);
    }

    cppOut << "void registerRecordClasses()" << Qt::endl << "{" << Qt::endl;
    for (int i = 0; i < records.size(); i++) {
        QDomElement e = records.at(i).toElement();
        cppOut << "    RecordRegistry::registerRecordClass(" << e.attribute("name") << "Record::id, create" << e.attribute("name") << "Record);\n";
    }
    cppOut << "}\n\n";

    hOut << "} // namespace Swinder\n\n";
    hOut << "#endif // SWINDER_RECORDS_H\n";

    cppOut << "} // namespace Swinder\n";
}
