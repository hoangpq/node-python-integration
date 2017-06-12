#include "pyvm.h"
#include "convert.h"
#include <iostream>

namespace vm {

    using namespace v8;
    using namespace node;
    using namespace std;

    Persistent<Function> PyVM::constructor;

    PyVM::PyVM(PyObject *pyObj): mPyObject(pyObj) {}

    PyVM::~PyVM() {
      Py_DECREF(mPyObject);
      mPyObject = NULL;
      Py_Finalize();
    }

    void PyVM::Call(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        PyVM *obj = ObjectWrap::Unwrap<PyVM>(args.Holder());
        PyObject *pDict = PyModule_GetDict(obj->mPyObject);
        // convert to char*
        String::Utf8Value str(args[0]);
        const char *funcName = *str;
        PyObject *pFunc = PyDict_GetItemString(pDict, funcName);
        int len = args.Length();

        PyObject *result;
        if (PyCallable_Check(pFunc) == 1) {
            if (len > 1) {
                PyObject *pargs = PyTuple_New(len - 1);
                for (int i = 1; i < args.Length(); i++) {
                    PyTuple_SetItem(pargs, i - 1, Convert::V8ToPy(isolate, args[i]));
                }
                result = PyObject_CallObject(pFunc, pargs);
            } else {
                result = PyObject_CallObject(pFunc, NULL);
            }
        } else {
            result = PyDict_GetItemString(pDict, funcName);
        }
        args.GetReturnValue().Set(Convert::PyToV8(isolate, result));
    }

    void PyVM::List(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        PyVM *obj = ObjectWrap::Unwrap<PyVM>(args.Holder());
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

    Local<Value> PyVM::GetConstant(Isolate* isolate, Local<String> key, PyVM &vm) {
        // unwrap node object
        PyObject *pDict = PyModule_GetDict(vm->mPyObject);

        String::Utf8Value str(key);
        const char *funcName = *str;
        PyObject* result = PyDict_GetItemString(pDict, funcName);

        return Convert::PyToV8(isolate, result);
    }

    void PyVM::Callback(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<String> fname = Local<String>::Cast(args.Callee()->GetName());

        // unwrap node object
        PyVM *vm = ObjectWrap::Unwrap<PyVM>(args.Holder());
        PyObject *pDict = PyModule_GetDict(vm->mPyObject);
        // convert to char*
        String::Utf8Value str(fname);
        const char *funcName = *str;

        PyObject *pFunc = PyDict_GetItemString(pDict, funcName);
        int len = args.Length();
        PyObject *result;
        if (PyCallable_Check(pFunc) == 1) {
            cout << funcName << "Can call" << endl;
            if (len > 0) {
                PyObject* pargs = PyTuple_New(len);
                for (int i = 0; i < len; i++) {
                    PyTuple_SetItem(pargs, i, Convert::V8ToPy(isolate, args[i]));
                }
                result = PyObject_CallObject(pFunc, pargs);
            } else {
                result = PyObject_CallObject(pFunc, NULL);
            }
        } else {
            result = PyDict_GetItemString(pDict, funcName);
        }
        Local<Value> val = Convert::PyToV8(isolate, result);

        // destroy
        Py_XDECREF(pDict);
        Py_XDECREF(pFunc);
        Py_XDECREF(result);

        args.GetReturnValue().Set(val);
    }

    void PyVM::Init(Local<Object> exports) {

        cout << "PyVM::Init" << endl;

        Isolate* isolate = exports->GetIsolate();
        // Prepare constructor template
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
        Local<ObjectTemplate> obj_tpl = tpl->InstanceTemplate();
        obj_tpl->SetInternalFieldCount(1);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(tpl, "import", Import);
        NODE_SET_PROTOTYPE_METHOD(tpl, "call", Call);
        NODE_SET_PROTOTYPE_METHOD(tpl, "list", List);

        // export construction
        constructor.Reset(isolate, tpl->GetFunction());
        Local<FunctionTemplate> import = FunctionTemplate::New(isolate, Import);
        exports->Set(String::NewFromUtf8(isolate, "import"), import->GetFunction());
    }

    void PyVM::Import(const FunctionCallbackInfo<Value>& args) {
        cout << "PyVM::Import" << endl;
        Isolate* isolate = args.GetIsolate();
        // convert to char*
        String::Utf8Value str(args[0]);
        const char *funcName = *str;
        PyObject *pName = PyString_FromString(*String::Utf8Value(args[0]->ToString()));
        PyObject *moduleMain = PyImport_Import(pName);
        Py_XDECREF(pName);

        PyVM* vm = new PyVM(moduleMain);
        const unsigned argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();

        PyObject *dict = PyModule_GetDict(moduleMain);
        PyObject* keys = PyMapping_Keys(dict);
        int len = PySequence_Length(keys);
        for (int i = 0; i < len; i++) {
            PyObject *key = PySequence_GetItem(keys, i),
                     *key_as_string = PyObject_Str(key);
            char* funcName = PyString_AsString(key_as_string);
            Local<String> jsKey = String::NewFromUtf8(isolate, funcName);

            if (PyCallable_Check(key) == 1) {
                Local<FunctionTemplate> getter = FunctionTemplate::New(isolate, Callback);
                Local<Function> func = getter->GetFunction();
                func->SetName(String::NewFromUtf8(isolate, funcName));
                instance->Set(jsKey, func);
            } else {
                instance->Set(jsKey, PyVM::GetConstant(isolate, jsKey, &vm));
            }
        }
        vm->Wrap(instance);
        args.GetReturnValue().Set(instance);
    }

    void PyVM::New(const FunctionCallbackInfo<Value>& args) {
        cout << "PyVM::New" << endl;
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
        // Init PythonVM
        Py_Initialize();
        PyVM::Init(exports);
    }

    NODE_MODULE(addon, InitAll);

}  // namespace vm
