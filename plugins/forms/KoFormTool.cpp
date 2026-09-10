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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QVBoxLayout>
#include <chrono>
#include <functional>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

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
    , m_previewCompressor(200ms, KoSignalCompressor::Mode::FirstInactive, this)
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
    widget->setWindowTitle(i18nc("@title:form properties", "Form Properties"));
    auto *layout = new QVBoxLayout(widget);
    auto *form = new QFormLayout();
    layout->addLayout(form);
    m_type = new QLabel(widget);
    form->addRow(i18nc("@label:form property", "&Type:"), m_type);
    m_name = new QLineEdit(widget);
    form->addRow(i18nc("@label:form property", "&Name:"), m_name);
    m_title = new QLineEdit(widget);
    form->addRow(i18nc("@label:form property", "&Help text:"), m_title);
    const auto addBoolean = [form, widget](const QString &label) {
        auto *box = new QComboBox(widget);
        box->addItem(i18nc("@item:form boolean", "No"), false);
        box->addItem(i18nc("@item:form boolean", "Yes"), true);
        form->addRow(label, box);
        return box;
    };
    m_enabled = addBoolean(i18nc("@label:form property", "&Enabled:"));
    m_readOnly = addBoolean(i18nc("@label:form property", "&Read-only:"));
    m_printable = addBoolean(i18nc("@label:form property", "&Printable:"));
    m_tabStop = addBoolean(i18nc("@label:form property", "&Tab stop:"));
    m_tabIndex = new QSpinBox(widget);
    m_tabIndex->setRange(0, 32767);
    form->addRow(i18nc("@label:form property", "Tab &order:"), m_tabIndex);
    m_specificForm = form;
    m_specificStartRow = form->rowCount();
    layout->addStretch();
    connect(m_name, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    connect(m_title, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    connect(&m_previewCompressor, &KoSignalCompressor::timeout, this, &KoFormTool::commitProperties);
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

void KoFormTool::rebuildSpecificProperties()
{
    while (m_specificForm && m_specificForm->rowCount() > m_specificStartRow) {
        m_specificForm->removeRow(m_specificForm->rowCount() - 1);
    }
    m_specificProperties.clear();
    m_entries = nullptr;
    if (!m_shape || !m_shape->formControl()) {
        return;
    }
    const auto addText = [this](const QString &key, const QString &label) {
        auto *edit = new QLineEdit(m_options);
        m_specificForm->addRow(label, edit);
        m_specificProperties.insert(key, edit);
        connect(edit, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    };
    const auto addBoolean = [this](const QString &key, const QString &label) {
        auto *box = new QComboBox(m_options);
        box->addItem(i18nc("@item:form boolean", "No"), false);
        box->addItem(i18nc("@item:form boolean", "Yes"), true);
        m_specificForm->addRow(label, box);
        m_specificProperties.insert(key, box);
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormTool::commitProperties);
    };
    const auto addEntries = [this](const QString &label) {
        m_entries = new QListWidget(m_options);
        m_entries->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        m_entries->setMaximumHeight(120);
        m_specificForm->addRow(label, m_entries);
        auto *buttons = new QWidget(m_options);
        auto *buttonLayout = new QHBoxLayout(buttons);
        auto *add = new QPushButton(i18nc("@button", "Add"), buttons);
        auto *remove = new QPushButton(i18nc("@button", "Remove"), buttons);
        buttonLayout->addWidget(add);
        buttonLayout->addWidget(remove);
        m_specificForm->addRow(QString(), buttons);
        connect(add, &QPushButton::clicked, this, [this] {
            auto *item = new QListWidgetItem(i18nc("@item:form", "New item"), m_entries);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_entries->setCurrentItem(item);
            m_entries->editItem(item);
        });
        connect(remove, &QPushButton::clicked, this, [this] {
            delete m_entries->takeItem(m_entries->currentRow());
            commitProperties();
        });
        connect(m_entries, &QListWidget::itemChanged, this, [this] {
            commitProperties();
        });
    };
    addText(u"data-field"_s, i18nc("@label:form relationship", "Data field:"));
    addText(u"linked-cell"_s, i18nc("@label:form relationship", "Linked cell:"));
    addText(u"xforms-bind"_s, i18nc("@label:form relationship", "XForms binding:"));
    if (m_shape->controlKind() == KoOdfForm::ControlKind::FixedText || m_shape->controlKind() == KoOdfForm::ControlKind::Frame) {
        addText(u"for"_s, i18nc("@label:form relationship", "Target control:"));
    }
    switch (m_shape->controlKind()) {
    case KoOdfForm::ControlKind::Text:
    case KoOdfForm::ControlKind::Textarea:
    case KoOdfForm::ControlKind::FormattedText:
    case KoOdfForm::ControlKind::Password:
    case KoOdfForm::ControlKind::File:
        addText(u"max-length"_s, i18nc("@label:form property", "Max. length:"));
        addBoolean(u"multi-line"_s, i18nc("@label:form property", "Multi-line:"));
        break;
    case KoOdfForm::ControlKind::Number:
    case KoOdfForm::ControlKind::ValueRange:
        addText(u"min-value"_s, i18nc("@label:form property", "Minimum:"));
        addText(u"max-value"_s, i18nc("@label:form property", "Maximum:"));
        addText(u"step-size"_s, i18nc("@label:form property", "Step:"));
        break;
    case KoOdfForm::ControlKind::Button:
        addBoolean(u"default-button"_s, i18nc("@label:form property", "Default button:"));
        addBoolean(u"toggle"_s, i18nc("@label:form property", "Toggle button:"));
        break;
    case KoOdfForm::ControlKind::Checkbox:
    case KoOdfForm::ControlKind::Radio:
        addBoolean(u"selected"_s, i18nc("@label:form property", "Selected:"));
        addBoolean(u"tristate"_s, i18nc("@label:form property", "Tri-state:"));
        break;
    case KoOdfForm::ControlKind::Combobox:
        addBoolean(u"autocomplete"_s, i18nc("@label:form property", "Auto-complete:"));
        addEntries(i18nc("@label:form property", "Items:"));
        break;
    case KoOdfForm::ControlKind::Listbox:
        addBoolean(u"multiple"_s, i18nc("@label:form property", "Multiple selection:"));
        addBoolean(u"dropdown"_s, i18nc("@label:form property", "Drop-down:"));
        addText(u"list-source"_s, i18nc("@label:form property", "List source:"));
        addEntries(i18nc("@label:form property", "Items:"));
        break;
    case KoOdfForm::ControlKind::Grid:
        addEntries(i18nc("@label:form property", "Columns:"));
        break;
    case KoOdfForm::ControlKind::Image:
    case KoOdfForm::ControlKind::ImageFrame:
        addText(u"image-data"_s, i18nc("@label:form property", "Image data:"));
        addText(u"image-position"_s, i18nc("@label:form property", "Image position:"));
        addText(u"image-align"_s, i18nc("@label:form property", "Image alignment:"));
        break;
    default:
        break;
    }
}

void KoFormTool::updateProperties()
{
    if (!m_options) {
        return;
    }
    const auto *control = m_shape ? m_shape->formControl() : nullptr;
    rebuildSpecificProperties();
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
    m_type->setText(m_shape ? m_shape->controlKindName() : QString());
    m_title->setText(control ? control->title() : QString());
    m_enabled->setCurrentIndex(control && !control->disabled());
    m_readOnly->setCurrentIndex(control && control->readOnly());
    m_printable->setCurrentIndex(control && control->printable());
    m_tabStop->setCurrentIndex(control && control->tabStop());
    m_tabIndex->setValue(control ? control->tabIndex() : 0);
    for (auto it = m_specificProperties.cbegin(); it != m_specificProperties.cend(); ++it) {
        const QSignalBlocker blocker(it.value());
        if (auto *edit = qobject_cast<QLineEdit *>(it.value())) {
            if (!control) {
                edit->clear();
            } else if (it.key() == "max-length"_L1) {
                edit->setText(static_cast<const KoOdfForm::Text *>(control)->maxLength());
            } else if (it.key() == "min-value"_L1) {
                edit->setText(static_cast<const KoOdfForm::Number *>(control)->minValue());
            } else if (it.key() == "max-value"_L1) {
                edit->setText(static_cast<const KoOdfForm::Number *>(control)->maxValue());
            } else if (it.key() == "step-size"_L1) {
                edit->setText(static_cast<const KoOdfForm::Number *>(control)->stepSize());
            } else if (it.key() == "list-source"_L1) {
                edit->setText(static_cast<const KoOdfForm::Listbox *>(control)->listSource());
            } else if (it.key() == "image-data"_L1) {
                edit->setText(static_cast<const KoOdfForm::Image *>(control)->imageData());
            } else if (it.key() == "image-position"_L1) {
                edit->setText(static_cast<const KoOdfForm::Image *>(control)->imagePosition());
            } else if (it.key() == "image-align"_L1) {
                edit->setText(static_cast<const KoOdfForm::Image *>(control)->imageAlign());
            } else if (it.key() == "data-field"_L1) {
                edit->setText(control->dataField());
            } else if (it.key() == "linked-cell"_L1) {
                edit->setText(control->linkedCell());
            } else if (it.key() == "xforms-bind"_L1) {
                edit->setText(control->xformsBind());
            } else {
                edit->setText(control->formAttribute(it.key()));
            }
        } else if (auto *box = qobject_cast<QComboBox *>(it.value())) {
            QString value;
            if (control) {
                value = control->formAttribute(it.key());
                if (it.key() == "default-button"_L1) {
                    value = static_cast<const KoOdfForm::Button *>(control)->defaultButton() ? u"true"_s : u"false"_s;
                } else if (it.key() == "toggle"_L1) {
                    value = static_cast<const KoOdfForm::Button *>(control)->toggle() ? u"true"_s : u"false"_s;
                } else if (it.key() == "selected"_L1
                           && (m_shape->controlKind() == KoOdfForm::ControlKind::Checkbox || m_shape->controlKind() == KoOdfForm::ControlKind::Radio)) {
                    value = (m_shape->controlKind() == KoOdfForm::ControlKind::Checkbox ? static_cast<const KoOdfForm::Checkbox *>(control)->selected()
                                                                                        : static_cast<const KoOdfForm::Radio *>(control)->selected())
                        ? u"true"_s
                        : u"false"_s;
                } else if (it.key() == "autocomplete"_L1) {
                    value = static_cast<const KoOdfForm::Combobox *>(control)->autoComplete() ? u"true"_s : u"false"_s;
                } else if (it.key() == "multiple"_L1) {
                    value = static_cast<const KoOdfForm::Listbox *>(control)->multiple() ? u"true"_s : u"false"_s;
                } else if (it.key() == "dropdown"_L1) {
                    value = static_cast<const KoOdfForm::Listbox *>(control)->dropdown() ? u"true"_s : u"false"_s;
                }
                if (it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Checkbox) {
                    value = control->formAttribute(u"current-state"_s);
                    if (value.isEmpty()) {
                        value = control->formAttribute(u"state"_s);
                    }
                    box->setCurrentIndex(value == "checked"_L1);
                    continue;
                }
                if (it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Radio) {
                    value = control->formAttribute(u"current-selected"_s);
                }
            }
            box->setCurrentIndex(value == "true"_L1);
        }
    }
    if (m_entries) {
        const QSignalBlocker blocker(m_entries);
        m_entries->clear();
        for (const auto &entry : control->entries()) {
            auto *item = new QListWidgetItem(entry.label, m_entries);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
}

void KoFormTool::commitProperties()
{
    const auto *before = m_shape ? m_shape->formControl() : nullptr;
    if (!before) {
        return;
    }
    if (before->name() == m_name->text() && before->title() == m_title->text() && before->disabled() == !m_enabled->currentData().toBool()
        && before->readOnly() == m_readOnly->currentData().toBool() && before->printable() == m_printable->currentData().toBool()
        && before->tabStop() == m_tabStop->currentData().toBool() && before->tabIndex() == m_tabIndex->value()) {
        bool changed = m_entries != nullptr;
        for (auto it = m_specificProperties.cbegin(); it != m_specificProperties.cend(); ++it) {
            const QString value = [&] {
                if (auto *edit = qobject_cast<QLineEdit *>(it.value())) {
                    return edit->text();
                }
                return it.value()->property("currentData").toString();
            }();
            Q_UNUSED(value);
            changed = true;
        }
        if (!changed) {
            return;
        }
    }
    KoOdfForm::GenericControl properties;
    static_cast<KoOdfForm::Control &>(properties) = *before;
    properties.setName(m_name->text());
    properties.setTitle(m_title->text());
    properties.setDisabled(!m_enabled->currentData().toBool());
    properties.setReadOnly(m_readOnly->currentData().toBool());
    properties.setPrintable(m_printable->currentData().toBool());
    properties.setTabStop(m_tabStop->currentData().toBool());
    properties.setTabIndex(m_tabIndex->value());
    for (auto it = m_specificProperties.cbegin(); it != m_specificProperties.cend(); ++it) {
        QString value;
        if (auto *edit = qobject_cast<QLineEdit *>(it.value())) {
            value = edit->text();
        } else if (auto *box = qobject_cast<QComboBox *>(it.value())) {
            value = box->currentData().toBool() ? u"true"_s : u"false"_s;
        }
        if (it.key() == "data-field"_L1) {
            properties.setDataField(value);
        } else if (it.key() == "linked-cell"_L1) {
            properties.setLinkedCell(value);
        } else if (it.key() == "xforms-bind"_L1) {
            properties.setXformsBind(value);
        } else if (it.key() == "max-length"_L1) {
            KoOdfForm::Text typed;
            static_cast<KoOdfForm::Control &>(typed) = properties;
            typed.setMaxLength(value);
            static_cast<KoOdfForm::Control &>(properties) = typed;
        } else if (it.key() == "min-value"_L1 || it.key() == "max-value"_L1 || it.key() == "step-size"_L1) {
            KoOdfForm::Number typed;
            static_cast<KoOdfForm::Control &>(typed) = properties;
            if (it.key() == "min-value"_L1) {
                typed.setMinValue(value);
            } else if (it.key() == "max-value"_L1) {
                typed.setMaxValue(value);
            } else {
                typed.setStepSize(value);
            }
            static_cast<KoOdfForm::Control &>(properties) = typed;
        } else if (it.key() == "default-button"_L1) {
            properties.setFormAttribute(u"default-button"_s, value);
        } else if (it.key() == "toggle"_L1) {
            properties.setFormAttribute(u"toggle"_s, value);
        } else if (it.key() == "autocomplete"_L1) {
            properties.setFormAttribute(u"autocomplete"_s, value);
        } else if (it.key() == "multiple"_L1) {
            properties.setFormAttribute(u"multiple"_s, value);
        } else if (it.key() == "dropdown"_L1) {
            properties.setFormAttribute(u"dropdown"_s, value);
        } else if (it.key() == "list-source"_L1) {
            properties.setFormAttribute(u"list-source"_s, value);
        } else if (it.key() == "image-data"_L1) {
            properties.setFormAttribute(u"image-data"_s, value);
        } else if (it.key() == "image-position"_L1) {
            properties.setFormAttribute(u"image-position"_s, value);
        } else if (it.key() == "image-align"_L1) {
            properties.setFormAttribute(u"image-align"_s, value);
        } else if (it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Checkbox) {
            properties.setFormAttribute(u"state"_s, value == "true"_L1 ? u"checked"_s : u"unchecked"_s);
            properties.setFormAttribute(u"current-state"_s, value == "true"_L1 ? u"checked"_s : u"unchecked"_s);
        } else if (it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Radio) {
            properties.setFormAttribute(u"selected"_s, value);
            properties.setFormAttribute(u"current-selected"_s, value);
        } else {
            properties.setFormAttribute(it.key(), value);
        }
    }
    if (m_entries) {
        const auto oldEntries = before->entries();
        QVector<KoOdfForm::Control::Entry> entries;
        entries.reserve(m_entries->count());
        for (int i = 0; i < m_entries->count(); ++i) {
            entries.append({m_entries->item(i)->text(), i < oldEntries.size() ? oldEntries.at(i).selected : false});
        }
        properties.setEntries(entries);
    }
    QPointer<KoFormTool> tool(this);
    canvas()->addCommand(new ChangeFormPropertiesCommand(m_shape, properties, [tool]() {
        if (tool) {
            if (tool->m_shape) {
                tool->canvas()->updateCanvas(tool->m_shape->boundingRect());
            }
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
    setToolTip(i18nc("@info:tooltip", "Form properties"));
    setIconName(u"document-properties"_s);
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(u"FormShape"_s);
}

KoToolBase *KoFormToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoFormTool(canvas);
}
