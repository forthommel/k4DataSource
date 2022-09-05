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
        self.assertEqual(electrons.get[ROOT.std.string]('src'), 'ReconstructedParticles')
        self.assertEqual(electrons.get[ROOT.std.string]('output'), 'electrons')
        self.assertEqual(electrons.get[int]('foo'), 42)
        self.assertEqual(electrons.get[int]('bar'), True) #FIXME
        self.assertEqual(electrons.get[ROOT.double]('baz'), 3.1415)

unittest.main()
