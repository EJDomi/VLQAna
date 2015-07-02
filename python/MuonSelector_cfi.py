import FWCore.ParameterSet.Config as cms

from Analysis.VLQAna.Muon_cfi import * 

defaultMuonSelectionParameters = cms.PSet(
    defaultMuonParameters, 
    muidtype = cms.string("TIGHT"), 
    muPtMin = cms.double(30),
    muPtMax = cms.double(10000),
    muAbsEtaMax = cms.double(2.1),
    muCharge = cms.double(1.0), 
    muIsoMin = cms.double(0.0),
    muIsoMax = cms.double(0.20), 
    )

defaultMuPSelectionParamaters = defaultMuonSelectionParameters.clone(
    muCharge  = cms.double(1.0),  
    )

defaultMuMSelectionParamaters = defaultMuonSelectionParameters.clone(
    muCharge  = cms.double(-1.0),  
    )
