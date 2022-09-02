import ROOT

ROOT.gSystem.Load('libk4DataSource')
ROOT.k4DataSource

files = ['root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root']
df = ROOT.MakeK4DataFrame(files, ['RecoJets'])

branches = ROOT.vector('string')()
for br in ['jets']:
    branches.push_back(br)

df.Snapshot('events', 'test.root', branches)
