import ROOT

ROOT.gSystem.Load('libk4DataSource')
_ds = ROOT.k4DataSource

class Process(object):
    df = None
    inputs = []
    sources = dict()

    def __init__(self, *args, **kwargs):
        self.inputs = kwargs.get('inputs', [])
        pass

    def __setattr__(self, attr_name, attr_value):
        if type(attr_value) == ROOT.k4Parameters:
            self.sources[attr_name] = attr_value
        else:
            return super(Process, self).__setattr__(attr_name, attr_value)

    def df(self):
        sources = []
        for k, v in self.sources.items():
            sources.append(v.set[str]('output', k))
        self.df = ROOT.MakeK4DataFrame(self.inputs, sources)
        return self.df

def Converter(name, **kwargs):
    parameters = ROOT.k4Parameters()
    if not parameters:
        raise RuntimeError('Failed to initialise parameters list for converter "{}"'.format(name))
    parameters.setName(name)
    for k, v in kwargs.items():
        parameters.set(k, v)
    return parameters
