/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormShapeRenderer.h"

#include <KLocalizedString>
#include <KoShape.h>
#include <KoViewConverter.h>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

using namespace Qt::StringLiterals;

namespace
{
// QStyle metrics are in widget pixels. Paint at 96 dpi, then transform the entire
// control to document coordinates so its text, indicators and borders zoom together.
constexpr qreal stylePixelsPerPoint = 96.0 / 72.0;

QString displayValue(const KoOdfForm::Control &control)
{
    return control.currentValue().isNull() ? control.value() : control.currentValue();
}

void drawField(QPainter &painter, QStyle *style, const QStyleOption &base, const QString &text, bool multiline = false)
{
    QStyleOptionFrame frame;
    static_cast<QStyleOption &>(frame) = base;
    // Breeze reports zero without a QWidget. These document controls still
    // need a frame, even though they are painted directly on the canvas.
    frame.lineWidth = qMax(1, style->pixelMetric(QStyle::PM_DefaultFrameWidth, &frame));
    frame.state |= QStyle::State_Sunken;
    painter.fillRect(frame.rect, frame.palette.brush(QPalette::Base));
    style->drawPrimitive(QStyle::PE_PanelLineEdit, &frame, &painter);
    const QRect contents = style->subElementRect(QStyle::SE_LineEditContents, &frame).adjusted(2, 1, -2, -1);
    painter.save();
    painter.setClipRect(contents, Qt::IntersectClip);
    style->drawItemText(&painter,
                        contents,
                        multiline ? Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap : Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                        base.palette,
                        base.state.testFlag(QStyle::State_Enabled),
                        text,
                        QPalette::Text);
    painter.restore();
}

void drawList(QPainter &painter, QStyle *style, const QStyleOption &base, const QVector<KoOdfForm::Control::Entry> &entries)
{
    QStyleOptionFrame frame;
    static_cast<QStyleOption &>(frame) = base;
    // Breeze reports zero without a QWidget. These document controls still
    // need a frame, even though they are painted directly on the canvas.
    frame.lineWidth = qMax(1, style->pixelMetric(QStyle::PM_DefaultFrameWidth, &frame));
    frame.state |= QStyle::State_Sunken;
    painter.fillRect(base.rect, base.palette.brush(QPalette::Base));
    style->drawPrimitive(QStyle::PE_Frame, &frame, &painter);
    QRect contents = base.rect.adjusted(frame.lineWidth, frame.lineWidth, -frame.lineWidth, -frame.lineWidth);
    const int rowHeight = base.fontMetrics.height() + 4;
    const int visibleRows = qMax(1, contents.height() / rowHeight);
    if (entries.size() > visibleRows) {
        QStyleOptionSlider scroll;
        static_cast<QStyleOption &>(scroll) = base;
        const int extent = qMin(contents.width(), style->pixelMetric(QStyle::PM_ScrollBarExtent, &scroll));
        scroll.rect = QRect(contents.right() - extent + 1, contents.top(), extent, contents.height());
        scroll.orientation = Qt::Vertical;
        scroll.minimum = 0;
        scroll.maximum = entries.size() - visibleRows;
        scroll.pageStep = visibleRows;
        scroll.singleStep = 1;
        style->drawComplexControl(QStyle::CC_ScrollBar, &scroll, &painter);
        contents.setRight(scroll.rect.left() - 1);
    }
    painter.save();
    painter.setClipRect(contents, Qt::IntersectClip);
    for (int i = 0; i < entries.size() && i * rowHeight < contents.height(); ++i) {
        QStyleOptionViewItem item;
        static_cast<QStyleOption &>(item) = base;
        item.rect = QRect(contents.left(), contents.top() + i * rowHeight, contents.width(), rowHeight);
        item.font = painter.font();
        item.text = entries[i].label;
        item.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        item.features = QStyleOptionViewItem::HasDisplay;
        if (entries[i].selected)
            item.state |= QStyle::State_Selected;
        style->drawControl(QStyle::CE_ItemViewItem, &item, &painter);
    }
    painter.restore();
}
}

static void paintFormControlByName(QPainter &painter,
                                   const KoViewConverter &converter,
                                   const QSizeF &size,
                                   KoOdfForm::ControlKind kind,
                                   const KoOdfForm::Control *control,
                                   const QImage &image);

void paintFormControl(QPainter &painter,
                      const KoViewConverter &converter,
                      const QSizeF &size,
                      KoOdfForm::ControlKind kind,
                      const KoOdfForm::Control *control,
                      const QImage &image)
{
    paintFormControlByName(painter, converter, size, kind, control, image);
}

static void paintFormControlByName(QPainter &painter,
                                   const KoViewConverter &converter,
                                   const QSizeF &size,
                                   KoOdfForm::ControlKind kind,
                                   const KoOdfForm::Control *control,
                                   const QImage &image)
{
    if (kind == KoOdfForm::ControlKind::Hidden || size.isEmpty())
        return;
    const QSize widgetSize(qMax(1, qRound(size.width() * stylePixelsPerPoint)), qMax(1, qRound(size.height() * stylePixelsPerPoint)));
    painter.save();
    KoShape::applyConversion(painter, converter);
    painter.scale(size.width() / widgetSize.width(), size.height() / widgetSize.height());
    const QRect rect(QPoint(), widgetSize);
    painter.setClipRect(rect, Qt::IntersectClip);
    QFont font = QApplication::font();
    if (font.pointSizeF() > 0)
        font.setPixelSize(qMax(1, qRound(font.pointSizeF() * stylePixelsPerPoint)));
    painter.setFont(font);
    QStyle *style = QApplication::style();
    QStyleOption base;
    base.rect = rect;
    base.fontMetrics = QFontMetrics(font);
    base.palette = QApplication::palette();
    base.direction = QApplication::layoutDirection();
    base.state = QStyle::State_Active;
    if (!control || !control->disabled())
        base.state |= QStyle::State_Enabled;
    else
        base.palette.setCurrentColorGroup(QPalette::Disabled);
    if (control && control->readOnly())
        base.state |= QStyle::State_ReadOnly;
    const QString label = control ? control->label() : QString();
    QString text = control ? displayValue(*control) : QString();
    const auto attribute = [control](const QString &name) {
        return control ? control->formAttribute(name) : QString();
    };

    if (kind == KoOdfForm::ControlKind::Checkbox || kind == KoOdfForm::ControlKind::Radio) {
        QStyleOptionButton button;
        static_cast<QStyleOption &>(button) = base;
        button.text = label;
        QString state = attribute(u"current-state"_s);
        if (state.isNull())
            state = attribute(u"state"_s);
        QString selected = attribute(u"current-selected"_s);
        if (selected.isNull())
            selected = attribute(u"selected"_s);
        if (kind == KoOdfForm::ControlKind::Checkbox && state == "unknown"_L1)
            button.state |= QStyle::State_NoChange;
        else if ((kind == KoOdfForm::ControlKind::Checkbox && state == "checked"_L1) || (kind == KoOdfForm::ControlKind::Radio && selected == "true"_L1))
            button.state |= QStyle::State_On;
        else
            button.state |= QStyle::State_Off;
        style->drawControl(kind == KoOdfForm::ControlKind::Checkbox ? QStyle::CE_CheckBox : QStyle::CE_RadioButton, &button, &painter);
    } else if (kind == KoOdfForm::ControlKind::Button || kind == KoOdfForm::ControlKind::Image) {
        QStyleOptionButton button;
        static_cast<QStyleOption &>(button) = base;
        button.text = label;
        button.state |= QStyle::State_Raised;
        if (attribute(u"default-button"_s) == "true"_L1)
            button.features |= QStyleOptionButton::DefaultButton;
        if (attribute(u"toggle"_s) == "true"_L1 && attribute(u"current-state"_s) == "checked"_L1)
            button.state |= QStyle::State_On;
        if (!image.isNull()) {
            button.icon = QPixmap::fromImage(image);
            button.iconSize =
                image.size().scaled(kind == KoOdfForm::ControlKind::Image ? rect.adjusted(4, 4, -4, -4).size() : rect.size().boundedTo(QSize(32, 32)),
                                    Qt::KeepAspectRatio);
        } else if (kind == KoOdfForm::ControlKind::Image) {
            button.icon = style->standardIcon(QStyle::SP_FileIcon);
            button.iconSize = QSize(16, 16);
        }
        style->drawControl(QStyle::CE_PushButton, &button, &painter);
    } else if (kind == KoOdfForm::ControlKind::FixedText) {
        style->drawItemText(&painter,
                            rect,
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,
                            base.palette,
                            base.state.testFlag(QStyle::State_Enabled),
                            label,
                            QPalette::WindowText);
    } else if (kind == KoOdfForm::ControlKind::Frame) {
        QStyleOptionGroupBox group;
        static_cast<QStyleOption &>(group) = base;
        group.text = label;
        group.lineWidth = qMax(1, style->pixelMetric(QStyle::PM_DefaultFrameWidth, &group));
        group.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
        style->drawComplexControl(QStyle::CC_GroupBox, &group, &painter);
    } else if (kind == KoOdfForm::ControlKind::ImageFrame) {
        drawField(painter, style, base, {});
        const QRect contents = rect.adjusted(3, 3, -3, -3);
        if (!image.isNull() && !contents.isEmpty()) {
            const QSize imageSize = image.size().scaled(contents.size(), Qt::KeepAspectRatio);
            painter.drawImage(QRect(contents.center() - QPoint(imageSize.width() / 2, imageSize.height() / 2), imageSize), image);
        } else {
            style->standardIcon(QStyle::SP_FileIcon)
                .paint(&painter, contents, Qt::AlignCenter, base.state.testFlag(QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
        }
    } else if (kind == KoOdfForm::ControlKind::Grid) {
        drawField(painter, style, base, {});
        const auto columns = control->entries();
        const QRect contents = rect.adjusted(2, 2, -2, -2);
        const int headerHeight = base.fontMetrics.height() + 8;
        const int columnCount = qMax(1, int(columns.size()));
        for (int i = 0; i < columnCount; ++i) {
            QStyleOptionHeader header;
            static_cast<QStyleOption &>(header) = base;
            const int left = contents.left() + i * contents.width() / columnCount;
            const int right = contents.left() + (i + 1) * contents.width() / columnCount;
            header.rect = QRect(left, contents.top(), right - left, headerHeight);
            header.text = i < columns.size() ? columns[i].label : QString();
            header.section = i;
            header.orientation = Qt::Horizontal;
            style->drawControl(QStyle::CE_Header, &header, &painter);
            painter.setPen(base.palette.color(QPalette::Mid));
            painter.drawLine(left, contents.top() + headerHeight, left, contents.bottom());
        }
        painter.setPen(base.palette.color(QPalette::Mid));
        for (int y = contents.top() + headerHeight; y < contents.bottom(); y += headerHeight)
            painter.drawLine(contents.left(), y, contents.right(), y);
    } else if (kind == KoOdfForm::ControlKind::ValueRange) {
        QStyleOptionSlider slider;
        static_cast<QStyleOption &>(slider) = base;
        slider.orientation = attribute(u"orientation"_s) == "vertical"_L1 ? Qt::Vertical : Qt::Horizontal;
        if (slider.orientation == Qt::Horizontal)
            slider.state |= QStyle::State_Horizontal;
        slider.minimum = attribute(u"min-value"_s).toInt();
        slider.maximum = attribute(u"max-value"_s).isEmpty() ? 100 : attribute(u"max-value"_s).toInt();
        slider.maximum = qMax(slider.minimum, slider.maximum);
        slider.sliderValue = slider.sliderPosition = qBound(slider.minimum, text.toInt(), slider.maximum);
        slider.singleStep = qMax(1, attribute(u"step-size"_s).toInt());
        slider.pageStep = qMax(1, attribute(u"page-step-size"_s).toInt());
        const QString implementation = attribute(u"control-implementation"_s);
        if (implementation.endsWith("SpinButton"_L1)) {
            const bool horizontal = slider.orientation == Qt::Horizontal;
            for (int i = 0; i < 2; ++i) {
                QStyleOptionButton button;
                static_cast<QStyleOption &>(button) = base;
                button.rect = horizontal ? QRect(i * rect.width() / 2, 0, (i + 1) * rect.width() / 2 - i * rect.width() / 2, rect.height())
                                         : QRect(0, i * rect.height() / 2, rect.width(), (i + 1) * rect.height() / 2 - i * rect.height() / 2);
                button.state |= QStyle::State_Raised;
                style->drawControl(QStyle::CE_PushButtonBevel, &button, &painter);
                style->drawPrimitive(horizontal ? (i == 0 ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight)
                                                : (i == 0 ? QStyle::PE_IndicatorArrowUp : QStyle::PE_IndicatorArrowDown),
                                     &button,
                                     &painter);
            }
        } else {
            const bool isSlider = implementation.endsWith("Slider"_L1);
            slider.upsideDown = isSlider && slider.orientation == Qt::Vertical;
            style->drawComplexControl(isSlider ? QStyle::CC_Slider : QStyle::CC_ScrollBar, &slider, &painter);
        }
    } else if (kind == KoOdfForm::ControlKind::Combobox || kind == KoOdfForm::ControlKind::Listbox) {
        const auto entries = control->entries();
        if (kind == KoOdfForm::ControlKind::Listbox) {
            for (const auto &entry : entries) {
                if (entry.selected) {
                    text = entry.label;
                    break;
                }
            }
        }
        const bool dropdown = attribute(u"dropdown"_s) == "true"_L1;
        if (dropdown) {
            QStyleOptionComboBox combo;
            static_cast<QStyleOption &>(combo) = base;
            combo.currentText = text;
            combo.editable = kind == KoOdfForm::ControlKind::Combobox && !control->readOnly();
            style->drawComplexControl(QStyle::CC_ComboBox, &combo, &painter);
            if (combo.editable) {
                QStyleOption field = base;
                field.rect = style->subControlRect(QStyle::CC_ComboBox, &combo, QStyle::SC_ComboBoxEditField);
                style->drawItemText(&painter,
                                    field.rect,
                                    Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                                    base.palette,
                                    base.state.testFlag(QStyle::State_Enabled),
                                    text,
                                    QPalette::Text);
            } else {
                style->drawControl(QStyle::CE_ComboBoxLabel, &combo, &painter);
            }
        } else {
            QStyleOption list = base;
            if (kind == KoOdfForm::ControlKind::Combobox) {
                QStyleOption field = base;
                field.rect.setHeight(qMin(rect.height(), base.fontMetrics.height() + 8));
                drawField(painter, style, field, text);
                list.rect.setTop(field.rect.bottom() + 1);
            }
            if (!list.rect.isEmpty())
                drawList(painter, style, list, entries);
        }
    } else if (kind == KoOdfForm::ControlKind::File) {
        QStyleOptionButton browse;
        static_cast<QStyleOption &>(browse) = base;
        browse.text = i18n("Browse…");
        const int width = qMin(rect.width(), base.fontMetrics.horizontalAdvance(browse.text) + 20);
        browse.rect.setLeft(rect.right() - width + 1);
        style->drawControl(QStyle::CE_PushButton, &browse, &painter);
        QStyleOption field = base;
        field.rect.setRight(browse.rect.left() - 1);
        if (!field.rect.isEmpty())
            drawField(painter, style, field, text);
    } else {
        if (kind == KoOdfForm::ControlKind::Password) {
            const QString echo = attribute(u"echo-char"_s);
            const QChar mask = echo.isEmpty() ? QChar(style->styleHint(QStyle::SH_LineEdit_PasswordCharacter, &base)) : echo.front();
            text = QString(text.size(), mask);
        }
        if ((kind == KoOdfForm::ControlKind::Number || kind == KoOdfForm::ControlKind::Date || kind == KoOdfForm::ControlKind::Time
             || kind == KoOdfForm::ControlKind::FormattedText)
            && attribute(u"spin-button"_s) == "true"_L1) {
            QStyleOptionSpinBox spin;
            static_cast<QStyleOption &>(spin) = base;
            spin.buttonSymbols = QAbstractSpinBox::UpDownArrows;
            if (!control->readOnly())
                spin.stepEnabled = QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;
            style->drawComplexControl(QStyle::CC_SpinBox, &spin, &painter);
            const QRect contents = style->subControlRect(QStyle::CC_SpinBox, &spin, QStyle::SC_SpinBoxEditField);
            style->drawItemText(&painter,
                                contents.adjusted(2, 0, -2, 0),
                                Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                                base.palette,
                                base.state.testFlag(QStyle::State_Enabled),
                                text,
                                QPalette::Text);
        } else {
            drawField(painter, style, base, text, kind == KoOdfForm::ControlKind::Textarea);
        }
    }
    painter.restore();
}
