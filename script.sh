#! /usr/bin/bash
ls -l $1 | egrep "^-----w----" | egrep -o "[a-zA-Z,.,-,_,/,\]+$"
