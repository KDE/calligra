
#ifndef KEXIDBFACTORY_H
#define KEXIDBFACTORY_H


#include <widgetfactory.h>

/**
 *
 * Lucijan Busch
 **/
class KexiDBFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		KexiDBFactory(QObject *parent, const char *name, const QStringList &args);
		~KexiDBFactory();

		virtual QString				name();
		virtual KFormDesigner::WidgetList	classes();
		virtual QWidget				*create(const QString &, QWidget *, const char *,
					 		 KFormDesigner::Container *);
		virtual void				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple) { return !multiple;}

	protected:
		virtual void  changeText(const QString &newText);

	public slots:
		void chDataSource();

	private:
		KFormDesigner::WidgetList		m_classes;
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
