/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _KEXI_KUGAR_HANDLER_ITEM_H_
#define _KEXI_KUGAR_HANDLER_ITEM_H_

#include <kexiprojecthandleritem.h>
#include <qstring.h>
#include <qmap.h>
#include <qstringlist.h>

class KexiKugarHandler;

class KexiKugarHandlerItem : public KexiProjectHandlerItem
{
	Q_OBJECT
public:
	KexiKugarHandlerItem(KexiKugarHandler *parent, const QString& name, const QString& mime,
                                const QString& identifier);
	~KexiKugarHandlerItem();
	const QString &reportTemplate() const;
	void setReportTemplate (const QString &reportTemplate);
	const QStringList &storedDatasets() const;
        void setStoredDataset(const QString &datasetName, const QString &data);
	void removeStoredDataset(const QString &datasetName);
	const QString &storedDataset(const QString &datasetName) const;
	KoDocument *designer(bool newReport=false);
	void store (KoStore *ks);

public slots:
	void pluginStorageFile(QString &path);
	void view(KexiView*);
private:
	QStringList m_storedDataSetNames;
	QDict<QString> m_storedDataSets;
	QString m_reportTemplate;
	QString m_sourceIdentifier;
	QString m_tempPath;
	KoDocument *m_designer;
	KoDocument *m_viewer;
	QString generateDataFile();
};

#endif
