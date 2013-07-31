#ifndef FAKE_kurlcompletion_H
#define FAKE_kurlcompletion_H

#include <kcompletion.h>
#include <kurl.h>

class KUrlCompletion : public KCompletion
{
public:
    KUrlCompletion() : KCompletion() {}    
    virtual QString makeCompletion(const QString& text) { return text; }
    virtual void setDir(const QString& dir) {}
    virtual QString dir() const { return QString(); }
    virtual bool isRunning() const { return false; }
    virtual void stop() {}
    //virtual Mode mode() const {}
    //virtual void setMode(Mode mode) {}
    virtual bool replaceEnv() const { return false; }
    virtual void setReplaceEnv(bool replace) {}
    virtual bool replaceHome() const { return false; }
    virtual void setReplaceHome(bool replace) {}
    QString replacedPath(const QString& text) const { return text; }
    static QString replacedPath(const QString& text, bool replaceHome, bool replaceEnv = true) { return text; }
};

#endif
