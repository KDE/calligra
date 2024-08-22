/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013 Jos van den Oever <jos@vandenoever.info>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef ODFWRITER_H
#define ODFWRITER_H

#include <KoXmlWriter.h>
#include <QDate>
#include <QStringList>
#include <QUrl>

class Duration
{
private:
    QTime m_time;

public:
    explicit Duration(const QTime &t)
        : m_time(t)
    {
    }
    QString toString() const
    {
        return m_time.toString("'PT'hh'H'mm'M'ss'S'");
    }
};

class OdfWriter
{
private:
    void operator=(const OdfWriter &) = delete;

protected:
    OdfWriter(KoXmlWriter *xml_, const char *tag, bool indent)
        : child(nullptr)
        , parent(nullptr)
        , xml(xml_)
    {
        xml->startElement(tag, indent);
    }
    OdfWriter(OdfWriter *p, const char *tag, bool indent)
        : child(nullptr)
        , parent(p)
        , xml(parent->xml)
    {
        if (parent->child) {
            parent->child->end();
        }
        parent->child = this;
        xml->startElement(tag, indent);
    }
    ~OdfWriter()
    {
        end();
    }
    void endChild()
    {
        if (child) {
            child->parent = nullptr;
            child->end();
            child = nullptr;
        }
    }
    OdfWriter(const OdfWriter &&o)
        : child(o.child)
        , parent(o.parent)
        , xml(o.xml)
    {
        // disable o and make the parent refer to this new copy
        o.xml = nullptr;
        if (parent && parent->child == &o) {
            parent->child = this;
        }
    }

public:
    void end()
    {
        if (xml) {
            endChild();
            xml->endElement();
            if (parent) {
                parent->child = nullptr;
            }
            xml = nullptr;
        }
    }
    void addTextNode(const QString &str)
    {
        endChild();
        xml->addTextNode(str);
    }
    void addAttribute(const char *name, const char *value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value);
    }
    void addAttribute(const char *name, const QString &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value);
    }
    void addAttribute(const char *name, quint64 value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, QString::number(value));
    }
    void addAttribute(const char *name, const QUrl &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.toString());
    }
    void addAttribute(const char *name, const QDate &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.toString(Qt::ISODate));
    }
    void addAttribute(const char *name, const QTime &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.toString(Qt::ISODate));
    }
    void addAttribute(const char *name, const QDateTime &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.toString(Qt::ISODate));
    }
    void addAttribute(const char *name, const QStringList &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.join(QChar(' ')));
    }
    void addAttribute(const char *name, const Duration &value)
    {
        Q_ASSERT(!child);
        xml->addAttribute(name, value.toString());
    }
    void addProcessingInstruction(const char *cstr)
    {
        endChild();
        xml->addProcessingInstruction(cstr);
    }
    template<class T>
    void addCompleteElement(T cstr)
    {
        endChild();
        xml->addCompleteElement(cstr);
    }

private:
    OdfWriter *child;
    OdfWriter *parent;

protected:
    mutable KoXmlWriter *xml;
};
#endif
