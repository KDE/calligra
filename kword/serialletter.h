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

#ifndef serialletter_h
#define serialletter_h

#include <kdialogbase.h>
#include <ktrader.h>


#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qdom.h>
#include <qtextstream.h>

#include "serialletter_interface.h"
#include "KWordSerialLetterDatabaseIface.h"

class QListBox;
class QPushButton;
class QLineEdit;
class QSpinBox;
class KWDocument;

//class KWSerialLetterDataBase;




/******************************************************************
 *
 * Class: KWSerialLetterDataBase
 *
 ******************************************************************/

class KWSerialLetterDataBase: public QObject,KWordSerialLetterDatabaseIface
{
Q_OBJECT

public:
    KWSerialLetterDataBase( KWDocument *doc_ );
    void showConfigDialog(QWidget *); // Select datasource type  and / or configure datasource

    QString getValue( const QString &name, int record = -1 ) const;  //accesses the plugin

    const QMap< QString, QString > &getRecordEntries() const; //accesses the plugin
    int getNumRecords() const; //accesses the plugin

    QDomElement save(QDomDocument &doc) const; // save some global config + plugin config
    void load( QDomElement& elem ); // save some global config + plugin config

    KWSerialLetterDataSource *loadPlugin(const QString& name);
    KWSerialLetterDataSource *openPluginFor(int type);

    virtual void refresh(bool force);
    virtual QStringList availablePlugins();
    virtual bool loadPlugin(QString name,QString command);
    virtual bool isConfigDialogShown();
protected:
    friend class KWSerialLetterConfigDialog;
    KWDocument *doc;
    int action;
    class KWSerialLetterDataSource *plugin;
    QMap<QString, QString> emptyMap;
    bool askUserForConfirmationAndConfig(KWSerialLetterDataSource *tmpPlugin,bool config,QWidget *par);
    bool rejectdcopcall;
};


class KWSerialLetterChoosePluginDialog : public KDialogBase
{
	Q_OBJECT
public:
	KWSerialLetterChoosePluginDialog(KTrader::OfferList);
	~KWSerialLetterChoosePluginDialog();
	class QComboBox *chooser;
};

class KWSerialLetterConfigDialog : public KDialogBase
{
    Q_OBJECT

public:
   KWSerialLetterConfigDialog ( QWidget *parent, KWSerialLetterDataBase *db );
   ~KWSerialLetterConfigDialog();
protected:
    QPushButton *edit;
    QPushButton *create;
    QPushButton *open;
    QPushButton *preview;
    QPushButton *document;
    KWSerialLetterDataBase *db_;
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
 * Class: KWSerialLetterVariableInsertDia
 *
 ******************************************************************/

class KWSerialLetterVariableInsertDia : public KDialogBase
{
    Q_OBJECT

public:
    KWSerialLetterVariableInsertDia( QWidget *parent, KWSerialLetterDataBase *db );
    QString getName() const;

protected:
    void resizeEvent( QResizeEvent *e );

protected slots:
	void slotSelectionChanged();
protected:
    QListBox *names;
    QVBox *back;

};

#endif
