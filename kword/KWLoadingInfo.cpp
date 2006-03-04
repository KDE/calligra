#include "KWLoadingInfo.h"
#include "KWFrame.h"

KWLoadingInfo::KWLoadingInfo()
{
    columns.columns = 1;
    // columns.ptColumnSpacing must be initialized by KWDocument

    hf.header = HF_SAME;
    hf.footer = HF_SAME;
    hf.ptHeaderBodySpacing = 10.0;
    hf.ptFooterBodySpacing = 10.0;
    hf.ptFootNoteBodySpacing = 10.0;
}
