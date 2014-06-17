#ifndef ODBMIGRATE_H
#define ODBMIGRATE_H
#include <QByteArray>
#include <migration/keximigrate.h>
#include <migration/keximigrate_p.h>

namespace KexiMigration
{
class OdbMigrate : public KexiMigrate
{
    Q_OBJECT
    KEXIMIGRATION_DRIVER
    public:
        explicit OdbMigrate(QObject *parent, const QVariantList& args = QVariantList());
        virtual ~OdbMigrate();

public:
    bool drv_connect();
   
};
}

#endif // ODBMIGRATE_H


