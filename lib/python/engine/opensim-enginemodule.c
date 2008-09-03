#include <pygobject.h>
 
void engine_register_classes (PyObject *d); 
extern PyMethodDef engine_functions[];
 
DL_EXPORT(void)
initengine(void)
{
    PyObject *m, *d;
 
    init_pygobject ();
 
    m = Py_InitModule ("opensim.engine", engine_functions);
    d = PyModule_GetDict (m);
 
    engine_register_classes (d);
 
    if (PyErr_Occurred ()) 
    {
        Py_FatalError ("can't initialise module engine");
    }
}

