/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002,2004-2005 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2004-2005,2007 David Faure <faure@kde.org>
 * Copyright (C) 2004,2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2012 Yue Liu <yue.liu@mail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBON_DOCUMENT_H
#define KARBON_DOCUMENT_H

#include <QString>
#include <QMap>
#include <QRectF>

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
    QByteArray nativeFormatMimeType() const override { return KARBON_MIME_TYPE; }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override { return KARBON_MIME_TYPE; }
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

    bool mergeNativeFormat(const QString & file);

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
    Private * const d;
};

#endif // KARBON_DOCUMENT_H

