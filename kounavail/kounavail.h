/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KOUNAVAIL_H
#define KOUNAVAIL_H

#include <KoDocument.h>
#include <QPaintEvent>

class KoUnavailPart : public KoDocument
{
    Q_OBJECT
    Q_PROPERTY( QByteArray mimetype READ nativeFormatMimeType WRITE setMimeType )
    Q_PROPERTY( QString unavailReason READ unavailReason WRITE setUnavailReason )
    Q_PROPERTY( QString realURL READ realURL WRITE setRealURL )
public:
    explicit KoUnavailPart( QWidget *parentWidget = 0, QObject* parent = 0 );

    virtual void paintContent( QPainter& painter, const QRect& rect);

    virtual bool loadOdf( KoOdfReadStore & odfStore );
    virtual bool saveOdf( SavingContext & documentContext );

    virtual bool loadXML( const KoXmlDocument &doc, KoStore *store );
    virtual bool saveFile();
    virtual QDomDocument saveXML();
    virtual bool saveChildren( KoStore* /*_store*/ ) { return true; }

    /** This is called by KoDocumentChild::save */
    virtual QByteArray nativeFormatMimeType() const { return m_mimetype; }
    /** This is called by KoDocumentChild::createUnavailDocument */
    void setMimeType( const QByteArray& mime );
    // keep in sync with koDocumentChild.h
    enum UnavailReason { DocumentNotFound, HandlerNotFound };
    /** This is called by KoDocumentChild::createUnavailDocument */
    void setUnavailReason( const QString& reason ) { m_reason = reason; }
    // stupid moc - I want a write-only property !
    QString unavailReason() const { return m_reason; }
    /** This is called by KoDocumentChild::createUnavailDocument
     * Note the trick: we directly modify the URL of the document,
     * the one returned by KPart's url()
     */
    void setRealURL( const QString& u ) { ReadWritePart::setUrl(u); }
    // stupid moc again
    QString realURL() const { return url().url(); }

protected:
    virtual KoView* createViewInstance( QWidget* parent);

    QDomDocument m_doc;
    QByteArray m_mimetype;
    QString m_reason;
};

#include <KoView.h>

class KoUnavailView : public KoView
{
    Q_OBJECT
public:
    explicit KoUnavailView( KoUnavailPart* part, QWidget* parent = 0 );
    virtual KoZoomController *zoomController() const { return 0; }

protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void updateReadWrite( bool ) {}
};

#include <KoFactory.h>

class KComponentData;
class KAboutData;

class KoUnavailFactory : public KoFactory
{
    Q_OBJECT
public:
    explicit KoUnavailFactory( QObject* parent = 0, const char* name = 0 );
    ~KoUnavailFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static const KComponentData &global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KComponentData* s_global;
    static KAboutData* s_aboutData;
};

#endif
