import Core as fcc
import ROOT


process = fcc.Process(
    inputs = ['root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root'],
)

process.electrons = fcc.Converter('RecoParticles',
    src = 'ReconstructedParticles',
)
process.recoParticles = fcc.Converter('RecoParticles')
process.jets = fcc.Converter('RecoJets',
    src = 'Jet',
)

#ROOT.EnableImplicitMT()

branches = ROOT.vector('string')()
for br in ['electrons', 'jets', 'recoParticles']:
    branches.push_back(br)

#df = process.df()
df = process.df().Range(0, 10)
df.Snapshot('events', 'test.root', branches)
