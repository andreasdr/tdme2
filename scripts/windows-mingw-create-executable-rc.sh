#!/bin/bash
cp resources/win32/app.rc $2
EXECUTABLE=`echo $1 | grep -o '[a-zA-Z0-9]*-main' | sed -e 's/\-main//'`
EXECUTABLE_LOWERCASE=`echo $1 | grep -o '[a-zA-Z0-9]*-main' | sed -e 's/\-main//' | tr '[:upper:]' '[:lower:]'`
if [ -f resources/win32/$EXECUTABLE_LOWERCASE-icon.ico ]; then
   sed -i "s/{__ICON__}/resources\/win32\/$EXECUTABLE_LOWERCASE-icon.ico/g" $2
else
   sed -i "s/{__ICON__}/resources\/win32\/default-icon.ico/g" $2
fi
sed -i "s/{__EXECUTABLE__}/$EXECUTABLE/g" $2

