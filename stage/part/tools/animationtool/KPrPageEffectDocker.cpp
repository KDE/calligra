/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2020 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageEffectDocker.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "KPrPage.h"
#include "KPrPageApplicationData.h"
#include "KPrPageTransition.h"
#include "KPrViewModePreviewPageEffect.h"
#include "commands/KPrPageEffectSetCommand.h"
#include "commands/KPrPageTransitionSetCommand.h"
#include "pageeffects/KPrPageEffectFactory.h"
#include "pageeffects/KPrPageEffectRegistry.h"
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAViewBase.h>

#include <algorithm>

bool orderFactoryByName(const KPrPageEffectFactory *factory1, const KPrPageEffectFactory *factory2)
{
    return factory1->name() < factory2->name();
}

KPrPageEffectDocker::KPrPageEffectDocker(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , m_view(nullptr)
    , m_previewMode(nullptr)
{
    setObjectName("KPrPageEffectDocker");
    QGridLayout *optionLayout = new QGridLayout();
    m_effectCombo = new QComboBox(this);
    m_effectCombo->addItem(i18n("No Effect"), QString(""));

    QList<KPrPageEffectFactory *> factories = KPrPageEffectRegistry::instance()->values();

    std::sort(factories.begin(), factories.end(), orderFactoryByName);

    foreach (KPrPageEffectFactory *factory, factories) {
        m_effectCombo->addItem(factory->name(), factory->id());
    }
    optionLayout->addWidget(m_effectCombo, 0, 0);

    connect(m_effectCombo, QOverload<int>::of(&QComboBox::activated), this, &KPrPageEffectDocker::slotEffectChanged);

    m_subTypeCombo = new QComboBox(this);

    connect(m_subTypeCombo, QOverload<int>::of(&QComboBox::activated), this, &KPrPageEffectDocker::slotSubTypeChanged);

    m_durationSpinBox = new QDoubleSpinBox(this);
    m_durationSpinBox->setRange(0.1, 60);
    m_durationSpinBox->setDecimals(1);
    m_durationSpinBox->setSuffix(i18n(" sec"));
    m_durationSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_durationSpinBox->setAlignment(Qt::AlignRight);
    m_durationSpinBox->setSingleStep(0.1);
    m_durationSpinBox->setValue(2.0);
    optionLayout->addWidget(m_durationSpinBox, 0, 1);

    connect(m_durationSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &KPrPageEffectDocker::slotDurationChanged);

    m_transitionType = new QComboBox(this);
    m_transitionType->addItem(i18n("Manual"));
    m_transitionType->addItem(i18n("Automatic"));
    // NOTE: Not used as the definition in odf spec does not make sense to me (danders)
    // m_transitionType->addItem(i18n("Semi-Automatic"));

    m_transitionTime = new QDoubleSpinBox(this);
    m_transitionTime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *label = new QLabel(this);
    label->setText(i18n("Slide Transition:"));
    QHBoxLayout *transitionLayout = new QHBoxLayout();
    transitionLayout->addWidget(label);
    transitionLayout->addWidget(m_transitionType);
    transitionLayout->addWidget(m_transitionTime);

    connect(m_transitionTime, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &KPrPageEffectDocker::slotTransitionChanged);
    connect(m_transitionType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrPageEffectDocker::slotTransitionChanged);

    m_applyToAllSlidesButton = new QPushButton(i18n("Apply To All Slides"));

    connect(m_applyToAllSlidesButton, &QAbstractButton::clicked, this, &KPrPageEffectDocker::slotApplyToAllSlides);

    // setup widget layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins({});
    layout->addLayout(optionLayout);
    layout->addWidget(m_subTypeCombo);
    layout->addLayout(transitionLayout);
    layout->addWidget(m_applyToAllSlidesButton);

    // The following widget activates a special feature in the
    // ToolOptionsDocker that makes the components of the widget align
    // to the top if there is extra space.
    QWidget *specialSpacer = new QWidget(this);
    specialSpacer->setObjectName("SpecialSpacer");
    layout->addWidget(specialSpacer);

    setLayout(layout);
}

void KPrPageEffectDocker::updateSubTypes(const KPrPageEffectFactory *factory)
{
    m_subTypeCombo->clear();
    if (factory) {
        m_subTypeCombo->setEnabled(true);

        const QMultiMap<QString, int> subTypesByName(factory->subTypesByName());
        auto it(subTypesByName.constBegin());
        for (; it != subTypesByName.constEnd(); ++it) {
            m_subTypeCombo->addItem(it.key(), it.value());
        }
    } else {
        m_subTypeCombo->setEnabled(false);
    }
}

void KPrPageEffectDocker::slotActivePageChanged()
{
    if (!m_view)
        return;

    // get the active page
    KPrPage *page = dynamic_cast<KPrPage *>(m_view->activePage());
    if (page) {
        // set the combo box according to the page's effect
        this->setEnabled(true);

        KPrPageApplicationData *pageData = KPrPage::pageData(page);
        KPrPageEffect *pageEffect = pageData->pageEffect();
        QString effectId = pageEffect ? pageEffect->id() : "";

        for (int i = 0; i < m_effectCombo->count(); ++i) {
            if (m_effectCombo->itemData(i).toString() == effectId) {
                m_effectCombo->setCurrentIndex(i);
                break;
            }
        }

        const KPrPageEffectFactory *factory = pageEffect ? KPrPageEffectRegistry::instance()->value(effectId) : 0;
        updateSubTypes(factory);

        for (int i = 0; i < m_subTypeCombo->count(); ++i) {
            if (m_subTypeCombo->itemData(i).toInt() == pageEffect->subType()) {
                m_subTypeCombo->setCurrentIndex(i);
                break;
            }
        }

        double duration = pageEffect ? static_cast<double>(pageEffect->duration()) / 1000 : 2.0;
        // block the signal so we don't get a new page effect when the page is changed
        m_durationSpinBox->blockSignals(true);
        m_durationSpinBox->setValue(duration);
        m_durationSpinBox->blockSignals(false);

        m_transitionType->blockSignals(true);
        m_transitionType->setCurrentIndex(pageData->pageTransition().type());
        m_transitionType->blockSignals(false);
        m_transitionTime->blockSignals(true);
        m_transitionTime->setValue(pageData->pageTransition().duration());
        m_transitionTime->blockSignals(false);
    } else {
        // disable the page effect docker as effects are only there on a normal page
        this->setEnabled(false);
    }
}

void KPrPageEffectDocker::slotEffectChanged(int index)
{
    // provide a preview of the chosen page effect
    KPrPageEffect *pageEffect = nullptr;
    QString effectId = m_effectCombo->itemData(index).toString();
    const KPrPageEffectFactory *factory = effectId != "" ? KPrPageEffectRegistry::instance()->value(effectId) : 0;
    updateSubTypes(factory);
    if (factory) {
        pageEffect = createPageEffect(factory, m_subTypeCombo->itemData(m_subTypeCombo->currentIndex()).toInt(), m_durationSpinBox->value());
    } else {
        // this is to avoid the assert that checks if the effect is different then the last one
        // The problem is that a undo is not yet reflected in the UI so it is possible to get the
        // same effect twice.
        // TODO
        KPrPageApplicationData *pageData = KPrPage::pageData(m_view->activePage());
        if (pageData->pageEffect() == nullptr) {
            return;
        }
    }

    m_view->kopaCanvas()->addCommand(new KPrPageEffectSetCommand(m_view->activePage(), pageEffect));

    setEffectPreview();
}

void KPrPageEffectDocker::slotSubTypeChanged(int index)
{
    QString effectId = m_effectCombo->itemData(m_effectCombo->currentIndex()).toString();
    const KPrPageEffectFactory *factory = KPrPageEffectRegistry::instance()->value(effectId);
    KPrPageEffect *pageEffect(createPageEffect(factory, m_subTypeCombo->itemData(index).toInt(), m_durationSpinBox->value()));

    m_view->kopaCanvas()->addCommand(new KPrPageEffectSetCommand(m_view->activePage(), pageEffect));

    setEffectPreview();
}

void KPrPageEffectDocker::slotDurationChanged(double duration)
{
    QString effectId = m_effectCombo->itemData(m_effectCombo->currentIndex()).toString();
    const KPrPageEffectFactory *factory = KPrPageEffectRegistry::instance()->value(effectId);

    if (factory) {
        KPrPageEffect *pageEffect(createPageEffect(factory, m_subTypeCombo->itemData(m_subTypeCombo->currentIndex()).toInt(), duration));

        m_view->kopaCanvas()->addCommand(new KPrPageEffectSetCommand(m_view->activePage(), pageEffect));
    }
}

void KPrPageEffectDocker::slotTransitionChanged()
{
    KPrPageTransition transition;
    transition.setType(static_cast<KPrPageTransition::Type>(m_transitionType->currentIndex()));
    transition.setDuration(m_transitionTime->value());
    m_view->kopaCanvas()->addCommand(new KPrPageTransitionSetCommand(m_view->activePage(), transition));
}

void KPrPageEffectDocker::slotApplyToAllSlides()
{
    m_view->kopaCanvas()->addCommand(KPrPageEffectDocker::applyToAllSlidesCommand());
}

KUndo2Command *KPrPageEffectDocker::applyToAllSlidesCommand()
{
    QList<KoPAPageBase *> m_pages = m_view->kopaDocument()->pages();
    QString m_effectId = m_effectCombo->itemData(m_effectCombo->currentIndex()).toString();
    int m_subType = m_subTypeCombo->itemData(m_subTypeCombo->currentIndex()).toInt();
    double m_duration = m_durationSpinBox->value();
    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Apply Slide Effect to all Slides"));
    const KPrPageEffectFactory *factory = m_effectId != "" ? KPrPageEffectRegistry::instance()->value(m_effectId) : 0;

    const KPrPageTransition &transition = KPrPage::pageData(m_view->activePage())->pageTransition();
    foreach (KoPAPageBase *page, m_pages) {
        if (page != m_view->activePage()) {
            if (factory) {
                KPrPageEffect *currentPageEffect(createPageEffect(factory, m_subType, m_duration));
                KPrPageEffect *oldPageEffect = KPrPage::pageData(page)->pageEffect();
                if (oldPageEffect != currentPageEffect) {
                    new KPrPageEffectSetCommand(page, currentPageEffect, cmd);
                } else {
                    delete currentPageEffect;
                }
            } else {
                KPrPageEffect *oldPageEffect = KPrPage::pageData(page)->pageEffect();
                if (oldPageEffect) {
                    KPrPageEffect *currentPageEffect = nullptr;
                    new KPrPageEffectSetCommand(page, currentPageEffect, cmd);
                }
            }
            new KPrPageTransitionSetCommand(page, transition, cmd);
        }
    }

    return cmd;
}

KPrPageEffect *KPrPageEffectDocker::createPageEffect(const KPrPageEffectFactory *factory, int subType, double duration)
{
    Q_ASSERT(factory);
    // TODO get data from input
    KPrPageEffectFactory::Properties properties(qRound(duration * 1000), subType);
    return factory->createPageEffect(properties);
}

void KPrPageEffectDocker::setView(KoPAViewBase *view)
{
    Q_ASSERT(view);
    m_view = view;
    connect(view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KPrPageEffectDocker::slotActivePageChanged);
    connect(view->proxyObject, &QObject::destroyed, this, &KPrPageEffectDocker::cleanup);

    if (m_view->activePage())
        slotActivePageChanged();
}

void KPrPageEffectDocker::setEffectPreview()
{
    QString effectId = m_effectCombo->itemData(m_effectCombo->currentIndex()).toString();
    const KPrPageEffectFactory *factory = KPrPageEffectRegistry::instance()->value(effectId);
    if (factory) {
        KPrPageEffect *pageEffect(createPageEffect(factory, m_subTypeCombo->itemData(m_subTypeCombo->currentIndex()).toInt(), m_durationSpinBox->value()));

        KPrPage *page = static_cast<KPrPage *>(m_view->activePage());
        KPrPage *oldpage = static_cast<KPrPage *>(m_view->kopaDocument()->pageByNavigation(page, KoPageApp::PagePrevious));

        if (!m_previewMode)
            m_previewMode = new KPrViewModePreviewPageEffect(m_view, m_view->kopaCanvas());

        m_previewMode->setPageEffect(pageEffect, page, oldpage); // also stops old if not already stopped
        m_view->setViewMode(m_previewMode); // play the effect (it reverts to normal  when done)
    }
}

void KPrPageEffectDocker::cleanup(QObject *object)
{
    if (object != m_view->proxyObject)
        return;

    m_view = nullptr;
}
