// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the definition of the interface to the Kugar KPart.


#ifndef _KUGAR_PART_H
#define _KUGAR_PART_H

#include <kparts/browserextension.h>
#include <klibloader.h>

#include <koFactory.h>

#include "kugar.h"


class KInstance;
class KugarBrowserExtension;


class KugarFactory : public KoFactory
{
    Q_OBJECT
public:
    KugarFactory( QObject* parent = 0, const char* name = 0 );
    ~KugarFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, const char *widgetName = 0,
                QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument",
                const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();
private:
    static KInstance* s_instance;
    static KAboutData* s_aboutdata;
};


class KugarPart: public KParts::ReadOnlyPart
{
	Q_OBJECT

public:
	KugarPart(QWidget *parent,const char *name,const QString& forcedUserTemplate);
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
	QString m_forcedUserTemplate;
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
