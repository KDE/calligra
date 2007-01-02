#ifndef KEXIFACTORY_H
#define KEXIFACTORY_H

#include <KoFactory.h>

class KAboutData;

class KDE_EXPORT KexiFactory : public KoFactory
{
    Q_OBJECT
public:
    KexiFactory(QObject* parent = 0, const char* name = 0);
    ~KexiFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
