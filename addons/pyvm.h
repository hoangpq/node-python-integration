#ifndef PYVM_H
#define PYVM_H

#include <node.h>
#include <Python.h>
#include <node_object_wrap.h>

namespace vm {
    using namespace v8;
    using namespace node;

    class PyVM : public node::ObjectWrap {

     public:
        PyObject* mPyObject;
        int val;
        static void Init(Local<Object> exports);

     private:
        explicit PyVM(PyObject *pyObj);
        ~PyVM();

        static void New(const FunctionCallbackInfo<Value>& args);
        static void Import(const FunctionCallbackInfo<Value>& args);
        static void Call(const FunctionCallbackInfo<Value>& args);
        static void List(const FunctionCallbackInfo<Value>& args);
        static void Callback(const FunctionCallbackInfo<Value>& args);

        // Callbacks that access maps
        static Local<Value> GetConstant(Isolate* isolate, Local<String> key, PyVM vm);
        static Persistent<Function> constructor;
    };

}  // namespace vm

#endif
