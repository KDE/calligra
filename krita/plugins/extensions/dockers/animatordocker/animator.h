#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <QVariant>

#include <kparts/plugin.h>

class KisView2;

/**
 * Template of view plugin
 */
class AnimatorPlugin : public KParts::Plugin
{
    Q_OBJECT
    public:
        AnimatorPlugin(QObject *parent, const QVariantList &);
        virtual ~AnimatorPlugin();
    private:
        KisView2* m_view;
};

#endif
