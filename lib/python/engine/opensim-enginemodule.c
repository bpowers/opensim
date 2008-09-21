#include <pygobject.h>
#include "../../opensim-simulator.h"

extern PyTypeObject G_GNUC_INTERNAL PyOpensimSimulator_Type;

void engine_simulator_add_constants(PyObject *d, const char *strip);
void engine_variable_register_classes(PyObject *d); 


// GROSS HACK to pass a file pointer from C to Python and wrap it as a 
// Python File object.
static PyObject *
opensim_get_file (PyGObject *self, PyObject *args, PyObject *kwargs)
{
  static char *kwlist[] = { "simulator", "file", NULL };
  PyObject *py_sim;
  PyObject *input_file_object;
  PyObject *ret;
  
  if (!PyArg_ParseTupleAndKeywords (args, kwargs,"OO:get_file", kwlist, 
                                    &py_sim, &input_file_object))
    return NULL;
  
  
  if (!PyObject_IsInstance (py_sim, (PyObject *) &PyOpensimSimulator_Type))
  {
    PyErr_SetString(PyExc_RuntimeError, 
                    "get_file() arg 1 can only be a Opensim.Simulator\n");
    return NULL;
  }
  
  if (!PyObject_IsInstance (input_file_object, (PyObject *) &PyGPointer_Type))
  {
    PyErr_SetString(PyExc_RuntimeError, 
                    "get_file() arg 2 can only be a gobject.GPointer\n");
    return NULL;
  }
  
  
  gchar *f_name;
  OpensimSimulator *sim = OPENSIM_SIMULATOR (((PyGObject *) py_sim)->obj);
  g_object_get (G_OBJECT (sim), "file_name", &f_name, NULL);
  
  if (!f_name)
    f_name = "stdout";
  
  ret = PyFile_FromFile (((PyGPointer *) input_file_object)->pointer, f_name, 
                         "w", NULL);
  
  if (f_name != "stdout") g_free (f_name);
  
  return ret;
}


const PyMethodDef engine_module_functions[] = {
  {"get_file", (PyCFunction)opensim_get_file, METH_VARARGS, 
   "Wraps a pointer into a file argument.\n"},
  { NULL, NULL, 0, NULL }
};


DL_EXPORT (void)
initengine (void)
{
    PyObject *m, *d;
 
    init_pygobject ();
 
    m = Py_InitModule ("opensim.engine", engine_module_functions);
    d = PyModule_GetDict (m);
 
    engine_simulator_register_classes (d);
    engine_variable_register_classes (d);
    engine_simulator_add_constants (m, "sim_");
 
    if (PyErr_Occurred ()) 
    {
        Py_FatalError ("can't initialise module engine");
    }
}

