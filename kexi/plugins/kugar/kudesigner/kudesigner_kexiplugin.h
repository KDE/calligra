/* This file is part of the KDE project
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KUGARDESIGNERKEXI_H
#define KUGARDESIGNERKEXI_H

#include <plugin.h>
#include <property.h>
#include <kdebug.h>
#include <kexidatasourcecombobox.h>
#include <propertywidgets.h>

class KexiDialogBase;
class KexiProject;
class KudesignerDoc;
class KoStore;

class KuKexi: public KuDesignerPlugin {
	Q_OBJECT
public:
        KuKexi(QObject *parent, const char* name, const QStringList& args);
        virtual ~KuKexi();
	virtual void createPluggedInEditor(QWidget *&retVal, PropertyEditor *editor,
        	Property*,CanvasBox *cb);
	virtual bool acceptsDrops(){kdDebug()<<"****ACCEPT DROPS"<<endl; return true;}
        virtual bool dragMove(QDragMoveEvent *,CanvasBox *) {return true;}
	virtual void newCanvasBox(int type, CanvasBox *cb);
	virtual bool store(KoStore*);
	virtual bool load(KoStore*);
protected slots:
	void slotDataSourceSelected(int level, int value);
signals:
	void getStorageFile(QString &path);
private:
	friend class KuKexiFieldComboBox;
	KexiDialogBase* m_dialog;
	KexiProject* m_kexi;
	KudesignerDoc* m_kudesigner;
	KexiDataSourceComboBox::ItemList m_sourceMapping;
	void updateSourceList();
	typedef QMap<int,QStringList> FieldMap;
	FieldMap m_fieldMap;
	typedef QMap<int,int> SectionMap;
	SectionMap m_sectionMap;
};

class KuKexiDataSourceComboBox: public KexiDataSourceComboBox, public PropertyWidget {
	Q_OBJECT
public:
	KuKexiDataSourceComboBox ( const PropertyEditor *editor, QString name,
		QString value, KexiDataSourceComboBox::ItemList data, int level,
		QWidget * parent=0, const char * name = 0 );
	virtual ~KuKexiDataSourceComboBox();

	virtual QString value() const;
	virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
    void propertyChanged(int level, int value);
private slots:
    void updateProperty(int val);
private:
    int m_level;
};

class KuKexiFieldComboBox: public KComboBox, public PropertyWidget {
	Q_OBJECT
public:
	KuKexiFieldComboBox ( const PropertyEditor *editor, QString name,
		QString value,  KuKexi *kukexi, int level,
		QWidget * parent=0, const char * name = 0 );
	virtual ~KuKexiFieldComboBox();

	virtual QString value() const;
	virtual void setValue(const QString value, bool emitChange=true);
	static QString convertFromStorageName(const QString& stn);
	static QString convertToStorageName(const QString& displayname);
signals:
    void propertyChanged(QString name, QString newValue);
    void propertyChanged(int level, int value);
private slots:
    void updateProperty(int val);
private:
    int m_level;
    KuKexi *m_kukexi;
    void fill();
};




#endif
