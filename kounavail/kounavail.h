/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
    KoUnavailPart( QWidget *parentWidget = 0, QObject* parent = 0 );

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool initDoc(InitDocFlags, QWidget* = 0) { return true; }
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* );
    virtual bool saveOasis(KoStore*, KoXmlWriter*);

    virtual bool loadXML( QIODevice *, const QDomDocument & );
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
    void setRealURL( const QString& u ) { m_url = u; }
    // stupid moc again
    QString realURL() const { return m_url.url(); }

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
    KoUnavailView( KoUnavailPart* part, QWidget* parent = 0 );

protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void updateReadWrite( bool ) {}
};

#include <KoFactory.h>

class KInstance;
class KAboutData;

class KoUnavailFactory : public KoFactory
{
    Q_OBJECT
public:
    KoUnavailFactory( QObject* parent = 0, const char* name = 0 );
    ~KoUnavailFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
