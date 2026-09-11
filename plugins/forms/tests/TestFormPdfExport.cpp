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
#include <qpdf/QPDFWriter.hh>

#include <QApplication>
#include <QFile>
#include <QHash>
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
                field.printable = true;
                field.currentValue = kind == KoOdfForm::ControlKind::Checkbox || kind == KoOdfForm::ControlKind::Radio ? u"Yes"_s : u"From ODF"_s;
                fields.append(field);
            }
        }
        QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
        QVERIFY(fields.size() > 2);

        QPDF basePdf;
        basePdf.emptyPDF();
        auto page = QPDFObjectHandle::newDictionary();
        page.replaceKey("/Type", QPDFObjectHandle::newName("/Page"));
        page.replaceKey(
            "/MediaBox",
            QPDFObjectHandle::newArray(
                {QPDFObjectHandle::newInteger(0), QPDFObjectHandle::newInteger(0), QPDFObjectHandle::newInteger(612), QPDFObjectHandle::newInteger(792)}));
        basePdf.addPage(page, false);
        QPDFWriter baseWriter(basePdf, pdfFile.toLocal8Bit().constData());
        baseWriter.write();

        QString error;
        QVERIFY2(exportFormFieldsToPdf(pdfFile, fields, &error), qPrintable(error));

        QPDF pdf;
        pdf.processFile(pdfFile.toLocal8Bit().constData());
        QPDFAcroFormDocumentHelper acroForm(pdf);
        QCOMPARE(static_cast<int>(acroForm.getFormFields().size()), fields.size());
    }
};

QTEST_MAIN(TestFormPdfExport)
#include "TestFormPdfExport.moc"
