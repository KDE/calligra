HOWTO setup the somefiletype->odf conversion and validation of the resulting odf's XML against RelaxNG

1. Install zip
2. Install jing
3. Put jing in trunk/src/calligra/tools/scripts/
4. put convertAndValidateODF.py in trunk/src/calligra/tools/scripts/
5. put CMakeList.txt in trunk/src/calligra/tools/scripts/
6. Add following to CMakeLists.txt in closest directory above scripts directory: add_subdirectory(scripts)
7. Make sure cmake-curses-gui is installed
8. Execute following in kde/build/calligra/: ccmake .
9. Go into build/calligra/tools/scripts and do make
10. Go into build/calligra/tools/scripts and do make test
 
NOTE! logs can be checked under Testing/Temporary/

