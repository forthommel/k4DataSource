import Core as fcc
import unittest

import ROOT

class TestConverter(unittest.TestCase):
    def testAttributes(self):
        electrons = fcc.Converter('RecoParticles',
            src = 'ReconstructedParticles',
            output = 'electrons',
            foo = 42,
            bar = True,
            baz = 3.1415,
        )
        params = electrons.parameters()
        self.assertEqual(params.get[ROOT.std.string]('src'), 'ReconstructedParticles')
        self.assertEqual(params.get[ROOT.std.string]('output'), 'electrons')
        self.assertEqual(params.get[int]('foo'), 42)
        self.assertEqual(params.get[int]('bar'), True) #FIXME
        self.assertEqual(params.get[ROOT.double]('baz'), 3.1415)

unittest.main()
