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
  using v8::Context;

  using namespace std;

  class Convert {
    public:
      static Local<Value> PytoV8Array(Isolate* isolate, PyObject* obj);
      static Local<Value> PyToV8Number(Isolate* isolate, PyObject* obj);
      static Local<Value> PytoV8Object(Isolate* isolate, PyObject* obj);
      
      static PyObject* V8ToPyNumber(Isolate *isolate, Local<Number> obj);
      static PyObject* V8ToPyDict(Isolate* isolate, Local<Object> obj);

      static Local<Value> PyToV8(Isolate* isolate, PyObject* obj);  
      static PyObject* V8ToPy(Isolate* isolate, Local<Value> obj);
  };

  /* Python dictionary to V8 Object */ 
  Local<Value> Convert::PytoV8Object(Isolate* isolate, PyObject* py_obj) {
    int len = PyMapping_Length(py_obj);
    Local<Object> obj = Object::New(isolate);
    PyObject* keys = PyMapping_Keys(py_obj);
    PyObject* values = PyMapping_Values(py_obj);
    for(int i = 0; i < len; ++i) {
      PyObject *key = PySequence_GetItem(keys, i), 
        *value = PySequence_GetItem(values, i),
        *key_as_string = PyObject_Str(key);
      obj->Set(String::NewFromUtf8(isolate, PyString_AsString(key_as_string)), Convert::PyToV8(isolate, value));
    }
    Py_XDECREF(keys);
    Py_XDECREF(values);
    return obj;
  }

  /* Python dictionary to V8 Object */
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

  /* Python int, float to V8 Number */
  Local<Value> Convert::PyToV8Number(Isolate* isolate, PyObject* obj) {
    Local<Value> number = Number::New(isolate, PyFloat_AsDouble(obj));
    Py_DECREF(obj);
    return number;
  }

  /* V8 Object to Python dictionary */
  PyObject* Convert::V8ToPyDict(Isolate* isolate, Local<Object> obj) {
    Local<Array> property_names = obj->GetPropertyNames();
    int len = property_names->Length();
    PyObject* py_dict = PyDict_New();
    for(int i = 0; i < len; ++i) {
      Local<String> str = property_names->Get(i)->ToString();
      Local<Value> value = obj->Get(str);
      PyDict_SetItemString(py_dict, *String::Utf8Value(str), Convert::V8ToPy(isolate, value));
    }
    return py_dict;
  }

  /* V8 Object to Python dictionary */
  PyObject* Convert::V8ToPyNumber(Isolate* isolate, Local<Number> obj) {
    return PyFloat_FromDouble(obj->NumberValue());
  }


  /* Convert Python datatype to v8 datatype */
  Local<Value> Convert::PyToV8(Isolate* isolate, PyObject* obj) {
    if (PySequence_Check(obj)) {
      return Convert::PytoV8Array(isolate, obj);
    } else if (PyNumber_Check(obj)) {
      return Convert::PyToV8Number(isolate, obj);
    } else if (PyMapping_Check(obj)) {
      return Convert::PytoV8Object(isolate, obj);
    } else {
      return Undefined(isolate);
    }
  }

  PyObject* Convert::V8ToPy(Isolate* isolate, Local<Value> obj) {
    if (obj->IsNumber()) {
      return Convert::V8ToPyNumber(obj->ToNumber());
    } else if (obj->IsObject()){
      return Convert::V8ToPyDict(isolate, obj);
    } else {
      return Py_None;
    }
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
            PyTuple_SetItem(pargs, i - 2, Convert::V8ToPy(isolate, args[i]));
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