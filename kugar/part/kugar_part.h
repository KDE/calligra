// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the definition of the interface to the Kugar KPart.


#ifndef _KUGAR_PART_H
#define _KUGAR_PART_H

#include <kparts/browserextension.h>
#include <klibloader.h>

#include "kugar.h"


class KInstance;
class KugarBrowserExtension;


class KugarFactory : public KLibFactory
{
	Q_OBJECT

public:
	KugarFactory();
	virtual ~KugarFactory();

	virtual QObject* create(QObject *parent = 0,const char *name = 0,
				const char *classname = "QObject",
				const QStringList &args = QStringList());
	static KInstance *instance();

private:
	static KInstance *s_instance;
};


class KugarPart: public KParts::ReadOnlyPart
{
	Q_OBJECT

public:
	KugarPart(QWidget *parent,const char *name);
	virtual ~KugarPart();

	void print();
	
	bool setReportTemplate(const QString &data)
	{
		return view -> setReportTemplate(data);
	}

protected:
	virtual bool openFile();
	virtual bool closeURL();

private slots:
	void slotPreferedTemplate(const QString &);

private:
	MReportViewer *view;
	KugarBrowserExtension *m_extension;
};


class KugarBrowserExtension : public KParts::BrowserExtension
{
	Q_OBJECT

	friend class KugarPart;

public:
	KugarBrowserExtension(KugarPart *parent);
	virtual ~KugarBrowserExtension();

public slots:
	void print();
};

#endif 
