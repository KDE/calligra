char kivio_module[] =
"import kivioc\n"\
"class KivioStencil:\n"\
"    def __init__(self,*args):\n"\
"        self.this = apply(kivioc.new_KivioStencil,args)\n"\
"        self.thisown = 1\n"\
"\n"\
"    def __del__(self):\n"\
"        if self.thisown == 1 :\n"\
"            kivioc.delete_KivioStencil(self)\n"\
"    def duplicate(*args):\n"\
"        val = apply(kivioc.KivioStencil_duplicate,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def x(*args):\n"\
"        val = apply(kivioc.KivioStencil_x,args)\n"\
"        return val\n"\
"    def setX(*args):\n"\
"        val = apply(kivioc.KivioStencil_setX,args)\n"\
"        return val\n"\
"    def y(*args):\n"\
"        val = apply(kivioc.KivioStencil_y,args)\n"\
"        return val\n"\
"    def setY(*args):\n"\
"        val = apply(kivioc.KivioStencil_setY,args)\n"\
"        return val\n"\
"    def w(*args):\n"\
"        val = apply(kivioc.KivioStencil_w,args)\n"\
"        return val\n"\
"    def setW(*args):\n"\
"        val = apply(kivioc.KivioStencil_setW,args)\n"\
"        return val\n"\
"    def h(*args):\n"\
"        val = apply(kivioc.KivioStencil_h,args)\n"\
"        return val\n"\
"    def setH(*args):\n"\
"        val = apply(kivioc.KivioStencil_setH,args)\n"\
"        return val\n"\
"    def rect(*args):\n"\
"        val = apply(kivioc.KivioStencil_rect,args)\n"\
"        return val\n"\
"    def setPosition(*args):\n"\
"        val = apply(kivioc.KivioStencil_setPosition,args)\n"\
"        return val\n"\
"    def setDimensions(*args):\n"\
"        val = apply(kivioc.KivioStencil_setDimensions,args)\n"\
"        return val\n"\
"    def fillStyle(*args):\n"\
"        val = apply(kivioc.KivioStencil_fillStyle,args)\n"\
"        return val\n"\
"    def setLineWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_setLineWidth,args)\n"\
"        return val\n"\
"    def lineWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_lineWidth,args)\n"\
"        return val\n"\
"    def hTextAlign(*args):\n"\
"        val = apply(kivioc.KivioStencil_hTextAlign,args)\n"\
"        return val\n"\
"    def vTextAlign(*args):\n"\
"        val = apply(kivioc.KivioStencil_vTextAlign,args)\n"\
"        return val\n"\
"    def setHTextAlign(*args):\n"\
"        val = apply(kivioc.KivioStencil_setHTextAlign,args)\n"\
"        return val\n"\
"    def setVTextAlign(*args):\n"\
"        val = apply(kivioc.KivioStencil_setVTextAlign,args)\n"\
"        return val\n"\
"    def setText(*args):\n"\
"        val = apply(kivioc.KivioStencil_setText,args)\n"\
"        return val\n"\
"    def text(*args):\n"\
"        val = apply(kivioc.KivioStencil_text,args)\n"\
"        return val\n"\
"    def textColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_textColor,args)\n"\
"        return val\n"\
"    def setTextColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_setTextColor,args)\n"\
"        return val\n"\
"    def fgColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_fgColor,args)\n"\
"        return val\n"\
"    def setFGColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_setFGColor,args)\n"\
"        return val\n"\
"    def bgColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_bgColor,args)\n"\
"        return val\n"\
"    def setBGColor(*args):\n"\
"        val = apply(kivioc.KivioStencil_setBGColor,args)\n"\
"        return val\n"\
"    def isSelected(*args):\n"\
"        val = apply(kivioc.KivioStencil_isSelected,args)\n"\
"        return val\n"\
"    def select(*args):\n"\
"        val = apply(kivioc.KivioStencil_select,args)\n"\
"        return val\n"\
"    def unselect(*args):\n"\
"        val = apply(kivioc.KivioStencil_unselect,args)\n"\
"        return val\n"\
"    def subSelect(*args):\n"\
"        val = apply(kivioc.KivioStencil_subSelect,args)\n"\
"        return val\n"\
"    def addToGroup(*args):\n"\
"        val = apply(kivioc.KivioStencil_addToGroup,args)\n"\
"        return val\n"\
"    def updateGeometry(*args):\n"\
"        val = apply(kivioc.KivioStencil_updateGeometry,args)\n"\
"        return val\n"\
"    def setStartAHType(*args):\n"\
"        val = apply(kivioc.KivioStencil_setStartAHType,args)\n"\
"        return val\n"\
"    def setStartAHWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_setStartAHWidth,args)\n"\
"        return val\n"\
"    def setStartAHLength(*args):\n"\
"        val = apply(kivioc.KivioStencil_setStartAHLength,args)\n"\
"        return val\n"\
"    def setEndAHType(*args):\n"\
"        val = apply(kivioc.KivioStencil_setEndAHType,args)\n"\
"        return val\n"\
"    def setEndAHWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_setEndAHWidth,args)\n"\
"        return val\n"\
"    def setEndAHLength(*args):\n"\
"        val = apply(kivioc.KivioStencil_setEndAHLength,args)\n"\
"        return val\n"\
"    def startAHType(*args):\n"\
"        val = apply(kivioc.KivioStencil_startAHType,args)\n"\
"        return val\n"\
"    def startAHWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_startAHWidth,args)\n"\
"        return val\n"\
"    def startAHLength(*args):\n"\
"        val = apply(kivioc.KivioStencil_startAHLength,args)\n"\
"        return val\n"\
"    def endAHType(*args):\n"\
"        val = apply(kivioc.KivioStencil_endAHType,args)\n"\
"        return val\n"\
"    def endAHWidth(*args):\n"\
"        val = apply(kivioc.KivioStencil_endAHWidth,args)\n"\
"        return val\n"\
"    def endAHLength(*args):\n"\
"        val = apply(kivioc.KivioStencil_endAHLength,args)\n"\
"        return val\n"\
"    def __repr__(self):\n"\
"        return \"<C KivioStencil instance at %s>\" % (self.this,)\n"\
"class KivioStencilPtr(KivioStencil):\n"\
"    def __init__(self,this):\n"\
"        self.this = this\n"\
"        self.thisown = 0\n"\
"        self.__class__ = KivioStencil\n"\
"\n"\
"class KivioLayer:\n"\
"    def __init__(self,*args):\n"\
"        self.this = apply(kivioc.new_KivioLayer,args)\n"\
"        self.thisown = 1\n"\
"\n"\
"    def __del__(self):\n"\
"        if self.thisown == 1 :\n"\
"            kivioc.delete_KivioLayer(self)\n"\
"    def visible(*args):\n"\
"        val = apply(kivioc.KivioLayer_visible,args)\n"\
"        return val\n"\
"    def setVisible(*args):\n"\
"        val = apply(kivioc.KivioLayer_setVisible,args)\n"\
"        return val\n"\
"    def connectable(*args):\n"\
"        val = apply(kivioc.KivioLayer_connectable,args)\n"\
"        return val\n"\
"    def setConnectable(*args):\n"\
"        val = apply(kivioc.KivioLayer_setConnectable,args)\n"\
"        return val\n"\
"    def addStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_addStencil,args)\n"\
"        return val\n"\
"    def removeStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_removeStencil,args)\n"\
"        return val\n"\
"    def firstStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_firstStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def nextStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_nextStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def prevStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_prevStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def currentStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_currentStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def lastStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_lastStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def takeStencil(*args):\n"\
"        val = apply(kivioc.KivioLayer_takeStencil,args)\n"\
"        if val: val = KivioStencilPtr(val) \n"\
"        return val\n"\
"    def __repr__(self):\n"\
"        return \"<C KivioLayer instance at %s>\" % (self.this,)\n"\
"class KivioLayerPtr(KivioLayer):\n"\
"    def __init__(self,this):\n"\
"        self.this = this\n"\
"        self.thisown = 0\n"\
"        self.__class__ = KivioLayer\n"\
"\n"\
"update = kivioc.update\n"\
"isStencilSelected = kivioc.isStencilSelected\n"\
"selectAllStencils = kivioc.selectAllStencils\n"\
"unselectAllStencils = kivioc.unselectAllStencils\n"\
"unselectStencil = kivioc.unselectStencil\n"\
"selectStencil = kivioc.selectStencil\n"\
"\n"\
"def curLayer(*args, **kwargs):\n"\
"    val = apply(kivioc.curLayer,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"setCurLayer = kivioc.setCurLayer\n"\
"\n"\
"def firstLayer(*args, **kwargs):\n"\
"    val = apply(kivioc.firstLayer,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"def nextLayer(*args, **kwargs):\n"\
"    val = apply(kivioc.nextLayer,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"def lastLayer(*args, **kwargs):\n"\
"    val = apply(kivioc.lastLayer,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"def prevLayer(*args, **kwargs):\n"\
"    val = apply(kivioc.prevLayer,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"removeCurrentLayer = kivioc.removeCurrentLayer\n"\
"addLayer = kivioc.addLayer\n"\
"insertLayer = kivioc.insertLayer\n"\
"\n"\
"def layerAt(*args, **kwargs):\n"\
"    val = apply(kivioc.layerAt,args,kwargs)\n"\
"    if val: val = KivioLayerPtr(val)\n"\
"    return val\n"\
"\n"\
"addStencil = kivioc.addStencil\n"\
"deleteSelectedStencils = kivioc.deleteSelectedStencils\n"\
"groupSelectedStencils = kivioc.groupSelectedStencils\n"\
"ungroupSelectedStencils = kivioc.ungroupSelectedStencils\n"\
"bringToFront = kivioc.bringToFront\n"\
"sendToBack = kivioc.sendToBack\n"\
"copy = kivioc.copy\n"\
"cut = kivioc.cut\n"\
"paste = kivioc.paste\n"\
"";
