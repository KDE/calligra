/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOTABLE_H
#define KOTABLE_H

class KoColumn;
class KoRow;
class KoCell;
#include "KoTblStyle.h"

class KoXmlWriter;
class KoGenStyles;

#include "koodf2_export.h"

#include <QMap>
#include <QPair>
#include <QVector>

/**
 * \class KoTable
 * \brief This represents a table in an ODF element.
 * Note that, at least for now, the class is meant to be used
 * only to write tables and as such lacks much of the functionality
 * needed to read written tables.
 *
 * All the pointers returned by this class are guaranteed to be not-null.
 * Do note, however, that there's no way to clear a cell, row or column.
 *
 * The KoTable owns all the pointer objects returned by
 * its methods.
 **/

class KOODF2_EXPORT KoTable
{
public:
    KOSTYLE_DECLARE_SHARED_POINTER(KoTable)

    KoTable();
    ~KoTable();

    KoRow *rowAt(int row);
    int rowCount() const;

    KoColumn *columnAt(int column);
    int columnCount() const;

    KoCell *cellAt(int row, int column);

    void saveOdf(KoXmlWriter &writer, KoGenStyles &styles);

    KoTblStyle::Ptr tableStyle();
    void setTableStyle(KoTblStyle::Ptr style);

    //     KoTableTemplate* tableTemplate();
    //     void setTableTemplate(KoTableTemplate* tableTemplate);
    //     TableTemplateFlags templateFlags();
    //     void setTemplateFlags(TableTemplateFlags templateFlags);

    bool printable() const;
    void setPrintable(bool printable);

    //     void setPrintRange(CellRange cellRange);
    //     CellRange printRange() const;

    void setName(const QString &name);
    QString name() const;

    void setProtected(bool isProtected);
    bool isPprotected() const;

    void setProtectionKey(const QString &password, const QString &protectionAlgorithmUri = QLatin1String("http://www.w3.org/2000/09/xmldsig#sha1"));
    QString protectionKey() const;
    QString protectionalgorithm() const;

private:
    KoTable(const KoTable &) = delete;
    KoTable &operator=(const KoTable &) = delete;

    QVector<KoColumn *> m_columns;
    QVector<KoRow *> m_rows;

    QMap<QPair<int, int>, KoCell *> m_cells;

    int m_rowCount;
    int m_columnCount;

    KoTblStyle::Ptr m_style;
    //     KoTableTemplate* m_template;
    //     TableTemplateFlags m_templateFlags;

    bool m_printable;
    //     CellRange m_printRange;
    QString m_name;

    bool m_protected;
    QString m_protectionKey;
    QString m_protectionAlgorithm;
};

#endif
