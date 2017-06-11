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

    void PyVM::Callback(const FunctionCallbackInfo<Value>& args) {
        cout << "Callable" << endl;
        args.GetReturnValue().Set(Local<Value>());
    }

    void PyVM::MapGet(Local<String> property, const PropertyCallbackInfo<Value>& info) {
        cout << "Map get" << endl;
        info.GetReturnValue().Set(Local<Value>());
    }

    void PyVM::MapSet(Local<Name> name, Local<Value> value_obj,
                                        const PropertyCallbackInfo<Value>& info) {
      info.GetReturnValue().Set(Local<Value>());
    }

    void PyVM::Init(Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        // Prepare constructor template
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
        Local<ObjectTemplate> obj_tpl = tpl->InstanceTemplate();

        proto->SetAccessor(String::NewFromUtf8(isolate, "valueOf"), MapGet);
        tpl->SetClassName(String::NewFromUtf8(isolate, "vm"));

        obj_tpl->SetInternalFieldCount(1);
        obj_tpl->SetNamedPropertyHandler(MapGet);

        // If we're calling `toString`, delegate to our version of ToString
        // proto->SetAccessor(String::NewSymbol("toString"), ToStringAccessor);
        // likewise for valueOf
        // Python objects can be called as functions.
        obj_tpl->SetCallAsFunctionHandler(Call, Handle<Value>());

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(tpl, "import", Import);
        NODE_SET_PROTOTYPE_METHOD(tpl, "call", Call);
        NODE_SET_PROTOTYPE_METHOD(tpl, "list", List);

        constructor.Reset(isolate, tpl->GetFunction());
        // exports->Set(String::NewFromUtf8(isolate, "vm"), tpl->GetFunction());

        Local<FunctionTemplate> import = FunctionTemplate::New(isolate, Import);
        exports->Set(String::NewFromUtf8(isolate, "import"), import->GetFunction());
    }

    void PyVM::Import(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        // convert to char*
        String::Utf8Value str(args[0]);
        const char *funcName = *str;
        PyObject *pName = PyString_FromString(*String::Utf8Value(args[0]->ToString()));
        PyObject *moduleMain = PyImport_Import(pName);

        Py_XDECREF(pName);
        PyVM* vm = new PyVM(moduleMain);

        proto->SetAccessor(String::NewFromUtf8(isolate, "valueOf"), MapGet);

        const unsigned argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();

        PyObject *dict = PyModule_GetDict(vm->mPyObject);
        PyObject* keys = PyMapping_Keys(dict);
        int len = PySequence_Length(keys);
        Local<Object> arr = Array::New(isolate, len);
        for (int i = 0; i < len; i++) {
            PyObject *key = PySequence_GetItem(keys, i),
                     *key_as_string = PyObject_Str(key);

            char* funcName = PyString_AsString(key_as_string);

        }


        vm->Wrap(instance);

        args.GetReturnValue().Set(instance);

    }

    void PyVM::New(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            args.GetReturnValue().Set(args.This());
        } else {

            cout << "Create template to handle function" << endl;

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
