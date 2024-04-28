/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2001-2005, 2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002, 2004-2005 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2004-2005, 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2004, 2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * SPDX-FileCopyrightText: 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2006 Casper Boemann <cbr@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * SPDX-FileCopyrightText: 2012 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBON_DOCUMENT_H
#define KARBON_DOCUMENT_H

#include <QMap>
#include <QRectF>
#include <QString>

#include <KoShapeBasedDocumentBase.h>

#include <KoPADocument.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlReader.h>

#include <karbonui_export.h>

class QRect;
class KarbonPart;
class KarbonCanvas;
class KoDataCenterBase;
class KoShape;
class KoShapeSavingContext;
class KoShapeLayer;
class KoImageCollection;
class KoStore;

#define KARBON_MIME_TYPE "application/vnd.oasis.opendocument.graphics"

/**
 * Keeps track of visual per document properties.
 * It loads initial settings and applies them to the document and its views.
 */
class KARBONUI_EXPORT KarbonDocument : public KoPADocument
{
    Q_OBJECT
public:
    explicit KarbonDocument(KarbonPart *part);
    ~KarbonDocument() override;

    KoOdf::DocumentType documentType() const override;

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override
    {
        return KARBON_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override
    {
        return KARBON_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override
    {
        return QStringList() << "application/vnd.oasis.opendocument.graphics-template";
    }

    /// Returns if status bar is shown
    bool showStatusBar() const;
    /// Shows/hides status bar
    void setShowStatusBar(bool b);
    /// update attached view(s) on the current doc settings
    /// at this time only the status bar is handled
    void reorganizeGUI();

    /// Returns maximum number of recent files
    uint maxRecentFiles() const;

    bool mergeNativeFormat(const QString &file);

public Q_SLOTS:
    void slotDocumentRestored();

Q_SIGNALS:
    void applyCanvasConfiguration(KarbonCanvas *canvas);

protected:
    const char *odfTagName(bool withNamespace) override;
    /// Reads settings from config file
    void initConfig();

private:
    class Private;
    Private *const d;
};

#endif // KARBON_DOCUMENT_H
