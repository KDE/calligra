/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormPdfExporter.h"

#include <KArchiveFile>
#include <KZip>

#include <KoPageLayout.h>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFAcroFormDocumentHelper.hh>

#include <QApplication>
#include <QFile>
#include <QHash>
#include <QPainter>
#include <QPdfWriter>
#include <QStyle>
#include <QStyleOptionButton>
#include <QTemporaryDir>
#include <QTest>
#include <QXmlStreamReader>

#include <array>

using namespace Qt::StringLiterals;

class TestFormPdfExport : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void exportFixture()
    {
        const QString fixture = QFINDTESTDATA("data/forms.odt");
        QVERIFY2(!fixture.isEmpty(), "The form ODF fixture is missing");
        KZip zip(fixture);
        QVERIFY(zip.open(QIODevice::ReadOnly));
        const auto *content = dynamic_cast<const KArchiveFile *>(zip.directory()->entry(u"content.xml"_s));
        QVERIFY(content);
        QVERIFY(content->data().contains("form:"));

        const auto points = [](const QString &value) {
            if (value.endsWith(u"in"_s)) {
                return value.left(value.size() - 2).toDouble() * 72.0;
            }
            if (value.endsWith(u"cm"_s)) {
                return value.left(value.size() - 2).toDouble() * 72.0 / 2.54;
            }
            return value.toDouble();
        };

        QTemporaryDir temporaryDir;
        QVERIFY(temporaryDir.isValid());
        const QString requestedOutput = qEnvironmentVariable("CALLIGRA_FORM_PDF_OUTPUT");
        const QString pdfFile = requestedOutput.isEmpty() ? temporaryDir.filePath(u"forms.pdf"_s) : requestedOutput;
        if (!requestedOutput.isEmpty()) {
            QFile::remove(pdfFile);
        }
        QList<FormPdfField> fields;
        QHash<QString, KoOdfForm::ControlKind> controls;
        QHash<QString, QString> labels;
        QXmlStreamReader xml(content->data());
        while (!xml.atEnd()) {
            xml.readNext();
            if (!xml.isStartElement()) {
                continue;
            }
            const auto attributes = xml.attributes();
            const QString qualifiedName = xml.qualifiedName().toString();
            if (qualifiedName.startsWith(u"form:"_s) && qualifiedName != u"form:form"_s && qualifiedName != u"form:properties"_s
                && qualifiedName != u"form:property"_s && qualifiedName != u"form:list-property"_s) {
                const QString id =
                    attributes.value(u"form:id"_s).toString().isEmpty() ? attributes.value(u"xml:id"_s).toString() : attributes.value(u"form:id"_s).toString();
                if (!id.isEmpty()) {
                    controls.insert(id, KoOdfForm::controlKindFromString(qualifiedName.mid(5)));
                    labels.insert(id, attributes.value(u"form:label"_s).toString());
                }
            } else if (qualifiedName == u"draw:control"_s) {
                const QString id = attributes.value(u"draw:control"_s).toString();
                const auto kind = controls.value(id, KoOdfForm::ControlKind::Unknown);
                if (kind == KoOdfForm::ControlKind::Unknown) {
                    continue;
                }
                FormPdfField field;
                field.id = id;
                field.name = KoOdfForm::controlKindName(kind);
                field.kind = kind;
                field.label = labels.value(id);
                field.rect = QRectF(points(attributes.value(u"svg:x"_s).toString()),
                                    points(attributes.value(u"svg:y"_s).toString()),
                                    points(attributes.value(u"svg:width"_s).toString()),
                                    points(attributes.value(u"svg:height"_s).toString()));
                field.currentValue = kind == KoOdfForm::ControlKind::Checkbox || kind == KoOdfForm::ControlKind::Radio ? u"Yes"_s : u"From ODF"_s;
                fields.append(field);
            }
        }
        QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
        QVERIFY(fields.size() > 2);

        {
            QPdfWriter writer(pdfFile);
            writer.setPageSize(QPageSize(QPageSize::Letter));
            QPainter painter(&writer);
            painter.drawText(QPointF(36, 24), u"Form export"_s);
            const auto style = QApplication::style();
            for (const auto &field : fields) {
                const QRectF rect = field.rect.translated(field.pageOffset);
                const QRect widgetRect = rect.toRect();
                if (field.kind == KoOdfForm::ControlKind::Checkbox || field.kind == KoOdfForm::ControlKind::Radio
                    || field.kind == KoOdfForm::ControlKind::Button) {
                    QStyleOptionButton option;
                    option.rect = widgetRect;
                    option.text = field.label;
                    option.state = QStyle::State_Enabled;
                    if (field.kind == KoOdfForm::ControlKind::Checkbox) {
                        option.state |= QStyle::State_On;
                    }
                    style->drawControl(field.kind == KoOdfForm::ControlKind::Checkbox    ? QStyle::CE_CheckBox
                                           : field.kind == KoOdfForm::ControlKind::Radio ? QStyle::CE_RadioButton
                                                                                         : QStyle::CE_PushButton,
                                       &option,
                                       &painter);
                } else {
                    painter.drawRect(widgetRect);
                    painter.drawText(widgetRect.adjusted(4, 0, -4, 0), Qt::AlignVCenter | Qt::AlignLeft, field.currentValue);
                }
            }
            painter.end();
        }

        QString error;
        QVERIFY2(exportFormFieldsToPdf(pdfFile, fields, &error), qPrintable(error));

        QPDF pdf;
        pdf.processFile(pdfFile.toLocal8Bit().constData());
        QPDFAcroFormDocumentHelper acroForm(pdf);
        QVERIFY(static_cast<int>(acroForm.getFormFields().size()) >= 12);
    }
};

QTEST_MAIN(TestFormPdfExport)
#include "TestFormPdfExport.moc"
