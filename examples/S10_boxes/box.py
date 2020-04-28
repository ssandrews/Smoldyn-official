__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2019-, Dilawar Singh"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import smoldyn as sm

sim = sm.Simulation()
sim.geometry = sm.Geometry(
        boundaries = ([0,100], [0,100], [0,100])
        , types = ['p', 'p', 'p'])

