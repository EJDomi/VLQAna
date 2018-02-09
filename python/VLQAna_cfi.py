import FWCore.ParameterSet.Config as cms

from Analysis.VLQAna.PickGenPart_cfi import *
from Analysis.VLQAna.JetMaker_cfi import *
from Analysis.VLQAna.ElectronMaker_cfi import *
from Analysis.VLQAna.MuonMaker_cfi import *

ana = cms.EDFilter("VLQAna", 
    runno                      = cms.InputTag("evtcleaner","runno"), 
    lumisec                    = cms.InputTag("evtcleaner","lumisec"), 
    evtno                      = cms.InputTag("evtcleaner","evtno"), 
    isData                     = cms.InputTag("evtcleaner","isData"), 
    hltdecision                = cms.InputTag("evtcleaner","hltdecision"), 
    evttype                    = cms.InputTag("evtcleaner","evttype"),
    evtwtGen                   = cms.InputTag("evtcleaner","evtwtGen"),
    evtwtPVBG                  = cms.InputTag("evtcleanerBG","evtwtPV"),
    evtwtPVH                   = cms.InputTag("evtcleanerH","evtwtPV"),
    evtwtPV                    = cms.InputTag("evtcleaner","evtwtPV"),
    evtwtPVLow                 = cms.InputTag("evtcleaner","evtwtPVLow"),
    evtwtPVHigh                = cms.InputTag("evtcleaner","evtwtPVHigh"),
    evtwtPVAlt                    = cms.InputTag("evtcleanerAlt","evtwtPV"),
    evtwtPVAltLow                 = cms.InputTag("evtcleanerAlt","evtwtPVLow"),
    evtwtPVAltHigh                = cms.InputTag("evtcleanerAlt","evtwtPVHigh"),
    npv                        = cms.InputTag("evtcleaner","npv"),
    npuTrue                    = cms.InputTag("evtcleaner","npuTrue"),
    htHat                      = cms.InputTag("evtcleaner","htHat"),
    lhewtids                   = cms.InputTag("evtcleaner","lhewtids"),
    lhewts                     = cms.InputTag("evtcleaner","lhewts"), 
    trigBit                     = cms.InputTag("evtcleaner","trigBit"), 
    trigName                     = cms.InputTag("evtcleaner","trigName"), 
    elselParams                = defaultElectronMakerParameters.clone(
      elPtMin = cms.double(50),
      applyIso = cms.bool(False), 
    elAbsEtaMax = cms.double(2.5),
      ), 
    muselParams                = defaultMuonMakerParameters.clone(
      muidtype = cms.string("MEDIUM"), 
      muPtMin = cms.double(47),
      muIsoMin = cms.double(0.00),
      muIsoMax = cms.double(1000), 
      muAbsEtaMax = cms.double(2.1),
      ), 
    jetAK4selParams            = defaultAK4JetSelectionParameters,
    jetAK8selParams            = defaultAK8CHSJetSelectionParameters,
    jetHTaggedselParams        = defaultCHSHJetSelectionParameters,
    jetAntiHTaggedselParams    = defaultCHSHJetSelectionParameters.clone(
      subjetCSVMin = cms.double(-1000000) ,
      subjetCSVMax = cms.double(1000000),
      ), 
    jetTopTaggedselParams      = defaultCHSTJetSelectionParameters,
    jetAntiTopTaggedselParams  = defaultCHSTJetSelectionParameters.clone(
      subjetHighestCSVMin = cms.double(-1000000),
      subjetHighestCSVMax = defaultCHSTJetSelectionParameters.subjetHighestCSVMin, 
      jettau3Bytau2Max    = cms.double(0.5) ,
      ),
    jetZTaggedselParams        = defaultCHSHJetSelectionParameters.clone(
      jetPrunedMassMin = cms.double(65.),
      jetPrunedMassMax = cms.double(105.),
      subjetCSVMin = cms.double(-1000000),
      jetPtMin = cms.double(200.),
      ),
    jetAntiZTaggedselParams        = defaultCHSHJetSelectionParameters.clone(
      jetPrunedMassMin = cms.double(65.),
      jetPrunedMassMax = cms.double(105.),
      subjetCSVMin = cms.double(-1000000),
      jettau2Bytau1Max = cms.double(1.0),
      jettau2Bytau1Min = cms.double(0.6),
      jetPt = cms.double(200.)
      ),
    leadingJetPtMin            = cms.double  (400.), 
    leadingJetPrunedMassMin    = cms.double  (50.), 
    HTMin                      = cms.double  (0.), 
    ExtraDRMin             = cms.double  (1.2), 
    doBTagSFUnc                = cms.bool(False), 
    storePreselEvts            = cms.bool(False), 
    doPreselOnly               = cms.bool(False),
    applyBTagSFs               = cms.bool(True),
    btageffmap                 = cms.string("TbtH_1200_LH_btagEff_loose.root"),
    btageffmapMed              = cms.string("TbtH_1200_LH_btagEff_medium.root"),
    sjbtagSFcsv                = cms.string('subjet_CSVv2_Moriond17_B_H.csv')
    )
