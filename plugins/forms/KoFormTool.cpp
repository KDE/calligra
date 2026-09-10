/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormTool.h"
#include "KoFormShape.h"

#include <KLocalizedString>
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <kundo2command.h>

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QVBoxLayout>
#include <functional>

using namespace Qt::StringLiterals;

namespace
{
class ChangeFormPropertiesCommand : public KUndo2Command
{
public:
    ChangeFormPropertiesCommand(KoFormShape *shape, const KoOdfForm::Control &properties, std::function<void()> refresh)
        : m_shape(shape)
        , m_refresh(std::move(refresh))
    {
        setText(kundo2_i18n("Change form properties"));
        static_cast<KoOdfForm::Control &>(m_before) = *shape->formControl();
        static_cast<KoOdfForm::Control &>(m_after) = properties;
    }
    void redo() override
    {
        m_shape->setControlProperties(m_after);
        m_refresh();
    }
    void undo() override
    {
        m_shape->setControlProperties(m_before);
        m_refresh();
    }

private:
    KoFormShape *m_shape;
    std::function<void()> m_refresh;
    KoOdfForm::GenericControl m_before;
    KoOdfForm::GenericControl m_after;
};
}

KoFormTool::KoFormTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
}

void KoFormTool::activate(ToolActivation, const QSet<KoShape *> &shapes)
{
    if (canvas() && canvas()->shapeManager()) {
        connect(canvas()->shapeManager(), &KoShapeManager::selectionChanged, this, &KoFormTool::shapeSelectionChanged, Qt::UniqueConnection);
    }
    m_shape = nullptr;
    for (KoShape *shape : shapes) {
        if ((m_shape = dynamic_cast<KoFormShape *>(shape))) {
            break;
        }
    }
    if (!m_shape) {
        Q_EMIT done();
        return;
    }
    useCursor(Qt::ArrowCursor);
    updateProperties();
}

QWidget *KoFormTool::createOptionWidget()
{
    auto *widget = new QWidget();
    m_options = widget;
    widget->setWindowTitle(i18n("Form Properties"));
    auto *layout = new QVBoxLayout(widget);
    auto *form = new QFormLayout();
    layout->addLayout(form);
    m_type = new QLabel(widget);
    form->addRow(i18nc("@label:form property", "&Type:"), m_type);
    m_name = new QLineEdit(widget);
    form->addRow(i18n("&Name:"), m_name);
    m_title = new QLineEdit(widget);
    form->addRow(i18n("&Help text:"), m_title);
    const auto addBoolean = [form, widget](const QString &label) {
        auto *box = new QComboBox(widget);
        box->addItem(i18n("No"), false);
        box->addItem(i18n("Yes"), true);
        form->addRow(label, box);
        return box;
    };
    m_enabled = addBoolean(i18n("&Enabled:"));
    m_readOnly = addBoolean(i18n("&Read-only:"));
    m_printable = addBoolean(i18n("&Printable:"));
    m_tabStop = addBoolean(i18n("&Tab stop:"));
    m_tabIndex = new QSpinBox(widget);
    m_tabIndex->setRange(0, 32767);
    form->addRow(i18n("Tab &order:"), m_tabIndex);
    layout->addStretch();
    connect(m_name, &QLineEdit::editingFinished, this, &KoFormTool::commitProperties);
    connect(m_title, &QLineEdit::editingFinished, this, &KoFormTool::commitProperties);
    connect(m_enabled, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        commitProperties();
    });
    connect(m_readOnly, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        commitProperties();
    });
    connect(m_printable, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        commitProperties();
    });
    connect(m_tabStop, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        commitProperties();
    });
    connect(m_tabIndex, &QSpinBox::editingFinished, this, &KoFormTool::commitProperties);
    updateProperties();
    return widget;
}

void KoFormTool::updateProperties()
{
    if (!m_options) {
        return;
    }
    const auto *control = m_shape ? m_shape->formControl() : nullptr;
    m_options->setEnabled(control != nullptr);
    const QSignalBlocker nameBlocker(m_name);
    const QSignalBlocker typeBlocker(m_type);
    const QSignalBlocker titleBlocker(m_title);
    const QSignalBlocker enabledBlocker(m_enabled);
    const QSignalBlocker readOnlyBlocker(m_readOnly);
    const QSignalBlocker printableBlocker(m_printable);
    const QSignalBlocker tabStopBlocker(m_tabStop);
    const QSignalBlocker tabIndexBlocker(m_tabIndex);
    m_name->setText(control ? control->name() : QString());
    m_type->setText(m_shape ? m_shape->controlKind() : QString());
    m_title->setText(control ? control->title() : QString());
    m_enabled->setCurrentIndex(control && !control->disabled());
    m_readOnly->setCurrentIndex(control && control->readOnly());
    m_printable->setCurrentIndex(control && control->printable());
    m_tabStop->setCurrentIndex(control && control->tabStop());
    m_tabIndex->setValue(control ? control->tabIndex() : 0);
}

void KoFormTool::commitProperties()
{
    const auto *before = m_shape ? m_shape->formControl() : nullptr;
    if (!before) {
        return;
    }
    if (before->name() == m_name->text() && before->title() == m_title->text() && before->disabled() == !m_enabled->currentData().toBool()
        && before->readOnly() == m_readOnly->currentData().toBool() && before->printable() == m_printable->currentData().toBool()
        && before->tabStop() == m_tabStop->currentData().toBool() && before->tabIndex() == m_tabIndex->value())
        return;
    KoOdfForm::GenericControl properties;
    static_cast<KoOdfForm::Control &>(properties) = *before;
    properties.setName(m_name->text());
    properties.setTitle(m_title->text());
    properties.setDisabled(!m_enabled->currentData().toBool());
    properties.setReadOnly(m_readOnly->currentData().toBool());
    properties.setPrintable(m_printable->currentData().toBool());
    properties.setTabStop(m_tabStop->currentData().toBool());
    properties.setTabIndex(m_tabIndex->value());
    QPointer<KoFormTool> tool(this);
    canvas()->addCommand(new ChangeFormPropertiesCommand(m_shape, properties, [tool]() {
        if (tool) {
            tool->updateProperties();
        }
    }));
}

void KoFormTool::deactivate()
{
    if (canvas() && canvas()->shapeManager()) {
        disconnect(canvas()->shapeManager(), &KoShapeManager::selectionChanged, this, &KoFormTool::shapeSelectionChanged);
    }
    m_shape = nullptr;
    updateProperties();
}

void KoFormTool::mouseReleaseEvent(KoPointerEvent *event)
{
    if (!event || !canvas() || !canvas()->shapeManager()) {
        return;
    }

    auto *shape = dynamic_cast<KoFormShape *>(canvas()->shapeManager()->shapeAt(event->point));
    if (shape && shape != m_shape) {
        m_shape = shape;
        updateProperties();
    }
}

void KoFormTool::shapeSelectionChanged()
{
    KoFormShape *shape = nullptr;
    if (canvas() && canvas()->shapeManager()) {
        for (KoShape *candidate : canvas()->shapeManager()->selection()->selectedShapes()) {
            if ((shape = dynamic_cast<KoFormShape *>(candidate))) {
                break;
            }
        }
    }
    if (m_shape == shape) {
        return;
    }
    m_shape = shape;
    updateProperties();
}

KoFormToolFactory::KoFormToolFactory()
    : KoToolFactoryBase(u"FormTool"_s)
{
    setToolTip(i18n("Form properties"));
    setIconName(u"document-properties"_s);
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(u"FormShape"_s);
}

KoToolBase *KoFormToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoFormTool(canvas);
}
