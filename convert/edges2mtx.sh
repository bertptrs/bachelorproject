#!/bin/bash

if [[ $# -lt 1 ]]
then
	echo "Usage: $0 EDGEFILE" 1>&2
	exit 1
fi

length=$(wc -l < "$1")
maxNode=$(awk '{print $1; print $2}' "$1" | sort -S 10% -nr --compress-program=gzip | head -n 1)

cat <<EOS
% Matrix Market file
% 
% Created using $0 by Bert Peters
%
% Original: $1
$maxNode $maxNode $length
EOS

cat "$1"
