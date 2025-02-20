#!/bin/bash
#
#SBATCH -p wn
#SBATCH --account=t3
#SBATCH --job-name=job-name
#SBATCH --mem=3000M                     # memory 3GB (per job)
#SBATCH --time 01:00:00
#SBATCH -o %x-%j.out    # replace default slurm-SLURM_JOB_ID.out; %x is a job-name (or script name when there is no job-name)
#SBATCH -e %x-%j.err    # replace default slurm-SLURM_JOB_ID.err


echo HOME: $HOME
echo USER: $USER
echo SLURM_JOB_ID: $SLURM_JOB_ID
echo HOSTNAME: $HOSTNAME

# each worker node has local /scratch space to be used during job run
mkdir -p /scratch/$USER/${SLURM_JOB_ID}
export TMPDIR=/scratch/$USER/${SLURM_JOB_ID}

#########################################################
source /cvmfs/cms.cern.ch/cmsset_default.sh
eval $(scram ru -sh)


# Extract the part after the last '/'
outputName=$(basename "${1}")
echo ${outputName}
# Remove the '.root' extension
#outputName="${filename%.root}"
#echo ${outputName}

cmsRun ../testPhotonMVA_Run3_cfg.py sample=${1} output=output_${2}/${outputName}
#########################################################
# cleaning of temporary working dir after job is completed:
rm  -rf /scratch/$USER/${SLURM_JOB_ID}

date
