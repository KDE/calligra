/* This file is part of the KOffice project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "htmlodfexport.h"

#include <qdom.h>
#include <qfontinfo.h>
#include <QFile>
#include <QString>
#include <QBuffer>
//Added by qt3to4:
#include <QByteArray>
#include <QFileInfo>
#include <QDir>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include <document.h>
#include <exportdialog.h>
#include <QtXmlPatterns>
#include <convert.h>


typedef KGenericFactory<HTMLOdfExport> HTMLOdfExportFactory;
K_EXPORT_COMPONENT_FACTORY(libhtmlodf_export, HTMLOdfExportFactory("kofficefilters"))


HTMLOdfExport::HTMLOdfExport(QObject* parent, const QStringList&) :
KoFilter(parent), m_dialog(new ExportDialog())
{
}

HTMLOdfExport::~HTMLOdfExport()
{
}



KoFilter::ConversionStatus HTMLOdfExport::convert(const QByteArray &from, const QByteArray &to)
{
    // check for proper conversion
    if (to != "text/html"
            || from != "application/vnd.oasis.opendocument.text")
        return KoFilter::NotImplemented;


    kDebug(30513) << "######################## HTMLOdfExport::convert ########################";

    QString inputFile = m_chain->inputFile();
    QString outputFile = m_chain->outputFile();

    if (m_dialog->exec() == QDialog::Rejected)
        return KoFilter::UserCancelled;


    // Create output files

    QFile out(outputFile);
        if (!out.open(QIODevice::WriteOnly)) {
            kError(30501) << "Unable to open output file!" << endl;
            out.close();
            return KoFilter::FileNotFound;
        }

        Conversion c1;
        c1.convert(&out);

        QFileInfo base(outputFile);
        QString filenamewithoutext = outputFile.left(outputFile.lastIndexOf('.'));
        QString directory=base.absolutePath();
        QDir dir(outputFile);
        dir.mkdir(filenamewithoutext);
        QString stylesheet=filenamewithoutext+"/style.css";
        QFile css(stylesheet);
        if (!css.open(QIODevice::WriteOnly)){
            kError(30501) << "Unable to open stylesheet!" << endl;
            css.close();
            return KoFilter::FileNotFound;
        }

       out.write("<html><h1> The Filter is under construction</h1></html>");
       out.close();
       css.close();



    struct Finalizer {
    public:
        Finalizer(KoStore *store) : m_store(store), m_genStyles(0), m_document(0), m_contentWriter(0), m_bodyWriter(0) { }
        ~Finalizer() {
            delete m_store; delete m_genStyles; delete m_document; delete m_contentWriter; delete m_bodyWriter;
        }

        KoStore *m_store;
        KoGenStyles *m_genStyles;
        Document *m_document;
        KoXmlWriter* m_contentWriter;
        KoXmlWriter* m_bodyWriter;
    };


    kDebug(30513) << "######################## HTMLOdfExport::convert done ####################";
    return KoFilter::OK;
}

#include <htmlodfexport.moc>
