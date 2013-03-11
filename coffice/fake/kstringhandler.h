#ifndef FAKE_KSTRINGHANDLER_H
#define FAKE_KSTRINGHANDLER_H

class KStringHandler
{
public:
    static int naturalCompare(const QString &s1, const QString &s2)
    {
        return s1.compare(s2, Qt::CaseInsensitive);
    }
};

#endif
 
