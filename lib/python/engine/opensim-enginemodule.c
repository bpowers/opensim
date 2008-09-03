#include <pygobject.h>

void engine_simulator_add_constants(PyObject *d, const char *strip);
void engine_variable_register_classes (PyObject *d); 
extern PyMethodDef engine_simulator_functions[];
 
DL_EXPORT(void)
initengine(void)
{
    PyObject *m, *d;
 
    init_pygobject ();
 
    m = Py_InitModule ("opensim.engine", engine_simulator_functions);
    d = PyModule_GetDict (m);
 
    engine_simulator_register_classes(d);
    engine_variable_register_classes(d);
    //engine_simulator_add_constants(d, NULL);
 
    if (PyErr_Occurred ()) 
    {
        Py_FatalError ("can't initialise module engine");
    }
}

