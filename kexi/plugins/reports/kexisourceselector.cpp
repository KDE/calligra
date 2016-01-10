/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
* Copyright (C) 2016 Jarosław Staniek <staniek@kde.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kexisourceselector.h"
#include "kexireportview.h"
#include <kexiproject.h>
#include "KexiDataSourceComboBox.h"
#include <core/KexiMainWindowIface.h>
#include <core/KexiWindow.h>

#include <kpushbutton.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>

#include <QDomElement>
#include <QLabel>

//#define NO_EXTERNAL_SOURCES

#ifdef NO_EXTERNAL_SOURCES
#ifdef __GNUC__
#warning enable external data sources for 2.3
#else
#pragma WARNING( enable external data sources for 2.3 )
#endif
#endif

class KexiSourceSelector::Private
{
public:
    Private()
    {
    }

    ~Private()
    {
    }

    KexiDB::Connection *conn;

    QVBoxLayout *layout;
    QComboBox *sourceType;
    KexiDataSourceComboBox *internalSource;
    KLineEdit *externalSource;
    KPushButton *setData;
};

KexiSourceSelector::KexiSourceSelector(KexiProject* project, QWidget* parent)
        : QWidget(parent)
        , d(new Private)
{
    d->conn = project->dbConnection();
    d->layout = new QVBoxLayout(this);
    d->sourceType = new QComboBox(this);
    d->internalSource = new KexiDataSourceComboBox(this);
    d->internalSource->setProject(project);
    d->externalSource = new KLineEdit(this);
    d->setData = new KPushButton(i18n("Set Data"));

    connect(d->setData, SIGNAL(clicked()), this, SIGNAL(sourceDataChanged()));

    d->sourceType->addItem(i18n("Internal"), QVariant("internal"));
    d->sourceType->addItem(i18n("External"), QVariant("external"));

#ifndef NO_EXTERNAL_SOURCES

//!@TODO enable when adding external data

    d->layout->addWidget(new QLabel(i18n("Source Type:"), this));
    d->layout->addWidget(d->sourceType);
    d->layout->addSpacing(10);
#else
    d->sourceType->setVisible(false);
    d->externalSource->setVisible(false);
#endif

    d->layout->addWidget(new QLabel(i18n("Internal Source:"), this));
    d->layout->addWidget(d->internalSource);
    d->layout->addSpacing(10);

#ifndef NO_EXTERNAL_SOURCES
    d->layout->addWidget(new QLabel(i18n("External Source:"), this));
    d->layout->addWidget(d->externalSource);
#endif
    d->layout->addSpacing(20);
    d->layout->addWidget(d->setData);
    d->layout->addStretch();
    setLayout(d->layout);
}

KexiSourceSelector::~KexiSourceSelector()
{
    delete d;
}

void KexiSourceSelector::setConnectionData(const QDomElement &c)
{
    if (c.attribute("type") == "internal") {
        d->sourceType->setCurrentIndex(d->sourceType->findData("internal"));
        d->internalSource->setDataSource(
                    c.attribute("source-class"), c.attribute("source"));
    }

    if (c.attribute("type") == "external") {
        d->sourceType->setCurrentIndex(d->sourceType->findText("external"));
        d->externalSource->setText(c.attribute("source"));
    }

    emit sourceDataChanged();
}

QDomElement KexiSourceSelector::connectionData()
{
    kDebug();
    QDomDocument dd;
    QDomElement conndata = dd.createElement("connection");

#ifndef NO_EXTERNAL_SOURCES
//!@TODO Make a better gui for selecting external data source

    conndata.setAttribute("type", d->sourceType->itemData(d->sourceType->currentIndex()).toString());

    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "internal") {
        conndata.setAttribute("source", d->internalSource->currentText());
        const QString sourceClass(d->internalSource->selectedPartClass());
        if (!sourceClass.isEmpty()) {
            conndata.setAttribute("source-class", sourceClass);
        }
    } else {
        conndata.setAttribute("source", d->externalSource->text());
    }
#else
    conndata.setAttribute("type", "internal");
    conndata.setAttribute("source", d->internalSource->currentText());
#endif
    return conndata;
}

KoReportData* KexiSourceSelector::createSourceData() const
{
//!@TODO Fix when enable external data
#ifndef NO_EXTERNAL_SOURCES
    KexiReportView *view = 0;
    if (KexiMainWindowIface::global()->currentWindow()) {
        view = qobject_cast<KexiReportView*>(KexiMainWindowIface::global()->currentWindow()->selectedView());
        if (!view) {
            return 0;
        }
    }
    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "internal" && d->internalSource->isSelectionValid()) {
        return new KexiDBReportData(d->internalSource->selectedName(),
                                             d->internalSource->selectedPartClass(),
                                             d->conn, view);
    }

#ifndef KEXI_MOBILE
    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "external") {
        return new KexiMigrateReportData(d->externalSource->text());
    }
#endif

#else
    if (d->internalSource->isSelectionValid()) {
        return new KexiDBReportData(d->internalSource->selectedName(), d->conn);
    }
#endif
    return 0;
}
