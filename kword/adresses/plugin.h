#ifndef __KWORD_ADDRESSES_PLUGIN_H__
#define __KWORD_ADDRESSES_PLUGIN_H__

#include <kparts/plugin.h>

class PluginKWordAddresses : public KParts::Plugin
{
  Q_OBJECT

public:
  PluginKWordAddresses( QObject* = 0L, const char* = 0L );
  virtual ~PluginKWordAddresses();

public slots:
  void slotImport();
  void slotExport();
  void slotConfigure();
};

#endif // __KWORD_ADDRESSES_PLUGIN_H__
