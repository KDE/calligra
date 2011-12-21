#include "SvgCustomSavingContext.h"
#include <KoXmlWriter.h>
#include <QDebug>

SvgCustomSavingContext::SvgCustomSavingContext()
{
    m_animationPropertiesWriter = new KoXmlWriter(&m_animationPropertiesBuffer, 1);
}


SvgCustomSavingContext::~SvgCustomSavingContext()
{
   
}

KoXmlWriter& SvgCustomSavingContext::animationPropertiesWriter()
{
    return *m_animationPropertiesWriter;
}

bool SvgCustomSavingContext::finalize()
{
    qDebug() << "custom";
    SvgSavingContext::finalize();
    outputDevice().write("\n");
    outputDevice().write(m_animationPropertiesBuffer.data());
    
    return true;
}

