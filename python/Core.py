import ROOT

ROOT.gSystem.Load('libk4DataSource')
_ds = ROOT.k4DataSource

class Process(object):
    df = None
    sources = dict()
    outputBranches = ROOT.std.vector('string')()

    def __init__(self, *args, **kwargs):
        if kwargs.get('implicitMT', True):
            ROOT.EnableImplicitMT()
        pass

    def __setattr__(self, attr_name, attr_value):
        if type(attr_value) == ROOT.k4Parameters:
            self.sources[attr_name] = attr_value
        if attr_name == 'output':
            for out in list(attr_value):
                self.outputBranches.push_back(out)
        else:
            return super(Process, self).__setattr__(attr_name, attr_value)

    def makeDF(self, inputs):
        sources = []
        for k, v in self.sources.items():
            sources.append(v.set[str]('output', k))
        self.df = ROOT.MakeK4DataFrame(inputs, sources)
        return self.df

def Converter(name, **kwargs):
    parameters = ROOT.k4Parameters()
    if not parameters:
        raise RuntimeError('Failed to initialise parameters list for converter "{}"'.format(name))
    parameters.setName(name)
    for k, v in kwargs.items():
        parameters.set(k, v)
    return parameters
