<?xml version="1.0"?>
<KivioShapeStencil creator="kate">
		<KivioSMLStencilSpawnerInfo>
			<Author data="Ian Reinhart Geiser"/>
			<Title data="Nand"/>
			<Id data="Nand"/>
			<Description data="Logical Nand Gate"/>
			<Version data="0.1"/>
			<Web data="www.kde.org"/>
			<Email data="geiseri@kde.org"/>
			<Copyright data="Copyright (C) 2002 Ian Reinhart Geiser"/>
			<AutoUpdate data="off"/>
		</KivioSMLStencilSpawnerInfo>
  
  	<KivioConnectorTarget x="20.0" y="00.0"/>
	<KivioConnectorTarget x="20.0" y="60.0"/>
	<KivioConnectorTarget x="10.0" y="60.0"/>
	<KivioConnectorTarget x="30.0" y="60.0"/>
	
	<Dimensions w="40.0" h="60.0" defaultAspect="1"/>
	<KivioShape type="ClosedPath" name="And">
		<KivioPoint x="20.0" y="10.0" type="bezier"/> 
		<KivioPoint x="40.0" y="10.0" type="bezier"/> 
		<KivioPoint x="40.0" y="40.0" type="bezier"/> 
		<KivioPoint x="40.0" y="60.0" type="bezier"/> 

		<KivioPoint x="40.0" y="60.0" type="bezier"/> 
		<KivioPoint x="30.0" y="60.0" type="bezier"/> 
		<KivioPoint x="20.0" y="60.0" type="bezier"/> 
		<KivioPoint x="00.0" y="60.0" type="bezier"/> 

		<KivioPoint x="00.0" y="60.0" type="bezier"/> 
		<KivioPoint x="00.0" y="30.0" type="bezier"/> 
		<KivioPoint x="00.0" y="10.0" type="bezier"/> 
		<KivioPoint x="20.0" y="10.0" type="bezier"/>
	</KivioShape>	
	<KivioShape type="Ellipse" name="Connector" x="15" y="00" w="10" h="10"/>
	<KivioShape type="TextBox" name="TextBox0"  x="0.0" y="0.0" w="40.0" h="60.0" />
</KivioShapeStencil>