import ROOT
import inspect

ROOT.gSystem.Load('libk4DataSource')
_ds = ROOT.k4DataSource

def Converter(name, *args, **kwargs):
    params = ROOT.k4Parameters()
    params.setName(name)
    for k, v in kwargs.items():
        params.set(k, v)
    print(params.serialise())
    conv = ROOT.k4DataConverterFactory.get().build(params)
    conv.describe()
    callers_local_vars = inspect.currentframe().f_back.f_locals.items()
    print([var_name for var_name, var_val in callers_local_vars])
    return conv
