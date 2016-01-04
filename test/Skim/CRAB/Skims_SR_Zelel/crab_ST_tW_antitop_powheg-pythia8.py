from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'ST_tW_antitop_channel_25ns_SR_Zelel'
config.General.workArea = 'B2GEDMNtuplesSkim_SR_Zelel_20Nov/'
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'skim_cfg.py' 
config.JobType.pyCfgParams = ['isData=False', 'skimType=SR_Zelel']

config.section_("Data")
config.Data.inputDataset = '/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/jkarancs-B2GAnaFW_v74x_V8p4_RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1-7b09a84a5c42d0a63b01d8e8e63c7a89/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5
config.Data.ignoreLocality = False
config.Data.publication = True
config.Data.outLFNDirBase = '/store/group/lpcbprime/noreplica/skhalil/B2GEDMNtuplesSkim_SR_Zelel_20Nov/'
# This string is used to construct the output dataset name

config.section_("Site")
config.Site.storageSite = 'T3_US_FNALLPC'

config.section_('User')