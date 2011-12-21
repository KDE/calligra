#ifndef SVGCUSTOMSAVINGCONTEXT_H
#define SVGCUSTOMSAVINGCONTEXT_H

#include <QtCore/QBuffer>
#include "flake_export.h"
#include "SvgSavingContext.h"

class KoXmlWriter;

class FLAKE_EXPORT SvgCustomSavingContext : public SvgSavingContext
{
public:
    SvgCustomSavingContext();

    bool finalize();

    /// Provides access to the aniamtion properties writer
    KoXmlWriter &animationPropertiesWriter();

    virtual ~SvgCustomSavingContext();
    
private:
    QBuffer m_animationPropertiesBuffer;
    KoXmlWriter *m_animationPropertiesWriter;
    
};

#endif // SVGCUSTOMSAVINGCONTEXT_H
