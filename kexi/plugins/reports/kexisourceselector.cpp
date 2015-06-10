/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
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
#include "KexiDataSourceComboBox.h"
#include <kexiproject.h>

#include <KLocalizedString>

#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDomElement>

//#define NO_EXTERNAL_SOURCES

#ifdef NO_EXTERNAL_SOURCES
//! @todo KEXI3 enable external data sources for 2.3
#endif

class KexiSourceSelector::Private
{
public:
    Private()
      : kexiDBData(0)
    {
    }

    ~Private()
    {
        delete kexiDBData;
#ifndef KEXI_MOBILE
        delete kexiMigrateData;
#endif
    }

    KDbConnection *conn;

    QVBoxLayout *layout;
    QComboBox *sourceType;
    KexiDataSourceComboBox *internalSource;
    QLineEdit *externalSource;
    QPushButton *setData;

    KexiDBReportData *kexiDBData;

#ifndef KEXI_MOBILE
    KexiMigrateReportData *kexiMigrateData;
#endif

};

KexiSourceSelector::KexiSourceSelector(KexiProject* project, QWidget* parent)
        : QWidget(parent)
        , d(new Private)
{
    d->conn = project->dbConnection();
    d->kexiDBData = 0;

#ifndef KEXI_MOBILE
    d->kexiMigrateData = 0;
#endif

    d->layout = new QVBoxLayout(this);
    d->sourceType = new QComboBox(this);
    d->internalSource = new KexiDataSourceComboBox(this);
    d->internalSource->setProject(project);
    d->externalSource = new QLineEdit(this);
    d->setData = new QPushButton(xi18n("Set Data"));

    connect(d->setData, SIGNAL(clicked()), this, SLOT(setDataClicked()));

    d->sourceType->addItem(xi18n("Internal"), QVariant("internal"));
    d->sourceType->addItem(xi18n("External"), QVariant("external"));

#ifndef NO_EXTERNAL_SOURCES

//!@TODO enable when adding external data

    d->layout->addWidget(new QLabel(xi18n("Source Type:"), this));
    d->layout->addWidget(d->sourceType);
    d->layout->addSpacing(10);
#else
    d->sourceType->setVisible(false);
    d->externalSource->setVisible(false);
#endif

    d->layout->addWidget(new QLabel(xi18n("Internal Source:"), this));
    d->layout->addWidget(d->internalSource);
    d->layout->addSpacing(10);

#ifndef NO_EXTERNAL_SOURCES
    d->layout->addWidget(new QLabel(xi18n("External Source:"), this));
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

void KexiSourceSelector::setConnectionData(QDomElement c)
{
    if (c.attribute("type") == "internal") {
        d->sourceType->setCurrentIndex(d->sourceType->findData("internal"));
        d->internalSource->setCurrentIndex(d->internalSource->findText(c.attribute("source")));
    }

    if (c.attribute("type") == "external") {
        d->sourceType->setCurrentIndex(d->sourceType->findText("external"));
        d->externalSource->setText(c.attribute("source"));
    }

    emit(setData(sourceData()));
}

QDomElement KexiSourceSelector::connectionData()
{
    qDebug();
    QDomDocument dd;
    QDomElement conndata = dd.createElement("connection");

#ifndef NO_EXTERNAL_SOURCES
//!@TODO Make a better gui for selecting external data source

    conndata.setAttribute("type", d->sourceType->itemData(d->sourceType->currentIndex()).toString());

    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "internal") {
        conndata.setAttribute("source", d->internalSource->currentText());
    } else {
        conndata.setAttribute("source", d->externalSource->text());
    }
#else
    conndata.setAttribute("type", "internal");
    conndata.setAttribute("source", d->internalSource->currentText());
#endif
    return conndata;
}

KoReportData* KexiSourceSelector::sourceData()
{
    if (d->kexiDBData) {
        delete d->kexiDBData;
        d->kexiDBData = 0;
    }

#ifndef KEXI_MOBILE
    if (d->kexiMigrateData) {
        delete d->kexiMigrateData;
        d->kexiMigrateData = 0;
    }
#endif

//!@TODO Fix when enable external data
#ifndef NO_EXTERNAL_SOURCES
    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "internal" && d->internalSource->isSelectionValid()) {
        d->kexiDBData = new KexiDBReportData(d->internalSource->selectedName(), d->internalSource->selectedPluginId(), d->conn);
        return d->kexiDBData;
    }

#ifndef KEXI_MOBILE
    if (d->sourceType->itemData(d->sourceType->currentIndex()).toString() == "external") {
        d->kexiMigrateData = new KexiMigrateReportData(d->externalSource->text());
        return d->kexiMigrateData;
    }
#endif

#else
    if (d->internalSource->isSelectionValid()) {
        d->kexiDBData = new KexiDBReportData(d->internalSource->selectedName(), d->conn);
        return d->kexiDBData;
    }
#endif
    return 0;
}

void KexiSourceSelector::setDataClicked()
{
    emit(setData(sourceData()));
}
