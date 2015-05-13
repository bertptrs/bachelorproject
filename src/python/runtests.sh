#!/bin/bash

start() {
	echo Starting $1
	(./test.py -a "$1" -s 26345 -t 35017 cage11.mtx 2>&1) | tee "$1.log" | sendmail bert.ljpeters@gmail.com &
}

start FordFulkerson
start PushLift
start PushRelabel
start EdmondsKarp

echo "Now waiting for jobs to finish..."

for i in `jobs -p`
do
	wait "$i"
done

cat *.log | sendmail bert.ljpeters@gmail.com
