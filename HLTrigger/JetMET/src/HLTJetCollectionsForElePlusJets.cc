#include <string>
#include <vector>

#include "TVector3.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "HLTrigger/JetMET/interface/HLTJetCollectionsForElePlusJets.h"
#include "HLTrigger/HLTcore/interface/defaultModuleLabel.h"

template <typename T>
HLTJetCollectionsForElePlusJets<T>::HLTJetCollectionsForElePlusJets(const edm::ParameterSet& iConfig)
    : hltElectronTag(iConfig.getParameter<edm::InputTag>("HltElectronTag")),
      sourceJetTag(iConfig.getParameter<edm::InputTag>("SourceJetTag")),
      //minJetPt_(iConfig.getParameter<double> ("MinJetPt")),
      //maxAbsJetEta_(iConfig.getParameter<double> ("MaxAbsJetEta")),
      //minNJets_(iConfig.getParameter<unsigned int> ("MinNJets")),
      minDeltaR_(iConfig.getParameter<double>("minDeltaR"))
//Only for VBF
//minSoftJetPt_(iConfig.getParameter< double > ("MinSoftJetPt")),
//minDeltaEta_(iConfig.getParameter< double > ("MinDeltaEta"))
{
  typedef std::vector<edm::RefVector<std::vector<T>, T, edm::refhelper::FindUsingAdvance<std::vector<T>, T>>>
      TCollectionVector;
  m_theElectronToken = consumes<trigger::TriggerFilterObjectWithRefs>(hltElectronTag);
  m_theJetToken = consumes<std::vector<T>>(sourceJetTag);
  produces<TCollectionVector>();
}

template <typename T>
HLTJetCollectionsForElePlusJets<T>::~HLTJetCollectionsForElePlusJets() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

template <typename T>
void HLTJetCollectionsForElePlusJets<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("HltElectronTag", edm::InputTag("triggerFilterObjectWithRefs"));
  desc.add<edm::InputTag>("SourceJetTag", edm::InputTag("jetCollection"));
  // desc.add<double> ("MinJetPt", 30.);
  // desc.add<double> ("MaxAbsJetEta", 2.6);
  // desc.add<unsigned int> ("MinNJets", 1);
  desc.add<double>("minDeltaR", 0.5);
  //Only for VBF
  // desc.add<double> ("MinSoftJetPt", 25.);
  //desc.add<double> ("MinDeltaEta", -1.);
  descriptions.add(defaultModuleLabel<HLTJetCollectionsForElePlusJets<T>>(), desc);
}

//
// member functions
//

// ------------ method called to produce the data  ------------
template <typename T>
void HLTJetCollectionsForElePlusJets<T>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;

  typedef vector<T> TCollection;
  typedef Ref<TCollection> TRef;

  typedef edm::RefVector<TCollection> TRefVector;

  typedef std::vector<edm::RefVector<std::vector<T>, T, edm::refhelper::FindUsingAdvance<std::vector<T>, T>>>
      TCollectionVector;

  edm::Handle<trigger::TriggerFilterObjectWithRefs> PrevFilterOutput;
  iEvent.getByToken(m_theElectronToken, PrevFilterOutput);

  //its easier on the if statement flow if I try everything at once, shouldnt add to timing
  // Electrons can be stored as objects of types TriggerCluster, TriggerElectron, or TriggerPhoton
  std::vector<edm::Ref<reco::RecoEcalCandidateCollection>> clusCands;
  PrevFilterOutput->getObjects(trigger::TriggerCluster, clusCands);

  std::vector<edm::Ref<reco::ElectronCollection>> eleCands;
  PrevFilterOutput->getObjects(trigger::TriggerElectron, eleCands);

  trigger::VRphoton photonCands;
  PrevFilterOutput->getObjects(trigger::TriggerPhoton, photonCands);

  //prepare the collection of 3-D vector for electron momenta
  std::vector<TVector3> ElePs;

  if (!clusCands.empty()) {  //try trigger cluster
    for (auto& clusCand : clusCands) {
      TVector3 positionVector(clusCand->superCluster()->position().x(),
                              clusCand->superCluster()->position().y(),
                              clusCand->superCluster()->position().z());
      ElePs.push_back(positionVector);
    }
  } else if (!eleCands.empty()) {  // try trigger electrons
    for (auto& eleCand : eleCands) {
      TVector3 positionVector(eleCand->superCluster()->position().x(),
                              eleCand->superCluster()->position().y(),
                              eleCand->superCluster()->position().z());
      ElePs.push_back(positionVector);
    }
  } else if (!photonCands.empty()) {  // try trigger photons
    for (auto& photonCand : photonCands) {
      TVector3 positionVector(photonCand->superCluster()->position().x(),
                              photonCand->superCluster()->position().y(),
                              photonCand->superCluster()->position().z());
      ElePs.push_back(positionVector);
    }
  }

  edm::Handle<TCollection> theJetCollectionHandle;
  iEvent.getByToken(m_theJetToken, theJetCollectionHandle);

  const TCollection& theJetCollection = *theJetCollectionHandle;

  //std::unique_ptr< TCollection >  theFilteredJetCollection(new TCollection);

  std::unique_ptr<TCollectionVector> allSelections(new TCollectionVector());

  //bool foundSolution(false);

  for (auto& EleP : ElePs) {
    // bool VBFJetPair = false;
    //std::vector<int> store_jet;
    TRefVector refVector;

    for (unsigned int j = 0; j < theJetCollection.size(); j++) {
      TVector3 JetP(theJetCollection[j].px(), theJetCollection[j].py(), theJetCollection[j].pz());
      double DR = EleP.DeltaR(JetP);

      if (DR > minDeltaR_)
        refVector.push_back(TRef(theJetCollectionHandle, j));
    }
    allSelections->push_back(refVector);
  }

  //iEvent.put(std::move(theFilteredJetCollection));
  iEvent.put(std::move(allSelections));

  return;
}
