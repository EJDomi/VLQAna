import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')

options.register('outFileName', 'os2lana.root',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Output file name"
    )

options.register('isData', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Is data?"
    )

options.setDefault('maxEvents', 1000)

options.parseArguments()

process = cms.Process("OS2LAna")

#from infiles_cfi import * 

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_17.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_18.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_19.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_20.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_26.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_37.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_38.root',
    'file:/afs/cern.ch/user/d/devdatta/eos/cms/store/group/phys_b2g/B2GAnaFW/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/B2GAnaFW_v74x_v6p1_25ns/150928_171042/0000/B2GEDMNtuple_41.root',

    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_1.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_10.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_11.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_12.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_13.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_14.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_15.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_16.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_17.root',
    #'/store/user/devdatta/DoubleMuon/crab_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075412/0000/B2GEDMNtuple_18.root',

    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_1.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_10.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_11.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_12.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_13.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_14.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_15.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_16.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_17.root',
    #'/store/user/devdatta/DoubleEG/crab_DoubleEG_Run2015D-PromptReco-v3_Sep26_v74x_V7_25ns/150926_075907/0000/B2GEDMNtuple_18.root',
    )
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.load("Analysis.VLQAna.HbbCandidateProducer_cfi") 

from Analysis.VLQAna.OS2LAna_cfi import * 

process.anaelel = ana.clone(
    isData = options.isData,
    hltPaths = cms.vstring (
        "HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v", 
        "HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v"
        ), 
    )

process.anamumu = ana.clone(
    isData = options.isData, 
    hltPaths = cms.vstring (
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v", 
        ), 
    )

process.anaelelBoosted = ana.clone(
    isData = options.isData,
    hltPaths = cms.vstring (
        "HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v", 
        "HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v"
        ), 
    BoostedZCandParams = defaultZCandSelectionParameters.clone(
        massMin = cms.double(75),
        massMax = cms.double(105),
        ptMin = cms.double(150.),
        ), 
    )

process.anamumuBoosted = ana.clone(
    isData = options.isData, 
    hltPaths = cms.vstring (
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v", 
        ), 
    BoostedZCandParams = defaultZCandSelectionParameters.clone(
        massMin = cms.double(75),
        massMax = cms.double(105),
        ptMin = cms.double(150.),
        ), 
    )

if not options.isData:
  process.anamumu.elpselParams.useVID = cms.bool(False)
  process.anaelel.elpselParams.useVID = cms.bool(False)
  process.anamumuBoosted.elpselParams.useVID = cms.bool(False)
  process.anaelelBoosted.elpselParams.useVID = cms.bool(False)
  process.anamumu.elmselParams.useVID = cms.bool(False)
  process.anaelel.elmselParams.useVID = cms.bool(False)
  process.anamumuBoosted.elmselParams.useVID = cms.bool(False)
  process.anaelelBoosted.elmselParams.useVID = cms.bool(False)

process.TFileService = cms.Service("TFileService",
       fileName = cms.string(
         options.outFileName
         )
       )

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("OS2LAnaEvts.root"),
    SelectEvents = cms.untracked.PSet(
      SelectEvents = cms.vstring('p')
      ),
    outputCommands = cms.untracked.vstring(
      "drop *",
      )
    )

## Event counter
from Analysis.EventCounter.eventcounter_cfi import eventCounter
process.allEvents = eventCounter.clone()
process.selectedEvents = eventCounter.clone()

process.p = cms.Path(
    process.allEvents
    *cms.ignore(process.anaelel)*cms.ignore(process.anaelelBoosted)
    *cms.ignore(process.anamumu)*cms.ignore(process.anamumuBoosted)
    * process.selectedEvents
    )
#process.outpath = cms.EndPath(process.out)
