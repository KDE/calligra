/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   Modified by Joseph wenninger, 2001

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef mailmerge_h
#define mailmerge_h

#include <kdialogbase.h>
#include <ktrader.h>


#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qtextstream.h>

#include "mailmerge_interface.h"
#include "KWordMailMergeDatabaseIface.h"

class QListBox;
class QPushButton;
class QLineEdit;
class QSpinBox;
class KWDocument;
class QVBox;

//class KWMailMergeDataBase;




/******************************************************************
 *
 * Class: KWMailMergeDataBase
 *
 ******************************************************************/

class KWMailMergeDataBase: public QObject,KWordMailMergeDatabaseIface
{
Q_OBJECT

public:
    KWMailMergeDataBase( KWDocument *doc_ );
    void showConfigDialog(QWidget *); // Select datasource type  and / or configure datasource

    QString getValue( const QString &name, int record = -1 ) const;  //accesses the plugin
    bool isSampleRecord();

    const QMap< QString, QString > &getRecordEntries() const; //accesses the plugin
    int getNumRecords() const; //accesses the plugin

    QDomElement save(QDomDocument &doc) const; // save some global config + plugin config
    void load( const QDomElement& elem ); // save some global config + plugin config

    KWMailMergeDataSource *loadPlugin(const QString& name);
    KWMailMergeDataSource *openPluginFor(int type,int &version);

    virtual void refresh(bool force);
    virtual QStringList availablePlugins();
    virtual bool loadPlugin(const QString &name,const QString &command);
    virtual bool isConfigDialogShown();
    
    int version();
private:
    int m_version;
protected:
    friend class KWMailMergeConfigDialog;
    KWDocument *doc;
    int action;
    class KWMailMergeDataSource *plugin;
    QMap<QString, QString> emptyMap;
    bool askUserForConfirmationAndConfig(KWMailMergeDataSource *tmpPlugin,bool config,QWidget *par,int version);
    bool rejectdcopcall;
};


class KWMailMergeChoosePluginDialog : public KDialogBase
{
  Q_OBJECT

  public:
    KWMailMergeChoosePluginDialog( KTrader::OfferList );
    ~KWMailMergeChoosePluginDialog();

    int currentPlugin() const;    

  private slots:
    void pluginChanged( int pos );

  private:
    class QComboBox *chooser;
    class QLabel *descriptionLabel;
    KTrader::OfferList pluginOffers;
};

class KWMailMergeConfigDialog : public KDialogBase
{
    Q_OBJECT

public:
   KWMailMergeConfigDialog ( QWidget *parent, KWMailMergeDataBase *db );
   ~KWMailMergeConfigDialog();
protected:
    QPushButton *edit;
    QPushButton *create;
    QPushButton *open;
    QPushButton *preview;
    QPushButton *document;
    KWMailMergeDataBase *db_;
    void enableDisableEdit();
    void doNewActions();

protected slots:
    void slotEditClicked();
    void slotCreateClicked();
    void slotOpenClicked();
    void slotPreviewClicked();
    void slotDocumentClicked();
};

/******************************************************************
 *
 * Class: KWMailMergeVariableInsertDia
 *
 ******************************************************************/

class KWMailMergeVariableInsertDia : public KDialogBase
{
  Q_OBJECT

  public:
    KWMailMergeVariableInsertDia( QWidget *parent, KWMailMergeDataBase *db );
    QString getName() const;

  protected slots:
    void slotSelectionChanged();

  private:
    KWMailMergeDataBase *m_db;

  protected:
    QListBox *names;
};

#endif
