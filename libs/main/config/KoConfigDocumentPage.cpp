/* This file is part of the KDE project
SPDX-FileCopyrightText: 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoConfigDocumentPage.h"

#include <KoComponentData.h>
#include <KoDocument.h>
#include <KoPart.h>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>

class Q_DECL_HIDDEN KoConfigDocumentPage::Private
{
public:
    Private(KoDocument *doc)
        : doc(doc)
    {
    }

    KoDocument *doc;
    KSharedConfigPtr config;

    QSpinBox *autoSave;
    int oldAutoSave;
    QCheckBox *createBackupFile;
    bool oldBackupFile;
};

KoConfigDocumentPage::KoConfigDocumentPage(KoDocument *doc, char *name)
    : d(std::make_unique<Private>(doc))
{
    setObjectName(name);

    d->config = d->doc->documentPart()->componentData().config();

    auto layout = new QFormLayout;

    d->oldAutoSave = doc->defaultAutoSave() / 60;

    d->oldBackupFile = true;

    if (d->config->hasGroup("Interface")) {
        KConfigGroup interfaceGroup = d->config->group("Interface");
        d->oldAutoSave = interfaceGroup.readEntry("AutoSave", d->oldAutoSave);
        d->oldBackupFile = interfaceGroup.readEntry("BackupFile", d->oldBackupFile);
    }

    d->autoSave = new QSpinBox(this);
    d->autoSave->setRange(0, 60);
    d->autoSave->setSingleStep(1);
    d->autoSave->setSpecialValueText(i18n("No autosave"));
    d->autoSave->setSuffix(i18nc("unit symbol for minutes, leading space as separator", " min"));
    d->autoSave->setValue(d->oldAutoSave);
    layout->addRow(i18n("Autosave interval:"), d->autoSave);

    d->createBackupFile = new QCheckBox(this);
    d->createBackupFile->setChecked(d->oldBackupFile);
    layout->addRow(i18n("Create backup file:"), d->createBackupFile);

    auto hbox = new QHBoxLayout(this);
    hbox->addStretch();
    hbox->addLayout(layout);
    hbox->addStretch();
}

KoConfigDocumentPage::~KoConfigDocumentPage() = default;

void KoConfigDocumentPage::apply()
{
    KConfigGroup interfaceGroup = d->config->group("Interface");

    int autoSave = d->autoSave->value();

    if (autoSave != d->oldAutoSave) {
        interfaceGroup.writeEntry("AutoSave", autoSave);
        d->doc->setAutoSave(autoSave * 60);
        d->oldAutoSave = autoSave;
    }

    bool state = d->createBackupFile->isChecked();

    if (state != d->oldBackupFile) {
        interfaceGroup.writeEntry("BackupFile", state);
        d->doc->setBackupFile(state);
        d->oldBackupFile = state;
    }
}

void KoConfigDocumentPage::slotDefault()
{
    d->autoSave->setValue(d->doc->defaultAutoSave() / 60);
    d->createBackupFile->setChecked(true);
}
