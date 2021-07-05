#!/bin/bash
set -evx
test -f libprocesshider.so

export LD_PRELOAD=$(realpath libprocesshider.so)

cwd=$(pwd)

rm -rfv tmp

mkdir tmp

## test start
echo "test start"

touch tmp/tmp1.txt
rm tmp/tmp1.txt

mkdir tmp/tmpdir.dir
rm -r tmp/tmpdir.dir

touch tmp/tmpln.file
ln -s tmp/tmpln.file tmp/tmpln.file.solflink

ln  tmp/tmpln.file tmp/tmpln.file.hardlink

ssh -V

bash --version

echo "all test pass"
