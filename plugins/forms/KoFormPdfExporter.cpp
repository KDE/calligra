/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormPdfExporter.h"

#ifdef CALLIGRA_WITH_QPDF
#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFWriter.hh>
#endif

#include <QApplication>
#include <QFile>
#include <QStyle>
#include <QStyleOptionButton>
#include <QTemporaryFile>

#include <exception>
#include <string>

using namespace Qt::StringLiterals;

#ifdef CALLIGRA_WITH_QPDF
namespace
{
std::string pdfString(const QString &value)
{
    std::string result = "(";
    const QByteArray utf8 = value.toUtf8();
    for (const char character : utf8) {
        if (character == '(' || character == ')' || character == '\\') {
            result += '\\';
        }
        result += character;
    }
    result += ')';
    return result;
}

QPDFObjectHandle makeAppearance(QPDF &pdf, const FormPdfField &field, const QRectF &rect, bool checked = false)
{
    const double width = qMax(1.0, rect.width());
    const double height = qMax(1.0, rect.height());
    std::string content = "q 0 0 0 RG 0.7 w 0.5 0.5 " + std::to_string(width - 1) + " " + std::to_string(height - 1) + " re S Q\n";
    if (field.kind == KoOdfForm::ControlKind::Checkbox && checked) {
        content += "q 0 0 0 RG 1 w 2 2 m " + std::to_string(width / 2) + " " + std::to_string(height / 2) + " l " + std::to_string(width - 2) + " "
            + std::to_string(height - 2) + " l S Q\n";
    } else if (field.kind != KoOdfForm::ControlKind::Checkbox && field.kind != KoOdfForm::ControlKind::Radio) {
        const QString text = field.currentValue.isEmpty() ? field.label : field.currentValue;
        if (!text.isEmpty()) {
            content += "BT /Helv 10 Tf 2 " + std::to_string(qMax(10.0, height - 4)) + " Td " + pdfString(text) + " Tj ET\n";
        }
    }
    auto appearance = pdf.newStream(content);
    auto appearanceDictionary = appearance.getDict();
    appearanceDictionary.replaceKey("/Type", QPDFObjectHandle::newName("/XObject"));
    appearanceDictionary.replaceKey("/Subtype", QPDFObjectHandle::newName("/Form"));
    appearanceDictionary.replaceKey("/FormType", QPDFObjectHandle::newInteger(1));
    appearanceDictionary.replaceKey("/BBox",
                                    QPDFObjectHandle::newArray({QPDFObjectHandle::newReal("0"),
                                                                QPDFObjectHandle::newReal("0"),
                                                                QPDFObjectHandle::newReal(QString::number(width).toStdString()),
                                                                QPDFObjectHandle::newReal(QString::number(height).toStdString())}));
    auto resources = QPDFObjectHandle::newDictionary();
    auto fonts = QPDFObjectHandle::newDictionary();
    auto helvetica = QPDFObjectHandle::newDictionary();
    helvetica.replaceKey("/Type", QPDFObjectHandle::newName("/Font"));
    helvetica.replaceKey("/Subtype", QPDFObjectHandle::newName("/Type1"));
    helvetica.replaceKey("/BaseFont", QPDFObjectHandle::newName("/Helvetica"));
    fonts.replaceKey("/Helv", helvetica);
    resources.replaceKey("/Font", fonts);
    appearanceDictionary.replaceKey("/Resources", resources);
    return pdf.makeIndirectObject(appearance);
}
}
#endif

namespace
{
void setError(QString *errorMessage, const QString &message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
}

}

bool exportFormFieldsToPdf(const QString &fileName, const QList<FormPdfField> &fields, QString *errorMessage)
{
    if (fields.isEmpty()) {
        return true;
    }

#ifndef CALLIGRA_WITH_QPDF
    Q_UNUSED(fileName)
    Q_UNUSED(fields)
    setError(errorMessage, QStringLiteral("QPDF support is not available in this build"));
    return false;
#else
    const QString temporaryName = fileName + QStringLiteral(".forms");
    QFile::remove(temporaryName);

    try {
        QPDF pdf;
        const QByteArray inputFileName = fileName.toLocal8Bit();
        pdf.processFile(inputFileName.constData());
        QPDFPageDocumentHelper pages(pdf);
        auto pageObjects = pages.getAllPages();
        auto catalog = pdf.getRoot();
        auto acroForm = catalog.getKey("/AcroForm");
        if (acroForm.isNull()) {
            acroForm = pdf.makeIndirectObject(QPDFObjectHandle::newDictionary());
            catalog.replaceKey("/AcroForm", acroForm);
        }
        auto formFields = acroForm.getKey("/Fields");
        if (formFields.isNull()) {
            formFields = QPDFObjectHandle::newArray();
            acroForm.replaceKey("/Fields", formFields);
        }
        acroForm.replaceKey("/NeedAppearances", QPDFObjectHandle::newBool(false));

        for (const FormPdfField &field : fields) {
            if (!field.printable || field.page < 0 || field.page >= static_cast<int>(pageObjects.size()) || field.rect.isEmpty()) {
                continue;
            }

            auto page = pageObjects.at(field.page);
            const auto mediaBox = page.getMediaBox().getArrayAsRectangle();
            // ODF shape coordinates use a top-left origin, while PDF rectangles use a
            // bottom-left origin.
            QRectF annotationRect = field.rect;
            if (field.kind == KoOdfForm::ControlKind::Checkbox || field.kind == KoOdfForm::ControlKind::Radio) {
                constexpr qreal stylePixelsPerPoint = 96.0 / 72.0;
                const QSize widgetSize(qMax(1, qRound(field.rect.width() * stylePixelsPerPoint)), qMax(1, qRound(field.rect.height() * stylePixelsPerPoint)));
                QStyleOptionButton option;
                option.rect = QRect(QPoint(), widgetSize);
                option.state = QStyle::State_Enabled;
                const auto element = field.kind == KoOdfForm::ControlKind::Checkbox ? QStyle::SE_CheckBoxIndicator : QStyle::SE_RadioButtonIndicator;
                const QRect indicator = QApplication::style()->subElementRect(element, &option);
                annotationRect = QRectF(field.rect.left() + indicator.left() / stylePixelsPerPoint,
                                        field.rect.top() + indicator.top() / stylePixelsPerPoint,
                                        indicator.width() / stylePixelsPerPoint,
                                        indicator.height() / stylePixelsPerPoint);
            }
            const double left = mediaBox.llx + annotationRect.left();
            const double right = mediaBox.llx + annotationRect.right();
            const double top = mediaBox.ury - annotationRect.top();
            const double bottom = mediaBox.ury - annotationRect.bottom();

            auto annotation = QPDFObjectHandle::newDictionary();
            annotation.replaceKey("/Type", QPDFObjectHandle::newName("/Annot"));
            annotation.replaceKey("/Subtype", QPDFObjectHandle::newName("/Widget"));
            annotation.replaceKey("/Rect",
                                  QPDFObjectHandle::newArray({QPDFObjectHandle::newReal(QString::number(left).toStdString()),
                                                              QPDFObjectHandle::newReal(QString::number(bottom).toStdString()),
                                                              QPDFObjectHandle::newReal(QString::number(right).toStdString()),
                                                              QPDFObjectHandle::newReal(QString::number(top).toStdString())}));
            annotation.replaceKey("/F", QPDFObjectHandle::newInteger(4));

            auto formField = pdf.makeIndirectObject(annotation);
            formField.replaceKey("/T", QPDFObjectHandle::newUnicodeString(field.name.toUtf8().toStdString()));
            if (!field.label.isEmpty()) {
                formField.replaceKey("/TU", QPDFObjectHandle::newUnicodeString(field.label.toUtf8().toStdString()));
            }
            if (!field.dataField.isEmpty()) {
                formField.replaceKey("/TM", QPDFObjectHandle::newUnicodeString(field.dataField.toUtf8().toStdString()));
            }
            formField.replaceKey("/DA", QPDFObjectHandle::newString("/Helv 10 Tf 0 g"));
            const int fieldFlags = (field.readOnly ? 1 : 0) | (field.required ? 2 : 0);
            if (fieldFlags != 0) {
                formField.replaceKey("/Ff", QPDFObjectHandle::newInteger(fieldFlags));
            }

            switch (field.kind) {
            case KoOdfForm::ControlKind::Checkbox:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Btn"));
                formField.replaceKey("/V", QPDFObjectHandle::newName(field.currentValue.isEmpty() ? "/Off" : "/Yes"));
                formField.replaceKey("/AS", QPDFObjectHandle::newName(field.currentValue.isEmpty() ? "/Off" : "/Yes"));
                break;
            case KoOdfForm::ControlKind::Radio:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Btn"));
                formField.replaceKey("/Ff", QPDFObjectHandle::newInteger(fieldFlags | (1 << 15)));
                formField.replaceKey("/V", QPDFObjectHandle::newName(field.currentValue.isEmpty() ? "/Off" : "/Yes"));
                formField.replaceKey("/AS", QPDFObjectHandle::newName(field.currentValue.isEmpty() ? "/Off" : "/Yes"));
                break;
            case KoOdfForm::ControlKind::Button:
            case KoOdfForm::ControlKind::Image:
            case KoOdfForm::ControlKind::ImageFrame:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Btn"));
                formField.replaceKey("/Ff", QPDFObjectHandle::newInteger(fieldFlags | (1 << 16)));
                break;
            case KoOdfForm::ControlKind::Combobox:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Ch"));
                formField.replaceKey("/Ff", QPDFObjectHandle::newInteger(fieldFlags | (1 << 17)));
                [[fallthrough]];
            case KoOdfForm::ControlKind::Listbox: {
                if (field.kind == KoOdfForm::ControlKind::Listbox) {
                    formField.replaceKey("/FT", QPDFObjectHandle::newName("/Ch"));
                }
                std::vector<QPDFObjectHandle> entries;
                entries.reserve(field.entries.size());
                for (const auto &entry : field.entries) {
                    entries.push_back(QPDFObjectHandle::newUnicodeString(entry.label.toUtf8().toStdString()));
                }
                formField.replaceKey("/Opt", QPDFObjectHandle::newArray(entries));
                formField.replaceKey("/V", QPDFObjectHandle::newUnicodeString(field.currentValue.toUtf8().toStdString()));
                break;
            }
            case KoOdfForm::ControlKind::Grid:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Ch"));
                break;
            case KoOdfForm::ControlKind::File:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Tx"));
                formField.replaceKey("/Ff", QPDFObjectHandle::newInteger(fieldFlags | (1 << 21)));
                formField.replaceKey("/V", QPDFObjectHandle::newUnicodeString(field.currentValue.toUtf8().toStdString()));
                break;
            case KoOdfForm::ControlKind::ValueRange:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Tx"));
                formField.replaceKey("/V", QPDFObjectHandle::newUnicodeString(field.currentValue.toUtf8().toStdString()));
                break;
            case KoOdfForm::ControlKind::Text:
            case KoOdfForm::ControlKind::Textarea:
            case KoOdfForm::ControlKind::FormattedText:
            case KoOdfForm::ControlKind::Password:
            case KoOdfForm::ControlKind::Number:
            case KoOdfForm::ControlKind::Date:
            case KoOdfForm::ControlKind::Time:
            case KoOdfForm::ControlKind::GenericControl:
                formField.replaceKey("/FT", QPDFObjectHandle::newName("/Tx"));
                formField.replaceKey("/V", QPDFObjectHandle::newUnicodeString(field.currentValue.toUtf8().toStdString()));
                break;
            default:
                continue;
            }

            auto appearance = QPDFObjectHandle::newDictionary();
            if (field.kind == KoOdfForm::ControlKind::Checkbox || field.kind == KoOdfForm::ControlKind::Radio) {
                auto states = QPDFObjectHandle::newDictionary();
                states.replaceKey("/Off", makeAppearance(pdf, field, annotationRect));
                states.replaceKey("/Yes", makeAppearance(pdf, field, annotationRect, !field.currentValue.isEmpty()));
                appearance.replaceKey("/N", states);
            } else {
                appearance.replaceKey("/N", makeAppearance(pdf, field, annotationRect));
            }
            formField.replaceKey("/AP", appearance);

            auto pageObject = page.getObjectHandle();
            auto annotations = pageObject.getKey("/Annots");
            if (annotations.isNull()) {
                annotations = QPDFObjectHandle::newArray();
                pageObject.replaceKey("/Annots", annotations);
            }
            annotations.appendItem(formField);
            formFields.appendItem(formField);
        }

        const QByteArray temporaryFileName = temporaryName.toLocal8Bit();
        QPDFWriter writer(pdf, temporaryFileName.constData());
        writer.write();
    } catch (const std::exception &exception) {
        QFile::remove(temporaryName);
        setError(errorMessage, QString::fromUtf8(exception.what()));
        return false;
    } catch (...) {
        QFile::remove(temporaryName);
        setError(errorMessage, QStringLiteral("QPDF failed while adding form fields"));
        return false;
    }

    if (!QFile::remove(fileName) || !QFile::rename(temporaryName, fileName)) {
        QFile::remove(temporaryName);
        setError(errorMessage, QStringLiteral("Could not replace the rendered PDF with its form-enabled version"));
        return false;
    }
    return true;
#endif
}
