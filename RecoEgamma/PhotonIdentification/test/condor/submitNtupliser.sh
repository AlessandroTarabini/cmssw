#!/bin/bash

dasgoclient -query="file dataset=/GJet_Pt-10to40_DoubleEMEnriched_TuneCP5_13p6TeV_pythia8/Run3Winter22MiniAOD-FlatPU0to70_122X_mcRun3_2021_realistic_v9-v2/MINIAODSIM | grep file.name" > miniAODs_low.txt
dasgoclient -query="file dataset=/GJet_Pt-40toInf_DoubleEMEnriched_TuneCP5_13p6TeV_pythia8/Run3Winter22MiniAOD-FlatPU0to70_122X_mcRun3_2021_realistic_v9-v2/MINIAODSIM | grep file.name" > miniAODs_high.txt

while IFS= read -r line; do
    sbatch -p short --account=t3 batchScript.sh "$line" "high"
done < miniAODs_high.txt

while IFS= read -r line; do
    sbatch -p short --account=t3 batchScript.sh "$line" "low"
done < miniAODs_low.txt

rm miniAODs_low.txt
rm miniAODs_high.txt
