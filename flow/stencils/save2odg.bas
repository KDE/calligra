
' Save document as an ODF file. 
Sub SaveAsODG( cFile ) 
   ' Save as an OpenDocument Graphics file. 
   cURL = ConvertToURL( cFile )
   oDoc = StarDesktop.loadComponentFromURL( cURL, "_blank", 0, Array())

   ' Set output file extension based on lower-case 
   ' version of input extension.
   Select Case LCase(Right(cFile,5))
     Case "shape"         ' Dia stencil file.
       cFileExt = "odg"
     Case Else
       cFileExt = "xxx"
    End Select
       
   cFile = Left( cFile, Len( cFile ) - 5 ) + cFileExt
   cURL = ConvertToURL( cFile )
   
   oDoc.storeAsURL( cURL, Array() )
   oDoc.close( True )

End Sub

