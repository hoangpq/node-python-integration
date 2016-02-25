// myobject.cc
#include "myobject.h"

namespace demo {

    using namespace v8;
    using namespace node;

    Persistent<Function> MyObject::constructor;

    MyObject::MyObject(PyObject *pyObj): mPyObject(pyObj) {}
    MyObject::MyObject(PyObject *pyObj, int val): mPyObject(pyObj), val(val) {}

    MyObject::~MyObject() {
      Py_DECREF(mPyObject);
      mPyObject = NULL;
      Py_Finalize();
    }

    void MyObject::MyFunction(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      MyObject *obj = ObjectWrap::Unwrap<MyObject>(args.Holder());

      char *funcName = *String::Utf8Value(args[0]->ToString());

      PyObject *func = PyObject_GetAttrString(obj->mPyObject, funcName);

      PyObject *result;
      if (PyCallable_Check(func)) {
         result = PyObject_CallObject(func, NULL);
      } else {
         result = PyDict_GetItemString(PyModule_GetDict(obj->mPyObject), funcName);
      }
      args.GetReturnValue().Set(Convert::PyToV8(isolate, result));
    }

    void MyObject::List(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      MyObject *obj = ObjectWrap::Unwrap<MyObject>(args.Holder());

      PyObject *dict = PyModule_GetDict(obj->mPyObject);
      PyObject* keys = PyMapping_Keys(dict);

      int len = PySequence_Length(keys);
      Local<Object> arr = Array::New(isolate, len);
      for (int i = 0; i < len; i++) {
        PyObject *key = PySequence_GetItem(keys, i),
                 *key_as_string = PyObject_Str(key);
        arr->Set(i, String::NewFromUtf8(isolate, PyString_AsString(key_as_string)));
      }

      args.GetReturnValue().Set(arr);
    }

    void MyObject::Init(Local<Object> exports) {
      Isolate* isolate = exports->GetIsolate();
      // Prepare constructor template
      Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
      tpl->SetClassName(String::NewFromUtf8(isolate, "MyObject"));
      tpl->InstanceTemplate()->SetInternalFieldCount(1);
      // Prototype
      NODE_SET_PROTOTYPE_METHOD(tpl, "import", Import);
      NODE_SET_PROTOTYPE_METHOD(tpl, "call", MyFunction);
      NODE_SET_PROTOTYPE_METHOD(tpl, "list", List);
      constructor.Reset(isolate, tpl->GetFunction());
      exports->Set(String::NewFromUtf8(isolate, "MyObject"), tpl->GetFunction());
    }

    void MyObject::Import(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Py_Initialize();

        PyObject *pName = PyString_FromString(*String::Utf8Value(args[0]->ToString()));
        PyObject *moduleMain = PyImport_Import(pName);
        MyObject* obj = new MyObject(moduleMain, 10);
        obj->Wrap(args.This());

        args.GetReturnValue().Set(args.This());
    }

    void MyObject::New(const FunctionCallbackInfo<Value>& args) {
      Isolate* isolate = args.GetIsolate();
      if (args.IsConstructCall()) {
        args.GetReturnValue().Set(args.This());
      } else {
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
      }
    }

    void InitAll(Local<Object> exports) {
      MyObject::Init(exports);
    }

    NODE_MODULE(addon, InitAll);


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

}  // namespace demo