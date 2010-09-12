/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "AutoFormatDialog.h"

#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

#include <kcombobox.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <KoXmlReader.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Localization.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"

#include "commands/AutoFormatCommand.h"

using namespace KSpread;

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
        : KDialog(parent)
        , d(new Private())
{
    setCaption(i18n("Sheet Style"));
    setObjectName("AutoAutoFormatDialog");
    setModal(true);
    setButtons(Ok | Cancel);

    d->selection = selection;
    QWidget *page = mainWidget();

    QVBoxLayout *vbox = new QVBoxLayout(page);
    vbox->setMargin(KDialog::marginHint());
    vbox->setSpacing(KDialog::spacingHint());

    QLabel *toplabel = new QLabel(i18n("Select the sheet style to apply:"), page);
    d->combo = new KComboBox(page);
    d->label = new QLabel(page);

    vbox->addWidget(toplabel);
    vbox->addWidget(d->combo);
    vbox->addWidget(d->label, 1);

    const KStandardDirs *const dirs = KGlobal::activeComponent().dirs();
    const QStringList lst = dirs->findAllResources("sheet-styles", "*.ksts", KStandardDirs::Recursive);

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

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(d->combo, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
}

AutoFormatDialog::~AutoFormatDialog()
{
    delete d;
}

void AutoFormatDialog::slotActivated(int index)
{
    enableButtonOk(true);

    const KStandardDirs *const dirs = KGlobal::activeComponent().dirs();
    QString image = dirs->findResource("sheet-styles", d->entries[index].image);
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
    const KStandardDirs *const dirs = KGlobal::activeComponent().dirs();
    QString xml = dirs->findResource("sheet-styles", d->entries[d->combo->currentIndex()].xml);
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
            if (!style.loadXML(tmpElement))
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

#include "AutoFormatDialog.moc"
