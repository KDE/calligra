#ifndef __KWORD_ADRESSES_FACTORY_H__
#define __KWORD_ADRESSES_FACTORY_H__

#include <kinstance.h>
#include <klibloader.h>

class KWordAddressesFactory : public KLibFactory
{
  Q_OBJECT

public:
  KWordAddressesFactory( QObject* = 0L, const char* = 0L );
  ~KWordAddressesFactory();

  virtual QObject* create( QObject* = 0, const char* = 0, const char* = "QObject", const QStringList& = QStringList() );

private:
  static KInstance* s_instance;
};

#endif // __KWORD_ADRESSES_FACTORY_H__
