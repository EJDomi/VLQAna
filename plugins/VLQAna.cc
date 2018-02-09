// -*- C++ -*-
//
// Package:    Analysis/VLQAna
// Class:      VLQAna
// 
/**\class VLQAna VLQAna.cc Analysis/VLQAna/plugins/VLQAna.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Devdatta Majumder
//         Created:  Fri, 27 Feb 2015 16:09:10 GMT
//
//

#include <iostream>
#include <memory>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/BoostedObjects/interface/GenParticleWithDaughters.h"
#include "AnalysisDataFormats/BoostedObjects/interface/Jet.h"

#include "Analysis/VLQAna/interface/HT.h"
#include "Analysis/VLQAna/interface/ElectronMaker.h"
#include "Analysis/VLQAna/interface/MuonMaker.h"
#include "Analysis/VLQAna/interface/JetMaker.h"
#include "Analysis/VLQAna/interface/Utilities.h"
#include "Analysis/VLQAna/interface/CandidateCleaner.h"
#include "Analysis/VLQAna/interface/TtHTree.h"
#include "Analysis/VLQAna/interface/BTagSFUtils.h"
#include "Analysis/VLQAna/interface/BTagSFMixedUtils.h"

#include "Analysis/VLQAna/interface/EventShapeVariables.h"
#include "Analysis/VLQAna/src/EventShapeVariables.cc"

#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TGraphAsymmErrors.h>

#include <sstream>

class VLQAna : public edm::EDFilter {
  public:
    explicit VLQAna(const edm::ParameterSet&);
    ~VLQAna();

  private:
    virtual void beginJob() override;
    virtual bool filter(edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    double getHTReweightingSF(double ht, double err); 

    // ----------member data ---------------------------
    edm::EDGetTokenT<int>            t_runno      ;
    edm::EDGetTokenT<int>            t_lumisec    ;
    edm::EDGetTokenT<int>            t_evtno      ;
    edm::EDGetTokenT<bool>           t_isData     ;
    edm::EDGetTokenT<bool>           t_hltdecision;
    edm::EDGetTokenT<string>         t_evttype    ;
    edm::EDGetTokenT<double>         t_evtwtGen   ;
    edm::EDGetTokenT<double>         t_evtwtPVBG    ;
    edm::EDGetTokenT<double>         t_evtwtPVH    ;
    edm::EDGetTokenT<double>         t_evtwtPV    ;
    edm::EDGetTokenT<double>         t_evtwtPVLow ;
    edm::EDGetTokenT<double>         t_evtwtPVHigh;
    edm::EDGetTokenT<double>         t_evtwtPVAlt    ;
    edm::EDGetTokenT<double>         t_evtwtPVAltLow ;
    edm::EDGetTokenT<double>         t_evtwtPVAltHigh;
    edm::EDGetTokenT<unsigned>       t_npv        ;
    edm::EDGetTokenT<int>            t_npuTrue    ;
    edm::EDGetTokenT<double>         t_htHat      ;
    edm::EDGetTokenT<vector<int>>    t_lhewtids   ;
    edm::EDGetTokenT<vector<double>> t_lhewts     ;
    edm::EDGetTokenT<vector<double>> t_trigBit     ;
    edm::EDGetTokenT<vector<string>> t_trigName     ;

    ElectronMaker electronmaker                   ; 
    MuonMaker muonmaker                           ; 

    JetMaker jetAK4maker                          ; 
    JetMaker jetAK8maker                          ; 
    JetMaker jetHTaggedmaker                      ; 
    JetMaker jetTopTaggedmaker                    ; 
    JetMaker jetAntiTopTaggedmaker                ; 
    JetMaker jetAntiHTaggedmaker                  ;
    JetMaker jetZTaggedmaker                      ; 
    JetMaker jetAntiZTaggedmaker                    ; 
 

    double leadingJetPtMin_                       ; 
    double leadingJetPrunedMassMin_               ; 
    double HTMin_                                 ; 
    double ExtraDRMin_                                 ; 
    bool   storePreselEvts_                       ; 
    bool   doPreselOnly_                          ; 
    bool   applyBTagSFs_                          ;
    const std::string btageffmap_                  ;
    const std::string btageffmapMed_                  ;
    const std::string sjbtagSFcsv_                  ;

    edm::Service<TFileService> fs                 ; 
    std::map<std::string, TH1D*> h1_              ; 
    std::map<std::string, TH2D*> h2_              ; 

    TtHEventInfoBranches selectedevt_; 
    TtHJetInfoBranches jets_ ; 
    TtHLeptonInfoBranches leptons_ ; 
    TTree* tree_ ; 

    std::unique_ptr<BTagSFMixedUtils> btagsfmixedutils_ ; 
    std::unique_ptr<BTagSFUtils> btagsfMedutils_ ; 
    std::unique_ptr<BTagSFUtils> btagsfLooseutils_ ; 

};

using namespace std; 

VLQAna::VLQAna(const edm::ParameterSet& iConfig) :
  t_runno                 (consumes<int>            (iConfig.getParameter<edm::InputTag>("runno"))),   
  t_lumisec               (consumes<int>            (iConfig.getParameter<edm::InputTag>("lumisec"))),   
  t_evtno                 (consumes<int>            (iConfig.getParameter<edm::InputTag>("evtno"))),   
  t_isData                (consumes<bool>           (iConfig.getParameter<edm::InputTag>("isData"))),
  t_hltdecision           (consumes<bool>           (iConfig.getParameter<edm::InputTag>("hltdecision"))),
  t_evttype               (consumes<string>         (iConfig.getParameter<edm::InputTag>("evttype"))),
  t_evtwtGen              (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtGen"))),
  t_evtwtPVBG               (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVBG"))),
  t_evtwtPVH               (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVH"))),
  t_evtwtPV               (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPV"))),
  t_evtwtPVLow            (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVLow"))),
  t_evtwtPVHigh           (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVHigh"))),
  t_evtwtPVAlt               (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVAlt"))),
  t_evtwtPVAltLow            (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVAltLow"))),
  t_evtwtPVAltHigh           (consumes<double>         (iConfig.getParameter<edm::InputTag>("evtwtPVAltHigh"))),
  t_npv                   (consumes<unsigned>       (iConfig.getParameter<edm::InputTag>("npv"))),
  t_npuTrue               (consumes<int>            (iConfig.getParameter<edm::InputTag>("npuTrue"))),
  t_htHat                 (consumes<double>         (iConfig.getParameter<edm::InputTag>("htHat"))),
  t_lhewtids              (consumes<vector<int>>    (iConfig.getParameter<edm::InputTag>("lhewtids"))),
  t_lhewts                (consumes<vector<double>> (iConfig.getParameter<edm::InputTag>("lhewts"))),
  t_trigBit                (consumes<vector<double>> (iConfig.getParameter<edm::InputTag>("trigBit"))),
  t_trigName                (consumes<vector<string>> (iConfig.getParameter<edm::InputTag>("trigName"))),
  electronmaker           (iConfig.getParameter<edm::ParameterSet>("elselParams"),consumesCollector()),
  muonmaker               (iConfig.getParameter<edm::ParameterSet>("muselParams"),consumesCollector()),
  jetAK4maker             (iConfig.getParameter<edm::ParameterSet>("jetAK4selParams"), consumesCollector()), 
  jetAK8maker             (iConfig.getParameter<edm::ParameterSet>("jetAK8selParams"), consumesCollector()), 
  jetHTaggedmaker         (iConfig.getParameter<edm::ParameterSet>("jetHTaggedselParams"), consumesCollector()), 
  jetTopTaggedmaker       (iConfig.getParameter<edm::ParameterSet>("jetTopTaggedselParams"), consumesCollector()),  
  jetAntiTopTaggedmaker   (iConfig.getParameter<edm::ParameterSet>("jetAntiTopTaggedselParams"), consumesCollector()),  
  jetAntiHTaggedmaker     (iConfig.getParameter<edm::ParameterSet>("jetAntiHTaggedselParams"), consumesCollector()), 
  jetZTaggedmaker         (iConfig.getParameter<edm::ParameterSet>("jetZTaggedselParams"), consumesCollector()), 
  jetAntiZTaggedmaker     (iConfig.getParameter<edm::ParameterSet>("jetAntiZTaggedselParams"), consumesCollector()), 
  leadingJetPtMin_        (iConfig.getParameter<double>           ("leadingJetPtMin")), 
  leadingJetPrunedMassMin_(iConfig.getParameter<double>           ("leadingJetPrunedMassMin")), 
  HTMin_                  (iConfig.getParameter<double>           ("HTMin")), 
  ExtraDRMin_         (iConfig.getParameter<double>           ("ExtraDRMin")), 
  storePreselEvts_        (iConfig.getParameter<bool>             ("storePreselEvts")),
  doPreselOnly_           (iConfig.getParameter<bool>             ("doPreselOnly")), 
  applyBTagSFs_           (iConfig.getParameter<bool>             ("applyBTagSFs")),
  btageffmap_             (iConfig.getParameter<std::string>      ("btageffmap")), 
  btageffmapMed_             (iConfig.getParameter<std::string>   ("btageffmapMed")), 
  sjbtagSFcsv_             (iConfig.getParameter<std::string>     ("sjbtagSFcsv")), 
  btagsfmixedutils_            (new BTagSFMixedUtils(sjbtagSFcsv_,BTagEntry::OP_LOOSE,BTagEntry::OP_MEDIUM,30.,450.,30.,450.,20.,1000.,btageffmap_,btageffmapMed_)),
  btagsfMedutils_            (new BTagSFUtils(sjbtagSFcsv_,BTagEntry::OP_MEDIUM,30.,450.,30.,450.,20.,1000.,btageffmapMed_)),
  btagsfLooseutils_            (new BTagSFUtils(sjbtagSFcsv_,BTagEntry::OP_LOOSE,30.,450.,30.,450.,20.,1000.,btageffmap_))
{

}

VLQAna::~VLQAna() {
}

bool VLQAna::filter(edm::Event& evt, const edm::EventSetup& iSetup) {
  using namespace edm;

  Handle<int>            h_runno         ; evt.getByToken(t_runno      , h_runno      ) ; 
  Handle<int>            h_lumisec       ; evt.getByToken(t_lumisec    , h_lumisec    ) ; 
  Handle<int>            h_evtno         ; evt.getByToken(t_evtno      , h_evtno      ) ; 
  Handle<bool>           h_isData        ; evt.getByToken(t_isData     , h_isData     ) ; 
  Handle<bool>           h_hltdecision   ; evt.getByToken(t_hltdecision, h_hltdecision) ; 
  Handle<string>         h_evttype       ; evt.getByToken(t_evttype    , h_evttype    ) ; 
  Handle<double>         h_evtwtGen      ; evt.getByToken(t_evtwtGen   , h_evtwtGen   ) ; 
  Handle<double>         h_evtwtPVBG     ; evt.getByToken(t_evtwtPVBG    , h_evtwtPVBG    ) ; 
  Handle<double>         h_evtwtPVH      ; evt.getByToken(t_evtwtPVH    , h_evtwtPVH    ) ; 
  Handle<double>         h_evtwtPV       ; evt.getByToken(t_evtwtPV    , h_evtwtPV    ) ; 
  Handle<double>         h_evtwtPVLow    ; evt.getByToken(t_evtwtPVLow , h_evtwtPVLow ) ; 
  Handle<double>         h_evtwtPVHigh   ; evt.getByToken(t_evtwtPVHigh, h_evtwtPVHigh) ; 
  Handle<double>         h_evtwtPVAlt       ; evt.getByToken(t_evtwtPVAlt    , h_evtwtPVAlt    ) ; 
  Handle<double>         h_evtwtPVAltLow    ; evt.getByToken(t_evtwtPVAltLow , h_evtwtPVAltLow ) ; 
  Handle<double>         h_evtwtPVAltHigh   ; evt.getByToken(t_evtwtPVAltHigh, h_evtwtPVAltHigh) ; 
  Handle<unsigned>       h_npv           ; evt.getByToken(t_npv        , h_npv        ) ; 
  Handle<int>            h_npuTrue       ; evt.getByToken(t_npuTrue    , h_npuTrue    ) ; 
  Handle<double>         h_htHat         ; evt.getByToken(t_htHat      , h_htHat      ) ; 
  Handle<vector<int>>    h_lhewtids      ; evt.getByToken(t_lhewtids   , h_lhewtids   ) ; 
  Handle<vector<double>> h_lhewts        ; evt.getByToken(t_lhewts     , h_lhewts     ) ; 
  Handle<vector<double>> h_trigBit        ; evt.getByToken(t_trigBit     , h_trigBit     ) ; 
  Handle<vector<string>> h_trigName        ; evt.getByToken(t_trigName     , h_trigName     ) ; 

  const int runno(*h_runno.product()) ; 
  const int lumisec(*h_lumisec.product()) ; 
  const int evtno(*h_evtno.product()) ; 
  const bool isData(*h_isData.product()) ; 
  const bool hltdecision(*h_hltdecision.product()) ; 
  double evtwt((*h_evtwtGen.product()) * (*h_evtwtPV.product())) ; 
  int npv(*h_npv.product()) ; 
  const double htHat((*h_htHat.product())) ; 
  h1_["cutflow"] -> Fill(1, evtwt) ; 

  //if ( isData && !hltdecision ) return false; 
  if ( !hltdecision ) return false; 

  h1_["cutflow"] -> Fill(2, evtwt) ; 

  vlq::JetCollection goodAK4Jets;
  jetAK4maker(evt, goodAK4Jets) ;
  // Event pre-selection
  if (goodAK4Jets.size() < 4) return false ; 
  h1_["cutflow"] -> Fill(3, evtwt) ; 

  HT htak4(goodAK4Jets) ; 
  // Event pre-selection
  if (htak4.getHT() < HTMin_) return false ; 
  h1_["cutflow"] -> Fill(4, evtwt) ; 

  vlq::JetCollection goodAK8Jets; 
  jetAK8maker(evt, goodAK8Jets); 

  // Event pre-selection
  if (goodAK8Jets.size() < 2) return false ; 
  h1_["cutflow"] -> Fill(5, evtwt) ; 

  // Event pre-selection
  if (goodAK8Jets.at(0).getPt() < leadingJetPtMin_ 
      && goodAK8Jets.at(0).getPrunedMass() < leadingJetPrunedMassMin_) return false ; 
  h1_["cutflow"] -> Fill(6, evtwt) ; 

  h1_["npv_noreweight"] -> Fill(*h_npv.product(), *h_evtwtGen.product()); 
  h1_["npv"] -> Fill(*h_npv.product(), evtwt); 

  if ( storePreselEvts_ || doPreselOnly_ ) { 
    h1_["Presel_HT"] -> Fill(htak4.getHT(), evtwt) ; 

    if (goodAK8Jets.size() > 1) 
      h1_["Presel_Mjj"] -> Fill( (goodAK8Jets.at(0).getP4() + goodAK8Jets.at(1).getP4()).Mag() ) ; 
    else 
      h1_["Presel_Mjj"] -> Fill( -1 ); 

    h1_["Presel_ptAK4_0"] -> Fill (goodAK4Jets.at(0).getPt(),evtwt) ;  
    h1_["Presel_ptAK4_1"] -> Fill (goodAK4Jets.at(1).getPt(),evtwt) ;  
    h1_["Presel_ptAK4_2"] -> Fill (goodAK4Jets.at(2).getPt(),evtwt) ;  
    h1_["Presel_ptAK4_3"] -> Fill (goodAK4Jets.at(3).getPt(),evtwt) ;  
    h1_["Presel_ptAK8_0"] -> Fill (goodAK8Jets.at(0).getPt(),evtwt) ;  

    h1_["Presel_etaAK4_0"] -> Fill (goodAK4Jets.at(0).getEta(),evtwt) ;  
    h1_["Presel_etaAK4_1"] -> Fill (goodAK4Jets.at(1).getEta(),evtwt) ;  
    h1_["Presel_etaAK4_2"] -> Fill (goodAK4Jets.at(2).getEta(),evtwt) ;  
    h1_["Presel_etaAK4_3"] -> Fill (goodAK4Jets.at(3).getEta(),evtwt) ;  
    h1_["Presel_etaAK8_0"] -> Fill (goodAK8Jets.at(0).getEta(),evtwt) ;  

    h1_["Presel_NAK4"] -> Fill(goodAK4Jets.size(), evtwt) ; 
    h1_["Presel_NAK8"] -> Fill(goodAK8Jets.size(), evtwt) ; 

  }

  vlq::JetCollection goodHTaggedJets, goodTopTaggedJets, antiHTaggedJets, antiTopTaggedJets, goodZTaggedJets, antiZTaggedJets ; 
  vlq::JetCollection goodMixedHTaggedJets, mixedAntiHTaggedJets, goodMixedZTaggedJets;
  double CSVv2M = 0.8484;
  double CSVv2L = 0.5426;
 
  jetHTaggedmaker(evt, goodHTaggedJets);
  jetTopTaggedmaker(evt, goodTopTaggedJets);
  jetAntiHTaggedmaker(evt, antiHTaggedJets);
  jetAntiTopTaggedmaker(evt, antiTopTaggedJets);
  jetZTaggedmaker(evt, goodZTaggedJets);
  jetAntiZTaggedmaker(evt, antiZTaggedJets);

  for ( vlq::Jet& hjet : goodHTaggedJets){
    if (hjet.getCSVSubjet0() > CSVv2M || hjet.getCSVSubjet1() > CSVv2M) goodMixedHTaggedJets.push_back(hjet);
  }
  for ( vlq::Jet& ahjet : antiHTaggedJets){
    if ((ahjet.getCSVSubjet0() > CSVv2M && ahjet.getCSVSubjet1() < CSVv2L) || (ahjet.getCSVSubjet0() < CSVv2L && ahjet.getCSVSubjet1() > CSVv2M)) mixedAntiHTaggedJets.push_back(ahjet);
  }
  for ( vlq::Jet& zjet : goodZTaggedJets){
    if (zjet.getCSVSubjet0() > CSVv2M || zjet.getCSVSubjet1() > CSVv2M) goodMixedZTaggedJets.push_back(zjet);
  }
  unsigned nAK4      (goodAK4Jets.size());
  unsigned nAK8      (goodAK8Jets.size());
  unsigned nHiggs    (goodMixedHTaggedJets.size()); 
  unsigned nTop      (goodTopTaggedJets.size()); 
  unsigned nZ      (goodMixedZTaggedJets.size()); 
  unsigned nAntiHiggs(mixedAntiHTaggedJets.size()); 
  unsigned nAntiTop(antiTopTaggedJets.size()); 
  unsigned nAntiZ(antiZTaggedJets.size()); 


  /// Create Event Shape Varibless
  std::vector<math::XYZVector> inputVectors;
  for ( vlq::Jet& jet : goodAK4Jets ){
  	inputVectors.push_back(math::XYZVector(jet.getP4().X(), jet.getP4().Y(), jet.getP4().Z()));
  }
  vlq::EventShapeVariables eventshapes(inputVectors);
  
  double isotropy     (eventshapes.isotropy());	
  double circularity  (eventshapes.circularity());
  double sphericity   (eventshapes.sphericity());
  double aplanarity   (eventshapes.aplanarity());
  double C            (eventshapes.C());
  double D            (eventshapes.D());
  double thrust       (eventshapes.thrust());
  double thrustminor  (eventshapes.thrustminor());


  //create extras AK4 jet collection and event shape variables for said collection
  vlq::JetCollection extraAK4Jets;
  std::vector<math::XYZVector> inputVectors_extras;

  for (vlq::Jet& jet : goodAK4Jets){
    if ( jet.getP4().DeltaR(goodAK8Jets.at(0).getP4()) > ExtraDRMin_ && jet.getP4().DeltaR(goodAK8Jets.at(1).getP4()) > ExtraDRMin_) {
        extraAK4Jets.push_back(jet);
    }
  }
  for ( vlq::Jet& jet : extraAK4Jets ){
  	inputVectors_extras.push_back(math::XYZVector(jet.getP4().X(), jet.getP4().Y(), jet.getP4().Z()));
  }
  vlq::EventShapeVariables eventshapes_extras(inputVectors_extras);
  
  double isotropy_ExtraAK4     (eventshapes_extras.isotropy());	
  double circularity_ExtraAK4  (eventshapes_extras.circularity());
  double sphericity_ExtraAK4   (eventshapes_extras.sphericity());
  double aplanarity_ExtraAK4   (eventshapes_extras.aplanarity());
  double C_ExtraAK4            (eventshapes_extras.C());
  double D_ExtraAK4            (eventshapes_extras.D());
  double thrust_ExtraAK4       (eventshapes_extras.thrust());
  double thrustminor_ExtraAK4  (eventshapes_extras.thrustminor());

  double mjjExtrajj(0) ;
  TLorentzVector jjEjj;

  if (extraAK4Jets.size() > 1) {
    jjEjj = goodAK8Jets.at(0).getP4() + goodAK8Jets.at(1).getP4() + extraAK4Jets.at(0).getP4() + extraAK4Jets.at(1).getP4();
    mjjExtrajj = jjEjj.Mag();
  }


  //// Create 4 regions of the ABCD method according the the scheme below 
  //// | A: Anti-H Anti-top | B: Anti-H Good top | 
  //// | C: Good H Anti-top | D: Good H Good top | 

  bool isRegionA(false), isRegionB(false), isRegionC(false), isRegionD(false), isRegionNotABCD(false); 
  bool isRegionA_Z(false), isRegionB_Z(false), isRegionC_Z(false), isRegionD_Z(false), isRegionNotABCD_Z(false); 

  std::unique_ptr<vlq::Jet> theHiggs ;  
  std::unique_ptr<vlq::Jet> theTop ;
  std::unique_ptr<vlq::Jet> theAntiHiggs ; 
  std::unique_ptr<vlq::Jet> theAntiTop ;
  std::unique_ptr<vlq::Jet> theZ ;
  std::unique_ptr<vlq::Jet> theAntiZ ;

  if (nTop > 0) {
    for (vlq::Jet& tjet : goodTopTaggedJets) {
      if (tjet.getIsleading()) {

        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nHiggs > 0) {
          for (vlq::Jet& hjet : goodMixedHTaggedJets){ 
            if (hjet.getIs2ndleading()){
              theHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionD = true; 
            }
          }
        }
      }
      else if (tjet.getIs2ndleading()) {         
 
        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nHiggs > 0) {
          for (vlq::Jet& hjet : goodMixedHTaggedJets){ 
            if (hjet.getIsleading()){
              theHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionD = true; 
            }
          }
        }
      }

      if (tjet.getIsleading() && !isRegionD) {

        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nAntiHiggs > 0){
          for (vlq::Jet& antihjet : mixedAntiHTaggedJets){ 
            if (antihjet.getIs2ndleading()){ 
              theAntiHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionB = true;
            }
          }
        }
      }

      else if (tjet.getIs2ndleading() && !isRegionD) {        
 
        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nAntiHiggs > 0 && !isRegionD){
          for (vlq::Jet& antihjet : mixedAntiHTaggedJets){ 
            if (antihjet.getIsleading()){ 
              theAntiHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionB = true;
            }
          }
        }
      }
    }
  }

  if (nAntiTop > 0 && (!isRegionD && !isRegionB)) {
    for (vlq::Jet& antitjet : antiTopTaggedJets) {
      if (antitjet.getIsleading()) {
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nHiggs > 0) {
          for (vlq::Jet& hjet : goodMixedHTaggedJets){ 
            if (hjet.getIs2ndleading()){
              theHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionC = true; 
            }
          }
        }
      }
      else if (antitjet.getIs2ndleading()) {         
 
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet)) ;

        if (nHiggs > 0) {
          for (vlq::Jet& hjet : goodMixedHTaggedJets){ 
            if (hjet.getIsleading()){
              theHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionC = true; 
            }
          }
        }
      }


      if (antitjet.getIsleading() && !isRegionC) {
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nAntiHiggs > 0){
          for (vlq::Jet& antihjet : mixedAntiHTaggedJets){ 
            if (antihjet.getIs2ndleading()){ 
              theAntiHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionA = true;
            }
          }
        }
      }
      else if (antitjet.getIs2ndleading() && !isRegionC) {         
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nAntiHiggs > 0 && !isRegionC){
          for (vlq::Jet& antihjet : mixedAntiHTaggedJets){ 
            if (antihjet.getIsleading()){ 
              theAntiHiggs = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionA = true;
            }
          }
        }
        else {isRegionNotABCD = true;}
      }
    }
  }

//------------------------ Z Regions ------------------------//
//
  if (nTop > 0) {
    for (vlq::Jet& tjet : goodTopTaggedJets) {
      if (tjet.getIsleading()) {

        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nZ > 0) {
          for (vlq::Jet& hjet : goodMixedZTaggedJets){ 
            if (hjet.getIs2ndleading()){
              theZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionD_Z = true; 
            }
          }
        }
      }
      else if (tjet.getIs2ndleading()) {         
 
        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nZ > 0) {
          for (vlq::Jet& hjet : goodMixedZTaggedJets){ 
            if (hjet.getIsleading()){
              theZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionD_Z = true; 
            }
          }
        }
      }

      if (tjet.getIsleading() && !isRegionD_Z) {

        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nAntiZ > 0){
          for (vlq::Jet& antihjet : antiZTaggedJets){ 
            if (antihjet.getIs2ndleading()){ 
              theAntiZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionB_Z = true;
            }
          }
        }
      }

      else if (tjet.getIs2ndleading() && !isRegionD_Z) {        
 
        theTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(tjet)) ;

        if (nAntiZ > 0 && !isRegionD_Z){
          for (vlq::Jet& antihjet : antiZTaggedJets){ 
            if (antihjet.getIsleading()){ 
              theAntiZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionB_Z = true;
            }
          }
        }
      }
    }
  }

  if (nAntiTop > 0 && (!isRegionD_Z && !isRegionB_Z)) {
    for (vlq::Jet& antitjet : antiTopTaggedJets) {
      if (antitjet.getIsleading()) {
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nZ > 0) {
          for (vlq::Jet& hjet : goodMixedZTaggedJets){ 
            if (hjet.getIs2ndleading()){
              theZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionC_Z = true; 
            }
          }
        }
      }
      else if (antitjet.getIs2ndleading()) {         
 
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet)) ;

        if (nZ > 0) {
          for (vlq::Jet& hjet : goodMixedZTaggedJets){ 
            if (hjet.getIsleading()){
              theZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(hjet));
              isRegionC_Z = true; 
            }
          }
        }
      }


      if (antitjet.getIsleading() && !isRegionC_Z) {
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nAntiZ > 0){
          for (vlq::Jet& antihjet : antiZTaggedJets){ 
            if (antihjet.getIs2ndleading()){ 
              theAntiZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionA_Z = true;
            }
          }
        }
      }
      else if (antitjet.getIs2ndleading() && !isRegionC_Z) {         
        theAntiTop = std::unique_ptr<vlq::Jet>(new vlq::Jet(antitjet));

        if (nAntiZ > 0 && !isRegionC_Z){
          for (vlq::Jet& antihjet : antiZTaggedJets){ 
            if (antihjet.getIsleading()){ 
              theAntiZ = std::unique_ptr<vlq::Jet>(new vlq::Jet(antihjet)); 
              isRegionA_Z = true;
            }
          }
        }
        else {isRegionNotABCD_Z = true;}
      }
    }
  }


  if ( !storePreselEvts_ && !isRegionA && !isRegionB && !isRegionC && !isRegionD && !isRegionA_Z && !isRegionB_Z && !isRegionC_Z && !isRegionD_Z ) return false ; 

  bool isOneOfABCD(isRegionA ^ isRegionB ^ isRegionC ^ isRegionD) ;
  if ( !storePreselEvts_ && isOneOfABCD == false) edm::LogInfo("ERROR ABCD") << ">>>> Check ABCD logic: Only one of A, B C, D should be true\n" ; 

  if ( theHiggs != nullptr ) h1_["cutflow"] -> Fill(7, evtwt) ; 
  if ( theTop != nullptr ) h1_["cutflow"] -> Fill(8, evtwt) ; 
  if ( isRegionA ) h1_["cutflow"] -> Fill(9, evtwt) ; 
  if ( isRegionB ) h1_["cutflow"] -> Fill(10, evtwt) ; 
  if ( isRegionC ) h1_["cutflow"] -> Fill(11, evtwt) ; 
  if ( isRegionD ) h1_["cutflow"] -> Fill(12, evtwt) ; 

  TLorentzVector p4_tprime, p4_TprimeDummy, p4_TprimeDummyA, p4_TprimeDummyC;
  double Mtprime(0), Mtprime_corr(0), MtprimeDummy(0), MtprimeDummy_corr(0), MtprimeDummyA(0), MtprimeDummyA_corr(0), MtprimeDummyC(0), MtprimeDummyC_corr(0) ;
  TLorentzVector p4COMTop;
  TVector3 boostedT; 
  double cosThetaT(-10);
 
  if (isRegionD) {
    p4_tprime = theTop->getP4() + theHiggs->getP4() ; 
    Mtprime = p4_tprime.Mag();
    Mtprime_corr = Mtprime - theTop->getMass() - theHiggs->getMass() + 172.5 + 125. ; 
    
    boostedT = p4_tprime.BoostVector();
    p4COMTop = theTop->getP4();
    p4COMTop.Boost(-1 * boostedT); 
    cosThetaT = p4COMTop.CosTheta();
  }
  else if (isRegionB) {
    p4_TprimeDummy = theTop->getP4() + theAntiHiggs->getP4() ; 
    MtprimeDummy = p4_TprimeDummy.Mag() ; 
    MtprimeDummy_corr = MtprimeDummy - theTop->getMass() - theAntiHiggs->getMass() + 172.5 + 125. ;
 
    boostedT = p4_TprimeDummy.BoostVector();
    p4COMTop = theTop->getP4();
    p4COMTop.Boost(-1 * boostedT); 
    cosThetaT = p4COMTop.CosTheta();
  }
  else if (isRegionA) {
    p4_TprimeDummyA = theAntiTop->getP4() + theAntiHiggs->getP4() ; 
    MtprimeDummyA = p4_TprimeDummyA.Mag() ; 
    MtprimeDummyA_corr = MtprimeDummyA - theAntiTop->getMass() - theAntiHiggs->getMass() + 172.5 + 125. ; 

    boostedT = p4_TprimeDummyA.BoostVector();
    p4COMTop = theAntiTop->getP4();
    p4COMTop.Boost(-1 * boostedT); 
    cosThetaT = p4COMTop.CosTheta();
  }
  else if (isRegionC) {
    p4_TprimeDummyC = theAntiTop->getP4() + theHiggs->getP4() ; 
    MtprimeDummyC = p4_TprimeDummyC.Mag() ; 
    MtprimeDummyC_corr = MtprimeDummyC - theAntiTop->getMass() - theHiggs->getMass() + 172.5 + 125. ; 

    boostedT = p4_TprimeDummyC.BoostVector();
    p4COMTop = theAntiTop->getP4();
    p4COMTop.Boost(-1 * boostedT); 
    cosThetaT = p4COMTop.CosTheta();
  }
 
  double evtwtHT(1), evtwtHTUp(1), evtwtHTDown(1);
  double toptagsf(1), toptagsfUp(1), toptagsfDown(1);
  double btagsf(1), btagsf_bcUp(1), btagsf_bcDown(1), btagsf_lUp(1), btagsf_lDown(1) ; 
  double btagsfMed(1), btagsfMed_bcUp(1), btagsfMed_bcDown(1), btagsfMed_lUp(1), btagsfMed_lDown(1) ; 
  double btagsfZ(1), btagsfZ_bcUp(1), btagsfZ_bcDown(1), btagsfZ_lUp(1), btagsfZ_lDown(1) ; 
  double btagsfLoose(1), btagsfLoose_bcUp(1), btagsfLoose_bcDown(1), btagsfLoose_lUp(1), btagsfLoose_lDown(1) ; 

  if ( !isData ) { 

    evtwtHT = getHTReweightingSF(htak4.getHT(), 0) ; 
    evtwtHTUp = getHTReweightingSF(htak4.getHT(), 1) ; 
    evtwtHTDown = getHTReweightingSF(htak4.getHT(), -1) ; 

    std::vector<double> sjcsvs(0) ; 
    std::vector<double> sjpts (0) ; 
    std::vector<double> sjetas(0) ; 
    std::vector<int> sjfls (0) ; 

    std::vector<double> topsjcsvs(0) ; 
    std::vector<double> topsjpts (0) ; 
    std::vector<double> topsjetas(0) ; 
    std::vector<int> topsjfls (0) ; 

    std::vector<double> Zsjcsvs(0) ; 
    std::vector<double> Zsjpts (0) ; 
    std::vector<double> Zsjetas(0) ; 
    std::vector<int> Zsjfls (0) ;

    std::vector<double> antiZsjcsvs(0) ; 
    std::vector<double> antiZsjpts (0) ; 
    std::vector<double> antiZsjetas(0) ; 
    std::vector<int> antiZsjfls (0) ;
 
    //// This needs to be revisied when the top tag WP is updated
    if ( theTop != nullptr ){
      if ( theTop->getPt() >= 400.) {
        toptagsf = 1.07 ;
        toptagsfUp = 1.12 ; 
        toptagsfDown = 1.04;
      }
      topsjcsvs.push_back( (theTop->getCSVSubjet0()) ); 
      topsjpts .push_back( (theTop->getPtSubjet0()) ); 
      topsjetas.push_back( (theTop->getEtaSubjet0()) ); 
      topsjfls .push_back( (theTop->getHadronFlavourSubjet0()) ); 
      topsjcsvs.push_back( (theTop->getCSVSubjet1()) ); 
      topsjpts .push_back( (theTop->getPtSubjet1()) ); 
      topsjetas.push_back( (theTop->getEtaSubjet1()) ); 
      topsjfls .push_back( (theTop->getHadronFlavourSubjet1()) ); 
    }
    if ( theAntiTop != nullptr ){
      topsjcsvs.push_back( (theAntiTop->getCSVSubjet0()) ); 
      topsjpts .push_back( (theAntiTop->getPtSubjet0()) ); 
      topsjetas.push_back( (theAntiTop->getEtaSubjet0()) ); 
      topsjfls .push_back( (theAntiTop->getHadronFlavourSubjet0()) ); 
      topsjcsvs.push_back( (theAntiTop->getCSVSubjet1()) ); 
      topsjpts .push_back( (theAntiTop->getPtSubjet1()) ); 
      topsjetas.push_back( (theAntiTop->getEtaSubjet1()) ); 
      topsjfls .push_back( (theAntiTop->getHadronFlavourSubjet1()) ); 
    }

    if ( theHiggs != nullptr ) {
      sjcsvs.push_back( (theHiggs->getCSVSubjet0()) ); 
      sjpts .push_back( (theHiggs->getPtSubjet0()) ); 
      sjetas.push_back( (theHiggs->getEtaSubjet0()) ); 
      sjfls .push_back( (theHiggs->getHadronFlavourSubjet0()) ); 
      sjcsvs.push_back( (theHiggs->getCSVSubjet1()) ); 
      sjpts .push_back( (theHiggs->getPtSubjet1()) ); 
      sjetas.push_back( (theHiggs->getEtaSubjet1()) ); 
      sjfls .push_back( (theHiggs->getHadronFlavourSubjet1()) ); 
    }
    else if ( theAntiHiggs != nullptr ) {
      sjcsvs.push_back( (theAntiHiggs->getCSVSubjet0()) ); 
      sjpts .push_back( (theAntiHiggs->getPtSubjet0()) ); 
      sjetas.push_back( (theAntiHiggs->getPtSubjet0()) ); 
      sjfls .push_back( (theAntiHiggs->getHadronFlavourSubjet0()) ); 
      sjcsvs.push_back( (theAntiHiggs->getCSVSubjet1()) ); 
      sjpts .push_back( (theAntiHiggs->getPtSubjet1()) ); 
      sjetas.push_back( (theAntiHiggs->getPtSubjet1()) ) ; 
      sjfls .push_back( (theAntiHiggs->getHadronFlavourSubjet1()) ); 
    }

    if ( theZ != nullptr ) {
      Zsjcsvs.push_back( (theZ->getCSVSubjet0()) ); 
      Zsjpts .push_back( (theZ->getPtSubjet0()) ); 
      Zsjetas.push_back( (theZ->getEtaSubjet0()) ); 
      Zsjfls .push_back( (theZ->getHadronFlavourSubjet0()) ); 
      Zsjcsvs.push_back( (theZ->getCSVSubjet1()) ); 
      Zsjpts .push_back( (theZ->getPtSubjet1()) ); 
      Zsjetas.push_back( (theZ->getEtaSubjet1()) ); 
      Zsjfls .push_back( (theZ->getHadronFlavourSubjet1()) ); 
    }
    else if ( theAntiZ != nullptr ) {
      antiZsjcsvs.push_back( (theAntiZ->getCSVSubjet0()) ); 
      antiZsjpts .push_back( (theAntiZ->getPtSubjet0()) ); 
      antiZsjetas.push_back( (theAntiZ->getPtSubjet0()) ); 
      antiZsjfls .push_back( (theAntiZ->getHadronFlavourSubjet0()) ); 
      antiZsjcsvs.push_back( (theAntiZ->getCSVSubjet1()) ); 
      antiZsjpts .push_back( (theAntiZ->getPtSubjet1()) ); 
      antiZsjetas.push_back( (theAntiZ->getPtSubjet1()) ) ; 
      antiZsjfls .push_back( (theAntiZ->getHadronFlavourSubjet1()) ); 
    }
    ////// Get btag SFs
    if (applyBTagSFs_ && (theHiggs != nullptr || theAntiHiggs != nullptr )) btagsfmixedutils_-> getBTagSFs(sjcsvs, sjpts, sjetas, sjfls, CSVv2L,CSVv2M, btagsf, btagsf_bcUp, btagsf_bcDown, btagsf_lUp, btagsf_lDown) ; 
    if (applyBTagSFs_ && (theZ != nullptr)) btagsfmixedutils_-> getBTagSFs(Zsjcsvs, Zsjpts, Zsjetas, Zsjfls, CSVv2L,CSVv2M, btagsfZ, btagsfZ_bcUp, btagsfZ_bcDown, btagsfZ_lUp, btagsfZ_lDown) ; 
    if (applyBTagSFs_ && (theTop != nullptr || theAntiTop != nullptr)) btagsfMedutils_-> getBTagSFs(topsjcsvs, topsjpts, topsjetas, topsjfls, jetTopTaggedmaker.idxsjCSVMin_, btagsfMed, btagsfMed_bcUp, btagsfMed_bcDown, btagsfMed_lUp, btagsfMed_lDown) ; 
    if (applyBTagSFs_ && (theAntiZ != nullptr)) btagsfLooseutils_-> getBTagSFs(antiZsjcsvs, antiZsjpts, antiZsjetas, antiZsjfls, jetAntiZTaggedmaker.idxsjCSVMax_, btagsfLoose, btagsfLoose_bcUp, btagsfLoose_bcDown, btagsfLoose_lUp, btagsfLoose_lDown) ; 

  } //// if !isData

  if ( isRegionB ) {
    h1_["RegB_HT"] -> Fill(htak4.getHT(), evtwt) ; 
    h1_["RegB_HT_wts"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf) ; 
    h1_["RegB_HT_btagsf_bcUp"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegB_HT_btagsf_bcDown"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegB_HT_btagsf_lUp"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegB_HT_btagsf_lDown"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegB_HT_toptagsfUp"] -> Fill(htak4.getHT(), evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_HT_toptagsfDown"] -> Fill(htak4.getHT(), evtwt*toptagsfDown*btagsf) ; 
    h1_["RegB_HT_htwtUp"] -> Fill(htak4.getHT(), evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_HT_htwtDown"] -> Fill(htak4.getHT(), evtwt*toptagsfDown*btagsf) ; 

    h1_["RegB_mtprime"] -> Fill(MtprimeDummy, evtwt) ; 
    h1_["RegB_mtprime_wts"] -> Fill(MtprimeDummy, evtwt*toptagsf*btagsf) ; 
    h1_["RegB_mtprime_btagsf_bcUp"] -> Fill(MtprimeDummy, evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegB_mtprime_btagsf_bcDown"] -> Fill(MtprimeDummy, evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegB_mtprime_btagsf_lUp"] -> Fill(MtprimeDummy, evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegB_mtprime_btagsf_lDown"] -> Fill(MtprimeDummy, evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegB_mtprime_toptagsfUp"] -> Fill(MtprimeDummy, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_mtprime_toptagsfDown"] -> Fill(MtprimeDummy, evtwt*toptagsfDown*btagsf) ; 
    h1_["RegB_mtprime_htwtUp"] -> Fill(MtprimeDummy, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_mtprime_htwtDown"] -> Fill(MtprimeDummy, evtwt*toptagsfDown*btagsf) ; 

    h1_["RegB_mtprime_corr"] -> Fill(MtprimeDummy_corr, evtwt) ; 
    h1_["RegB_mtprime_corr_wts"] -> Fill(MtprimeDummy_corr, evtwt*toptagsf*btagsf) ; 
    h1_["RegB_mtprime_corr_btagsf_bcUp"] -> Fill(MtprimeDummy_corr, evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegB_mtprime_corr_btagsf_bcDown"] -> Fill(MtprimeDummy_corr, evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegB_mtprime_corr_btagsf_lUp"] -> Fill(MtprimeDummy_corr, evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegB_mtprime_corr_btagsf_lDown"] -> Fill(MtprimeDummy_corr, evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegB_mtprime_corr_toptagsfUp"] -> Fill(MtprimeDummy_corr, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_mtprime_corr_toptagsfDown"] -> Fill(MtprimeDummy_corr, evtwt*toptagsfDown*btagsf) ; 
    h1_["RegB_mtprime_corr_htwtUp"] -> Fill(MtprimeDummy_corr, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegB_mtprime_corr_htwtDown"] -> Fill(MtprimeDummy_corr, evtwt*toptagsfDown*btagsf) ; 
  }

  if ( isRegionD ) {
    h1_["RegD_HT"] -> Fill(htak4.getHT(), evtwt) ; 
    h1_["RegD_HT_wts"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf) ; 
    h1_["RegD_HT_btagsf_bcUp"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegD_HT_btagsf_bcDown"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegD_HT_btagsf_lUp"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegD_HT_btagsf_lDown"] -> Fill(htak4.getHT(), evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegD_HT_toptagsfUp"] -> Fill(htak4.getHT(), evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_HT_toptagsfDown"] -> Fill(htak4.getHT(), evtwt*toptagsfDown*btagsf) ; 
    h1_["RegD_HT_htwtUp"] -> Fill(htak4.getHT(), evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_HT_htwtDown"] -> Fill(htak4.getHT(), evtwt*toptagsfDown*btagsf) ; 

    h1_["RegD_mtprime"] -> Fill(Mtprime, evtwt) ; 
    h1_["RegD_mtprime_wts"] -> Fill(Mtprime, evtwt*toptagsf*btagsf) ; 
    h1_["RegD_mtprime_btagsf_bcUp"] -> Fill(Mtprime, evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegD_mtprime_btagsf_bcDown"] -> Fill(Mtprime, evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegD_mtprime_btagsf_lUp"] -> Fill(Mtprime, evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegD_mtprime_btagsf_lDown"] -> Fill(Mtprime, evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegD_mtprime_toptagsfUp"] -> Fill(Mtprime, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_mtprime_toptagsfDown"] -> Fill(Mtprime, evtwt*toptagsfDown*btagsf) ; 
    h1_["RegD_mtprime_htwtUp"] -> Fill(Mtprime, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_mtprime_htwtDown"] -> Fill(Mtprime, evtwt*toptagsfDown*btagsf) ; 

    h1_["RegD_mtprime_corr"] -> Fill(Mtprime_corr, evtwt) ; 
    h1_["RegD_mtprime_corr_wts"] -> Fill(Mtprime_corr, evtwt*toptagsf*btagsf) ; 
    h1_["RegD_mtprime_corr_btagsf_bcUp"] -> Fill(Mtprime_corr, evtwt*toptagsf*btagsf_bcUp) ; 
    h1_["RegD_mtprime_corr_btagsf_bcDown"] -> Fill(Mtprime_corr, evtwt*toptagsf*btagsf_bcDown) ; 
    h1_["RegD_mtprime_corr_btagsf_lUp"] -> Fill(Mtprime_corr, evtwt*toptagsf*btagsf_lUp) ; 
    h1_["RegD_mtprime_corr_btagsf_lDown"] -> Fill(Mtprime_corr, evtwt*toptagsf*btagsf_lDown) ; 
    h1_["RegD_mtprime_corr_toptagsfUp"] -> Fill(Mtprime_corr, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_mtprime_corr_toptagsfDown"] -> Fill(Mtprime_corr, evtwt*toptagsfDown*btagsf) ; 
    h1_["RegD_mtprime_corr_htwtUp"] -> Fill(Mtprime_corr, evtwt*toptagsfUp*btagsf) ; 
    h1_["RegD_mtprime_corr_htwtDown"] -> Fill(Mtprime_corr, evtwt*toptagsfDown*btagsf) ; 
  }

  std::vector<std::pair<int, double>> lhe_ids_wts;
  for (auto idx : index(*h_lhewtids.product()) ) {
    int id = (*h_lhewtids.product()).at(idx.first) ; 
    double wt = (*h_lhewts.product()).at(idx.first) ; 
    lhe_ids_wts.push_back(std::make_pair(id, wt)) ; 
  }

  selectedevt_.runno_ = int(runno);
  selectedevt_.lumisec_ = int(lumisec);
  selectedevt_.evtno_ = int(evtno);
  selectedevt_.EvtWeight_ = double(*h_evtwtGen.product());
  selectedevt_.EvtWtPVBG_ = double(*h_evtwtPVBG.product()) ; 
  selectedevt_.EvtWtPVH_ = double(*h_evtwtPVH.product()) ; 
  selectedevt_.EvtWtPV_ = double(*h_evtwtPV.product()) ; 
  selectedevt_.EvtWtPVLow_ = double(*h_evtwtPVLow.product()) ; 
  selectedevt_.EvtWtPVHigh_ = double(*h_evtwtPVHigh.product()) ; 
  selectedevt_.EvtWtPVAlt_ = double(*h_evtwtPVAlt.product()) ; 
  selectedevt_.EvtWtPVAltLow_ = double(*h_evtwtPVAltLow.product()) ; 
  selectedevt_.EvtWtPVAltHigh_ = double(*h_evtwtPVAltHigh.product()) ; 
  selectedevt_.EvtWtHT_ = evtwtHT;
  selectedevt_.EvtWtHTUp_ = evtwtHTUp;
  selectedevt_.EvtWtHTDown_ = evtwtHTDown;
  selectedevt_.npv_ = npv ; 
  selectedevt_.npuTrue_ = int(*h_npuTrue.product()) ; 
  selectedevt_.toptagsf_ = toptagsf ; 
  selectedevt_.toptagsf_Up_ = toptagsfUp ; 
  selectedevt_.toptagsf_Down_ = toptagsfDown ; 
  selectedevt_.btagsf_ = btagsf ; 
  selectedevt_.btagsf_bcUp_ = btagsf_bcUp ; 
  selectedevt_.btagsf_bcDown_ = btagsf_bcDown ; 
  selectedevt_.btagsf_lUp_ = btagsf_lUp ; 
  selectedevt_.btagsf_lDown_ = btagsf_lDown ; 
  selectedevt_.btagsfMed_ = btagsfMed ; 
  selectedevt_.btagsfMed_bcUp_ = btagsfMed_bcUp ; 
  selectedevt_.btagsfMed_bcDown_ = btagsfMed_bcDown ; 
  selectedevt_.btagsfMed_lUp_ = btagsfMed_lUp ; 
  selectedevt_.btagsfMed_lDown_ = btagsfMed_lDown ; 
  selectedevt_.btagsfZ_ = btagsfZ ; 
  selectedevt_.btagsfZ_bcUp_ = btagsfZ_bcUp ; 
  selectedevt_.btagsfZ_bcDown_ = btagsfZ_bcDown ; 
  selectedevt_.btagsfZ_lUp_ = btagsfZ_lUp ; 
  selectedevt_.btagsfZ_lDown_ = btagsfZ_lDown ; 
  selectedevt_.btagsfLoose_ = btagsfLoose ; 
  selectedevt_.btagsfLoose_bcUp_ = btagsfLoose_bcUp ; 
  selectedevt_.btagsfLoose_bcDown_ = btagsfLoose_bcDown ; 
  selectedevt_.btagsfLoose_lUp_ = btagsfLoose_lUp ; 
  selectedevt_.btagsfLoose_lDown_ = btagsfLoose_lDown ; 
  selectedevt_.mjjExtrajj_ = mjjExtrajj ;
  selectedevt_.mtprime_ = Mtprime ;
  selectedevt_.mtprimeDummy_ = MtprimeDummy ;
  selectedevt_.mtprimeDummyA_ = MtprimeDummyA ;
  selectedevt_.mtprimeDummyC_ = MtprimeDummyC ;
  selectedevt_.mtprime_corr_ = Mtprime_corr ;
  selectedevt_.mtprimeDummy_corr_ = MtprimeDummy_corr ;
  selectedevt_.mtprimeDummyA_corr_ = MtprimeDummyA_corr ;
  selectedevt_.mtprimeDummyC_corr_ = MtprimeDummyC_corr ;
  selectedevt_.ht_ = htak4.getHT();
  selectedevt_.nAK4_ = int(nAK4);
  selectedevt_.nAK8_ = int(nAK8);
  selectedevt_.isRegionA_ = isRegionA ; 
  selectedevt_.isRegionB_ = isRegionB ; 
  selectedevt_.isRegionC_ = isRegionC ; 
  selectedevt_.isRegionD_ = isRegionD ; 
  selectedevt_.isRegionNotABCD_ = isRegionNotABCD;
  selectedevt_.isRegionA_Z_ = isRegionA_Z ; 
  selectedevt_.isRegionB_Z_ = isRegionB_Z ; 
  selectedevt_.isRegionC_Z_ = isRegionC_Z ; 
  selectedevt_.isRegionD_Z_ = isRegionD_Z ; 
  selectedevt_.isRegionNotABCD_Z_ = isRegionNotABCD_Z;
  selectedevt_.lhewts_ = lhe_ids_wts;
  selectedevt_.trigBit_ = *h_trigBit.product();
  selectedevt_.trigName_ = *h_trigName.product();
  selectedevt_.htHat_ = htHat ;
  selectedevt_.isotropy_ = isotropy ;
  selectedevt_.circularity_ = circularity ;
  selectedevt_.sphericity_ = sphericity ;
  selectedevt_.aplanarity_ = aplanarity ;
  selectedevt_.C_ = C ;
  selectedevt_.D_ = D ;
  selectedevt_.thrust_ = thrust ;
  selectedevt_.thrustminor_ = thrustminor ; 
  selectedevt_.isotropy_ExtraAK4_ = isotropy_ExtraAK4 ;
  selectedevt_.circularity_ExtraAK4_ = circularity_ExtraAK4 ;
  selectedevt_.sphericity_ExtraAK4_ = sphericity_ExtraAK4 ;
  selectedevt_.aplanarity_ExtraAK4_ = aplanarity_ExtraAK4 ;
  selectedevt_.C_ExtraAK4_ = C_ExtraAK4 ;
  selectedevt_.D_ExtraAK4_ = D_ExtraAK4 ;
  selectedevt_.thrust_ExtraAK4_ = thrust_ExtraAK4 ;
  selectedevt_.thrustminor_ExtraAK4_ = thrustminor_ExtraAK4 ; 
  selectedevt_.cosThetaT_ = cosThetaT; 

  jets_.idxAK4             .clear() ; jets_.idxAK4             .reserve(goodAK4Jets.size()) ;   
  jets_.ptAK4              .clear() ; jets_.ptAK4              .reserve(goodAK4Jets.size()) ; 
  jets_.etaAK4             .clear() ; jets_.etaAK4             .reserve(goodAK4Jets.size()) ;   
  jets_.phiAK4             .clear() ; jets_.phiAK4             .reserve(goodAK4Jets.size()) ;   
  jets_.MAK4               .clear() ; jets_.MAK4               .reserve(goodAK4Jets.size()) ;   
  jets_.csvAK4             .clear() ; jets_.csvAK4             .reserve(goodAK4Jets.size()) ;   
  jets_.partonFlavourAK4   .clear() ; jets_.partonFlavourAK4   .reserve(goodAK4Jets.size()) ;   
  jets_.hadronFlavourAK4   .clear() ; jets_.hadronFlavourAK4   .reserve(goodAK4Jets.size()) ;   

  for (vlq::Jet jet : goodAK4Jets) {
    jets_.idxAK4             .push_back(jet.getIndex()) ; 
    jets_.ptAK4              .push_back(jet.getPt()) ; 
    jets_.etaAK4             .push_back(jet.getEta()) ; 
    jets_.phiAK4             .push_back(jet.getPhi()) ; 
    jets_.MAK4               .push_back(jet.getMass()) ; 
    jets_.csvAK4             .push_back(jet.getCSV());
    jets_.partonFlavourAK4   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourAK4   .push_back(jet.getHadronFlavour());
  }

  jets_.idxExtraAK4             .clear() ; jets_.idxExtraAK4             .reserve(extraAK4Jets.size()) ;   
  jets_.ptExtraAK4              .clear() ; jets_.ptExtraAK4              .reserve(extraAK4Jets.size()) ; 
  jets_.etaExtraAK4             .clear() ; jets_.etaExtraAK4             .reserve(extraAK4Jets.size()) ;   
  jets_.phiExtraAK4             .clear() ; jets_.phiExtraAK4             .reserve(extraAK4Jets.size()) ;   
  jets_.MExtraAK4               .clear() ; jets_.MExtraAK4               .reserve(extraAK4Jets.size()) ;   
  jets_.csvExtraAK4             .clear() ; jets_.csvExtraAK4             .reserve(extraAK4Jets.size()) ;   
  jets_.partonFlavourExtraAK4   .clear() ; jets_.partonFlavourExtraAK4   .reserve(extraAK4Jets.size()) ;   
  jets_.hadronFlavourExtraAK4   .clear() ; jets_.hadronFlavourExtraAK4   .reserve(extraAK4Jets.size()) ;   

  for (vlq::Jet jet : extraAK4Jets) {
      jets_.idxExtraAK4             .push_back(jet.getIndex()) ; 
      jets_.ptExtraAK4              .push_back(jet.getPt()) ; 
      jets_.etaExtraAK4             .push_back(jet.getEta()) ; 
      jets_.phiExtraAK4             .push_back(jet.getPhi()) ; 
      jets_.MExtraAK4               .push_back(jet.getMass()) ; 
      jets_.csvExtraAK4             .push_back(jet.getCSV());
      jets_.partonFlavourExtraAK4   .push_back(jet.getPartonFlavour());
      jets_.hadronFlavourExtraAK4   .push_back(jet.getHadronFlavour());
  }

  jets_.idxAK8             .clear() ; jets_.idxAK8             .reserve(goodAK8Jets.size()) ;  
  jets_.ptAK8              .clear() ; jets_.ptAK8              .reserve(goodAK8Jets.size()) ;  
  jets_.etaAK8             .clear() ; jets_.etaAK8             .reserve(goodAK8Jets.size()) ;  
  jets_.phiAK8             .clear() ; jets_.phiAK8             .reserve(goodAK8Jets.size()) ;  
  jets_.MAK8               .clear() ; jets_.MAK8               .reserve(goodAK8Jets.size()) ;  
  jets_.SoftDropMassAK8    .clear() ; jets_.SoftDropMassAK8    .reserve(goodAK8Jets.size()) ;  
  jets_.PrunedMassAK8      .clear() ; jets_.PrunedMassAK8      .reserve(goodAK8Jets.size()) ;  
  jets_.tau1AK8            .clear() ; jets_.tau1AK8            .reserve(goodAK8Jets.size()) ;  
  jets_.tau2AK8            .clear() ; jets_.tau2AK8            .reserve(goodAK8Jets.size()) ;  
  jets_.tau3AK8            .clear() ; jets_.tau3AK8            .reserve(goodAK8Jets.size()) ;  
  jets_.csvAK8             .clear() ; jets_.csvAK8             .reserve(goodAK8Jets.size()) ;  
  jets_.partonFlavourAK8   .clear() ; jets_.partonFlavourAK8   .reserve(goodAK8Jets.size()) ;  
  jets_.hadronFlavourAK8   .clear() ; jets_.hadronFlavourAK8   .reserve(goodAK8Jets.size()) ;  
  jets_.doubleBAK8         .clear() ; jets_.doubleBAK8         .reserve(goodAK8Jets.size()) ;  
  jets_.sj0CSVAK8          .clear() ; jets_.sj0CSVAK8          .reserve(goodAK8Jets.size()) ;  
  jets_.sj1CSVAK8          .clear() ; jets_.sj1CSVAK8          .reserve(goodAK8Jets.size()) ;  
  jets_.hadronFlavourSJ0AK8.clear() ; jets_.hadronFlavourSJ0AK8.reserve(goodAK8Jets.size()) ;  
  jets_.hadronFlavourSJ1AK8.clear() ; jets_.hadronFlavourSJ1AK8.reserve(goodAK8Jets.size()) ;  
  jets_.sj0ptAK8            .clear() ; jets_.sj0ptAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj1ptAK8            .clear() ; jets_.sj1ptAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj0etaAK8            .clear() ; jets_.sj0etaAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj1etaAK8            .clear() ; jets_.sj1etaAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj0phiAK8            .clear() ; jets_.sj0phiAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj1phiAK8            .clear() ; jets_.sj1phiAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj0EnergyAK8            .clear() ; jets_.sj0EnergyAK8           .reserve(goodAK8Jets.size()) ;
  jets_.sj1EnergyAK8            .clear() ; jets_.sj1EnergyAK8           .reserve(goodAK8Jets.size()) ;

  for (vlq::Jet jet : goodAK8Jets) {
    jets_.idxAK8             .push_back(jet.getIndex()) ; 
    jets_.ptAK8              .push_back(jet.getPt()) ; 
    jets_.etaAK8             .push_back(jet.getEta()) ; 
    jets_.phiAK8             .push_back(jet.getPhi()) ; 
    jets_.MAK8               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassAK8    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassAK8      .push_back(jet.getPrunedMass()) ;
    jets_.tau1AK8            .push_back(jet.getTau1()) ;
    jets_.tau2AK8            .push_back(jet.getTau2()) ;
    jets_.tau3AK8            .push_back(jet.getTau3()) ;
    jets_.csvAK8             .push_back(jet.getCSV());
    jets_.partonFlavourAK8   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourAK8   .push_back(jet.getHadronFlavour());
    jets_.doubleBAK8         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVAK8          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVAK8          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0AK8.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1AK8.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptAK8           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptAK8           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaAK8           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaAK8           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiAK8           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiAK8           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyAK8           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyAK8           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxHTagged             .clear() ; jets_.idxHTagged             .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.ptHTagged              .clear() ; jets_.ptHTagged              .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.etaHTagged             .clear() ; jets_.etaHTagged             .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.phiHTagged             .clear() ; jets_.phiHTagged             .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.MHTagged               .clear() ; jets_.MHTagged               .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.SoftDropMassHTagged    .clear() ; jets_.SoftDropMassHTagged    .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.PrunedMassHTagged      .clear() ; jets_.PrunedMassHTagged      .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.tau1HTagged            .clear() ; jets_.tau1HTagged            .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.tau2HTagged            .clear() ; jets_.tau2HTagged            .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.tau3HTagged            .clear() ; jets_.tau3HTagged            .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.csvHTagged             .clear() ; jets_.csvHTagged             .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.partonFlavourHTagged   .clear() ; jets_.partonFlavourHTagged   .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.hadronFlavourHTagged   .clear() ; jets_.hadronFlavourHTagged   .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.doubleBHTagged         .clear() ; jets_.doubleBHTagged         .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.sj0CSVHTagged          .clear() ; jets_.sj0CSVHTagged          .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.sj1CSVHTagged          .clear() ; jets_.sj1CSVHTagged          .reserve(goodMixedHTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0HTagged.clear() ; jets_.hadronFlavourSJ0HTagged.reserve(goodMixedHTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1HTagged.clear() ; jets_.hadronFlavourSJ1HTagged.reserve(goodMixedHTaggedJets.size()) ;   
  jets_.sj0ptHTagged            .clear() ; jets_.sj0ptHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj1ptHTagged            .clear() ; jets_.sj1ptHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj0etaHTagged            .clear() ; jets_.sj0etaHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj1etaHTagged            .clear() ; jets_.sj1etaHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj0phiHTagged            .clear() ; jets_.sj0phiHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj1phiHTagged            .clear() ; jets_.sj1phiHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj0EnergyHTagged            .clear() ; jets_.sj0EnergyHTagged           .reserve(goodMixedHTaggedJets.size()) ;
  jets_.sj1EnergyHTagged            .clear() ; jets_.sj1EnergyHTagged           .reserve(goodMixedHTaggedJets.size()) ;

  for (vlq::Jet jet : goodMixedHTaggedJets) {
    jets_.idxHTagged             .push_back(jet.getIndex()) ; 
    jets_.ptHTagged              .push_back(jet.getPt()) ; 
    jets_.etaHTagged             .push_back(jet.getEta()) ; 
    jets_.phiHTagged             .push_back(jet.getPhi()) ; 
    jets_.MHTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassHTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassHTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1HTagged            .push_back(jet.getTau1()) ;
    jets_.tau2HTagged            .push_back(jet.getTau2()) ;
    jets_.tau3HTagged            .push_back(jet.getTau3()) ;
    jets_.csvHTagged             .push_back(jet.getCSV());
    jets_.partonFlavourHTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourHTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBHTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVHTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVHTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0HTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1HTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptHTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptHTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaHTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaHTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiHTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiHTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyHTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyHTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxAntiHTagged             .clear() ; jets_.idxAntiHTagged             .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.ptAntiHTagged              .clear() ; jets_.ptAntiHTagged              .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.etaAntiHTagged             .clear() ; jets_.etaAntiHTagged             .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.phiAntiHTagged             .clear() ; jets_.phiAntiHTagged             .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.MAntiHTagged               .clear() ; jets_.MAntiHTagged               .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.SoftDropMassAntiHTagged    .clear() ; jets_.SoftDropMassAntiHTagged    .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.PrunedMassAntiHTagged      .clear() ; jets_.PrunedMassAntiHTagged      .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.tau1AntiHTagged            .clear() ; jets_.tau1AntiHTagged            .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.tau2AntiHTagged            .clear() ; jets_.tau2AntiHTagged            .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.tau3AntiHTagged            .clear() ; jets_.tau3AntiHTagged            .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.csvAntiHTagged             .clear() ; jets_.csvAntiHTagged             .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.partonFlavourAntiHTagged   .clear() ; jets_.partonFlavourAntiHTagged   .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.hadronFlavourAntiHTagged   .clear() ; jets_.hadronFlavourAntiHTagged   .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.doubleBAntiHTagged         .clear() ; jets_.doubleBAntiHTagged         .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.sj0CSVAntiHTagged          .clear() ; jets_.sj0CSVAntiHTagged          .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.sj1CSVAntiHTagged          .clear() ; jets_.sj1CSVAntiHTagged          .reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0AntiHTagged.clear() ; jets_.hadronFlavourSJ0AntiHTagged.reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1AntiHTagged.clear() ; jets_.hadronFlavourSJ1AntiHTagged.reserve(mixedAntiHTaggedJets.size()) ;   
  jets_.sj0ptAntiHTagged            .clear() ; jets_.sj0ptAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj1ptAntiHTagged            .clear() ; jets_.sj1ptAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj0etaAntiHTagged            .clear() ; jets_.sj0etaAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj1etaAntiHTagged            .clear() ; jets_.sj1etaAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj0phiAntiHTagged            .clear() ; jets_.sj0phiAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj1phiAntiHTagged            .clear() ; jets_.sj1phiAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj0EnergyAntiHTagged            .clear() ; jets_.sj0EnergyAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;
  jets_.sj1EnergyAntiHTagged            .clear() ; jets_.sj1EnergyAntiHTagged           .reserve(mixedAntiHTaggedJets.size()) ;

  for (vlq::Jet jet : mixedAntiHTaggedJets) {
    jets_.idxAntiHTagged             .push_back(jet.getIndex()) ; 
    jets_.ptAntiHTagged              .push_back(jet.getPt()) ; 
    jets_.etaAntiHTagged             .push_back(jet.getEta()) ; 
    jets_.phiAntiHTagged             .push_back(jet.getPhi()) ; 
    jets_.MAntiHTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassAntiHTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassAntiHTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1AntiHTagged            .push_back(jet.getTau1()) ;
    jets_.tau2AntiHTagged            .push_back(jet.getTau2()) ;
    jets_.tau3AntiHTagged            .push_back(jet.getTau3()) ;
    jets_.csvAntiHTagged             .push_back(jet.getCSV());
    jets_.partonFlavourAntiHTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourAntiHTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBAntiHTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVAntiHTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVAntiHTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0AntiHTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1AntiHTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptAntiHTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptAntiHTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaAntiHTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaAntiHTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiAntiHTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiAntiHTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyAntiHTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyAntiHTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxTopTagged             .clear() ; jets_.idxTopTagged             .reserve(goodTopTaggedJets.size()) ;   
  jets_.ptTopTagged              .clear() ; jets_.ptTopTagged              .reserve(goodTopTaggedJets.size()) ;   
  jets_.etaTopTagged             .clear() ; jets_.etaTopTagged             .reserve(goodTopTaggedJets.size()) ;   
  jets_.phiTopTagged             .clear() ; jets_.phiTopTagged             .reserve(goodTopTaggedJets.size()) ;   
  jets_.MTopTagged               .clear() ; jets_.MTopTagged               .reserve(goodTopTaggedJets.size()) ;   
  jets_.SoftDropMassTopTagged    .clear() ; jets_.SoftDropMassTopTagged    .reserve(goodTopTaggedJets.size()) ;   
  jets_.PrunedMassTopTagged      .clear() ; jets_.PrunedMassTopTagged      .reserve(goodTopTaggedJets.size()) ;   
  jets_.tau1TopTagged            .clear() ; jets_.tau1TopTagged            .reserve(goodTopTaggedJets.size()) ;   
  jets_.tau2TopTagged            .clear() ; jets_.tau2TopTagged            .reserve(goodTopTaggedJets.size()) ;   
  jets_.tau3TopTagged            .clear() ; jets_.tau3TopTagged            .reserve(goodTopTaggedJets.size()) ;   
  jets_.csvTopTagged             .clear() ; jets_.csvTopTagged             .reserve(goodTopTaggedJets.size()) ;   
  jets_.partonFlavourTopTagged   .clear() ; jets_.partonFlavourTopTagged   .reserve(goodTopTaggedJets.size()) ;   
  jets_.hadronFlavourTopTagged   .clear() ; jets_.hadronFlavourTopTagged   .reserve(goodTopTaggedJets.size()) ;   
  jets_.doubleBTopTagged         .clear() ; jets_.doubleBTopTagged         .reserve(goodTopTaggedJets.size()) ;   
  jets_.sj0CSVTopTagged          .clear() ; jets_.sj0CSVTopTagged          .reserve(goodTopTaggedJets.size()) ;   
  jets_.sj1CSVTopTagged          .clear() ; jets_.sj1CSVTopTagged          .reserve(goodTopTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0TopTagged.clear() ; jets_.hadronFlavourSJ0TopTagged.reserve(goodTopTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1TopTagged.clear() ; jets_.hadronFlavourSJ1TopTagged.reserve(goodTopTaggedJets.size()) ;   
  jets_.sj0ptTopTagged            .clear() ; jets_.sj0ptTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj1ptTopTagged            .clear() ; jets_.sj1ptTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj0etaTopTagged            .clear() ; jets_.sj0etaTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj1etaTopTagged            .clear() ; jets_.sj1etaTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj0phiTopTagged            .clear() ; jets_.sj0phiTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj1phiTopTagged            .clear() ; jets_.sj1phiTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj0EnergyTopTagged            .clear() ; jets_.sj0EnergyTopTagged           .reserve(goodTopTaggedJets.size()) ;
  jets_.sj1EnergyTopTagged            .clear() ; jets_.sj1EnergyTopTagged           .reserve(goodTopTaggedJets.size()) ;

  for (vlq::Jet jet : goodTopTaggedJets) {
    jets_.idxTopTagged             .push_back(jet.getIndex()) ; 
    jets_.ptTopTagged              .push_back(jet.getPt()) ; 
    jets_.etaTopTagged             .push_back(jet.getEta()) ; 
    jets_.phiTopTagged             .push_back(jet.getPhi()) ; 
    jets_.MTopTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassTopTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassTopTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1TopTagged            .push_back(jet.getTau1()) ;
    jets_.tau2TopTagged            .push_back(jet.getTau2()) ;
    jets_.tau3TopTagged            .push_back(jet.getTau3()) ;
    jets_.csvTopTagged             .push_back(jet.getCSV());
    jets_.partonFlavourTopTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourTopTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBTopTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVTopTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVTopTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0TopTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1TopTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptTopTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptTopTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaTopTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaTopTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiTopTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiTopTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyTopTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyTopTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxAntiTopTagged             .clear() ; jets_.idxAntiTopTagged             .reserve(antiTopTaggedJets.size()) ;   
  jets_.ptAntiTopTagged              .clear() ; jets_.ptAntiTopTagged              .reserve(antiTopTaggedJets.size()) ;   
  jets_.etaAntiTopTagged             .clear() ; jets_.etaAntiTopTagged             .reserve(antiTopTaggedJets.size()) ;   
  jets_.phiAntiTopTagged             .clear() ; jets_.phiAntiTopTagged             .reserve(antiTopTaggedJets.size()) ;   
  jets_.MAntiTopTagged               .clear() ; jets_.MAntiTopTagged               .reserve(antiTopTaggedJets.size()) ;   
  jets_.SoftDropMassAntiTopTagged    .clear() ; jets_.SoftDropMassAntiTopTagged    .reserve(antiTopTaggedJets.size()) ;   
  jets_.PrunedMassAntiTopTagged      .clear() ; jets_.PrunedMassAntiTopTagged      .reserve(antiTopTaggedJets.size()) ;   
  jets_.tau1AntiTopTagged            .clear() ; jets_.tau1AntiTopTagged            .reserve(antiTopTaggedJets.size()) ;   
  jets_.tau2AntiTopTagged            .clear() ; jets_.tau2AntiTopTagged            .reserve(antiTopTaggedJets.size()) ;   
  jets_.tau3AntiTopTagged            .clear() ; jets_.tau3AntiTopTagged            .reserve(antiTopTaggedJets.size()) ;   
  jets_.csvAntiTopTagged             .clear() ; jets_.csvAntiTopTagged             .reserve(antiTopTaggedJets.size()) ;   
  jets_.partonFlavourAntiTopTagged   .clear() ; jets_.partonFlavourAntiTopTagged   .reserve(antiTopTaggedJets.size()) ;   
  jets_.hadronFlavourAntiTopTagged   .clear() ; jets_.hadronFlavourAntiTopTagged   .reserve(antiTopTaggedJets.size()) ;   
  jets_.doubleBAntiTopTagged         .clear() ; jets_.doubleBAntiTopTagged         .reserve(antiTopTaggedJets.size()) ;   
  jets_.sj0CSVAntiTopTagged          .clear() ; jets_.sj0CSVAntiTopTagged          .reserve(antiTopTaggedJets.size()) ;   
  jets_.sj1CSVAntiTopTagged          .clear() ; jets_.sj1CSVAntiTopTagged          .reserve(antiTopTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0AntiTopTagged.clear() ; jets_.hadronFlavourSJ0AntiTopTagged.reserve(antiTopTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1AntiTopTagged.clear() ; jets_.hadronFlavourSJ1AntiTopTagged.reserve(antiTopTaggedJets.size()) ;   
  jets_.sj0ptAntiTopTagged            .clear() ; jets_.sj0ptAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj1ptAntiTopTagged            .clear() ; jets_.sj1ptAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj0etaAntiTopTagged            .clear() ; jets_.sj0etaAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj1etaAntiTopTagged            .clear() ; jets_.sj1etaAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj0phiAntiTopTagged            .clear() ; jets_.sj0phiAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj1phiAntiTopTagged            .clear() ; jets_.sj1phiAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj0EnergyAntiTopTagged            .clear() ; jets_.sj0EnergyAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;
  jets_.sj1EnergyAntiTopTagged            .clear() ; jets_.sj1EnergyAntiTopTagged           .reserve(antiTopTaggedJets.size()) ;

  for (vlq::Jet jet : antiTopTaggedJets) {
    jets_.idxAntiTopTagged             .push_back(jet.getIndex()) ; 
    jets_.ptAntiTopTagged              .push_back(jet.getPt()) ; 
    jets_.etaAntiTopTagged             .push_back(jet.getEta()) ; 
    jets_.phiAntiTopTagged             .push_back(jet.getPhi()) ; 
    jets_.MAntiTopTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassAntiTopTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassAntiTopTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1AntiTopTagged            .push_back(jet.getTau1()) ;
    jets_.tau2AntiTopTagged            .push_back(jet.getTau2()) ;
    jets_.tau3AntiTopTagged            .push_back(jet.getTau3()) ;
    jets_.csvAntiTopTagged             .push_back(jet.getCSV());
    jets_.partonFlavourAntiTopTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourAntiTopTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBAntiTopTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVAntiTopTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVAntiTopTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0AntiTopTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1AntiTopTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptAntiTopTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptAntiTopTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaAntiTopTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaAntiTopTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiAntiTopTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiAntiTopTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyAntiTopTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyAntiTopTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxZTagged             .clear() ; jets_.idxZTagged             .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.ptZTagged              .clear() ; jets_.ptZTagged              .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.etaZTagged             .clear() ; jets_.etaZTagged             .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.phiZTagged             .clear() ; jets_.phiZTagged             .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.MZTagged               .clear() ; jets_.MZTagged               .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.SoftDropMassZTagged    .clear() ; jets_.SoftDropMassZTagged    .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.PrunedMassZTagged      .clear() ; jets_.PrunedMassZTagged      .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.tau1ZTagged            .clear() ; jets_.tau1ZTagged            .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.tau2ZTagged            .clear() ; jets_.tau2ZTagged            .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.tau3ZTagged            .clear() ; jets_.tau3ZTagged            .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.csvZTagged             .clear() ; jets_.csvZTagged             .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.partonFlavourZTagged   .clear() ; jets_.partonFlavourZTagged   .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.hadronFlavourZTagged   .clear() ; jets_.hadronFlavourZTagged   .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.doubleBZTagged         .clear() ; jets_.doubleBZTagged         .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.sj0CSVZTagged          .clear() ; jets_.sj0CSVZTagged          .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.sj1CSVZTagged          .clear() ; jets_.sj1CSVZTagged          .reserve(goodMixedZTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0ZTagged.clear() ; jets_.hadronFlavourSJ0ZTagged.reserve(goodMixedZTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1ZTagged.clear() ; jets_.hadronFlavourSJ1ZTagged.reserve(goodMixedZTaggedJets.size()) ;   
  jets_.sj0ptZTagged            .clear() ; jets_.sj0ptZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj1ptZTagged            .clear() ; jets_.sj1ptZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj0etaZTagged            .clear() ; jets_.sj0etaZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj1etaZTagged            .clear() ; jets_.sj1etaZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj0phiZTagged            .clear() ; jets_.sj0phiZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj1phiZTagged            .clear() ; jets_.sj1phiZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj0EnergyZTagged            .clear() ; jets_.sj0EnergyZTagged           .reserve(goodMixedZTaggedJets.size()) ;
  jets_.sj1EnergyZTagged            .clear() ; jets_.sj1EnergyZTagged           .reserve(goodMixedZTaggedJets.size()) ;

  for (vlq::Jet jet : goodMixedZTaggedJets) {
    jets_.idxZTagged             .push_back(jet.getIndex()) ; 
    jets_.ptZTagged              .push_back(jet.getPt()) ; 
    jets_.etaZTagged             .push_back(jet.getEta()) ; 
    jets_.phiZTagged             .push_back(jet.getPhi()) ; 
    jets_.MZTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassZTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassZTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1ZTagged            .push_back(jet.getTau1()) ;
    jets_.tau2ZTagged            .push_back(jet.getTau2()) ;
    jets_.tau3ZTagged            .push_back(jet.getTau3()) ;
    jets_.csvZTagged             .push_back(jet.getCSV());
    jets_.partonFlavourZTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourZTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBZTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVZTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVZTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0ZTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1ZTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptZTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptZTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaZTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaZTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiZTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiZTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyZTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyZTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  jets_.idxAntiZTagged             .clear() ; jets_.idxAntiZTagged             .reserve(antiZTaggedJets.size()) ;   
  jets_.ptAntiZTagged              .clear() ; jets_.ptAntiZTagged              .reserve(antiZTaggedJets.size()) ;   
  jets_.etaAntiZTagged             .clear() ; jets_.etaAntiZTagged             .reserve(antiZTaggedJets.size()) ;   
  jets_.phiAntiZTagged             .clear() ; jets_.phiAntiZTagged             .reserve(antiZTaggedJets.size()) ;   
  jets_.MAntiZTagged               .clear() ; jets_.MAntiZTagged               .reserve(antiZTaggedJets.size()) ;   
  jets_.SoftDropMassAntiZTagged    .clear() ; jets_.SoftDropMassAntiZTagged    .reserve(antiZTaggedJets.size()) ;   
  jets_.PrunedMassAntiZTagged      .clear() ; jets_.PrunedMassAntiZTagged      .reserve(antiZTaggedJets.size()) ;   
  jets_.tau1AntiZTagged            .clear() ; jets_.tau1AntiZTagged            .reserve(antiZTaggedJets.size()) ;   
  jets_.tau2AntiZTagged            .clear() ; jets_.tau2AntiZTagged            .reserve(antiZTaggedJets.size()) ;   
  jets_.tau3AntiZTagged            .clear() ; jets_.tau3AntiZTagged            .reserve(antiZTaggedJets.size()) ;   
  jets_.csvAntiZTagged             .clear() ; jets_.csvAntiZTagged             .reserve(antiZTaggedJets.size()) ;   
  jets_.partonFlavourAntiZTagged   .clear() ; jets_.partonFlavourAntiZTagged   .reserve(antiZTaggedJets.size()) ;   
  jets_.hadronFlavourAntiZTagged   .clear() ; jets_.hadronFlavourAntiZTagged   .reserve(antiZTaggedJets.size()) ;   
  jets_.doubleBAntiZTagged         .clear() ; jets_.doubleBAntiZTagged         .reserve(antiZTaggedJets.size()) ;   
  jets_.sj0CSVAntiZTagged          .clear() ; jets_.sj0CSVAntiZTagged          .reserve(antiZTaggedJets.size()) ;   
  jets_.sj1CSVAntiZTagged          .clear() ; jets_.sj1CSVAntiZTagged          .reserve(antiZTaggedJets.size()) ;   
  jets_.hadronFlavourSJ0AntiZTagged.clear() ; jets_.hadronFlavourSJ0AntiZTagged.reserve(antiZTaggedJets.size()) ;   
  jets_.hadronFlavourSJ1AntiZTagged.clear() ; jets_.hadronFlavourSJ1AntiZTagged.reserve(antiZTaggedJets.size()) ;   
  jets_.sj0ptAntiZTagged            .clear() ; jets_.sj0ptAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj1ptAntiZTagged            .clear() ; jets_.sj1ptAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj0etaAntiZTagged            .clear() ; jets_.sj0etaAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj1etaAntiZTagged            .clear() ; jets_.sj1etaAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj0phiAntiZTagged            .clear() ; jets_.sj0phiAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj1phiAntiZTagged            .clear() ; jets_.sj1phiAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj0EnergyAntiZTagged            .clear() ; jets_.sj0EnergyAntiZTagged           .reserve(antiZTaggedJets.size()) ;
  jets_.sj1EnergyAntiZTagged            .clear() ; jets_.sj1EnergyAntiZTagged           .reserve(antiZTaggedJets.size()) ;

  for (vlq::Jet jet : antiZTaggedJets) {
    jets_.idxAntiZTagged             .push_back(jet.getIndex()) ; 
    jets_.ptAntiZTagged              .push_back(jet.getPt()) ; 
    jets_.etaAntiZTagged             .push_back(jet.getEta()) ; 
    jets_.phiAntiZTagged             .push_back(jet.getPhi()) ; 
    jets_.MAntiZTagged               .push_back(jet.getMass()) ; 
    jets_.SoftDropMassAntiZTagged    .push_back(jet.getSoftDropMass()) ;
    jets_.PrunedMassAntiZTagged      .push_back(jet.getPrunedMass()) ;
    jets_.tau1AntiZTagged            .push_back(jet.getTau1()) ;
    jets_.tau2AntiZTagged            .push_back(jet.getTau2()) ;
    jets_.tau3AntiZTagged            .push_back(jet.getTau3()) ;
    jets_.csvAntiZTagged             .push_back(jet.getCSV());
    jets_.partonFlavourAntiZTagged   .push_back(jet.getPartonFlavour());
    jets_.hadronFlavourAntiZTagged   .push_back(jet.getHadronFlavour());
    jets_.doubleBAntiZTagged         .push_back(jet.getDoubleBAK8()) ;
    jets_.sj0CSVAntiZTagged          .push_back(jet.getCSVSubjet0()) ;
    jets_.sj1CSVAntiZTagged          .push_back(jet.getCSVSubjet1()) ;
    jets_.hadronFlavourSJ0AntiZTagged.push_back(jet.getHadronFlavourSubjet0()) ;
    jets_.hadronFlavourSJ1AntiZTagged.push_back(jet.getHadronFlavourSubjet1()) ;
    jets_.sj0ptAntiZTagged           .push_back(jet.getPtSubjet0()) ;
    jets_.sj1ptAntiZTagged           .push_back(jet.getPtSubjet1()) ;
    jets_.sj0etaAntiZTagged           .push_back(jet.getEtaSubjet0()) ;
    jets_.sj1etaAntiZTagged           .push_back(jet.getEtaSubjet1()) ;
    jets_.sj0phiAntiZTagged           .push_back(jet.getPhiSubjet0()) ;
    jets_.sj1phiAntiZTagged           .push_back(jet.getPhiSubjet1()) ;
    jets_.sj0EnergyAntiZTagged           .push_back(jet.getEnergySubjet0()) ;
    jets_.sj1EnergyAntiZTagged           .push_back(jet.getEnergySubjet1()) ;
  }

  //// Lepton veto 
  vlq::ElectronCollection goodElectrons; 
  electronmaker(evt, goodElectrons) ;

  vlq::MuonCollection goodMuons; 
  muonmaker(evt, goodMuons) ; 

  selectedevt_.nEl_ = int(goodElectrons.size());
  selectedevt_.nMu_ = int(goodMuons.size());

  leptons_.idxEl             .clear() ; leptons_.idxEl             .reserve(goodElectrons.size()) ;   
  leptons_.ptEl              .clear() ; leptons_.ptEl              .reserve(goodElectrons.size()) ; 
  leptons_.etaEl             .clear() ; leptons_.etaEl             .reserve(goodElectrons.size()) ;   
  leptons_.phiEl             .clear() ; leptons_.phiEl             .reserve(goodElectrons.size()) ;   
  leptons_.EEl               .clear() ; leptons_.EEl               .reserve(goodElectrons.size()) ;  
  leptons_.IsoDREl               .clear() ; leptons_.IsoDREl               .reserve(goodElectrons.size()) ;  
  leptons_.dR_Iso2DEl               .clear() ; leptons_.dR_Iso2DEl               .reserve(goodElectrons.size()) ;  
  leptons_.ptrel_Iso2DEl               .clear() ; leptons_.ptrel_Iso2DEl               .reserve(goodElectrons.size()) ;  
  leptons_.idxMu             .clear() ; leptons_.idxMu             .reserve(goodMuons.size()) ;   
  leptons_.ptMu              .clear() ; leptons_.ptMu              .reserve(goodMuons.size()) ; 
  leptons_.etaMu             .clear() ; leptons_.etaMu             .reserve(goodMuons.size()) ;   
  leptons_.phiMu             .clear() ; leptons_.phiMu             .reserve(goodMuons.size()) ;   
  leptons_.EMu               .clear() ; leptons_.EMu               .reserve(goodMuons.size()) ;  
  leptons_.IsoDRMu               .clear() ; leptons_.IsoDRMu               .reserve(goodMuons.size()) ;  
  leptons_.dR_Iso2DMu               .clear() ; leptons_.dR_Iso2DMu               .reserve(goodMuons.size()) ;  
  leptons_.ptrel_Iso2DMu               .clear() ; leptons_.ptrel_Iso2DMu               .reserve(goodMuons.size()) ;  
 
  h1_["nel"]->Fill(goodElectrons.size(),evtwt*toptagsf*btagsf) ;
  h1_["nmu"]->Fill(goodMuons.size(),evtwt*toptagsf*btagsf) ;

  int nel_passreliso(0), nmu_passreliso(0);

  for (vlq::Electron el : goodElectrons ) {
    h1_["elIsoDR"] ->Fill(el.getIso03(),evtwt*toptagsf*btagsf) ; 
    //// Get 2D isolation
    std::pair<double, double> el_drmin_ptrel(Utilities::drmin_pTrel<vlq::Electron, vlq::Jet>(el, goodAK4Jets)) ;
    h2_["elIso2D"] -> Fill(el_drmin_ptrel.first, el_drmin_ptrel.second,evtwt*toptagsf*btagsf) ;
    //// Get rel. iso
    if (el.getIso03() < 0.0571) ++nel_passreliso;
 
    leptons_.idxEl           .push_back(el.getIndex());
    leptons_.ptEl            .push_back(el.getPt());
    leptons_.etaEl           .push_back(el.getEta());
    leptons_.phiEl           .push_back(el.getPhi());
    leptons_.EEl             .push_back(el.getE());
    leptons_.IsoDREl         .push_back(el.getIso03());
    leptons_.dR_Iso2DEl      .push_back(el_drmin_ptrel.first);
    leptons_.ptrel_Iso2DEl   .push_back(el_drmin_ptrel.second);
  }

  for (vlq::Muon mu : goodMuons ) {
    h1_["muIsoDR"] ->Fill(mu.getIso04(),evtwt*toptagsf*btagsf) ; 
    //// Get 2D isolation
    std::pair<double, double> mu_drmin_ptrel(Utilities::drmin_pTrel<vlq::Muon, vlq::Jet>(mu, goodAK4Jets)) ;
    h2_["muIso2D"] -> Fill(mu_drmin_ptrel.first, mu_drmin_ptrel.second,evtwt*toptagsf*btagsf) ;
    //// Get rel. iso
    if ( mu.getIso04() < 0.3) ++nmu_passreliso ;

//fill muon variables
    leptons_.idxMu           .push_back(mu.getIndex());
    leptons_.ptMu            .push_back(mu.getPt());
    leptons_.etaMu           .push_back(mu.getEta());
    leptons_.phiMu           .push_back(mu.getPhi());
    leptons_.EMu             .push_back(mu.getE());
    leptons_.IsoDRMu         .push_back(mu.getIso04());
    leptons_.dR_Iso2DMu      .push_back(mu_drmin_ptrel.first);
    leptons_.ptrel_Iso2DMu   .push_back(mu_drmin_ptrel.second);

  }

  h1_["nelAfterRelIso"] -> Fill(nel_passreliso,evtwt*toptagsf*btagsf) ;
  h1_["nmuAfterRelIso"] -> Fill(nmu_passreliso,evtwt*toptagsf*btagsf) ;

  for (vlq::Electron el : goodElectrons) {
  }

  for (vlq::Muon mu : goodMuons) {
  }

  //// Apply 2D isolation
  CandidateCleaner cleanleptons(0.4,40); //// The second argument is for lepton 2D iso, setting to -1 disables it

  cleanleptons(goodElectrons,goodAK4Jets) ;
  cleanleptons(goodMuons,goodAK4Jets) ;

  //cleanleptons(goodElectrons,goodAK8Jets) ;
  //cleanleptons(goodMuons,goodAK8Jets) ;

  selectedevt_.nCleanedEl_ = int(goodElectrons.size());
  selectedevt_.nCleanedMu_ = int(goodMuons.size());

  leptons_.idxCleanedEl             .clear() ; leptons_.idxCleanedEl             .reserve(goodElectrons.size()) ;   
  leptons_.ptCleanedEl              .clear() ; leptons_.ptCleanedEl              .reserve(goodElectrons.size()) ; 
  leptons_.etaCleanedEl             .clear() ; leptons_.etaCleanedEl             .reserve(goodElectrons.size()) ;   
  leptons_.phiCleanedEl             .clear() ; leptons_.phiCleanedEl             .reserve(goodElectrons.size()) ;   
  leptons_.ECleanedEl               .clear() ; leptons_.ECleanedEl               .reserve(goodElectrons.size()) ;  
  leptons_.IsoDRAfterIso2DEl .clear() ; leptons_.IsoDRAfterIso2DEl .reserve(goodElectrons.size()) ;  
  leptons_.idxCleanedMu             .clear() ; leptons_.idxCleanedMu             .reserve(goodMuons.size()) ;   
  leptons_.ptCleanedMu              .clear() ; leptons_.ptCleanedMu              .reserve(goodMuons.size()) ; 
  leptons_.etaCleanedMu             .clear() ; leptons_.etaCleanedMu             .reserve(goodMuons.size()) ;   
  leptons_.phiCleanedMu             .clear() ; leptons_.phiCleanedMu             .reserve(goodMuons.size()) ;   
  leptons_.ECleanedMu               .clear() ; leptons_.ECleanedMu               .reserve(goodMuons.size()) ;  
  leptons_.IsoDRAfterIso2DMu .clear() ; leptons_.IsoDRAfterIso2DMu .reserve(goodMuons.size()) ;  


  h1_["nelAfter2DIso"]->Fill(goodElectrons.size(),evtwt*toptagsf*btagsf) ;
  h1_["nmuAfter2DIso"]->Fill(goodMuons.size(),evtwt*toptagsf*btagsf) ;

  for (vlq::Electron el : goodElectrons ) {
    h1_["elIsoDRAfter2DIso"] ->Fill(el.getIso03(),evtwt*toptagsf*btagsf) ; 

//fill cleaned electron info
    leptons_.IsoDRAfterIso2DEl      .push_back(el.getIso03());

    leptons_.idxCleanedEl           .push_back(el.getIndex());
    leptons_.ptCleanedEl            .push_back(el.getPt());
    leptons_.etaCleanedEl           .push_back(el.getEta());
    leptons_.phiCleanedEl           .push_back(el.getPhi());
    leptons_.ECleanedEl             .push_back(el.getE());
  }

  for (vlq::Muon mu : goodMuons ) {
    h1_["muIsoDRAfter2DIso"] ->Fill(mu.getIso04(),evtwt*toptagsf*btagsf) ;

//fill cleaned muon info 
    leptons_.IsoDRAfterIso2DMu      .push_back(mu.getIso04());

    leptons_.idxCleanedMu           .push_back(mu.getIndex());
    leptons_.ptCleanedMu            .push_back(mu.getPt());
    leptons_.etaCleanedMu           .push_back(mu.getEta());
    leptons_.phiCleanedMu           .push_back(mu.getPhi());
    leptons_.ECleanedMu             .push_back(mu.getE());
  }

  tree_->Fill();
  return true;

}

// ------------ method called once each job just before starting event loop  ------------
void VLQAna::beginJob() {

  h1_["npv_noreweight"] = fs->make<TH1D>("npv_noreweight", ";N(PV);;", 51, -0.5, 50.5) ; 
  h1_["npv"] = fs->make<TH1D>("npv", ";N(PV);;", 51, -0.5, 50.5) ; 

  if ( !doPreselOnly_ ) {
    tree_ = fs->make<TTree>("tree", "TtHT") ; 
    selectedevt_.RegisterTree(tree_,"SelectedEvent") ; 
    jets_.RegisterTree(tree_,"JetInfo") ; 
    leptons_.RegisterTree(tree_,"LeptonInfo") ; 
  }

  if (storePreselEvts_ || doPreselOnly_) {

    h1_["Presel_HT"] = fs->make<TH1D>("Presel_HT", "H_{T};H_{T} [GeV];;",50,500,3000) ; 
    h1_["Presel_Mjj"] = fs->make<TH1D>("Presel_Mjj", "M_{j1,j2} [GeV];;",50,500,3000) ; 

    h1_["Presel_ptAK4_0"] = fs->make<TH1D>("Presel_ptAK4_0", "p_{T} AK4;p_{T} (1st AK4 jet) [GeV];;",50,0,2000) ; 
    h1_["Presel_ptAK4_1"] = fs->make<TH1D>("Presel_ptAK4_1", "p_{T} AK4;p_{T} (2nd AK4 jet) [GeV];;",50,0,2000) ; 
    h1_["Presel_ptAK4_2"] = fs->make<TH1D>("Presel_ptAK4_2", "p_{T} AK4;p_{T} (3rd AK4 jet) [GeV];;",50,0,2000) ; 
    h1_["Presel_ptAK4_3"] = fs->make<TH1D>("Presel_ptAK4_3", "p_{T} AK4;p_{T} (4th AK4 jet) [GeV];;",50,0,2000) ; 
    h1_["Presel_ptAK8_0"] = fs->make<TH1D>("Presel_ptAK8_0", "p_{T} AK8;p_{T} (1st AK8 jet) [GeV];;",50,0,2000) ; 

    h1_["Presel_etaAK4_0"] = fs->make<TH1D>("Presel_etaAK4_0", "#eta AK4;#eta (1st AK4 jet) [GeV];;",60,-6.,6.) ; 
    h1_["Presel_etaAK4_1"] = fs->make<TH1D>("Presel_etaAK4_1", "#eta AK4;#eta (2nd AK4 jet) [GeV];;",60,-6.,6.) ; 
    h1_["Presel_etaAK4_2"] = fs->make<TH1D>("Presel_etaAK4_2", "#eta AK4;#eta (3rd AK4 jet) [GeV];;",60,-6.,6.) ; 
    h1_["Presel_etaAK4_3"] = fs->make<TH1D>("Presel_etaAK4_3", "#eta AK4;#eta (4th AK4 jet) [GeV];;",60,-6.,6.) ; 
    h1_["Presel_etaAK8_0"] = fs->make<TH1D>("Presel_etaAK8_0", "#eta AK8;#eta (1st AK8 jet) [GeV];;",60,-6.,6.) ; 

    h1_["Presel_NAK4"] = fs->make<TH1D>("Presel_NAK4", ";N(AK4 jets);;", 21, -0.5, 20.5) ; 
    h1_["Presel_NAK8"] = fs->make<TH1D>("Presel_NAK8", ";N(AK4 jets);;", 11, -0.5, 10.5) ; 

    if ( doPreselOnly_ ) {
      h1_["cutflow"] = fs->make<TH1D>("cutflow", "cut flow", 5, 0.5, 5.5) ;  
      h1_["cutflow"] -> GetXaxis() -> SetBinLabel(1,  "All") ; 
      h1_["cutflow"] -> GetXaxis() -> SetBinLabel(2,  "Trig.+PV") ; 
      h1_["cutflow"] -> GetXaxis() -> SetBinLabel(3,  "N(AK4)>=4") ; 
      h1_["cutflow"] -> GetXaxis() -> SetBinLabel(4,  "H_{T}>1000GeV") ; 
      h1_["cutflow"] -> GetXaxis() -> SetBinLabel(5,  "N(AK8)>=1") ; 
      return ; 
    }

  }

  h1_["cutflow"] = fs->make<TH1D>("cutflow", "cut flow", 12, 0.5, 12.5) ;  
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(1,  "All") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(2,  "Trig.+PV") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(3,  "N(AK4)>=4") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(4,  "H_{T}>H_{T}^{min}") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(5,  "N(AK8)>=1") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(6,  "p_{T}(AK8_{0})>p_{T}^{min}(AK8_{0})") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(7,  "N(H)>0") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(8,  "N(top)>0") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(9,  "RegionA") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(10, "RegionB") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(11, "RegionC") ; 
  h1_["cutflow"] -> GetXaxis() -> SetBinLabel(12, "RegionD") ; 

  h1_["RegD_HT"] = fs->make<TH1D>("RegD_HT", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_wts"] = fs->make<TH1D>("RegD_HT_wts", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_btagsf_bcUp"] = fs->make<TH1D>("RegD_HT_btagsf_bcUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_btagsf_bcDown"] = fs->make<TH1D>("RegD_HT_btagsf_bcDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_btagsf_lUp"] = fs->make<TH1D>("RegD_HT_btagsf_lUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_btagsf_lDown"] = fs->make<TH1D>("RegD_HT_btagsf_lDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_toptagsfUp"] = fs->make<TH1D>("RegD_HT_toptagsfUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_toptagsfDown"] = fs->make<TH1D>("RegD_HT_toptagsfDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_htwtUp"] = fs->make<TH1D>("RegD_HT_htwtUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegD_HT_htwtDown"] = fs->make<TH1D>("RegD_HT_htwtDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 

  h1_["RegB_HT"] = fs->make<TH1D>("RegB_HT", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_wts"] = fs->make<TH1D>("RegB_HT_wts", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_btagsf_bcUp"] = fs->make<TH1D>("RegB_HT_btagsf_bcUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_btagsf_bcDown"] = fs->make<TH1D>("RegB_HT_btagsf_bcDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_btagsf_lUp"] = fs->make<TH1D>("RegB_HT_btagsf_lUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_btagsf_lDown"] = fs->make<TH1D>("RegB_HT_btagsf_lDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_toptagsfUp"] = fs->make<TH1D>("RegB_HT_toptagsfUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_toptagsfDown"] = fs->make<TH1D>("RegB_HT_toptagsfDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_htwtUp"] = fs->make<TH1D>("RegB_HT_htwtUp", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 
  h1_["RegB_HT_htwtDown"] = fs->make<TH1D>("RegB_HT_htwtDown", "H_{T};H_{T} [GeV];;",50,1000,3000) ; 

  //////////

  h1_["RegD_mtprime"] = fs->make<TH1D>("RegD_mtprime", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_wts"] = fs->make<TH1D>("RegD_mtprime_wts", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_btagsf_bcUp"] = fs->make<TH1D>("RegD_mtprime_btagsf_bcUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_btagsf_bcDown"] = fs->make<TH1D>("RegD_mtprime_btagsf_bcDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_btagsf_lUp"] = fs->make<TH1D>("RegD_mtprime_btagsf_lUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_btagsf_lDown"] = fs->make<TH1D>("RegD_mtprime_btagsf_lDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_toptagsfUp"] = fs->make<TH1D>("RegD_mtprime_toptagsfUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_toptagsfDown"] = fs->make<TH1D>("RegD_mtprime_toptagsfDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_htwtUp"] = fs->make<TH1D>("RegD_mtprime_htwtUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_htwtDown"] = fs->make<TH1D>("RegD_mtprime_htwtDown", "M(T);M(T) [GeV];;",80,500,2500) ; 

  h1_["RegD_mtprime_corr"] = fs->make<TH1D>("RegD_mtprime_corr", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_wts"] = fs->make<TH1D>("RegD_mtprime_corr_wts", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_btagsf_bcUp"] = fs->make<TH1D>("RegD_mtprime_corr_btagsf_bcUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_btagsf_bcDown"] = fs->make<TH1D>("RegD_mtprime_corr_btagsf_bcDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_btagsf_lUp"] = fs->make<TH1D>("RegD_mtprime_corr_btagsf_lUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_btagsf_lDown"] = fs->make<TH1D>("RegD_mtprime_corr_btagsf_lDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_toptagsfUp"] = fs->make<TH1D>("RegD_mtprime_corr_toptagsfUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_toptagsfDown"] = fs->make<TH1D>("RegD_mtprime_corr_toptagsfDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_htwtUp"] = fs->make<TH1D>("RegD_mtprime_corr_htwtUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegD_mtprime_corr_htwtDown"] = fs->make<TH1D>("RegD_mtprime_corr_htwtDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 

  h1_["RegB_mtprime"] = fs->make<TH1D>("RegB_mtprime", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_wts"] = fs->make<TH1D>("RegB_mtprime_wts", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_btagsf_bcUp"] = fs->make<TH1D>("RegB_mtprime_btagsf_bcUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_btagsf_bcDown"] = fs->make<TH1D>("RegB_mtprime_btagsf_bcDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_btagsf_lUp"] = fs->make<TH1D>("RegB_mtprime_btagsf_lUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_btagsf_lDown"] = fs->make<TH1D>("RegB_mtprime_btagsf_lDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_toptagsfUp"] = fs->make<TH1D>("RegB_mtprime_toptagsfUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_toptagsfDown"] = fs->make<TH1D>("RegB_mtprime_toptagsfDown", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_htwtUp"] = fs->make<TH1D>("RegB_mtprime_htwtUp", "M(T);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_htwtDown"] = fs->make<TH1D>("RegB_mtprime_htwtDown", "M(T);M(T) [GeV];;",80,500,2500) ; 

  h1_["RegB_mtprime_corr"] = fs->make<TH1D>("RegB_mtprime_corr", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_wts"] = fs->make<TH1D>("RegB_mtprime_corr_wts", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_btagsf_bcUp"] = fs->make<TH1D>("RegB_mtprime_corr_btagsf_bcUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_btagsf_bcDown"] = fs->make<TH1D>("RegB_mtprime_corr_btagsf_bcDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_btagsf_lUp"] = fs->make<TH1D>("RegB_mtprime_corr_btagsf_lUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_btagsf_lDown"] = fs->make<TH1D>("RegB_mtprime_corr_btagsf_lDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_toptagsfUp"] = fs->make<TH1D>("RegB_mtprime_corr_toptagsfUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_toptagsfDown"] = fs->make<TH1D>("RegB_mtprime_corr_toptagsfDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_htwtUp"] = fs->make<TH1D>("RegB_mtprime_corr_htwtUp", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 
  h1_["RegB_mtprime_corr_htwtDown"] = fs->make<TH1D>("RegB_mtprime_corr_htwtDown", "M(T) - M(H-jet) - M(top-jet) + M(H) + M(top);M(T) [GeV];;",80,500,2500) ; 

  h1_["nel"] = fs->make<TH1D>("nel", "nel;N(electrons);Events;;",5,-0.5,4.5) ;
  h1_["nmu"] = fs->make<TH1D>("nmu", "nmu;N(muons);Events;;",5,-0.5,4.5) ;

  h1_["elIsoDR"] = fs->make<TH1D>("elIsoDR", "elIsoDR;Electron rel. iso in #DeltaR < 0.4, EA corr.;Electrons;;",200,0,10) ;
  h1_["muIsoDR"] = fs->make<TH1D>("muIsoDR", "muIsoDR;Muon rel. iso. in #DeltaR < 0.3, #delta#beta corr.;Muons;;",200,0.,10) ;

  h2_["elIso2D"] = fs->make<TH2D>("elIso2D", "elIso2D;#DeltaR(e,jet);p_{T}^{rel} [GeV];Electrons;",20,0,4,200,0,100) ;
  h2_["muIso2D"] = fs->make<TH2D>("muIso2D", "muIso2D;#DeltaR(#mu,jet);p_{T}^{rel} [GeV];Muons;",20,0,4,200,0.,100) ;

  h1_["nelAfterRelIso"] = fs->make<TH1D>("nelAfterRelIso", "nelAfterRelIso;N(electrons);Events;;",5,-0.5,4.5) ;
  h1_["nmuAfterRelIso"] = fs->make<TH1D>("nmuAfterRelIso", "nmuAfterRelIso;N(muons);Events;;",5,-0.5,4.5) ;

  h1_["nelAfter2DIso"] = fs->make<TH1D>("nelAfter2DIso", "nelAfter2DIso;N(electrons);Events;;",5,-0.5,4.5) ;
  h1_["nmuAfter2DIso"] = fs->make<TH1D>("nmuAfter2DIso", "nmuAfter2DIso;N(muons);Events;;",5,-0.5,4.5) ;

  h1_["elIsoDRAfter2DIso"] = fs->make<TH1D>("elIsoDRAfter2DIso", "elIsoDRAfter2DIso;Electron rel. iso in #DeltaR < 0.4, EA corr.;Events;;",200,0,10) ;
  h1_["muIsoDRAfter2DIso"] = fs->make<TH1D>("muIsoDRAfter2DIso", "muIsoDRAfter2DIso;Muon rel. iso. in #DeltaR < 0.3, #delta#beta corr.;Events;;",200,0.,10) ;

}

// ------------ method called once each job just after ending the event loop  ------------
void VLQAna::endJob() {
  return ; 
}

double VLQAna::getHTReweightingSF(double ht, double err) {
  double wt(1);
  std::unique_ptr<TF1> htReweightFun     = std::unique_ptr<TF1>( new TF1("htReweightFun", "1.25868 - (0.000210855*x)",1000, 10000) ) ; 
  std::unique_ptr<TF1> htReweightFunErr  = std::unique_ptr<TF1>( new TF1("htReweightFunErr", 
        "TMath::Sqrt( (0.00432715*0.00432715) + ((3.27325*x/1000000)*(3.27325*x/1000000)) - (2*x*0.00432715*3.27325*0.97547/1000000) )", 
        1000, 10000) ) ;
  wt = htReweightFun->Eval(ht) + (err*htReweightFunErr->Eval(ht)) ; 
  return wt;
}

DEFINE_FWK_MODULE(VLQAna);
