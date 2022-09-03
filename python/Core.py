import ROOT

ROOT.gSystem.Load('libk4DataSource')
_ds = ROOT.k4DataSource

def Converter(name, *args, **kwargs):
    params = ROOT.k4Parameters()
    params.setName(name)
    for k, v in kwargs.items():
        params.set(k, v)
    return ROOT.k4DataConverterFactory.get().build(params)
