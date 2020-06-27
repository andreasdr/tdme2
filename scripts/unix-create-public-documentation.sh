#!/bin/sh
tail -n +6 ../README.md > README.md
rm -rf public
doxygen doxygen.public.config
rm README.md
cd public
cd html
find . > ../public.filelist.txt
cd ..
sed -e 's/^\.\//put current.uploading\//' public.filelist.txt > public.filelist-put.txt
grep "PROJECT_NUMBER" ../doxygen.public.config|sed -n -e 's/^.*PROJECT_NUMBER.*= //p' > public.version
mv html current.uploading
ftp -p -n ftp.drewke.net <<END_SCRIPT
user drewke.net PASSWORD
binary
cd tdme2-documentation
cd alpha
cd public
mkdir current.uploading
cd current.uploading
mkdir search
cd ..
`cat public.filelist-put.txt`
rename current.uploading `cat public.version`
quit
END_SCRIPT
cd ..
rm -rf public
