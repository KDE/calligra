/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "AutoFormatDialog.h"

#include <QLabel>
#include <QVBoxLayout>

#include <kcombobox.h>
#include <kconfig.h>
#include <kmessagebox.h>

#include <KoResourcePaths.h>

#include <KoXmlReader.h>

#include "core/Style.h"
#include "core/ksp/SheetsKsp.h"
#include "../Selection.h"

#include "../commands/AutoFormatCommand.h"

using namespace Calligra::Sheets;

struct Entry {
    QString xml;
    QString image;
    QString config;
    QString name;
};

class AutoFormatDialog::Private
{
public:
    Selection* selection;
    KComboBox* combo;
    QLabel* label;
    QList<Entry> entries;
    QList<Style> styles;

public:
    bool parseXML(const KoXmlDocument& doc);
};

AutoFormatDialog::AutoFormatDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
        , d(new Private())
{
    setCaption(i18n("Sheet Style"));
    setObjectName(QLatin1String("AutoAutoFormatDialog"));
    setModal(true);
    setButtons(Ok | Cancel);

    d->selection = selection;
    QWidget *page = mainWidget();

    QVBoxLayout *vbox = new QVBoxLayout(page);

    QLabel *toplabel = new QLabel(i18n("Select the sheet style to apply:"), page);
    d->combo = new KComboBox(page);
    d->label = new QLabel(page);

    vbox->addWidget(toplabel);
    vbox->addWidget(d->combo);
    vbox->addWidget(d->label, 1);

    const QStringList lst = KoResourcePaths::findAllResources("sheet-styles", "*.ksts", KoResourcePaths::Recursive);

    int index = 0;
    QStringList::ConstIterator it = lst.begin();
    for (; it != lst.end(); ++it) {
        KConfig config(*it, KConfig::SimpleConfig);
        const KConfigGroup sheetStyleGroup = config.group("Sheet-Style");

        Entry e;
        e.config = *it;
        e.xml = sheetStyleGroup.readEntry("XML");
        e.image = sheetStyleGroup.readEntry("Image");
        e.name = sheetStyleGroup.readEntry("Name");

        d->entries.append(e);

        d->combo->insertItem(index++, e.name);
    }

    slotActivated(0);

    connect(this, &KoDialog::okClicked, this, &AutoFormatDialog::slotOk);
    connect(d->combo, QOverload<int>::of(&QComboBox::activated), this, &AutoFormatDialog::slotActivated);
}

AutoFormatDialog::~AutoFormatDialog()
{
    delete d;
}

void AutoFormatDialog::slotActivated(int index)
{
    enableButtonOk(true);

    QString image = KoResourcePaths::findResource("sheet-styles", d->entries[index].image);
    if (image.isEmpty()) {
        KMessageBox::error(this, i18n("Could not find image %1.", d->entries[index].image));
        enableButtonOk(false);
        return;
    }

    QPixmap pixmap(image);
    if (pixmap.isNull()) {
        KMessageBox::error(this, i18n("Could not load image %1.", image));
        enableButtonOk(false);
        return;
    }
    d->label->setPixmap(pixmap);
}

void AutoFormatDialog::slotOk()
{
    QString xml = KoResourcePaths::findResource("sheet-styles", d->entries[d->combo->currentIndex()].xml);
    if (xml.isEmpty()) {
        KMessageBox::error(this, i18n("Could not find sheet-style XML file '%1'.", d->entries[d->combo->currentIndex()].xml));
        return;
    }

    QFile file(xml);
    file.open(QIODevice::ReadOnly);
    KoXmlDocument doc;
    doc.setContent(&file);
    file.close();

    if (!d->parseXML(doc)) {
        KMessageBox::error(this, i18n("Parsing error in sheet-style XML file %1.", d->entries[d->combo->currentIndex()].xml));
        return;
    }

    //
    // Set colors, borders etc.
    //
    AutoFormatCommand* command = new AutoFormatCommand();
    command->setSheet(d->selection->activeSheet());
    command->setStyles(d->styles);
    command->add(*d->selection);
    if (!command->execute(d->selection->canvas()))
        delete command;

    accept();
}

bool AutoFormatDialog::Private::parseXML(const KoXmlDocument& doc)
{
    styles.clear();
    for (int i = 0; i < 16; ++i)
        styles.append(Style());

    KoXmlElement e = doc.documentElement().firstChild().toElement();
    for (; !e.isNull(); e = e.nextSibling().toElement()) {
        if (e.tagName() == "cell") {
            Style style;
            KoXmlElement tmpElement(e.namedItem("format").toElement());
            if (!Ksp::loadStyle (&style, tmpElement))
                return false;

            int row = e.attribute("row").toInt();
            int column = e.attribute("column").toInt();
            int i = (row - 1) * 4 + (column - 1);
            if (i < 0 || i >= 16)
                return false;

            styles[i] = style;
        }
    }
    return true;
}

