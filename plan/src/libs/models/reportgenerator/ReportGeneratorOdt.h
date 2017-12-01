/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef REPORTGENERATORODT_H
#define REPORTGENERATORODT_H

#include "ReportGenerator.h"

#include <QModelIndexList>

#include <KoXmlReaderForward.h>

#include <qdom.h>
#include <QSortFilterProxyModel>

class QIODevice;
class QString;

class KoXmlWriter;
class KoStore;
class KoOdfWriteStore;
class KoOdfReadStore;

namespace KPlato
{

class Project;
class ScheduleManager;
class ItemModelBase;


class KPLATOMODELS_EXPORT ReportGeneratorOdt : public ReportGenerator
{
public:
    explicit ReportGeneratorOdt();
    ~ReportGeneratorOdt();

    bool open();
    void close();

    bool createReport();

protected:
    bool createReportOdt();
    bool handleTextP(KoXmlWriter &writer, const KoXmlElement &textp);
    void handleDrawFrame(KoXmlWriter &writer, const KoXmlElement &frame);
    void treatText(KoXmlWriter &writer, const KoXmlText &text);
    void treatTable(KoXmlWriter &writer, const KoXmlElement &tableElement);
    bool treatTableHeaderRows(KoXmlWriter &writer, const KoXmlElement &headerRowElement);
    bool treatTableRow(KoXmlWriter &writer, const KoXmlElement &rowElement);
    void treatUserFieldGet(KoXmlWriter &writer, const KoXmlElement &e);
    void writeElementAttributes(KoXmlWriter &writer, const KoXmlElement &element, const QStringList &exclude = QStringList());
    void writeChildElements(KoXmlWriter &writer, const KoXmlElement &parent);
    bool copyFile(KoStore &from, KoStore &to, const QString &file);
    KoStore *copyStore(KoOdfReadStore &reader, const QString &outfile);

    void treatEmbededObjects(KoOdfReadStore &reader, KoStore &outStore);
    void treatChart(KoOdfReadStore &reader, KoStore &outStore, const QString &name, const QString &file);
    void treatGantt(KoOdfReadStore &reader, KoStore &outStore, const QString &name, const QString &file);
    void writeChartElements(KoXmlWriter &writer, const KoXmlElement &parent);

    void listChildNodes(const QDomNode &parent);

    void handleUserFieldDecls(KoXmlWriter &writer, const KoXmlElement &decls);
    QAbstractItemModel *dataModel(const QString &name) const;

public:
    class UserField {
    public:
        enum Variant { None, Header, Rows, Variable, Translation };
        UserField() : seqNr(-1), serieNr(0) {}
        int variant() const;
        void begin();
        bool next();
        QString headerData(const QString &columnName) const;
        QString data(const QString &columnName) const;

        int rowCount();
        int columnCount() const {return columns.count();}
        void setModel(QAbstractItemModel *model, int role);
        int column(const QString &columnName) const;

        QString name; // Name of this field (eg: table1)
        QString type; // Type of field (eg: table)
        QString dataName; // Name associated with the data (eg: tasks)
        QStringList properties; // Info on how to handle the data (eg: values=bcws,bcwp,acwp)
        QStringList headerNames; // Lowercase list of all header names
        QStringList columns; // Lowercase list of headernames that shall be used
        QSortFilterProxyModel model;
        int seqNr; // A sequence number used to tabulate column names (eg: seqNr=2: table1.name2)
        int serieNr; // A chart needs to know which data serie it works with
        QString hasLabels; // A chart needs to know if data shall contain legends/labels

        QModelIndex currentIndex;
    };

    UserField *findUserField(const KoXmlElement &decl) const;

private:
    KoStore *m_templateStore;
    QStringList m_manifestfiles;

    QList<QString> m_sortedfields;
    QList<QString> m_activefields;

    QMap<QString, UserField*> m_userfields;

    QMap<QString, QString> m_embededcharts;
    QMap<QString, QString> m_embededgantts;

    QStringList m_keys;
    QStringList m_variables;

    QMap<QString, QAbstractItemModel*> m_datamodels;
    QMap<QString, int> m_headerrole;
    QList<ItemModelBase*> m_basemodels;

    QList<QByteArray> m_tags; // cache tags for survival
};

QDebug operator<<(QDebug dbg, ReportGeneratorOdt::UserField *f);
QDebug operator<<(QDebug dbg, ReportGeneratorOdt::UserField &f);

} //namespace KPlato

#endif // REPORTGENERATOR_H
