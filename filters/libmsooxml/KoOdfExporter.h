/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009 Jarosław Staniek <staniek@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOODFEXPORTER_H
#define KOODFEXPORTER_H

#include "komsooxml_export.h"
#include <KoFilter.h>

class KoXmlWriter;
class KoStore;
class KoGenStyles;

/**
 * @brief Convenience structure encapsulating XML writers used when writing ODF document.
 */
struct KOMSOOXML_EXPORT KoOdfWriters {
    /**
     * Creates structure encapsulating XML writers. All members are set initially to 0.
     */
    KoOdfWriters();
    KoXmlWriter *content;
    KoXmlWriter *body;
    KoXmlWriter *meta;
    KoXmlWriter *manifest;
    KoGenStyles *mainStyles;
};

/**
 * @brief The base class for filters exporting to ODF.
 *
 * @todo Move to libs, e.g. komain
 *
 * @author Jarosław Staniek <staniek@kde.org>
 */
class KOMSOOXML_EXPORT KoOdfExporter : public KoFilter
{
    Q_OBJECT
public:
    ~KoOdfExporter() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

protected:
    /**
     * This is the constructor your filter has to call, obviously.
     * @param bodyContentElement element name for the content:
     *                           "text" for ODT format, "presentation" for ODP,
     *                           "spreadsheet" for ODS, "drawing" for ODG.
     *                           office:text element will be created within office:body, etc.
     * @param parent parent object.
     */
    KoOdfExporter(const QString &bodyContentElement, QObject *parent = nullptr);

    /**
     * @return true if @a mime is accepted source mime type.
     * Implement it for your filter.
     */
    virtual bool acceptsSourceMimeType(const QByteArray &mime) const = 0;

    /**
     * @return true if @a mime is accepted destination mime type.
     * Implement it for your filter.
     */
    virtual bool acceptsDestinationMimeType(const QByteArray &mime) const = 0;

    /**
     * This method is called in convert() after creating @a outputStore, @a writers and @a mainStyles.
     * Implement it for your filter with code that fills the ODF structures with converted data.
     */
    virtual KoFilter::ConversionStatus createDocument(KoStore *outputStore, KoOdfWriters *writers) = 0;

    /**
     * This method is called when writing the xml-settings to the ODF document.
     */
    virtual void writeConfigurationSettings(KoXmlWriter *settings) const = 0;

private:
    class Private;
    Private *d;
};

#endif /* KOODFEXPORTER_H */
