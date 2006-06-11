/*
   This file is part of the KDE project
   Copyright (C) 2004 Tobias Koenig <tokoe@kde.org>
 
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

#ifndef _KWMAILMERGE_KSPREAD_H_
#define _KWMAILMERGE_KSPREAD_H_

#include <qdom.h>
#include <QMap>

#include <kspread_doc.h>
#include <kspread_sheet.h>

#include "KWMailMergeDataSource.h"

class KWMailMergeKSpread: public KWMailMergeDataSource
{
  Q_OBJECT

  public:
    KWMailMergeKSpread( KInstance *inst, QObject *parent );
    ~KWMailMergeKSpread();

    /**
       Saves the mail merge list to the kword document.
     */
    virtual void save( QDomDocument&, QDomElement& );

    /**
       Loads the mail merge list stored in the kword document.
     */
    virtual void load( QDomElement& );

    /**
       @param name	The name of the value e.g. "Family name".
       @param record	The position of the the entry in mail merge list.
       @return		The value of the mail merge variable.
       
       If @p record equals -1, @p name is returned.
     */
    virtual class QString getValue( const QString &name, int record = -1 ) const;

    /**
       @return	The number of available contacts in mail merge list.
     */
    virtual int getNumRecords() const;

    /**
       Only for compatability reasons.
       
       @param force	Hasn't any effect.
     */
    virtual void refresh( bool force );

    /**
       Shows a KWMailMergeKSpreadConfig dialog for selecting entries from KAddressbook.
     */
    virtual bool showConfigDialog( QWidget*, int action);


    void setURL( const KUrl &url ) { _url = url; }
    KUrl url() const { return _url; }

    void setSpreadSheetNumber( int number ) { _spreadSheetNumber = number; }
    int spreadSheetNumber() const { return _spreadSheetNumber; }

  protected:
    friend class KWMailMergeKSpreadConfig;

  private slots:
    void initSpreadSheets();

  private:
    void initDocument();

    int rows() const;
    int columns() const;

    QString cellText( const KSpread::Cell* ) const;

    KSpread::Doc *_document;
    KSpread::Sheet* _sheet;
    KUrl _url;
    int _spreadSheetNumber;

    QMap<QString, int> _columnMap;
};

#endif

