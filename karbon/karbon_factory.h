/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __KARBON_FACTORY_H__
#define __KARBON_FACTORY_H__

#include <koFactory.h>

class KInstance;
class KAboutData;

class KarbonFactory : public KoFactory
{
	Q_OBJECT
public:
	KarbonFactory( QObject* parent = 0, const char* name = 0 );
	~KarbonFactory();

	virtual KParts::Part *createPart( QWidget *parentWidget = 0,
	const char* widgetName = 0, QObject* parent = 0, const char* name = 0,
	const char* classname = "KoDocument", const QStringList& args = QStringList() );

	static KInstance* instance();
	static KAboutData* aboutData();

private:
	static KInstance*	s_instance;
	static KAboutData*	s_aboutData;
};

#endif
