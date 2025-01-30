#!/bin/bash

dasgoclient -query="file dataset=/GJet_Pt-10to40_DoubleEMEnriched_TuneCP5_13p6TeV_pythia8/Run3Winter22MiniAOD-FlatPU0to70_122X_mcRun3_2021_realistic_v9-v2/MINIAODSIM | grep file.name" > miniAODs.txt
dasgoclient -query="file dataset=/GJet_Pt-40toInf_DoubleEMEnriched_TuneCP5_13p6TeV_pythia8/Run3Winter22MiniAOD-FlatPU0to70_122X_mcRun3_2021_realistic_v9-v2/MINIAODSIM | grep file.name" >> miniAODs.txt

while IFS= read -r line; do
    sbatch -p short --account=t3 batchScript.sh "$line"
done < miniAODs.txt

rm miniAODs.txt
