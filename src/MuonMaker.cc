#include "Analysis/VLQAna/interface/MuonMaker.h" 
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;
using namespace edm ; 

MuonMaker::MuonMaker (edm::ParameterSet const& iConfig, edm::ConsumesCollector && iC) : 
  t_muCharge                   (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muChargeLabel"))), 
  t_muDxy                      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muDxyLabel"))), 
  t_muDz                       (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muDzLabel"))), 
  t_muE                        (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muELabel"))), 
  t_muEta                      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muEtaLabel"))), 
  t_muGenMuonCharge            (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGenMuonChargeLabel"))), 
  t_muGenMuonE                 (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGenMuonELabel"))), 
  t_muGenMuonEta               (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGenMuonEtaLabel"))), 
  t_muGenMuonPhi               (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGenMuonPhiLabel"))), 
  t_muGenMuonPt                (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGenMuonPtLabel"))), 
  t_muGlbTrkNormChi2           (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muGlbTrkNormChi2Label"))), 
  t_muInTrkNormChi2            (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muInTrkNormChi2Label"))), 
  t_muIsGlobalMuon             (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsGlobalMuonLabel"))), 
  t_muIsLooseMuon              (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsLooseMuonLabel"))), 
  t_muIsPFMuon                 (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsPFMuonLabel"))), 
  t_muIsSoftMuon               (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsSoftMuonLabel"))), 
  t_muIsTightMuon              (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsTightMuonLabel"))), 
  t_muIsMediumMuon              (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsMediumMuonLabel"))), 
  t_muIsTrackerMuon            (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIsTrackerMuonLabel"))), 
  t_muIso04                    (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muIso04Label"))), 
  t_muKey                      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muKeyLabel"))), 
  t_muNumberMatchedStations    (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberMatchedStationsLabel"))), 
  t_muNumberOfPixelLayers      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberOfPixelLayersLabel"))), 
  t_muNumberOfValidTrackerHits (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberOfValidTrackerHitsLabel"))), 
  t_muNumberTrackerLayers      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberTrackerLayersLabel"))), 
  t_muNumberValidMuonHits      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberValidMuonHitsLabel"))), 
  t_muNumberValidPixelHits     (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muNumberValidPixelHitsLabel"))), 
  t_muPhi                      (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muPhiLabel"))), 
  t_muPt                       (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muPtLabel"))), 
  t_muSumChargedHadronPt       (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muSumChargedHadronPtLabel"))), 
  t_muSumNeutralHadronPt       (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muSumNeutralHadronPtLabel"))), 
  t_muSumPUPt                  (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muSumPUPtLabel"))), 
  t_muSumPhotonPt              (iC.consumes<vector<float>>(iConfig.getParameter<edm::InputTag>("muSumPhotonPtLabel"))), 
  muPtMin_                     (iConfig.getParameter<double>("muPtMin")), 
  muPtMax_                     (iConfig.getParameter<double>("muPtMax")), 
  muAbsEtaMax_                 (iConfig.getParameter<double>("muAbsEtaMax")), 
  muIsoMin_                    (iConfig.getParameter<double>("muIsoMin")), 
  muIsoMax_                    (iConfig.getParameter<double>("muIsoMax"))  
{
  std::string muidtypestr = iConfig.getParameter<std::string>("muidtype") ; 
  if ( muidtypestr == "LOOSE" ) type_ = LOOSE ; 
  else if ( muidtypestr == "MEDIUM" ) type_ = MEDIUM ; 
  else if ( muidtypestr == "TIGHT" ) type_ = TIGHT ; 
  else edm::LogError("MuonMaker::MuonMaker") << " >>>> WrongMuonIdType: " << type_<< " Check muon id type !!!" ; 
}

MuonMaker::~MuonMaker () {} 

void MuonMaker::operator () (edm::Event& evt, vlq::MuonCollection& muons) { 

  Handle<vector<float>> h_muCharge                  ; evt.getByToken(t_muCharge                  ,h_muCharge                  );
  Handle<vector<float>> h_muDxy                     ; evt.getByToken(t_muDxy                     ,h_muDxy                     );
  Handle<vector<float>> h_muDz                      ; evt.getByToken(t_muDz                      ,h_muDz                      );
  Handle<vector<float>> h_muE                       ; evt.getByToken(t_muE                       ,h_muE                       );
  Handle<vector<float>> h_muEta                     ; evt.getByToken(t_muEta                     ,h_muEta                     );
  Handle<vector<float>> h_muGenMuonCharge           ; evt.getByToken(t_muGenMuonCharge           ,h_muGenMuonCharge           );
  Handle<vector<float>> h_muGenMuonE                ; evt.getByToken(t_muGenMuonE                ,h_muGenMuonE                );
  Handle<vector<float>> h_muGenMuonEta              ; evt.getByToken(t_muGenMuonEta              ,h_muGenMuonEta              );
  Handle<vector<float>> h_muGenMuonPhi              ; evt.getByToken(t_muGenMuonPhi              ,h_muGenMuonPhi              );
  Handle<vector<float>> h_muGenMuonPt               ; evt.getByToken(t_muGenMuonPt               ,h_muGenMuonPt               );
  Handle<vector<float>> h_muGlbTrkNormChi2          ; evt.getByToken(t_muGlbTrkNormChi2          ,h_muGlbTrkNormChi2          );
  Handle<vector<float>> h_muInTrkNormChi2           ; evt.getByToken(t_muInTrkNormChi2           ,h_muInTrkNormChi2           );
  Handle<vector<float>> h_muIsGlobalMuon            ; evt.getByToken(t_muIsGlobalMuon            ,h_muIsGlobalMuon            );
  Handle<vector<float>> h_muIsLooseMuon             ; evt.getByToken(t_muIsLooseMuon             ,h_muIsLooseMuon             );
  Handle<vector<float>> h_muIsPFMuon                ; evt.getByToken(t_muIsPFMuon                ,h_muIsPFMuon                );
  Handle<vector<float>> h_muIsSoftMuon              ; evt.getByToken(t_muIsSoftMuon              ,h_muIsSoftMuon              );
  Handle<vector<float>> h_muIsTightMuon             ; evt.getByToken(t_muIsTightMuon             ,h_muIsTightMuon             );
  Handle<vector<float>> h_muIsMediumMuon             ; evt.getByToken(t_muIsMediumMuon             ,h_muIsMediumMuon             );
  Handle<vector<float>> h_muIsTrackerMuon           ; evt.getByToken(t_muIsTrackerMuon           ,h_muIsTrackerMuon           );
  Handle<vector<float>> h_muIso04                   ; evt.getByToken(t_muIso04                   ,h_muIso04                   );
  Handle<vector<float>> h_muKey                     ; evt.getByToken(t_muKey                     ,h_muKey                     );
  Handle<vector<float>> h_muNumberMatchedStations   ; evt.getByToken(t_muNumberMatchedStations   ,h_muNumberMatchedStations   );
  Handle<vector<float>> h_muNumberOfPixelLayers     ; evt.getByToken(t_muNumberOfPixelLayers     ,h_muNumberOfPixelLayers     );
  Handle<vector<float>> h_muNumberOfValidTrackerHits; evt.getByToken(t_muNumberOfValidTrackerHits,h_muNumberOfValidTrackerHits);
  Handle<vector<float>> h_muNumberTrackerLayers     ; evt.getByToken(t_muNumberTrackerLayers     ,h_muNumberTrackerLayers     );
  Handle<vector<float>> h_muNumberValidMuonHits     ; evt.getByToken(t_muNumberValidMuonHits     ,h_muNumberValidMuonHits     );
  Handle<vector<float>> h_muNumberValidPixelHits    ; evt.getByToken(t_muNumberValidPixelHits    ,h_muNumberValidPixelHits    );
  Handle<vector<float>> h_muPhi                     ; evt.getByToken(t_muPhi                     ,h_muPhi                     );
  Handle<vector<float>> h_muPt                      ; evt.getByToken(t_muPt                      ,h_muPt                      );
  Handle<vector<float>> h_muSumChargedHadronPt      ; evt.getByToken(t_muSumChargedHadronPt      ,h_muSumChargedHadronPt      );
  Handle<vector<float>> h_muSumNeutralHadronPt      ; evt.getByToken(t_muSumNeutralHadronPt      ,h_muSumNeutralHadronPt      );
  Handle<vector<float>> h_muSumPUPt                 ; evt.getByToken(t_muSumPUPt                 ,h_muSumPUPt                 );
  Handle<vector<float>> h_muSumPhotonPt             ; evt.getByToken(t_muSumPhotonPt             ,h_muSumPhotonPt             );

  for (unsigned imu = 0; imu < (h_muPt.product())->size(); ++imu) {

    double muPt = (h_muPt.product())->at(imu) ; 
    double muAbsEta  = std::abs((h_muEta.product())->at(imu)) ; 
    double muIso = (h_muIso04.product())->at(imu) ; 

    bool passMuId(false) ; 
    bool isLooseMu((h_muIsLooseMuon.product())->at(imu) > 0) ; 
    bool isMediumMu((h_muIsMediumMuon.product())->at(imu) > 0) ;
    bool isTightMu((h_muIsTightMuon.product())->at(imu) > 0) ;
    if (type_ == LOOSE && isLooseMu) passMuId = true ;
    else if (type_ == MEDIUM && isMediumMu) passMuId = true ;
    else if (type_ == TIGHT && isTightMu) passMuId = true ;
    else passMuId = false;

    if ( muPt >= muPtMin_ && muPt < muPtMax_ && muAbsEta <= muAbsEtaMax_ 
        && passMuId 
        && muIso >= muIsoMin_ && muIso < muIsoMax_ 
       ) {
    vlq::Muon muon ; 
    TLorentzVector  muP4;
    muP4.SetPtEtaPhiE( (h_muPt.product())->at(imu), (h_muEta.product())->at(imu), (h_muPhi.product())->at(imu), (h_muE.product())->at(imu) ) ;
    muon.setP4                      (muP4)                                              ; 
    muon.setIndex                   (imu)                                               ;
    muon.setCharge                  ((h_muCharge.product())->at(imu))                   ;
    muon.setDxy                     ((h_muDxy.product())->at(imu))                      ;
    muon.setDz                      ((h_muDz.product())->at(imu))                       ;
    muon.setE                       ((h_muE.product())->at(imu))                        ;
    muon.setEta                     ((h_muEta.product())->at(imu))                      ;
    muon.setGenMuonCharge           ((h_muGenMuonCharge.product())->at(imu))            ;
    muon.setGenMuonE                ((h_muGenMuonE.product())->at(imu))                 ;
    muon.setGenMuonEta              ((h_muGenMuonEta.product())->at(imu))               ;
    muon.setGenMuonPhi              ((h_muGenMuonPhi.product())->at(imu))               ;
    muon.setGenMuonPt               ((h_muGenMuonPt.product())->at(imu))                ;
    muon.setGlbTrkNormChi2          ((h_muGlbTrkNormChi2.product())->at(imu))           ;
    muon.setInTrkNormChi2           ((h_muInTrkNormChi2.product())->at(imu))            ;
    muon.setIsGlobalMuon            ((h_muIsGlobalMuon.product())->at(imu))             ;
    muon.setIsLooseMuon             ((h_muIsLooseMuon.product())->at(imu))              ;
    muon.setIsPFMuon                ((h_muIsPFMuon.product())->at(imu))                 ;
    muon.setIsSoftMuon              ((h_muIsSoftMuon.product())->at(imu))               ;
    muon.setIsTightMuon             ((h_muIsTightMuon.product())->at(imu))              ;
    muon.setIsTrackerMuon           ((h_muIsTrackerMuon.product())->at(imu))            ;
    muon.setIso04                   ((h_muIso04.product())->at(imu))                    ;
    muon.setKey                     ((h_muKey.product())->at(imu))                      ;
    muon.setMass                    (muP4.Mag())                                        ;
    muon.setNumberMatchedStations   ((h_muNumberMatchedStations.product())->at(imu))    ;
    muon.setNumberOfPixelLayers     ((h_muNumberOfPixelLayers.product())->at(imu))      ;
    muon.setNumberOfValidTrackerHits((h_muNumberOfValidTrackerHits.product())->at(imu)) ;
    muon.setNumberTrackerLayers     ((h_muNumberTrackerLayers.product())->at(imu))      ;
    muon.setNumberValidMuonHits     ((h_muNumberValidMuonHits .product())->at(imu))     ;
    muon.setNumberValidPixelHits    ((h_muNumberValidPixelHits .product())->at(imu))    ;
    muon.setPhi                     ((h_muPhi .product())->at(imu))                     ;
    muon.setPt                      ((h_muPt .product())->at(imu))                      ;
    muon.setSumChargedHadronPt      ((h_muSumChargedHadronPt.product())->at(imu))       ;
    muon.setSumNeutralHadronPt      ((h_muSumNeutralHadronPt.product())->at(imu))       ;
    muon.setSumPUPt                 ((h_muSumPUPt.product())->at(imu))                  ;
    muon.setSumPhotonPt             ((h_muSumPhotonPt.product())->at(imu))              ;

    muons.push_back(muon) ; 
    }
  }

}
