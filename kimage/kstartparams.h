#include <qstring.h>
#include <qstringlist.h>

class KStartParams
{
public:
  KStartParams( int& argc, const char** argv );
  ~KStartParams();

  int countParams();
  QString getParam( uint _index );
  void deleteParam( uint _index );
  bool paramIsPresent( const QString& _param );

private:
  QStringList m_paramList;
};
