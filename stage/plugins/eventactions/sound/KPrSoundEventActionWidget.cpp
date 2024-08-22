// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrSoundEventActionWidget.h"

#include <QComboBox>
#include <QFileDialog>
#include <QUrl>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "KPrSoundEventAction.h"
#include <KPrEventActionData.h>
#include <KPrSoundCollection.h>
#include <KPrSoundData.h>
#include <KoEventActionAddCommand.h>
#include <KoEventActionRemoveCommand.h>

KPrSoundEventActionWidget::KPrSoundEventActionWidget(QWidget *parent)
    : KPrEventActionWidget(parent)
    , m_shape(nullptr)
    , m_eventAction(nullptr)
    , m_soundCollection(nullptr)
    , m_soundCombo(new QComboBox())
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(m_soundCombo);

    connect(m_soundCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrSoundEventActionWidget::soundComboChanged);

    setEnabled(false);
    updateCombo("");
}

KPrSoundEventActionWidget::~KPrSoundEventActionWidget() = default;

void KPrSoundEventActionWidget::setData(KPrEventActionData *eventActionData)
{
    m_shape = eventActionData->shape();
    m_eventAction = eventActionData->eventAction();
    // TODO get the sound out ot the action
    QString title;
    KPrSoundEventAction *eventAction = dynamic_cast<KPrSoundEventAction *>(m_eventAction);
    if (eventAction) {
        title = eventAction->soundData()->title();
    }
    m_soundCollection = eventActionData->soundCollection();
    setEnabled(m_shape && m_soundCollection);
    updateCombo(title);
}

void KPrSoundEventActionWidget::soundComboChanged()
{
    if (!m_shape) {
        return;
    }

    KPrSoundData *soundData = nullptr;
    if (m_soundCombo->currentIndex() > 1) { // a previous sound was chosen
        // copy it rather then just point to it - so the refcount is updated
        soundData = new KPrSoundData(*m_soundCollection->findSound(m_soundCombo->currentText()));
    } else if (m_soundCombo->currentIndex() == 1) { // "Import..." was chosen
        QUrl url = QFileDialog::getOpenFileUrl();
        if (!url.isEmpty()) {
            soundData = new KPrSoundData(m_soundCollection, url.toLocalFile());
            // TODO shouldn't that come from the sound collection
            // what if the user opens a already opened sound again?
            QFile *file = new QFile(url.toLocalFile());
            file->open(QIODevice::ReadOnly);
            soundData->loadFromFile(file); // also closes the file and deletes the class
        }
    }

    // TODO better name e.g. on new or remove sound
    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Change sound action"));
    if (m_eventAction) {
        new KoEventActionRemoveCommand(m_shape, m_eventAction, cmd);
        m_eventAction = nullptr;
    }

    if (soundData) {
        KPrSoundEventAction *eventAction = new KPrSoundEventAction();
        eventAction->setSoundData(soundData);
        m_eventAction = eventAction;
        new KoEventActionAddCommand(m_shape, eventAction, cmd);
    }

    Q_EMIT addCommand(cmd);

    updateCombo(soundData ? soundData->title() : "");
}

void KPrSoundEventActionWidget::updateCombo(const QString &title)
{
    m_soundCombo->blockSignals(true);

    m_soundCombo->clear();
    m_soundCombo->addItem(i18n("No sound"));
    m_soundCombo->addItem(i18n("Import..."));
    if (m_soundCollection) {
        m_soundCombo->addItems(m_soundCollection->titles());
    }
    if (title.isEmpty()) {
        m_soundCombo->setCurrentIndex(0);
    } else {
        m_soundCombo->setCurrentIndex(m_soundCombo->findText(title));
    }

    m_soundCombo->blockSignals(false);
}
