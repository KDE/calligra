/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KFORMULA_FORMULADOCUMENT_H
#define KFORMULA_FORMULADOCUMENT_H

// Calligra
#include <KoDocument.h>
#include <KoXmlReaderForward.h>

class KoView;
class KoOdfReadStore;
class QPainter;
class QWidget;

class KoFormulaShape;

#define FORMULA_MIME_TYPE "application/vnd.oasis.opendocument.formula"

class FormulaDocument : public KoDocument
{
public:
    explicit FormulaDocument(KoFormulaShape *parent);
    ~FormulaDocument() override;

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override
    {
        return FORMULA_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override
    {
        return FORMULA_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override
    {
        return QStringList() << "application/x-kformula"
                             << "application/vnd.oasis.opendocument.formula-template"
                             << "text/mathml";
    }

    bool loadOdf(KoOdfReadStore &odfStore) override;
    bool loadXML(const KoXmlDocument &doc, KoStore *store) override;

    bool saveOdf(SavingContext &context) override;
    KoView *createViewInstance(QWidget *parent);

    void paintContent(QPainter &painter, const QRect &rect) override;

private:
    class Private;
    Private *const d;
};

#endif // KFORMULA_FORMULADOCUMENT_H
