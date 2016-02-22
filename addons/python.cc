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
  using v8::Array;
  using v8::Number;
  using v8::Undefined;
  using v8::Null;
  using v8::NumberObject;

  using namespace std;

  class Convert {
    public:
      static Local<Value> PytoV8Array(Isolate* isolate, PyObject* obj);
      static Local<Value> PyToV8Number(Isolate* isolate, PyObject* obj);
      static PyObject* V8ToPyNumber(Local<Number> val);
      static Local<Value> PyToV8(Isolate* isolate, PyObject* obj);
  };

  Local<Value> Convert::PytoV8Array(Isolate* isolate, PyObject* obj) {
    // get len of list sequence
    int len = PySequence_Length(obj);
    Local<Object> array = Array::New(isolate, len);
    PyObject* item;
    for (int i = 0; i < len; i++) {
      item = PySequence_GetItem(obj, i);
      array->Set(i, Convert::PyToV8Number(isolate, item));
    }
    Py_DECREF(item);
    return array;
  }

  Local<Value> Convert::PyToV8Number(Isolate* isolate, PyObject* obj) {
    Local<Value> number = Number::New(isolate, PyFloat_AsDouble(obj));
    Py_DECREF(obj);
    return number;
  }

  PyObject* Convert::V8ToPyNumber(Local<Number> val) {
    return PyFloat_FromDouble(val->NumberValue());
  }

  Local<Value> Convert::PyToV8(Isolate* isolate, PyObject* obj) {
    Local<Value> res = Object::New(isolate);
    if (strcmp(obj->ob_type->tp_name, "list") == 0) {
      res = Convert::PytoV8Array(isolate, obj);
    }
    if (strcmp(obj->ob_type->tp_name, "float") == 0 ||
      strcmp(obj->ob_type->tp_name, "int") == 0) {
      res = Convert::PyToV8Number(isolate, obj);
    }
    // clean PyObject
    Py_DECREF(obj);
    return res;
  }

  void Execute(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      Py_Initialize();
      PyObject *result, *pName, *pModule, *pDict, *pFunc;
      pName = PyString_FromString(*String::Utf8Value(args[0]->ToString()));
      pModule = PyImport_Import(pName);
      pDict = PyModule_GetDict(pModule);
      pFunc = PyDict_GetItemString(pDict, *String::Utf8Value(args[1]->ToString()));
      if (PyCallable_Check(pFunc)) {
        if (args.Length() > 2) {
          PyObject *pargs = PyTuple_New(args.Length() - 2);
          for (int i = 2; i < args.Length(); i++) {
            if (args[i]->IsNumber()) {
              PyTuple_SetItem(pargs, i - 2, Convert::V8ToPyNumber(args[i]->ToNumber()));
            }
          }
          result = PyObject_CallObject(pFunc, pargs);
        } else {
          result = PyObject_CallObject(pFunc, NULL);
        }
      }
      // Clean up
      Py_DECREF(pModule);
      Py_DECREF(pName);
      Handle<Value> res = Convert::PyToV8(isolate, result);
      Py_DECREF(result);
      Py_Finalize();
      args.GetReturnValue().Set(res);
  }

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
    // clean python vm
    Py_Finalize();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, result->ob_type->tp_name));
    // args.GetReturnValue().Set(d_Val);
    // args.GetReturnValue().Set(String::NewFromUtf8(isolate, native_string.c_str()));
  }

  void init(Local<Object> exports) {
      NODE_SET_METHOD(exports, "execute", Execute);
      NODE_SET_METHOD(exports, "getUndefined", Method);
  }

  NODE_MODULE(addon, init)

}  // namespace demo