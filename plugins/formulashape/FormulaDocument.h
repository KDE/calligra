/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2010 Inge Wallin <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULA_FORMULADOCUMENT_H
#define KFORMULA_FORMULADOCUMENT_H

// Calligra
#include <KoDocument.h>
#include <KoXmlReaderForward.h>

#include <QFont>

class KoView;
class KoOdfReadStore;
class QPainter;
class QWidget;
class QDomElement;

class KoXmlWriter;
class KoFormulaShape;
class KoOdfLoadingContext;
class KoShapeLoadingContext;

#define FORMULA_MIME_TYPE "application/vnd.oasis.opendocument.formula"

class FormulaDocument : public KoDocument
{
public:
    explicit FormulaDocument(KoFormulaShape *parent);
    ~FormulaDocument();

    /// reimplemented from KoDocument
    virtual QByteArray nativeFormatMimeType() const { return FORMULA_MIME_TYPE; }
    /// reimplemented from KoDocument
    virtual QByteArray nativeOasisMimeType() const {return FORMULA_MIME_TYPE; }
    /// reimplemented from KoDocument
    virtual QStringList extraNativeMimeTypes() const
    {
        return QStringList() << "application/x-kformula"
                             << "application/vnd.oasis.opendocument.formula-template"
                             << "text/mathml";

    }

    bool loadOdf( KoOdfReadStore &odfStore );
    bool loadXML( const KoXmlDocument &doc, KoStore *store );
    bool loadEmbeddedDocument(KoStore *store,const KoXmlElement &objectElement,
                              const KoOdfLoadingContext &odfLoadingContext);
    bool loadOdfEmbedded(const KoXmlElement &mathElement, KoShapeLoadingContext &context);

    void processMathML(QDomElement &element, KoXmlWriter &writer);

    bool saveOdf( SavingContext &context );

    KoView *createViewInstance( QWidget *parent );

    QString content() const;
    void setContent(QString mathML);

    const QFont& font() const;
    void setFont(QFont font);

    const QColor& backgroundColor() const;
    void setBackgroundColor(QColor color);

    const QColor& foregroundColor() const;
    void setForegroundColor(QColor color);

    void paintContent( QPainter &painter, const QRect &rect );

private:
    class Private;
    Private * const d;
};

#endif // KFORMULA_FORMULADOCUMENT_H

