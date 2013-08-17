#ifndef MYPREFS_H
#define MYPREFS_H

#include <kconfigskeleton.h>

class MyPrefs : public KConfigSkeleton
{
  public:
    MyPrefs( const QString &a ) : KConfigSkeleton( a ) {}
};

#endif
