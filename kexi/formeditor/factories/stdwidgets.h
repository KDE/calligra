
#ifndef _STDWIDGETS_H_
#define _STDWIDGETS_H_

#include "../formdesigner/widgetfactory.h"

/**
 *
 * Lucijan Busch
 **/

class StdWidgets : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		StdWidgets(QObject *parent, const char *name, const QStringList &args);
		~StdWidgets();

		virtual QString				name();
		virtual KFormDesigner::WidgetList	classes() { return m_classes; }
		virtual QWidget				*create(const QString &, QWidget *, const char *,
					 		 KFormDesigner::Container *);

	private:
		KFormDesigner::WidgetList		m_classes;
};

#endif
