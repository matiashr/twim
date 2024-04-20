#!/bin/b
for f in $1/*;do stcat $f >src/$(basename $f).st;done
