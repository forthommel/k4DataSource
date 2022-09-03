import Core as fcc
import ROOT

files = ['root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root']

electrons = fcc.Converter('RecoParticles',
    src = 'ReconstructedParticles',
    output = 'electrons',
)

df = ROOT.MakeK4DataFrame(files, [electrons])

branches = ROOT.vector('string')()
for br in ['electrons']:
    branches.push_back(br)

df = df.Range(0, 10)
df.Snapshot('events', 'test.root', branches)
