import Core as fcc

test = fcc.Module('test')
print(test)

electrons = fcc.Converter('RecoParticles',
    src = 'ReconstructedParticles',
)
print(electrons)
