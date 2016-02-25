// hello.cc
#include <node.h>
#include <Python.h>
#include <string>
#include <node_object_wrap.h>
#include "nan.h"

/*namespace demo {

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
      static Local<Value> PyToV8Object(Isolate *isolate, PyObject *obj);
      // convert python datatype to v8 datatype
      static Local<Value> PyToV8(Isolate* isolate, PyObject* obj);
      
      static PyObject* V8ToPySeq(Isolate *isolate, Local<Value> obj);
      static PyObject* V8ToPyNumber(Isolate *isolate, Local<Value> obj);
      static PyObject* V8ToPyDict(Isolate *isolate, Local<Value> obj);
      // convert v8 datatype to python datatype
      static PyObject* V8ToPy(Isolate *isolate, Local<Value> obj);
  };

  // python dict -> v8 object
  Local<Value> Convert::PyToV8Object(Isolate *isolate, PyObject *obj) {
    Local<Object> jsObj = Object::New(isolate);
    int len = PyMapping_Length(obj);
    PyObject* keys = PyMapping_Keys(obj);
    PyObject* values = PyMapping_Values(obj);
    for(int i = 0; i < len; ++i) {
      PyObject *key = PySequence_GetItem(keys, i),
               *value = PySequence_GetItem(values, i),
               *key_as_string = PyObject_Str(key);
      jsObj->Set(String::NewFromUtf8(isolate, PyString_AsString(key_as_string)), Convert::PyToV8(isolate, value));
      Py_XDECREF(key);
      Py_XDECREF(key_as_string);
    }
    return jsObj;
  }

  // python array -> v8 array
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

  // python number -> v8 number
  Local<Value> Convert::PyToV8Number(Isolate* isolate, PyObject* obj) {
    Local<Value> number = Number::New(isolate, PyFloat_AsDouble(obj));
    Py_DECREF(obj);
    return number;
  }

  // convert python data to v8 data
  Local<Value> Convert::PyToV8(Isolate* isolate, PyObject* obj) {
    if (PyNumber_Check(obj)) {
      return Convert::PyToV8Number(isolate, obj);
    } else if (PySequence_Check(obj)) {
      return Convert::PytoV8Array(isolate, obj);
    } else if (PyMapping_Check(obj)) {
      return Convert::PyToV8Object(isolate, obj);
    } else {
      return Undefined(isolate);
    }
  }

  // v8 number to python number
  PyObject* Convert::V8ToPyNumber(Isolate *isolate, Local<Value> obj) {
    return PyFloat_FromDouble(obj->NumberValue());
  }

  // v8 number to python number
  PyObject* Convert::V8ToPySeq(Isolate *isolate, Local<Value> &obj) {
    if (obj->IsArray()) {
      Local<Array> array = Array::Cast(*obj);
      int len = array->Length();
      PyObject* pyList = PyList_New(len);
      for (int i = 0; i < len; i++) {
        Local<Value> jsVal = array->Get(i);
        PyList_SET_ITEM(pyList, i, Convert::V8ToPy(isolate, jsVal));
      }
      return pyList;
    } else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  }

  // v8 object to python dict
  PyObject* Convert::V8ToPyDict(Isolate *isolate, Local<Value> obj) {
    Local<Object> jsObj = obj->ToObject();
    Local<Array> property_names = jsObj->GetPropertyNames();
    int len = property_names->Length();
    PyObject* py_dict = PyDict_New();
    for(int i = 0; i < len; ++i) {
      Local<String> str = property_names->Get(i)->ToString();
      Local<Value> js_val = jsObj->Get(str);
      PyDict_SetItemString(py_dict, *String::Utf8Value(str), Convert::V8ToPy(isolate, js_val));
    }
    return py_dict;
  }

  PyObject* Convert::V8ToPy(Isolate* isolate, Local<Value> obj) {
    if (obj->IsNumber()) {
      return Convert::V8ToPyNumber(isolate, obj);
    } else if (obj->IsObject()) {
      return Convert::V8ToPyDict(isolate, obj);
    } else if (obj->IsArray()) {
      return Convert::V8ToPySeq(isolate, obj);
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

    Py_Initialize();

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

*/

// myobject.cc
#include "myobject.h"
#include "nan.h"

namespace demo {

    using namespace node;
    using namespace v8;

    Persistent<Function> MyObject::constructor;

    MyObject::MyObject(double value) : value_(value) {}
    MyObject::MyObject(PyObject *pyObj): mPyObject(pyObj) {}

    MyObject::~MyObject() {}

    void MyObject::MyFunction(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, "hello world"));
    }

    void MyObject::MinusOne(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      args.GetReturnValue().Set(Number::New(isolate, 10));
    }

    void MyObject::Init(Local<Object> exports) {
      Isolate* isolate = exports->GetIsolate();

      // Prepare constructor template
      Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
      tpl->SetClassName(String::NewFromUtf8(isolate, "MyObject"));
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Prototype
      NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);
      NODE_SET_PROTOTYPE_METHOD(tpl, "method", MinusOne);
      NODE_SET_PROTOTYPE_METHOD(tpl, "import", Import);
      constructor.Reset(isolate, tpl->GetFunction());
      exports->Set(String::NewFromUtf8(isolate, "MyObject"), tpl->GetFunction());
    }

    void MyObject::Import(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        HandleScope scope(isolate);

        Py_Initialize();

        PyObject *pName, *pModule;
        pName = PyString_FromString(*String::Utf8Value(args[0]->ToString()));
        pModule = PyImport_Import(pName);

        MyObject* wrapper = new MyObject(pModule);
        Local<Object> jsobject = Object::New(isolate);

        PyObject *dict = PyModule_GetDict(pModule);
        int len = PyMapping_Length(dict);
        PyObject* keys = PyMapping_Keys(dict);
        PyObject* values = PyMapping_Values(dict);

        for(int i = 0; i < len; ++i) {
            PyObject *key = PySequence_GetItem(keys, i),
            *value = PySequence_GetItem(values, i),
            *key_as_string = PyObject_Str(key);
            if (PyCallable_Check(value)) {

                Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, MinusOne);
                Local<Function> fn = tpl->GetFunction();

                // Local<Function> cons = Nan::New<Function>(constructor);
                // Local<Object> instance = cons->NewInstance();

                jsobject->Set(String::NewFromUtf8(isolate, PyString_AsString(key_as_string)), fn);
            }
            // jsobject->Set(String::NewFromUtf8(isolate, PyString_AsString(key_as_string)), Convert::PyToV8(isolate, value));
            Py_XDECREF(key);
            Py_XDECREF(key_as_string);
        }

        // wrapper->Wrap(obj);

        Py_DECREF(pModule);
        Py_DECREF(pName);

        Py_Finalize();

        args.GetReturnValue().Set(jsobject);
    }

    void MyObject::New(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();

      if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
        PyObject *pName, *pModule;

        Py_Initialize();

        pName = PyString_FromString("sys");
        pModule = PyImport_Import(pName);
        MyObject* wrapper = new MyObject(pModule);

        Py_Finalize();

        // MyObject* obj = new MyObject(value);
        wrapper->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
      } else {
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
      }
    }

    void MyObject::PlusOne(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      MyObject* obj = ObjectWrap::Unwrap<MyObject>(args.Holder());
      obj->value_ += 1;
      args.GetReturnValue().Set(Number::New(isolate, obj->value_));
    }

    void InitAll(Local<Object> exports) {
      MyObject::Init(exports);
    }

    NODE_MODULE(addon, InitAll)

}  // namespace demo