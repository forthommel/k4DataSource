import Core as fcc

process = fcc.Process(
    implicitMT = False,
)

# first define all intermediate converters and expected output branches

process.electrons = fcc.Converter('RecoParticles',
    src = 'ReconstructedParticles',
)
process.recoParticles = fcc.Converter('RecoParticles')
process.jets = fcc.Converter('RecoJets',
    src = 'Jet',
)
process.output = ['electrons', 'jets', 'recoParticles']

# user-defined processing

df = process.makeDF([
    'root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root'
])
df = df.Range(0, 10)

df.Snapshot('events', 'test.root', process.outputBranches)
