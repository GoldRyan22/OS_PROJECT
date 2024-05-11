#! /usr/bin/bash
ls -l $1 | egrep "^----------" | egrep -o "[a-zA-Z,.,-,_,/,\]+$"
