<?xml version="1.0"?>
<KivioShapeStencil creator="kate">
		<KivioSMLStencilSpawnerInfo>
			<Author data="Ian Reinhart Geiser"/>
			<Title data="Nor"/>
			<Id data="Nor"/>
			<Description data="Logical Nor Gate"/>
			<Version data="0.1"/>
			<Web data="www.kde.org"/>
			<Email data="geiseri@kde.org"/>
			<Copyright data="Copyright (C) 2002 Ian Reinhart Geiser"/>
			<AutoUpdate data="off"/>
		</KivioSMLStencilSpawnerInfo>
  
       <KivioConnectorTarget  x="20" y="0"/>
	<KivioConnectorTarget  x="10" y="54"/>
	<KivioConnectorTarget  x="20" y="52.5"/>
	<KivioConnectorTarget  x="30" y="54"/>

	<Dimensions w="40" h="60" defaultAspect="1"/>
	<KivioShape type="ClosedPath" name="Or">
		<KivioPoint x="20" y="10" type="bezier"/>   
		<KivioPoint x="40" y="20" type="bezier"/>  
		<KivioPoint x="40" y="30" type="bezier"/>  
		<KivioPoint x="40" y="60" type="bezier"/>  

		<KivioPoint x="40" y="60" type="bezier"/>  
		<KivioPoint x="30" y="50" type="bezier"/>  
		<KivioPoint x="10" y="50" type="bezier"/>  
		<KivioPoint x="0" y="60" type="bezier"/>  

		<KivioPoint x="0" y="60" type="bezier"/>  
		<KivioPoint x="0" y="40" type="bezier"/>  
		<KivioPoint x="0" y="20" type="bezier"/>  
		<KivioPoint x="20" y="10" type="bezier"/> 
	</KivioShape>
	<KivioShape type="Ellipse" name="Connector" x="15" y="00" w="10" h="10"/>
	<KivioShape type="TextBox" name="TextBox0"  x="0.0" y="0.0" w="40.0" h="60.0" />
</KivioShapeStencil>