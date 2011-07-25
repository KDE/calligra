/* This file is part of the KDE project
    * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
    *
    * This library is free software; you can redistribute it and/or
    * modify it under the terms of the GNU Library General Public
    * License as published by the Free Software Foundation; either
    * version 2 of the License, or (at your option) any later version.
    *
    * This library is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    * Library General Public License for more details.
    *
    * You should have received a copy of the GNU Library General Public License
    * along with this library; see the file COPYING.LIB.  If not, write to
    * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    * Boston, MA 02110-1301, USA.
    */

#include "SvgWriter_Stage.h"
//#include "SvgWriter.h"
#include "SvgWriter_generic.h"
#include "SvgAnimationData.h"
#include "KoShape.h"

#include "plugins/presentationviewportshape/PresentationViewPortShape.h"
#include <QDebug>

SvgWriter_Stage::SvgWriter_Stage(const QList<KoShapeLayer*> &layers, const QSizeF& pageSize): SvgWriter_generic(layers, pageSize)
{
  m_hasAppData = true;
  m_appDataId = PresentationViewPortShapeId;
  
  m_frames = new QTextStream(&m_appData, QIODevice::ReadWrite);
          
  saveScript();
  //addAppData(m_framesString);
  }


SvgWriter_Stage::SvgWriter_Stage(const QList< KoShape* >& toplevelShapes, const QSizeF& pageSize): SvgWriter_generic(toplevelShapes, pageSize)
{
  m_hasAppData = true;
  m_frames = new QTextStream(&m_appData, QIODevice::ReadWrite);
          
  saveScript();
  }

SvgWriter_Stage::~SvgWriter_Stage()
{

}

void SvgWriter_Stage::saveViewPortShape(PresentationViewPortShape* shape, QTextStream* body)
{
    *body << "<path" << getID(shape);
    qDebug() << "Shape ID = " << m_shapeIds[shape];

    //getStyle(shape, body);
    //fill wriiten to SVG
    //TODO other attributes - stroke, etc. ?
    *body << " fill=\"none\" stroke=\"#000000\" stroke-width=\"1.25\" stroke-linecap=\"square\" stroke-linejoin=\"miter\" stroke-miterlimit=\"2\" ";
    *body << " d=\"" << shape->toString() << "\"";
    *body << getTransform(shape->transformation(), " transform");//transform being written to SVG

    *body << " />" << endl;

}

//TODO PVPShape should have the animation data, not Frame.
void SvgWriter_Stage::saveAnimationTags(PresentationViewPortShape* shape)
{
   if(shape->applicationData() == 0){
      forTesting(shape);// Adds a frame object to this shape
    }
        
    SvgAnimationData * appData = dynamic_cast<SvgAnimationData*>(shape->applicationData());
    Frame *frameObj;// = new Frame();
    frameObj = &(appData->frame());
          
    frameObj->writeToStream(m_frames);       

}

void SvgWriter_Stage::saveAppData(KoShape* shape, QTextStream* body)
{
  //saveViewPortShape(dynamic_cast<PresentationViewPortShape*>(shape), body);
  saveAnimationTags(dynamic_cast<PresentationViewPortShape*>(shape));   
}

void SvgWriter_Stage::saveScript()
{
      m_script  = "Function.prototype.bind=function(c){var a=Array.prototype.slice.call(arguments,1),b=this;return function(){b.apply(c,a.concat(Array.prototype.slice.call(arguments)))}};var sozi=sozi||{};sozi.Display=function(a){this.controller=a;this.cx=0;this.cy=0;this.width=1;this.height=1;this.rotate=0;this.clip=true};sozi.Display.prototype.svgNs=&quot;http://www.w3.org/2000/svg&quot;;sozi.Display.prototype.onLoad=function(){var b,a=document.createElementNS(this.svgNs,&quot;clipPath&quot;);this.svgRoot=document.documentElement;this.svgRoot.removeAttribute(&quot;viewBox&quot;);this.initialBBox=this.svgRoot.getBBox();this.wrapper=document.createElementNS(this.svgNs,&quot;g&quot;);this.wrapper.setAttribute(&quot;id&quot;,&quot;sozi-wrapper&quot;);while(true){b=this.svgRoot.firstChild;if(!b){break}this.svgRoot.removeChild(b);this.wrapper.appendChild(b)}this.svgRoot.appendChild(this.wrapper);this.clipRect=document.createElementNS(this.svgNs,&quot;rect&quot;);this.clipRect.setAttribute(&quot;id&quot;,&quot;sozi-clip-rect&quot;);a.setAttribute(&quot;id&quot;,&quot;sozi-clip-path&quot;);a.appendChild(this.clipRect);this.svgRoot.appendChild(a);this.svgRoot.setAttribute(&quot;clip-path&quot;,&quot;url(#sozi-clip-path)&quot;);this.svgRoot.setAttribute(&quot;width&quot;,window.innerWidth);this.svgRoot.setAttribute(&quot;height&quot;,window.innerHeight)};sozi.Display.prototype.resize=function(){this.svgRoot.setAttribute(&quot;width&quot;,window.innerWidth);this.svgRoot.setAttribute(&quot;height&quot;,window.innerHeight);this.update()};sozi.Display.prototype.getFrameGeometry=function(){var a={};a.scale=Math.min(window.innerWidth/this.width,window.innerHeight/this.height);a.width=this.width*a.scale;a.height=this.height*a.scale;a.x=(window.innerWidth-a.width)/2;a.y=(window.innerHeight-a.height)/2;return a};sozi.Display.prototype.getElementGeometry=function(d){var j,g,a,k,h,f,i=d.getCTM(),e=Math.sqrt(i.a*i.a+i.b*i.b);if(d.nodeName===&quot;rect&quot;){j=d.x.baseVal.value;g=d.y.baseVal.value;a=d.width.baseVal.value;k=d.height.baseVal.value}else{h=d.getBBox();j=h.x;g=h.y;a=h.width;k=h.height}f=document.documentElement.createSVGPoint();f.x=j+a/2;f.y=g+k/2;f=f.matrixTransform(i);return{cx:f.x,cy:f.y,width:a*e,height:k*e,rotate:Math.atan2(i.b,i.a)*180/Math.PI}};sozi.Display.prototype.getDocumentGeometry=function(){return{cx:this.initialBBox.x+this.initialBBox.width/2,cy:this.initialBBox.y+this.initialBBox.height/2,width:this.initialBBox.width,height:this.initialBBox.height,rotate:0,clip:false}};sozi.Display.prototype.getCurrentGeometry=function(){return{cx:this.cx,cy:this.cy,width:this.width,height:this.height,rotate:this.rotate,clip:this.clip}};sozi.Display.prototype.drag=function(b,a){var c=this.getFrameGeometry(),d=this.rotate*Math.PI/180;this.cx-=(b*Math.cos(d)-a*Math.sin(d))/c.scale;this.cy-=(b*Math.sin(d)+a*Math.cos(d))/c.scale;this.clip=false;if(this.tableOfContentsIsVisible()){this.hideTableOfContents()}this.update()};sozi.Display.prototype.update=function(){var a=this.getFrameGeometry(),c=-this.cx+this.width/2+a.x/a.scale,b=-this.cy+this.height/2+a.y/a.scale;this.wrapper.setAttribute(&quot;transform&quot;,&quot;scale(&quot;+a.scale+&quot;)translate(&quot;+c+&quot;,&quot;+b+&quot;)rotate(&quot;+(-this.rotate)+&quot;,&quot;+this.cx+&quot;,&quot;+this.cy+&quot;)&quot;);this.clipRect.setAttribute(&quot;x&quot;,this.clip?a.x:0);this.clipRect.setAttribute(&quot;y&quot;,this.clip?a.y:0);this.clipRect.setAttribute(&quot;width&quot;,this.clip?a.width:window.innerWidth);this.clipRect.setAttribute(&quot;height&quot;,this.clip?a.height:window.innerHeight)};sozi.Display.prototype.showFrame=function(b){var a;for(a in b.geometry){if(b.geometry.hasOwnProperty(a)){this[a]=b.geometry[a]}}this.update()};sozi.Display.prototype.applyZoomFactor=function(a){this.width/=a;this.height/=a};sozi.Display.prototype.installTableOfContents=function(){var c=Math.floor(window.innerHeight/Math.max((this.controller.frames.length+1),40)),b=document.createElementNS(this.svgNs,&quot;rect&quot;),d=0,a,g,f,e;this.tocGroup=document.createElementNS(this.svgNs,&quot;g&quot;);this.tocGroup.setAttribute(&quot;visibility&quot;,&quot;hidden&quot;);this.tocGroup.appendChild(b);this.svgRoot.appendChild(this.tocGroup);b.setAttribute(&quot;fill&quot;,&quot;#eee&quot;);b.setAttribute(&quot;stroke&quot;,&quot;#888&quot;);b.setAttribute(&quot;x&quot;,&quot;0&quot;);b.setAttribute(&quot;y&quot;,&quot;0&quot;);b.setAttribute(&quot;height&quot;,(this.controller.frames.length+1)*c);for(a=0;a&lt;this.controller.frames.length;a++){g=this.controller.frames[a];f=document.createElementNS(this.svgNs,&quot;text&quot;);f.appendChild(document.createTextNode(g.title));f.setAttribute(&quot;x&quot;,c/2);f.setAttribute(&quot;y&quot;,c*(a+1.3));f.setAttribute(&quot;fill&quot;,&quot;black&quot;);f.setAttribute(&quot;style&quot;,&quot;font-size: &quot;+(c*0.9)+&quot;px;font-family: Verdana, sans-serif&quot;);f.addEventListener(&quot;click&quot;,function(i,h){this.hideTableOfContents();this.controller.moveToFrame(i);h.stopPropagation()}.bind(this,a),false);f.addEventListener(&quot;mouseover&quot;,function(){this.setAttribute(&quot;fill&quot;,&quot;#08c&quot;)},false);f.addEventListener(&quot;mouseout&quot;,function(){this.setAttribute(&quot;fill&quot;,&quot;black&quot;)},false);this.tocGroup.appendChild(f);e=f.getBBox().width;if(e&gt;d){d=e}}b.setAttribute(&quot;width&quot;,d+c)};sozi.Display.prototype.showTableOfContents=function(){this.clipRect.setAttribute(&quot;x&quot;,0);this.clipRect.setAttribute(&quot;y&quot;,0);this.clipRect.setAttribute(&quot;width&quot;,window.innerWidth);this.clipRect.setAttribute(&quot;height&quot;,window.innerHeight);this.tocGroup.setAttribute(&quot;visibility&quot;,&quot;visible&quot;)};sozi.Display.prototype.hideTableOfContents=function(){var a=this.getFrameGeometry();this.tocGroup.setAttribute(&quot;visibility&quot;,&quot;hidden&quot;);this.clipRect.setAttribute(&quot;x&quot;,a.x);this.clipRect.setAttribute(&quot;y&quot;,a.y);this.clipRect.setAttribute(&quot;width&quot;,a.width);this.clipRect.setAttribute(&quot;height&quot;,a.height)};sozi.Display.prototype.tableOfContentsIsVisible=function(){return this.tocGroup.getAttribute(&quot;visibility&quot;)===&quot;visible&quot;};var sozi=sozi||{};sozi.Animator=function(b,a,c){this.timeStepMs=b||40;this.onStep=a;this.onDone=c;this.durationMs=0;this.data={};this.initialTime=0;this.started=false;this.timer=0};sozi.Animator.prototype.start=function(a,b){this.durationMs=a;this.data=b;this.initialTime=Date.now();this.onStep(0,this.data);if(!this.started){this.started=true;this.timer=window.setInterval(this.step.bind(this),this.timeStepMs)}};sozi.Animator.prototype.stop=function(){if(this.started){window.clearInterval(this.timer);this.started=false}};sozi.Animator.prototype.step=function(){var a=Date.now()-this.initialTime;if(a&gt;=this.durationMs){this.stop();this.onStep(1,this.data);this.onDone()}else{this.onStep(a/this.durationMs,this.data)}};var sozi=sozi||{};sozi.Player=function(){this.display=new sozi.Display(this,false);this.animator=new sozi.Animator(40,this.onAnimationStep.bind(this),this.onAnimationDone.bind(this));this.frames=[];this.playing=false;this.waiting=false;this.sourceFrameIndex=0;this.currentFrameIndex=0};sozi.Player.prototype.profiles={linear:function(a){return a},accelerate:function(a){return Math.pow(a,3)},&quot;strong-accelerate&quot;:function(a){return Math.pow(a,5)},decelerate:function(a){return 1-Math.pow(1-a,3)},&quot;strong-decelerate&quot;:function(a){return 1-Math.pow(1-a,5)},&quot;accelerate-decelerate&quot;:function(a){var b=a&lt;=0.5?a:1-a,c=Math.pow(2*b,3)/2;return a&lt;=0.5?c:1-c},&quot;strong-accelerate-decelerate&quot;:function(a){var b=a&lt;=0.5?a:1-a,c=Math.pow(2*b,5)/2;return a&lt;=0.5?c:1-c},&quot;decelerate-accelerate&quot;:function(a){var b=a&lt;=0.5?a:1-a,c=(1-Math.pow(1-2*b,2))/2;return a&lt;=0.5?c:1-c},&quot;strong-decelerate-accelerate&quot;:function(a){var b=a&lt;=0.5?a:1-a,c=(1-Math.pow(1-2*b,3))/2;return a&lt;=0.5?c:1-c}};sozi.Player.prototype.soziNs=&quot;http://sozi.baierouge.fr&quot;;sozi.Player.prototype.dragButton=1;sozi.Player.prototype.defaultDurationMs=500;sozi.Player.prototype.defaultZoomPercent=-10;sozi.Player.prototype.defaultProfile=sozi.Player.prototype.profiles.linear;sozi.Player.prototype.scaleFactor=1.05;sozi.Player.prototype.defaults={title:&quot;Untitled&quot;,sequence:&quot;0&quot;,hide:&quot;true&quot;,clip:&quot;true&quot;,&quot;timeout-enable&quot;:&quot;false&quot;,&quot;timeout-ms&quot;:&quot;5000&quot;,&quot;transition-duration-ms&quot;:&quot;1000&quot;,&quot;transition-zoom-percent&quot;:&quot;0&quot;,&quot;transition-profile&quot;:&quot;linear&quot;};sozi.Player.prototype.onLoad=function(){var a=this.onWheel.bind(this);this.display.onLoad();this.readFrames();this.display.installTableOfContents();this.startFromIndex(this.getFrameIndexFromURL());this.display.svgRoot.addEventListener(&quot;click&quot;,this.onClick.bind(this),false);this.display.svgRoot.addEventListener(&quot;mousedown&quot;,this.onMouseDown.bind(this),false);this.display.svgRoot.addEventListener(&quot;mouseup&quot;,this.onMouseUp.bind(this),false);this.display.svgRoot.addEventListener(&quot;mousemove&quot;,this.onMouseMove.bind(this),false);this.display.svgRoot.addEventListener(&quot;keypress&quot;,this.onKeyPress.bind(this),false);this.display.svgRoot.addEventListener(&quot;keydown&quot;,this.onKeyDown.bind(this),false);window.addEventListener(&quot;hashchange&quot;,this.onHashChange.bind(this),false);window.addEventListener(&quot;resize&quot;,this.display.resize.bind(this.display),false);this.display.svgRoot.addEventListener(&quot;DOMMouseScroll&quot;,a,false);window.onmousewheel=a;this.dragButtonIsDown=false};sozi.Player.prototype.onMouseDown=function(a){if(a.button===0){this.dragButtonIsDown=true;this.dragged=false;this.dragClientX=a.clientX;this.dragClientY=a.clientY}else{if(a.button===1){if(this.display.tableOfContentsIsVisible()){this.display.hideTableOfContents()}else{this.stop();this.display.showTableOfContents()}}}a.stopPropagation()};sozi.Player.prototype.onMouseMove=function(a){if(this.dragButtonIsDown){this.stop();this.dragged=true;this.display.drag(a.clientX-this.dragClientX,a.clientY-this.dragClientY);this.dragClientX=a.clientX;this.dragClientY=a.clientY}a.stopPropagation()};sozi.Player.prototype.onMouseUp=function(a){if(a.button===0){this.dragButtonIsDown=false}a.stopPropagation()};sozi.Player.prototype.onClick=function(a){if(!this.dragged){this.moveToNext()}a.stopPropagation()};sozi.Player.prototype.onWheel=function(a){var b=0;if(!a){a=window.event}if(a.wheelDelta){b=a.wheelDelta;if(window.opera){b=-b}}else{if(a.detail){b=-a.detail}}if(b!==0){this.zoom(b)}a.stopPropagation();a.preventDefault()};sozi.Player.prototype.onKeyPress=function(a){switch(a.charCode){case 43:this.zoom(1);break;case 45:this.zoom(-1);break;case 61:this.moveToCurrent();break;case 70:case 102:this.showAll();break}a.stopPropagation()};sozi.Player.prototype.onKeyDown=function(a){switch(a.keyCode){case 36:this.moveToFirst();break;case 35:this.moveToLast();break;case 38:this.jumpToPrevious();break;case 33:case 37:this.moveToPrevious();break;case 40:this.jumpToNext();break;case 34:case 39:case 13:this.moveToNext();break;case 32:this.moveToNext();break}a.stopPropagation()};sozi.Player.prototype.onHashChange=function(){var a=this.getFrameIndexFromURL();if(a!==this.currentFrameIndex){this.moveToFrame(a)}};sozi.Player.prototype.onAnimationStep=function(c,f){var e=1-c,d=f.profile(c),b=1-d,a,g;for(a in f.initialState){if(f.initialState.hasOwnProperty(a)){if(typeof f.initialState[a]===&quot;number&quot;&amp;&amp;typeof f.finalState[a]===&quot;number&quot;){this.display[a]=f.finalState[a]*d+f.initialState[a]*b}}}if(f.zoomWidth&amp;&amp;f.zoomWidth.k!==0){g=c-f.zoomWidth.ts;this.display.width=f.zoomWidth.k*g*g+f.zoomWidth.ss}if(f.zoomHeight&amp;&amp;f.zoomHeight.k!==0){g=c-f.zoomHeight.ts;this.display.height=f.zoomHeight.k*g*g+f.zoomHeight.ss}this.display.clip=f.finalState.clip;this.display.update()};sozi.Player.prototype.onAnimationDone=function(){this.sourceFrameIndex=this.currentFrameIndex;if(this.playing){this.waitTimeout()}};sozi.Player.prototype.getFrameIndexFromURL=function(){var a=window.location.hash?parseInt(window.location.hash.slice(1),10)-1:0;if(isNaN(a)||a&lt;0){return 0}else{if(a&gt;=this.frames.length){return this.frames.length-1}else{return a}}};sozi.Player.prototype.readAttribute=function(b,a){var c=b.getAttributeNS(this.soziNs,a);return c===&quot;&quot;?this.defaults[a]:c};sozi.Player.prototype.readFrames=function(){var a=document.getElementsByTagNameNS(this.soziNs,&quot;frame&quot;),c,b,d;for(b=0;b&lt;a.length;b++){c=document.getElementById(a[b].getAttributeNS(this.soziNs,&quot;refid&quot;));if(c){d={geometry:this.display.getElementGeometry(c),title:this.readAttribute(a[b],&quot;title&quot;),sequence:parseInt(this.readAttribute(a[b],&quot;sequence&quot;),10),hide:this.readAttribute(a[b],&quot;hide&quot;)===&quot;true&quot;,timeoutEnable:this.readAttribute(a[b],&quot;timeout-enable&quot;)===&quot;true&quot;,timeoutMs:parseInt(this.readAttribute(a[b],&quot;timeout-ms&quot;),10),transitionDurationMs:parseInt(this.readAttribute(a[b],&quot;transition-duration-ms&quot;),10),transitionZoomPercent:parseInt(this.readAttribute(a[b],&quot;transition-zoom-percent&quot;),10),transitionProfile:this.profiles[this.readAttribute(a[b],&quot;transition-profile&quot;)||&quot;linear&quot;]};if(d.hide){c.setAttribute(&quot;visibility&quot;,&quot;hidden&quot;)}d.geometry.clip=this.readAttribute(a[b],&quot;clip&quot;)===&quot;true&quot;;this.frames.push(d)}}this.frames.sort(function(f,e){return f.sequence-e.sequence})};sozi.Player.prototype.startFromIndex=function(a){this.playing=true;this.waiting=false;this.sourceFrameIndex=a;this.currentFrameIndex=a;this.display.showFrame(this.frames[a]);this.waitTimeout()};sozi.Player.prototype.stop=function(){this.animator.stop();if(this.waiting){window.clearTimeout(this.nextFrameTimeout);this.waiting=false}this.playing=false;this.sourceFrameIndex=this.currentFrameIndex};sozi.Player.prototype.waitTimeout=function(){var a;if(this.frames[this.currentFrameIndex].timeoutEnable){this.waiting=true;a=(this.currentFrameIndex+1)%this.frames.length;this.nextFrameTimeout=window.setTimeout(this.moveToFrame.bind(this,a),this.frames[this.currentFrameIndex].timeoutMs)}};sozi.Player.prototype.getZoomData=function(e,m,k){var n={ss:((e&lt;0)?Math.max(m,k):Math.min(m,k))*(100-e)/100,ts:0.5,k:0},i,h,g,f,l,j;if(e!==0){i=m-k;h=m-n.ss;g=k-n.ss;if(i!==0){f=Math.sqrt(h*g);l=(h-f)/i;j=(h+f)/i;n.ts=(l&gt;0&amp;&amp;l&lt;=1)?l:j}n.k=h/n.ts/n.ts}return n};sozi.Player.prototype.jumpToFrame=function(a){this.stop();if(this.display.tableOfContentsIsVisible()){this.display.hideTableOfContents()}this.sourceFrameIndex=a;this.currentFrameIndex=a;this.display.showFrame(this.frames[a]);window.location.hash=&quot;#&quot;+(a+1)};sozi.Player.prototype.moveToFrame=function(c){var b=this.defaultDurationMs,f=this.defaultZoomPercent,d=this.defaultProfile,a,e;if(this.waiting){window.clearTimeout(this.nextFrameTimeout);this.waiting=false}if(c===(this.currentFrameIndex+1)%this.frames.length){b=this.frames[c].transitionDurationMs;f=this.frames[c].transitionZoomPercent;d=this.frames[c].transitionProfile}if(this.display.tableOfContentsIsVisible()){this.display.hideTableOfContents()}if(f!==0){a=this.getZoomData(f,this.display.width,this.frames[c].geometry.width);e=this.getZoomData(f,this.display.height,this.frames[c].geometry.height)}this.playing=true;this.currentFrameIndex=c;this.animator.start(b,{initialState:this.display.getCurrentGeometry(),finalState:this.frames[this.currentFrameIndex].geometry,profile:d,zoomWidth:a,zoomHeight:e});window.location.hash=&quot;#&quot;+(c+1)};sozi.Player.prototype.moveToFirst=function(){this.moveToFrame(0)};sozi.Player.prototype.jumpToPrevious=function(){var a=this.currentFrameIndex;if(!this.animator.started||this.sourceFrameIndex&lt;=this.currentFrameIndex){a-=1}if(a&gt;=0){this.jumpToFrame(a)}};sozi.Player.prototype.moveToPrevious=function(){var a=this.currentFrameIndex,b;for(a--;a&gt;=0;a--){b=this.frames[a];if(!b.timeoutEnable||b.timeoutMs!==0){this.moveToFrame(a);break}}};sozi.Player.prototype.jumpToNext=function(){var a=this.currentFrameIndex;if(!this.animator.started||this.sourceFrameIndex&gt;=this.currentFrameIndex){a+=1}if(a&lt;this.frames.length){this.jumpToFrame(a)}};sozi.Player.prototype.moveToNext=function(){if(this.currentFrameIndex&lt;this.frames.length-1||this.frames[this.currentFrameIndex].timeoutEnable){this.moveToFrame((this.currentFrameIndex+1)%this.frames.length)}};sozi.Player.prototype.moveToLast=function(){this.moveToFrame(this.frames.length-1)};sozi.Player.prototype.moveToCurrent=function(){this.moveToFrame(this.currentFrameIndex)};sozi.Player.prototype.showAll=function(){this.stop();if(this.display.tableOfContentsIsVisible()){this.display.hideTableOfContents()}this.animator.start(this.defaultDurationMs,{initialState:this.display.getCurrentGeometry(),finalState:this.display.getDocumentGeometry(),profile:this.defaultProfile})};sozi.Player.prototype.zoom=function(a){this.stop();if(this.display.tableOfContentsIsVisible()){this.display.hideTableOfContents()}this.display.applyZoomFactor(a&gt;0?this.scaleFactor:1/this.scaleFactor);this.display.clip=false;this.display.update()};window.addEventListener(&quot;load&quot;,sozi.Player.prototype.onLoad.bind(new sozi.Player()),false);";
      
      *m_frames << "<script id=" << "\"sozi-script\">" << m_script << "</script>" << endl;
}

      //This function will eventually be removed.
      //Only used for dummy data.
void SvgWriter_Stage::forTesting(KoShape * shape)
{
     //First save Frame properties to a shape. This is for testing only.
    SvgAnimationData  *obj = new SvgAnimationData();
    Frame *frame = new Frame(); //Default properties set 
    frame->setRefId(m_shapeIds[shape]);
    
    obj->addNewFrame(shape, frame);
}


/*
void SvgWriter_Stage::saveAppData_temp(KoShape *shape)
{
  
    if(shape->applicationData() == 0){
      forTesting(shape);// Adds a frame object to this shape
    }
        
    SvgAnimationData * appData = dynamic_cast<SvgAnimationData*>(shape->applicationData());
    Frame *frameObj;// = new Frame();
    frameObj = &(appData->frame());
          
    frameObj->writeToStream(m_frames);       
    }
  */    