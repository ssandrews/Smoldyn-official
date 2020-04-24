/*
 * Python module.
 * Author: Dilawar Singh <dilawar.s.rajput@gmail.com>
 */

#include <iostream>
#include <string>
#include <variant>
#include <map>

using namespace std;

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"
#include "pybind11/functional.h"

#include "../Smoldyn/smoldynfuncs.h"

#include "Smoldyn.h"
#include "SmoldynSpecies.h"

using namespace pybind11::literals;   // for _a 



/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Initialize and run the simulation. This function takes input file
 * and run the simulation. This is more or less same as `main()` function in
 * smoldyn.c file.
 *
 * @Param filepath
 * @Param flags
 *
 * @Returns
 */
/* ----------------------------------------------------------------------------*/
int init_and_run(const string& filepath, const string& flags)
{
    int er = 0, wflag;

    string filename, fileroot;
    auto pos = filepath.find_last_of('/');

    fileroot = filepath.substr(0, pos + 1);
    filename = filepath.substr(pos + 1);

    simptr sim = nullptr;
#ifdef OPTION_VCELL
    er = simInitAndLoad(fileroot.c_str(), filename.c_str(), &sim, flags.c_str(),
                        new SimpleValueProviderFactory(), new SimpleMesh());
#else
    er =
        simInitAndLoad(fileroot.c_str(), filename.c_str(), &sim, flags.c_str());
#endif
    if(!er) {
        // if (!tflag && sim->graphss && sim->graphss->graphics != 0)
        // gl2glutInit(0, "");
        er = simUpdateAndDisplay(sim);
    }
    if(!er)
        er = scmdopenfiles((cmdssptr)sim->cmds, wflag);
    if(er) {
        simLog(sim, 4, "%sSimulation skipped\n", er ? "\n" : "");
    }
    else {
        fflush(stdout);
        fflush(stderr);
        if(!sim->graphss || sim->graphss->graphics == 0) {
            er = smolsimulate(sim);
            endsimulate(sim, er);
        }
        else {
            smolsimulategl(sim);
        }
    }
    simfree(sim);
    simfuncfree();
    return er;
}

PYBIND11_MODULE(_smoldyn, m)
{
    m.doc() = R"pbdoc(
        smoldyn
        -----------------------
    )pbdoc";

    py::class_<SmoldynDefine>(m, "__Define__")
        .def(py::init<>())
        .def("__setitem__", &SmoldynDefine::setDefine)
        .def("__getitem__", &SmoldynDefine::getDefine)
        .def("__repr__", &SmoldynDefine::__repr__)
        ;

    /* Species */
    py::class_<SmoldynSpecies>(m, "Species")
        .def(py::init<const string&>())
        .def("__repr__",[](const SmoldynSpecies& sp) {
                return "<smoldyn.Species: name=" + sp.getName() + ">";
                })
        .def_property("difc", &SmoldynSpecies::getDiffConst, &SmoldynSpecies::setDiffConst)
        .def_property("color", &SmoldynSpecies::getColor, &SmoldynSpecies::setColor)
        .def_property("display_size", &SmoldynSpecies::getDisplaySize, &SmoldynSpecies::setDisplaySize)
        ;

    py::enum_<MolecState>(m, "mState")
        .value("MSsoln", MolecState::MSsoln)
        .value("MSfront", MolecState::MSfront)
        .value("MSback", MolecState::MSback)
        .value("MSup", MolecState::MSup)
        .value("MSdown", MolecState::MSdown)
        .value("MSbsoln", MolecState::MSbsoln)
        .value("MSall", MolecState::MSall)
        .value("MSnone", MolecState::MSnone)
        .value("MSsome", MolecState::MSsome)
        ;

    /* Model */
    py::class_<Smoldyn>(m, "Model")
        .def(py::init<>())
        .def_property_readonly("define", &Smoldyn::getDefine, py::return_value_policy::reference)
        .def_property("dim", &Smoldyn::getDim, &Smoldyn::setDim)
        .def_property("seed", &Smoldyn::getRandomSeed, &Smoldyn::setRandomSeed)
        .def_property("bounds", &Smoldyn::getBounds, &Smoldyn::setBounds)
        .def("run", &Smoldyn::run, "stoptime"_a, "starttime"_a=0.0, "dt"_a=1e-5)
        .def("setPartitions", &Smoldyn::setPartitions)
        .def("addSpecies", &Smoldyn::addSpecies, "name"_a, "mollist"_a="")
        .def("setSpeciesMobility", &Smoldyn::setSpeciesMobility
                , "species"_a, "state"_a, "diffConst"_a, "drift"_a=0
                , "difmatrix"_a=0)
        ;

    /* Function */
    m.def("load_model", &init_and_run, "Load model from a txt file");


    /* attributes */
    m.attr("__version__") = SMOLDYN_VERSION;
}
