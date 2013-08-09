/******************************************************************************
 *  Copyright 2013 Sebastian Kügler <sebas@kde.org>                           *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or             *
 *  modify it under the terms of the GNU Lesser General Public                *
 *                                                                            *
 *  License as published by the Free Software Foundation; either              *
 *  version 2.1 of the License, or (at your option) version 3, or any         *
 *  later version accepted by the membership of KDE e.V. (or its              *
 *  successor approved by the membership of KDE e.V.), which shall            *
 *  act as a proxy defined in Section 6 of version 3 of the license.          *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Lesser General Public License for more details.                           *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public          *
 *  License along with this library.  If not, see                             *
 *  <http://www.gnu.org/licenses/>.                                           *
 *                                                                            *
 ******************************************************************************/

#include "kconfigtojson.h"

#include <qcommandlineparser.h>
#include <qcommandlineoption.h>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QTextStream>

#include <kdesktopfile.h>
#include <kconfiggroup.h>


static QTextStream cout(stdout);
static QTextStream cerr(stderr);

KConfigToJson::KConfigToJson(QCommandLineParser *parser, const QCommandLineOption &i, const QCommandLineOption &o)
  : m_parser(parser),
    input(i),
    output(o)
{
}

int KConfigToJson::runMain()
{
    if (!m_parser->isSet(input)) {
        cout << "Usage --help. In short: desktoptojson -i inputfile.desktop -o outputfile.json" << endl;
        return 1;
    }

    if (!resolveFiles()) {
        cerr << "Failed to resolve filenames" << m_inFile << m_outFile << endl;
        return 1;
    }

    return convert(m_inFile, m_outFile) ? 0 : 1;
}

bool KConfigToJson::resolveFiles()
{
    if (m_parser->isSet(input)) {
        m_inFile = m_parser->value(input);
        const QFileInfo fi(m_inFile);
        if (!fi.exists()) {
            cerr << "File not found: " + m_inFile;
            return false;
        }
        if (!fi.isAbsolute()) {
            m_inFile = QDir::currentPath() + QDir::separator() + m_inFile;
        }
    }

    if (m_parser->isSet(output)) {
        m_outFile = m_parser->value(output);
    } else if (!m_inFile.isEmpty()) {
        m_outFile = m_inFile;
        m_outFile.replace(QStringLiteral(".desktop"), QStringLiteral(".json"));
    }

    return m_inFile != m_outFile && !m_inFile.isEmpty() && !m_outFile.isEmpty();
}

bool KConfigToJson::convert(const QString &src, const QString &dest)
{
    KDesktopFile df(src);
    KConfigGroup c = df.desktopGroup();

    static const QSet<QString> boolkeys = QSet<QString>()
            << QStringLiteral("Hidden") << QStringLiteral("X-KDE-PluginInfo-EnabledByDefault");
    static const QSet<QString> stringlistkeys = QSet<QString>()
            << QStringLiteral("X-KDE-ServiceTypes") << QStringLiteral("X-KDE-PluginInfo-Depends");

    QVariantMap vm;
    foreach (const QString &k, c.keyList()) {
        if (boolkeys.contains(k)) {
            vm[k] = c.readEntry(k, false);
        } else if (stringlistkeys.contains(k)) {
            vm[k] = c.readEntry(k, QStringList());
        } else {
            vm[k] = c.readEntry(k, QString());
        }
    }

    QJsonObject jo = QJsonObject::fromVariantMap(vm);
    QJsonDocument jdoc;
    jdoc.setObject(jo);

    QFile file(dest);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cerr << "Failed to open " << dest << endl;
        return false;
    }

    file.write(jdoc.toJson());
    cout << "Generated " << dest << endl;
    return true;
}
