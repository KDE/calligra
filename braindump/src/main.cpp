/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include <QApplication>
#include <QCommandLineParser>
#include <QLoggingCategory>

#include <kiconloader.h>

#include "AboutData.h"
#include "MainWindow.h"
#include "KoGlobal.h"
#include "RootSection.h"
#include "SectionsIO.h"

#include <Calligra2Migration.h>

int main(int argc, char **argv)
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" braindump
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules("calligra.*.debug=false\n"
                                     "calligra.*.warning=true");

    QApplication app(argc, argv);

    // Migrate data from kde4 to kf5 locations
    Calligra2Migration m("braindump");
    m.setConfigFiles(QStringList() << QStringLiteral("braindumprc"));
    m.setUiFiles(QStringList() << QStringLiteral("braindumpview.rc"));
    m.migrate();

    KAboutData about = newBrainDumpAboutData();
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    about.setupCommandLine(&parser);

    parser.process(app);

    about.processCommandLine(&parser);

    KIconLoader::global()->addAppDir("calligra");
    KoGlobal::initialize();

    RootSection* doc = new RootSection;

    MainWindow* window = new MainWindow(doc);
    window->setVisible(true);

    app.exec();

    // Ensure the root section is saved
    doc->sectionsIO()->save();

    delete doc;
    app.exit(0);
}
