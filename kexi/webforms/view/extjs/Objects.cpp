/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QString>

#include <kdebug.h>

#include <core/kexipartinfo.h>
#include <core/kexipartitem.h>

#include <pion/net/HTTPResponseWriter.hpp>

#include "../../model/Database.h"
#include "../../model/DataProvider.h"

#include "Objects.h"

using namespace pion::net;

namespace KexiWebForms {
namespace View {

static bool caseInsensitiveLessThan(const QString &s1, const QString &s2) {
    return s1.toLower() < s2.toLower();
}

static void addList(KexiDB::ObjectType objectType, pion::net::HTTPResponseWriterPtr writer) {
    KexiWebForms::Model::Database db;
    QHash<QString, QString> oNames(db.getNames(objectType));
    QStringList captions(oNames.uniqueKeys());
    qSort(captions.begin(), captions.end(), caseInsensitiveLessThan);

    QString HTML;
    foreach(const QString& caption, captions) {
        QStringList names(oNames.values(caption));
        foreach(const QString& name, names) {
            if (!(name == "kexi__users")) //! @note temporary work around
                HTML.append(QString::fromLatin1("{\n"
                                                "\t\ttext: '%1',\n"
                                                "\t\tid: '%2',\n"
                                                "\t\tleaf: true\n"
                                                "\t},").arg(caption).arg(name));
        }
    }

    writer->write(HTML.toUtf8().constData());
}

void Objects::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
    writer->write("[{\n"
                  "\ttext: 'Tables',\n"
                  "\texpanded: true,\n"
                  "\tchildren: [");
    addList(KexiDB::TableObjectType, writer);
    writer->write("]\n"
                  "},");
    writer->write("{\n"
                  "\ttext: 'Queries',\n"
                  "\texpanded: true,\n"
                  "\tchildren: [");
    addList(KexiDB::QueryObjectType, writer);
    writer->write("]\n"
                  "}]\n");
}

}
}
