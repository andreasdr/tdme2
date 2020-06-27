#!/bin/sh
tail -n +6 ../README.md > README.md
rm -rf private
doxygen doxygen.private.config
rm README.md
cd private
cd html
find . > ../private.filelist.txt
cd ..
sed -e 's/^\.\//put current.uploading\//' private.filelist.txt > private.filelist-put.txt
grep "PROJECT_NUMBER" ../doxygen.private.config|sed -n -e 's/^.*PROJECT_NUMBER.*= //p' > private.version
mv html current.uploading
ftp -p -n ftp.drewke.net <<END_SCRIPT
user drewke.net PASSWORD
binary
cd tdme2-documentation
cd alpha
cd private
mkdir current.uploading
cd current.uploading
mkdir search
cd ..
`cat private.filelist-put.txt`
rename current.uploading `cat private.version`
quit
END_SCRIPT
cd ..
rm -rf private
