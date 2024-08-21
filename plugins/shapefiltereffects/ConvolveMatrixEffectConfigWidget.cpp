/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ConvolveMatrixEffectConfigWidget.h"
#include "ConvolveMatrixEffect.h"
#include "KoFilterEffect.h"
#include "MatrixDataModel.h"

#include <KComboBox>
#include <KLocalizedString>
#include <QDialog>

#include <KConfigGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableView>

ConvolveMatrixEffectConfigWidget::ConvolveMatrixEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    m_edgeMode = new KComboBox(this);
    m_edgeMode->addItem(i18n("Duplicate"));
    m_edgeMode->addItem(i18n("Wrap"));
    m_edgeMode->addItem(i18n("None"));
    g->addWidget(new QLabel(i18n("Edge mode:"), this), 0, 0);
    g->addWidget(m_edgeMode, 0, 1, 1, 3);

    m_orderX = new QSpinBox(this);
    m_orderX->setRange(1, 30);
    m_orderY = new QSpinBox(this);
    m_orderY->setRange(1, 30);
    g->addWidget(new QLabel(i18n("Kernel size:"), this), 1, 0);
    g->addWidget(m_orderX, 1, 1);
    g->addWidget(new QLabel("X", this), 1, 2, Qt::AlignHCenter);
    g->addWidget(m_orderY, 1, 3);

    m_targetX = new QSpinBox(this);
    m_targetX->setRange(0, 30);
    m_targetY = new QSpinBox(this);
    m_targetY->setRange(0, 30);
    g->addWidget(new QLabel(i18n("Target point:"), this), 2, 0);
    g->addWidget(m_targetX, 2, 1);
    g->addWidget(new QLabel("X", this), 2, 2, Qt::AlignHCenter);
    g->addWidget(m_targetY, 2, 3);

    m_divisor = new QDoubleSpinBox(this);
    m_bias = new QDoubleSpinBox(this);
    g->addWidget(new QLabel(i18n("Divisor:"), this), 3, 0);
    g->addWidget(m_divisor, 3, 1);
    g->addWidget(new QLabel(i18n("Bias:"), this), 3, 2);
    g->addWidget(m_bias, 3, 3);

    m_preserveAlpha = new QCheckBox(i18n("Preserve alpha"), this);
    g->addWidget(m_preserveAlpha, 4, 1, 1, 3);

    QPushButton *kernelButton = new QPushButton(i18n("Edit kernel"), this);
    g->addWidget(kernelButton, 5, 0, 1, 4);

    setLayout(g);

    connect(m_edgeMode, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &ConvolveMatrixEffectConfigWidget::edgeModeChanged);
    connect(m_orderX, &QSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::orderChanged);
    connect(m_orderY, &QSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::orderChanged);
    connect(m_targetX, &QSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::targetChanged);
    connect(m_targetY, &QSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::targetChanged);
    connect(m_divisor, &QDoubleSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::divisorChanged);
    connect(m_bias, &QDoubleSpinBox::valueChanged, this, &ConvolveMatrixEffectConfigWidget::biasChanged);
    connect(kernelButton, &QAbstractButton::clicked, this, &ConvolveMatrixEffectConfigWidget::editKernel);
    connect(m_preserveAlpha, &QAbstractButton::toggled, this, &ConvolveMatrixEffectConfigWidget::preserveAlphaChanged);

    m_matrixModel = new MatrixDataModel(this);
}

bool ConvolveMatrixEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<ConvolveMatrixEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_edgeMode->blockSignals(true);
    m_edgeMode->setCurrentIndex(m_effect->edgeMode());
    m_edgeMode->blockSignals(false);
    m_orderX->blockSignals(true);
    m_orderX->setValue(m_effect->order().x());
    m_orderX->blockSignals(false);
    m_orderY->blockSignals(true);
    m_orderY->setValue(m_effect->order().y());
    m_orderY->blockSignals(false);
    m_targetX->blockSignals(true);
    m_targetX->setMaximum(m_orderX->value());
    m_targetX->setValue(m_effect->target().x() + 1);
    m_targetX->blockSignals(false);
    m_targetY->blockSignals(true);
    m_targetY->setMaximum(m_orderY->value());
    m_targetY->setValue(m_effect->target().y() + 1);
    m_targetY->blockSignals(false);
    m_divisor->blockSignals(true);
    m_divisor->setValue(m_effect->divisor());
    m_divisor->blockSignals(false);
    m_bias->blockSignals(true);
    m_bias->setValue(m_effect->bias());
    m_bias->blockSignals(false);
    m_preserveAlpha->blockSignals(true);
    m_preserveAlpha->setChecked(m_effect->isPreserveAlphaEnabled());
    m_preserveAlpha->blockSignals(false);

    return true;
}

void ConvolveMatrixEffectConfigWidget::edgeModeChanged(int id)
{
    if (!m_effect)
        return;

    switch (id) {
    case ConvolveMatrixEffect::Duplicate:
        m_effect->setEdgeMode(ConvolveMatrixEffect::Duplicate);
        break;
    case ConvolveMatrixEffect::Wrap:
        m_effect->setEdgeMode(ConvolveMatrixEffect::Wrap);
        break;
    case ConvolveMatrixEffect::None:
        m_effect->setEdgeMode(ConvolveMatrixEffect::None);
        break;
    }
    Q_EMIT filterChanged();
}

void ConvolveMatrixEffectConfigWidget::orderChanged(int)
{
    if (!m_effect)
        return;

    QPoint newOrder(m_orderX->value(), m_orderY->value());
    QPoint oldOrder = m_effect->order();
    if (newOrder != oldOrder) {
        m_effect->setOrder(newOrder);
        const int newSize = newOrder.x() * newOrder.y();
        const int oldSize = oldOrder.x() * oldOrder.y();
        QVector<qreal> kernel = m_effect->kernel();
        if (newSize > oldSize) {
            kernel.insert(kernel.end(), newSize - oldSize, 0);
        } else {
            kernel.resize(newSize);
        }
        m_effect->setKernel(kernel);
        Q_EMIT filterChanged();
    }

    m_targetX->setMaximum(newOrder.x());
    m_targetY->setMaximum(newOrder.y());
}

void ConvolveMatrixEffectConfigWidget::targetChanged(int)
{
    if (!m_effect)
        return;

    QPoint newTarget(m_targetX->value() - 1, m_targetY->value() - 1);
    QPoint oldTarget = m_effect->target();
    if (newTarget != oldTarget) {
        m_effect->setTarget(newTarget);
        Q_EMIT filterChanged();
    }
}

void ConvolveMatrixEffectConfigWidget::divisorChanged(double divisor)
{
    if (!m_effect)
        return;

    if (divisor != m_effect->divisor()) {
        m_effect->setDivisor(divisor);
        Q_EMIT filterChanged();
    }
}

void ConvolveMatrixEffectConfigWidget::biasChanged(double bias)
{
    if (!m_effect)
        return;

    if (bias != m_effect->bias()) {
        m_effect->setBias(bias);
        Q_EMIT filterChanged();
    }
}

void ConvolveMatrixEffectConfigWidget::preserveAlphaChanged(bool checked)
{
    if (!m_effect)
        return;

    m_effect->enablePreserveAlpha(checked);
    Q_EMIT filterChanged();
}

void ConvolveMatrixEffectConfigWidget::editKernel()
{
    if (!m_effect)
        return;

    QVector<qreal> oldKernel = m_effect->kernel();
    QPoint kernelSize = m_effect->order();
    m_matrixModel->setMatrix(oldKernel, kernelSize.y(), kernelSize.x());
    connect(m_matrixModel, &QAbstractItemModel::dataChanged, this, &ConvolveMatrixEffectConfigWidget::kernelChanged);

    QPointer<QDialog> dlg = new QDialog(this);
    QTableView *table = new QTableView(dlg);
    table->setModel(m_matrixModel);
    table->horizontalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    dlg->setLayout(mainLayout);
    mainLayout->addWidget(table);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::Key_Return);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, dlg.data(), &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dlg.data(), &QDialog::reject);
    if (dlg->exec() == QDialog::Accepted) {
        m_effect->setKernel(m_matrixModel->matrix());
        Q_EMIT filterChanged();
    } else {
        m_effect->setKernel(oldKernel);
    }
    delete dlg;

    disconnect(m_matrixModel, &QAbstractItemModel::dataChanged, this, &ConvolveMatrixEffectConfigWidget::kernelChanged);
}

void ConvolveMatrixEffectConfigWidget::kernelChanged()
{
    if (!m_effect)
        return;

    m_effect->setKernel(m_matrixModel->matrix());
    Q_EMIT filterChanged();
}
