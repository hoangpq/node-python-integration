// hello.cc
#include <node.h>
#include <Python.h>
#include <string>

namespace demo {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Undefined;
using v8::Handle;

using namespace std;

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Handle<Object> location_obj = Handle<Object>::Cast(args[0]);
  Handle<Value> lat_Value = location_obj->Get(String::NewFromUtf8(isolate, "lat"));
  Handle<Value> txt_Value = location_obj->Get(String::NewFromUtf8(isolate, "txt"));
  double n_Val = lat_Value->NumberValue() + 10;
  // args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
  PyObject* as_string = PyString_FromString(*String::Utf8Value(txt_Value->ToString()));
  string native_string(PyString_AsString(as_string));
  Py_XDECREF(as_string);

  /* execute python code */
  Py_Initialize();
  PyRun_SimpleString("import sys; sys.path.append('.')");
  PyRun_SimpleString("print 'Hello, world'");

  /* execute some code */
  PyObject *moduleMainString = PyString_FromString("__main__");
  PyObject *moduleMain = PyImport_Import(moduleMainString);

  PyRun_SimpleString(
      "def mul(a, b):                                 \n"\
      "   arr = map(lambda x: x*x, [1, 2, 3])         \n"\
      "   return reduce(lambda x,y: x+y, arr)         \n"\
  );

  PyObject *func = PyObject_GetAttrString(moduleMain, "mul");
  PyObject *pargs = PyTuple_Pack(2, PyFloat_FromDouble(3.0), PyFloat_FromDouble(4.0));

  PyObject *result = PyObject_CallObject(func, pargs);
  double d_Val = PyFloat_AsDouble(result);

  Py_Finalize();

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, result->ob_type->tp_name));
  // args.GetReturnValue().Set(d_Val);

  // args.GetReturnValue().Set(String::NewFromUtf8(isolate, native_string.c_str()));
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getUndefined", Method);
}

NODE_MODULE(addon, init)

}  // namespace demo