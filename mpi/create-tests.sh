#!/bin/bash

if [[ $# -lt 4 ]]; then
	echo "Usage: $0 source sink max_workers graphfile [implementation]"
	exit 1
fi

SOURCE=$1
SINK=$2
WORKERCOUNTS=$(seq 1 $3)
GRAPHFILE=$4
IMPLEMENTATION=${5:-1}

TESTDIR=tests
SUITENAME=${TESTDIR}/"run_$(date +"%Y%m%d-%H%M%S")"

mkdir -p "$TESTDIR"

for COUNT in $WORKERCOUNTS; do
	filename="${SUITENAME}_IMPL${IMPLEMENTATION}_${SOURCE}_${SINK}_${COUNT}_cores.sh"
	cat << EOF > "$filename"
#!/bin/bash
#$ -pe openmpi ${COUNT}
#$ -l h_rt=0:15:00
#$ -N PUSHLIFT_IMPL${IMPLEMENTATION}_${SOURCE}_${SINK}_${COUNT}
#$ -cwd

APP=./pushlift
ARGS="-s ${SINK} -t ${SOURCE} -f ${GRAPHFILE} -i ${IMPLEMENTATION}"

# Get OpenMPI settings
. /etc/bashrc
module load openmpi/gcc
module load gcc/4.9.0

# Make new hostfile specifying the cores per node wanted
ncores=8
HOSTFILE=\$TMPDIR/hosts
for host in \`uniq \$TMPDIR/machines\`; do
    echo \$host slots=\$ncores
done > \$HOSTFILE
nhosts=\`wc -l < \$HOSTFILE\`
totcores=\`expr \$nhosts \* \$ncores\`

# Use regular ssh-based startup instead of OpenMPI/SGE native one
unset PE_HOSTFILE
PATH=/usr/bin:\$PATH

echo "Running maxflow on ${COUNT} MPI workers from ${SOURCE} to ${SINK} on implementation ${IMPLEMENTATION}."

\$MPI_RUN -np \$totcores --hostfile \$HOSTFILE \$APP \$ARGS
EOF

	chmod +x $filename
	echo "Created testfile for $COUNT cores"
done
