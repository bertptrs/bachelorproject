#!/bin/bash

if [[ $# -lt 4 ]]; then
	echo "Usage: $0 source sink workers graphfile [implementation]"
	exit 1
fi

SOURCE=$1
SINK=$2
COUNT=$3
GRAPHFILE=$4
IMPLEMENTATION=${5:-1}

if [ $COUNT -lt 8 ]; then
	NWORKERS=$COUNT
else
	NWORKERS=8
fi

TESTDIR=tests
SUITENAME=${TESTDIR}/"run_$(date +"%Y%m%d-%H%M%S")"

mkdir -p "$TESTDIR"

filename="${SUITENAME}_IMPL${IMPLEMENTATION}_${SOURCE}_${SINK}_${COUNT}_cores.sh"
cat << EOF > "$filename"
#!/bin/bash
#$ -pe openmpi ${COUNT}
#$ -N PUSHLIFT_IMPL${IMPLEMENTATION}_${SOURCE}_${SINK}_${COUNT}
#$ -l h_rt=2:00:00
#$ -cwd

APP=./pushlift
ARGS="-t ${SINK} -s ${SOURCE} -f ${GRAPHFILE} -i ${IMPLEMENTATION}"

# Get OpenMPI settings
. /etc/bashrc
module load openmpi/gcc
module load gcc/4.9.0

# Make new hostfile specifying the cores per node wanted
ncores=${NCORES}
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
