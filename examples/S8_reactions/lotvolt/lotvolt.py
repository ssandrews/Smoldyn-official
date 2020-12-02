# Simulation file for Lotka-Voltera reaction
import smoldyn

s = smoldyn.Simulation([-100, -100, -10], [100, 100, 10], boundary_type="p")
rabbit = s.addSpecies("rabbit", color="red", display_size=2, mol_list="rlist", difc=100)
fox = s.addSpecies("fox", color="blue", display_size=3, mol_list="flist", difc=100)
s.addMoleculePerBox(1)

r1 = s.addReaction("r1", subs=[rabbit], prds=[rabbit, rabbit], rate=10)
r2 = s.addReaction("r2", subs=[rabbit, fox], prds=[fox, fox], rate=8000)
r3 = s.addReaction("r3", subs=[fox], prds=[], rate=10)

rabbit.addToSolution(1000)
fox.addToSolution(1000)

s.setTiff("OpenGl")
s.setGraphics("opengl", iter=5, text_display=["time", "rabbit", "fox"])

s.setOutputFile("lotvoltout.txt")
s.addCommand("molcount lotvoltout.txt", "i", on=0, off=20, step=0.01)
s.addCommand("molcount stdout", "i", on=0, off=20, step=0.1)

s = s.run(5, dt=0.001, overwrite=True)


# graph of results. This doesn't run because Python quits first.
import matplotlib.pyplot as plt
import numpy as np
t,rabbit,fox = np.loadtxt("lotvoltout.txt",unpack=True)
plt.plot(t,rabbit,'r-',t,fox,'b-')
plt.show()

