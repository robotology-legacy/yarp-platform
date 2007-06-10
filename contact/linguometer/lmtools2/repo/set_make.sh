#!/bin/bash
ROOT="/home/michele/ContactCache/"
for experiment in `ls -1 $ROOT | grep "exp_"`; do
	echo "Copying Makefile to $ROOT/$experiment"
	cp Makefile $ROOT/$experiment/
done
