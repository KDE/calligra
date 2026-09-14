/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormTool.h"
#include "KoFormEventsWidget.h"
#include "KoFormShape.h"

#include <KDateComboBox>
#include <KTimeComboBox>
#include <KoDocument.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeController.h>

#include <algorithm>

#include <KLocalizedString>
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <kundo2command.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QFont>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPen>
#include <QPixmap>
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
constexpr int GridWidthRole = Qt::UserRole + 1;
constexpr int GridTypeRole = Qt::UserRole + 2;
constexpr int GridBindingRole = Qt::UserRole + 3;
class ChangeFormPropertiesCommand : public KUndo2Command
{
public:
    ChangeFormPropertiesCommand(KoFormShape *shape,
                                const KoOdfForm::Control &properties,
                                const QMap<QString, QString> &formEvents,
                                std::function<void()> refresh)
        : m_shape(shape)
        , m_refresh(std::move(refresh))
        , m_afterFormEvents(formEvents)
    {
        setText(kundo2_i18n("Change form properties"));
        static_cast<KoOdfForm::Control &>(m_before) = *shape->formControl();
        static_cast<KoOdfForm::Control &>(m_after) = properties;
        m_beforeFormEvents = shape->formEventHandlers();
    }
    void redo() override
    {
        m_shape->setControlProperties(m_after);
        m_shape->setFormEventHandlers(m_afterFormEvents);
        m_refresh();
    }
    void undo() override
    {
        m_shape->setControlProperties(m_before);
        m_shape->setFormEventHandlers(m_beforeFormEvents);
        m_refresh();
    }

private:
    KoFormShape *m_shape;
    std::function<void()> m_refresh;
    KoOdfForm::GenericControl m_before;
    KoOdfForm::GenericControl m_after;
    QMap<QString, QString> m_beforeFormEvents;
    QMap<QString, QString> m_afterFormEvents;
};
}

KoFormTool::KoFormTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_previewCompressor(200ms, KoSignalCompressor::Mode::FirstInactive, this)
{
}

void KoFormTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (!canvas()) {
        return;
    }
    KoFormShape *shape = m_shape;
    if (!shape) {
        for (KoShape *candidate : canvas()->shapeManager()->selection()->selectedShapes()) {
            shape = dynamic_cast<KoFormShape *>(candidate);
            if (shape) {
                break;
            }
        }
    }
    if (!shape) {
        return;
    }
    painter.save();
    painter.setTransform(shape->absoluteTransformation(&converter) * painter.transform());
    KoShape::applyConversion(painter, converter);
    QPen pen(QColor(0, 173, 245, 127));
    pen.setWidth(0);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRectF(QPointF(0, 0), shape->size()));
    painter.restore();
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
    if (m_shape) {
        canvas()->updateCanvas(m_shape->boundingRect());
    }
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
        auto *box = new QCheckBox(widget);
        box->setText(label);
        form->addRow(QString{}, box);
        return box;
    };
    m_enabled = addBoolean(i18nc("@label:form property", "&Enabled"));
    m_readOnly = addBoolean(i18nc("@label:form property", "&Read-only"));
    m_printable = addBoolean(i18nc("@label:form property", "&Printable"));
    m_tabStop = addBoolean(i18nc("@label:form property", "&Tab stop"));
    m_tabIndex = new QSpinBox(widget);
    m_tabIndex->setRange(0, 32767);
    form->addRow(i18nc("@label:form property", "Tab &order:"), m_tabIndex);
    m_specificForm = form;
    m_eventOptions = new QWidget();
    m_eventOptions->setWindowTitle(i18nc("@title:form events", "Events"));
    auto *eventLayout = new QVBoxLayout(m_eventOptions);
    m_events = new KoFormEventsWidget(m_eventOptions);
    eventLayout->addWidget(m_events);
    connect(m_events, &KoFormEventsWidget::eventsChanged, this, &KoFormTool::commitProperties);
    m_formEventOptions = new QWidget();
    m_formEventOptions->setWindowTitle(i18nc("@title:form events", "Form Events"));
    auto *formEventLayout = new QVBoxLayout(m_formEventOptions);
    m_formEvents = new KoFormEventsWidget(m_formEventOptions);
    formEventLayout->addWidget(m_formEvents);
    connect(m_formEvents, &KoFormEventsWidget::eventsChanged, this, &KoFormTool::commitProperties);
    m_specificStartRow = form->rowCount();
    layout->addStretch();
    connect(m_name, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    connect(m_title, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    connect(&m_previewCompressor, &KoSignalCompressor::timeout, this, &KoFormTool::commitProperties);
    connect(m_enabled, &QCheckBox::toggled, this, [this] {
        commitProperties();
    });
    connect(m_readOnly, &QCheckBox::toggled, this, [this] {
        commitProperties();
    });
    connect(m_printable, &QCheckBox::toggled, this, [this] {
        commitProperties();
    });
    connect(m_tabStop, &QCheckBox::toggled, this, [this] {
        commitProperties();
    });
    connect(m_tabIndex, &QSpinBox::editingFinished, this, &KoFormTool::commitProperties);
    updateProperties();
    return widget;
}

QList<QPointer<QWidget>> KoFormTool::createOptionWidgets()
{
    QWidget *properties = createOptionWidget();
    return {properties, m_eventOptions, m_formEventOptions};
}

void KoFormTool::rebuildSpecificProperties()
{
    while (m_specificForm && m_specificForm->rowCount() > m_specificStartRow) {
        m_specificForm->removeRow(m_specificForm->rowCount() - 1);
    }
    m_specificProperties.clear();
    m_entries = nullptr;
    m_imagePreview = nullptr;
    if (!m_shape || !m_shape->formControl()) {
        while (m_eventForm && m_eventForm->rowCount() > 0) {
            m_eventForm->removeRow(m_eventForm->rowCount() - 1);
        }
        return;
    }
    const auto addText = [this](const QString &key, const QString &label, QFormLayout *targetForm = nullptr) {
        auto *edit = new QLineEdit(m_options);
        (targetForm ? targetForm : m_specificForm)->addRow(label, edit);
        m_specificProperties.insert(key, edit);
        connect(edit, &QLineEdit::textChanged, &m_previewCompressor, &KoSignalCompressor::start);
    };
    const auto addNumberText = [this, &addText](const QString &key, const QString &label) {
        addText(key, label);
        auto *edit = qobject_cast<QLineEdit *>(m_specificProperties.value(key));
        edit->setValidator(new QDoubleValidator(edit));
    };
    const auto addFileText = [this, &addText](const QString &key, const QString &label) {
        addText(key, label);
        auto *edit = qobject_cast<QLineEdit *>(m_specificProperties.value(key));
        auto *action = edit->addAction(QIcon::fromTheme(u"document-open-symbolic"_s), QLineEdit::TrailingPosition);
        connect(action, &QAction::triggered, this, [this, edit] {
            const QString file = QFileDialog::getOpenFileName(m_options, i18nc("@title:form", "Select file"));
            if (!file.isEmpty()) {
                edit->setText(file);
            }
        });
    };
    const auto addBoolean = [this](const QString &key, const QString &label) {
        auto *box = new QCheckBox(m_options);
        box->setText(label);
        m_specificForm->addRow(QString(), box);
        m_specificProperties.insert(key, box);
        connect(box, &QCheckBox::toggled, this, &KoFormTool::commitProperties);
    };
    const auto addFormatSelector = [this](const QString &key, const QString &label, const QList<QPair<QString, QString>> &formats) {
        auto *box = new QComboBox(m_options);
        box->addItem(i18nc("@item:form format", "Custom"), QString());
        for (const auto &format : formats) {
            box->addItem(format.first, format.second);
        }
        m_specificForm->addRow(label, box);
        m_specificProperties.insert(key, box);
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormTool::commitProperties);
    };
    const auto addDateEditor = [this](const QString &key, const QString &label) {
        auto *edit = new KDateComboBox(m_options);
        m_specificForm->addRow(label, edit);
        m_specificProperties.insert(key, edit);
        connect(edit, &KDateComboBox::dateChanged, this, &KoFormTool::commitProperties);
    };
    const auto addTimeEditor = [this](const QString &key, const QString &label) {
        auto *edit = new KTimeComboBox(m_options);
        m_specificForm->addRow(label, edit);
        m_specificProperties.insert(key, edit);
        connect(edit, &KTimeComboBox::timeChanged, this, &KoFormTool::commitProperties);
    };
    const auto addTargetSelector = [this](const QString &key, const QString &label) {
        auto *box = new QComboBox(m_options);
        box->addItem(i18nc("@item:form label target", "None"), QString());
        for (KoShape *candidate : registeredFormShapes()) {
            auto *target = dynamic_cast<KoFormShape *>(candidate);
            if (!target || target == m_shape || target->document() != m_shape->document()) {
                continue;
            }
            const QString displayName = target->formControl() && !target->formControl()->name().isEmpty() ? target->formControl()->name() : target->controlId();
            box->addItem(displayName, target->controlId());
        }
        m_specificForm->addRow(label, box);
        m_specificProperties.insert(key, box);
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormTool::commitProperties);
    };
    const auto addEnumSelector = [this](const QString &key, const QString &label, const QList<QPair<QString, QString>> &values) {
        auto *box = new QComboBox(m_options);
        for (const auto &value : values) {
            box->addItem(value.first, value.second);
        }
        m_specificForm->addRow(label, box);
        m_specificProperties.insert(key, box);
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormTool::commitProperties);
    };
    const auto addInteger = [this](const QString &key, const QString &label, int defaultValue) {
        auto *spin = new QSpinBox(m_options);
        spin->setRange(1, 100);
        spin->setValue(defaultValue);
        m_specificForm->addRow(label, spin);
        m_specificProperties.insert(key, spin);
        connect(spin, &QSpinBox::editingFinished, this, &KoFormTool::commitProperties);
    };
    const auto addEntries = [this](const QString &label, bool checkable = false) {
        m_entries = new QListWidget(m_options);
        m_entries->setProperty("checkable", checkable);
        m_entries->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        m_entries->setMaximumHeight(120);
        m_specificForm->addRow(label, m_entries);
        auto *buttons = new QWidget(m_options);
        auto *buttonLayout = new QHBoxLayout(buttons);
        auto *add = new QPushButton(i18nc("@button", "Add"), buttons);
        auto *remove = new QPushButton(i18nc("@button", "Remove"), buttons);
        auto *up = new QPushButton(QIcon::fromTheme(u"arrow-up-symbolic"_s), QString(), buttons);
        auto *down = new QPushButton(QIcon::fromTheme(u"arrow-down-symbolic"_s), QString(), buttons);
        up->setToolTip(i18nc("@info:tooltip", "Move the selected item up"));
        down->setToolTip(i18nc("@info:tooltip", "Move the selected item down"));
        buttonLayout->addWidget(add);
        buttonLayout->addWidget(remove);
        buttonLayout->addWidget(up);
        buttonLayout->addWidget(down);
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
        connect(up, &QPushButton::clicked, this, [this] {
            const int row = m_entries->currentRow();
            if (row > 0) {
                auto *item = m_entries->takeItem(row);
                m_entries->insertItem(row - 1, item);
                m_entries->setCurrentRow(row - 1);
                commitProperties();
            }
        });
        connect(down, &QPushButton::clicked, this, [this] {
            const int row = m_entries->currentRow();
            if (row >= 0 && row + 1 < m_entries->count()) {
                auto *item = m_entries->takeItem(row);
                m_entries->insertItem(row + 1, item);
                m_entries->setCurrentRow(row + 1);
                commitProperties();
            }
        });
        connect(m_entries, &QListWidget::itemChanged, this, [this] {
            commitProperties();
        });
        connect(m_entries, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
            QDialog dialog(m_options);
            dialog.setWindowTitle(i18nc("@title:form", "Edit item"));
            auto *form = new QFormLayout(&dialog);
            auto *label = new QLineEdit(item->text(), &dialog);
            auto *value = new QLineEdit(item->data(Qt::UserRole).toString(), &dialog);
            form->addRow(i18nc("@label:form item", "Label:"), label);
            form->addRow(i18nc("@label:form item", "Value:"), value);
            QLineEdit *width = nullptr;
            QComboBox *type = nullptr;
            QLineEdit *binding = nullptr;
            if (m_shape && m_shape->controlKind() == KoOdfForm::ControlKind::Grid) {
                width = new QLineEdit(item->data(GridWidthRole).toString(), &dialog);
                type = new QComboBox(&dialog);
                type->addItems({i18nc("@item:form grid column type", "Text"),
                                i18nc("@item:form grid column type", "Number"),
                                i18nc("@item:form grid column type", "Date")});
                type->setCurrentText(item->data(GridTypeRole).toString());
                binding = new QLineEdit(item->data(GridBindingRole).toString(), &dialog);
                form->addRow(i18nc("@label:form grid column", "Width:"), width);
                form->addRow(i18nc("@label:form grid column", "Type:"), type);
                form->addRow(i18nc("@label:form grid column", "Linked cell:"), binding);
            }
            auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            form->addRow(buttons);
            connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            if (dialog.exec() == QDialog::Accepted) {
                item->setText(label->text());
                item->setData(Qt::UserRole, value->text());
                if (width) {
                    item->setData(GridWidthRole, width->text());
                    item->setData(GridTypeRole, type->currentText());
                    item->setData(GridBindingRole, binding->text());
                }
                commitProperties();
            }
        });
    };
    const auto addSelectedValue = [this] {
        auto *box = new QComboBox(m_options);
        m_specificForm->addRow(i18nc("@label:form property", "Selected value:"), box);
        m_specificProperties.insert(u"selected-value"_s, box);
        connect(box, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormTool::commitProperties);
    };
    addText(u"data-field"_s, i18nc("@label:form relationship", "Data field:"));
    addText(u"linked-cell"_s, i18nc("@label:form relationship", "Linked cell:"));
    addText(u"xforms-bind"_s, i18nc("@label:form relationship", "XForms binding:"));
    if (m_shape->controlKind() == KoOdfForm::ControlKind::FixedText || m_shape->controlKind() == KoOdfForm::ControlKind::Frame) {
        addTargetSelector(u"for"_s, i18nc("@label:form relationship", "Target control:"));
    }
    switch (m_shape->controlKind()) {
    case KoOdfForm::ControlKind::Text:
    case KoOdfForm::ControlKind::Textarea:
    case KoOdfForm::ControlKind::FormattedText:
    case KoOdfForm::ControlKind::Password:
        addText(u"max-length"_s, i18nc("@label:form property", "Max. length:"));
        addBoolean(u"multi-line"_s, i18nc("@label:form property", "Multi-line:"));
        break;
    case KoOdfForm::ControlKind::File:
        addFileText(u"current-value"_s, i18nc("@label:form property", "File:"));
        break;
    case KoOdfForm::ControlKind::Number:
    case KoOdfForm::ControlKind::ValueRange:
        addNumberText(u"min-value"_s, i18nc("@label:form property", "Minimum:"));
        addNumberText(u"max-value"_s, i18nc("@label:form property", "Maximum:"));
        addNumberText(u"step-size"_s, i18nc("@label:form property", "Step:"));
        addFormatSelector(u"format"_s,
                          i18nc("@label:form property", "Format:"),
                          {{i18nc("@item:form number format", "Decimal"), u"0.00"_s},
                           {i18nc("@item:form number format", "Percentage"), u"0%"_s},
                           {i18nc("@item:form number format", "Currency"), u"$#,##0.00"_s}});
        break;
    case KoOdfForm::ControlKind::Date:
        addDateEditor(u"current-value"_s, i18nc("@label:form property", "Value:"));
        addFormatSelector(u"date-format"_s,
                          i18nc("@label:form property", "Date format:"),
                          {{i18nc("@item:form date format", "Short date"), u"MM/DD/YY"_s},
                           {i18nc("@item:form date format", "Long date"), u"MMMM D, YYYY"_s},
                           {i18nc("@item:form date format", "ISO date"), u"YYYY-MM-DD"_s}});
        break;
    case KoOdfForm::ControlKind::Time:
        addTimeEditor(u"current-value"_s, i18nc("@label:form property", "Value:"));
        addFormatSelector(u"time-format"_s,
                          i18nc("@label:form property", "Time format:"),
                          {{i18nc("@item:form time format", "Short time"), u"HH:MM"_s},
                           {i18nc("@item:form time format", "Long time"), u"HH:MM:SS"_s},
                           {i18nc("@item:form time format", "ISO time"), u"HH:MM:SSZ"_s}});
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
        addSelectedValue();
        break;
    case KoOdfForm::ControlKind::Listbox:
        addBoolean(u"multiple"_s, i18nc("@label:form property", "Multiple selection:"));
        addBoolean(u"dropdown"_s, i18nc("@label:form property", "Drop-down:"));
        addText(u"list-source"_s, i18nc("@label:form property", "List source:"));
        if (auto *source = qobject_cast<QLineEdit *>(m_specificProperties.value(u"list-source"_s))) {
            auto *action = source->addAction(QIcon::fromTheme(u"view-grid-symbolic"_s), QLineEdit::TrailingPosition);
            connect(action, &QAction::triggered, this, [this, source] {
                bool accepted = false;
                const QString value = QInputDialog::getText(m_options,
                                                            i18nc("@title:form", "Select list source"),
                                                            i18nc("@label:form", "Cell range:"),
                                                            QLineEdit::Normal,
                                                            source->text(),
                                                            &accepted);
                if (accepted) {
                    source->setText(value);
                }
            });
        }
        addEntries(i18nc("@label:form property", "Items:"), true);
        addSelectedValue();
        break;
    case KoOdfForm::ControlKind::Grid:
        addEntries(i18nc("@label:form property", "Columns:"));
        addInteger(u"row-count"_s, i18nc("@label:form property", "Rows:"), 3);
        break;
    case KoOdfForm::ControlKind::Image:
    case KoOdfForm::ControlKind::ImageFrame:
        addFileText(u"image-data"_s, i18nc("@label:form property", "Image data:"));
        addEnumSelector(u"image-position"_s,
                        i18nc("@label:form property", "Image position:"),
                        {{i18nc("@item:form image position", "Normal"), u"normal"_s},
                         {i18nc("@item:form image position", "Center"), u"center"_s},
                         {i18nc("@item:form image position", "Tile"), u"tile"_s},
                         {i18nc("@item:form image position", "Stretch"), u"stretch"_s}});
        addEnumSelector(u"image-align"_s,
                        i18nc("@label:form property", "Image alignment:"),
                        {{i18nc("@item:form image alignment", "Left"), u"left"_s},
                         {i18nc("@item:form image alignment", "Center"), u"center"_s},
                         {i18nc("@item:form image alignment", "Right"), u"right"_s}});
        addEnumSelector(u"image-scale"_s,
                        i18nc("@label:form property", "Scaling:"),
                        {{i18nc("@item:form image scaling", "None"), u"none"_s},
                         {i18nc("@item:form image scaling", "Fit"), u"fit"_s},
                         {i18nc("@item:form image scaling", "Fill"), u"fill"_s}});
        addEnumSelector(u"image-source"_s,
                        i18nc("@label:form property", "Image source:"),
                        {{i18nc("@item:form image source", "Embedded"), u"embedded"_s}, {i18nc("@item:form image source", "Linked"), u"linked"_s}});
        addBoolean(u"preserve-aspect"_s, i18nc("@label:form property", "Preserve aspect ratio"));
        m_imagePreview = new QLabel(m_options);
        m_imagePreview->setMinimumSize(96, 64);
        m_imagePreview->setAlignment(Qt::AlignCenter);
        m_imagePreview->setFrameShape(QFrame::StyledPanel);
        m_specificForm->addRow(i18nc("@label:form property", "Preview:"), m_imagePreview);
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
    m_enabled->setChecked(control && !control->disabled());
    m_readOnly->setChecked(control && control->readOnly());
    m_printable->setChecked(control && control->printable());
    m_tabStop->setChecked(control && control->tabStop());
    m_tabIndex->setValue(control ? control->tabIndex() : 0);
    if (m_events) {
        m_events->setEvents(control ? control->eventHandlers() : QMap<QString, QString>());
    }
    if (m_formEvents) {
        m_formEvents->setEvents(m_shape ? m_shape->formEventHandlers() : QMap<QString, QString>());
    }
    if (auto *document = dynamic_cast<KoDocument *>(canvas()->shapeController()->documentBase())) {
        m_events->setScripts(document->scripts());
        m_formEvents->setScripts(document->scripts());
    }
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
            } else if (it.key() == "current-value"_L1) {
                edit->setText(control->currentValue());
            } else if (it.key().startsWith("event-"_L1)) {
                edit->setText(control->eventHandler(it.key().mid(6)));
            } else {
                edit->setText(control->formAttribute(it.key()));
            }
        } else if (auto *date = qobject_cast<KDateComboBox *>(it.value())) {
            date->setDate(QDate::fromString(control ? control->currentValue() : QString(), Qt::ISODate));
        } else if (auto *time = qobject_cast<KTimeComboBox *>(it.value())) {
            time->setTime(QTime::fromString(control ? control->currentValue() : QString(), Qt::ISODate));
        } else if (auto *box = qobject_cast<QComboBox *>(it.value())) {
            if (it.key() == "for"_L1) {
                const QString targetId = control ? control->formAttribute(it.key()) : QString();
                box->setCurrentIndex(box->findData(targetId));
                continue;
            }
            if (it.key() == "format"_L1 || it.key() == "date-format"_L1 || it.key() == "time-format"_L1 || it.key() == "image-position"_L1
                || it.key() == "image-align"_L1 || it.key() == "image-scale"_L1 || it.key() == "image-source"_L1) {
                const QString value = control ? control->formAttribute(it.key()) : QString();
                const int index = box->findData(value);
                box->setCurrentIndex(index >= 0 ? index : 0);
                continue;
            }
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
        } else if (auto *checkBox = qobject_cast<QCheckBox *>(it.value())) {
            QString value = control ? control->formAttribute(it.key()) : QString();
            if (control && it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Checkbox) {
                value = control->formAttribute(u"current-state"_s);
                if (value.isEmpty()) {
                    value = control->formAttribute(u"state"_s);
                }
            } else if (control && it.key() == "selected"_L1 && m_shape->controlKind() == KoOdfForm::ControlKind::Radio) {
                value = control->formAttribute(u"current-selected"_s);
            }
            checkBox->setChecked(value == "true"_L1 || value == "checked"_L1);
        } else if (auto *spin = qobject_cast<QSpinBox *>(it.value())) {
            const int rows = control ? control->formAttribute(it.key()).toInt() : 0;
            spin->setValue(rows > 0 ? rows : 3);
        }
    }
    if (m_entries) {
        const QSignalBlocker blocker(m_entries);
        m_entries->clear();
        for (const auto &entry : control->entries()) {
            auto *item = new QListWidgetItem(entry.label, m_entries);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            if (m_entries->property("checkable").toBool()) {
                item->setCheckState(entry.selected ? Qt::Checked : Qt::Unchecked);
            }
            item->setData(Qt::UserRole, entry.value);
            item->setData(GridWidthRole, entry.width);
            item->setData(GridTypeRole, entry.type);
            item->setData(GridBindingRole, entry.binding);
        }
        if (auto *selected = qobject_cast<QComboBox *>(m_specificProperties.value(u"selected-value"_s))) {
            const QSignalBlocker blocker(selected);
            selected->clear();
            for (const auto &entry : control->entries()) {
                selected->addItem(entry.label, entry.value);
            }
            const int index = selected->findData(control->currentValue());
            selected->setCurrentIndex(index >= 0 ? index : -1);
        }
    }
    if (m_imagePreview && control) {
        const QString imagePath = static_cast<const KoOdfForm::Image *>(control)->imageData();
        const QPixmap pixmap(imagePath);
        m_imagePreview->setPixmap(pixmap.isNull() ? QPixmap() : pixmap.scaled(m_imagePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_imagePreview->setText(pixmap.isNull() ? i18nc("@info:form", "No image selected") : QString());
    }
}

void KoFormTool::commitProperties()
{
    const auto *before = m_shape ? m_shape->formControl() : nullptr;
    if (!before) {
        return;
    }
    const auto beforeFormEvents = m_shape->formEventHandlers();
    const auto currentFormEvents = m_formEvents ? m_formEvents->events() : QMap<QString, QString>();
    const auto currentControlEvents = m_events ? m_events->events() : QMap<QString, QString>();
    auto currentScripts = m_events ? m_events->scripts() : KoOdfScript::Scripts();
    if (m_formEvents) {
        for (const auto &script : m_formEvents->scripts()) {
            const auto duplicate = std::find_if(currentScripts.cbegin(), currentScripts.cend(), [&script](const auto &existing) {
                return existing.name == script.name;
            });
            if (duplicate == currentScripts.cend()) {
                currentScripts.append(script);
            }
        }
    }
    if (auto *document = dynamic_cast<KoDocument *>(canvas()->shapeController()->documentBase())) {
        document->setScripts(currentScripts);
    }
    if (before->name() == m_name->text() && before->title() == m_title->text() && before->disabled() == !m_enabled->isChecked()
        && before->readOnly() == m_readOnly->isChecked() && before->printable() == m_printable->isChecked() && before->tabStop() == m_tabStop->isChecked()
        && before->tabIndex() == m_tabIndex->value() && beforeFormEvents == currentFormEvents && before->eventHandlers() == currentControlEvents) {
        bool changed = m_entries != nullptr;
        for (auto it = m_specificProperties.cbegin(); it != m_specificProperties.cend(); ++it) {
            const QString value = [&] {
                if (auto *edit = qobject_cast<QLineEdit *>(it.value())) {
                    return edit->text();
                }
                if (auto *spin = qobject_cast<QSpinBox *>(it.value())) {
                    return QString::number(spin->value());
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
    properties.setDisabled(!m_enabled->isChecked());
    properties.setReadOnly(m_readOnly->isChecked());
    properties.setPrintable(m_printable->isChecked());
    properties.setTabStop(m_tabStop->isChecked());
    properties.setTabIndex(m_tabIndex->value());
    if (m_events) {
        const auto &eventHandlers = currentControlEvents;
        for (const auto &event : before->eventHandlers().keys()) {
            properties.setEventHandler(event, eventHandlers.value(event));
        }
        for (auto it = eventHandlers.cbegin(); it != eventHandlers.cend(); ++it) {
            properties.setEventHandler(it.key(), it.value());
        }
    }
    for (auto it = m_specificProperties.cbegin(); it != m_specificProperties.cend(); ++it) {
        QString value;
        if (auto *edit = qobject_cast<QLineEdit *>(it.value())) {
            value = edit->text();
        } else if (auto *box = qobject_cast<QComboBox *>(it.value())) {
            if (it.key() == "for"_L1 || it.key() == "selected-value"_L1 || it.key() == "format"_L1 || it.key() == "date-format"_L1
                || it.key() == "time-format"_L1 || it.key() == "image-position"_L1 || it.key() == "image-align"_L1 || it.key() == "image-scale"_L1
                || it.key() == "image-source"_L1) {
                value = box->currentData().toString();
            } else {
                value = box->currentData().toBool() ? u"true"_s : u"false"_s;
            }
        } else if (auto *date = qobject_cast<KDateComboBox *>(it.value())) {
            value = date->date().toString(Qt::ISODate);
        } else if (auto *time = qobject_cast<KTimeComboBox *>(it.value())) {
            value = time->time().toString(Qt::ISODate);
        } else if (auto *checkBox = qobject_cast<QCheckBox *>(it.value())) {
            value = checkBox->isChecked() ? u"true"_s : u"false"_s;
        } else if (auto *spin = qobject_cast<QSpinBox *>(it.value())) {
            value = QString::number(spin->value());
        }
        if (it.key() == "data-field"_L1) {
            properties.setDataField(value);
        } else if (it.key() == "linked-cell"_L1) {
            properties.setLinkedCell(value);
        } else if (it.key() == "xforms-bind"_L1) {
            properties.setXformsBind(value);
        } else if (it.key() == "current-value"_L1) {
            properties.setCurrentValue(value);
        } else if (it.key() == "selected-value"_L1) {
            properties.setCurrentValue(value);
        } else if (it.key().startsWith("event-"_L1)) {
            properties.setEventHandler(it.key().mid(6), value);
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
            auto *item = m_entries->item(i);
            const QString value = item->data(Qt::UserRole).toString();
            KoOdfForm::Control::Entry entry;
            entry.label = item->text();
            entry.value = value;
            entry.selected = m_entries->property("checkable").toBool() && item->checkState() == Qt::Checked;
            entry.element = i < oldEntries.size() ? oldEntries.at(i).element : QStringLiteral("option");
            entry.width = item->data(GridWidthRole).toString();
            entry.type = item->data(GridTypeRole).toString();
            entry.binding = item->data(GridBindingRole).toString();
            entries.append(entry);
        }
        properties.setEntries(entries);
    }
    QPointer<KoFormTool> tool(this);
    canvas()->addCommand(std::make_unique<ChangeFormPropertiesCommand>(m_shape, properties, currentFormEvents, [tool]() {
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
        const QRectF oldBounds = m_shape ? m_shape->boundingRect() : QRectF();
        m_shape = shape;
        updateProperties();
        if (!oldBounds.isEmpty()) {
            canvas()->updateCanvas(oldBounds);
        }
        canvas()->updateCanvas(m_shape->boundingRect());
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
    const QRectF oldBounds = m_shape ? m_shape->boundingRect() : QRectF();
    m_shape = shape;
    updateProperties();
    if (!oldBounds.isEmpty()) {
        canvas()->updateCanvas(oldBounds);
    }
    if (m_shape) {
        canvas()->updateCanvas(m_shape->boundingRect());
    }
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
