#ifndef KEXI_DRAGOBJECTS_H_
#define KEXI_DRAGOBJECTS_H_

#include <qdragobject.h>

class QString;
class QWidget;

class KexiFieldDrag : public QStoredDrag
{
        public:
                KexiFieldDrag(const QString& sourceType, const QString& source,const QString& field, 
		QWidget *parent=0, const char *name=0);
                ~KexiFieldDrag() { };

                static bool canDecode( QDragMoveEvent* e);
		static bool decode( QDropEvent* e, QString& sourceType, QString& source, QString& field );

};

class KexiDataProviderDrag : public QStoredDrag
{
        public:
                KexiDataProviderDrag(const QString& sourceType, const QString& source, 
		QWidget *parent=0, const char *name=0);
                ~KexiDataProviderDrag() { };

                static bool canDecode( QDragMoveEvent* e);
		static bool decode( QDropEvent* e, QString& sourceType, QString& source);

};

#endif
