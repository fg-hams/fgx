#!/bin/bash

FILES = *.so

for f in *

do
    install_name_tool -change libosg.92.dylib @executable_path/../Frameworks/libosg.92.dylib $f
    install_name_tool -change libosgSim.92.dylib @executable_path/../Frameworks/libosgSim.92.dylib $f
    install_name_tool -change libosgAnimation.92.dylib @executable_path/../Frameworks/libosgAnimation.92.dylib $f
    install_name_tool -change libosgTerrain.92.dylib @executable_path/../Frameworks/libosgTerrain.92.dylib $f
    install_name_tool -change libosgDB.92.dylib	@executable_path/../Frameworks/libosgDB.92.dylib $f
    install_name_tool -change libosgText.92.dylib @executable_path/../Frameworks/libosgText.92.dylib $f
    install_name_tool -change libosgFX.92.dylib	@executable_path/../Frameworks/libosgFX.92.dylib $f
    install_name_tool -change libosgUtil.92.dylib @executable_path/../Frameworks/libosgUtil.92.dylib $f
    install_name_tool -change libosgGA.92.dylib	@executable_path/../Frameworks/libosgGA.92.dylib $f	
    install_name_tool -change libosgViewer.92.dylib @executable_path/../Frameworks/libosgViewer.92.dylib $f
    install_name_tool -change libosgManipulator.92.dylib @executable_path/../Frameworks/libosgManipulator.92.dylib $f
    install_name_tool -change libosgVolume.92.dylib @executable_path/../Frameworks/libosgVolume.92.dylib $f
    install_name_tool -change libosgParticle.92.dylib @executable_path/../Frameworks/libosgParticle.92.dylib $f
    install_name_tool -change libosgWidget.92.dylib @executable_path/../Frameworks/libosgWidget.92.dylib $f
    install_name_tool -change libosgPresentation.92.dylib @executable_path/../Frameworks/libosgPresentation.92.dylib $f
    install_name_tool -change libosgQt.92.dylib @executable_path/../Frameworks/libosgQt.92.dylib $f
    install_name_tool -change libOpenThreads.12.dylib @executable_path/../Frameworks/libOpenThreads.12.dylib $f
    install_name_tool -change libosgShadow.92.dylib @executable_path/../Frameworks/libosgShadow.92.dylib $f
    
    otool -L $f
done



