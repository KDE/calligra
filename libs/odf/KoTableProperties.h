/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOTABLEPROPERTIES_H
#define KOTABLEPROPERTIES_H

#include "KoTblStyle.h"

class KoTableTemplate;

class TableProperties
{
public:
    TableStyle* tableStyle();
    void setTableStyle(TableStyle* style);

    TableTemplate* tableTemplate();
    void setTableTemplate(KoTableTemplate* tableTemplate);
    TableTemplateFlags templateFlags();
    void setTemplateFlags(TableTemplateFlags templateFlags);

    bool printable() const;
    void setPrintable(bool printable);

    void setPrintRange(CellRange cellRange);
    CellRange printRange() const;

    void setName(QString name);
    QString name() const;

    void setProtected(bool isProtected);
    bool isPprotected() const;
    void setPlainPassword(QString password, QString uri = "http://www.w3.org/2000/09/xmldsig#sha1");

private:
    void saveOdf(KoXmlWriter* writer, KoGenStyles* styles);

    TableStyle* m_style;
    TableTemplate* m_template;
    TableTemplateFlags m_templateFlags;

    bool m_printable;
    CellRange m_printRange;
    QString m_name;

    bool m_protected;
    QString m_password;
};

#endif
