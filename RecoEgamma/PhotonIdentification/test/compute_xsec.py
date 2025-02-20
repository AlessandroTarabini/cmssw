# Script to compute the cross section from an entire MiniAOD
# You need first to dump the list of files in a txt file using dasgoclient
# Once you have it, this is your inputFileList
import os

# parameters to EDIT
inputFilelist = "miniAODs_low.txt"
maxEvents = "-1" # You may increase maxEvents for a better estimation (e.g. set to -1)

# get inputFiles
filelist = open(inputFilelist, 'r').readlines()
inputFiles = 'inputFiles="'
for rootfile in filelist:
    if('root' in rootfile):
       inputFiles += 'root://cmsxrootd.fnal.gov/'+rootfile+','

inputFiles = inputFiles[:-1]
inputFiles += '"'
print(inputFiles)
# compute cross section
command = 'cmsRun genXsec_cfg.py {} maxEvents=-1'.format(inputFiles)
# print(command)
os.system(command)
