#!/bin/bash

if [ "$#" -ne 2 ]
then
  echo "Please supply all parameters"	
  echo "Usage: ./random-number-generator.sh count filename"
  exit 1
fi

shuf -i 0-$1 -n $1 > .temp

cat .temp | tr "\n" " " > $2

echo "" >> $2

chmod a-x $2

rm .temp