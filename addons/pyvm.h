#ifndef PYVM_H
#define PYVM_H

#include <node.h>
#include <Python.h>
#include <node_object_wrap.h>

namespace vm {

    using namespace v8;
    using namespace node;

    class Convert {
        public:
            static Local<Value> PytoV8Array(Isolate* isolate, PyObject* obj);
            static Local<Value> PyToV8Number(Isolate* isolate, PyObject* obj);
            static Local<Value> PyToV8Object(Isolate *isolate, PyObject *obj);
            // convert python datatype to v8 datatype
            static Local<Value> PyToV8(Isolate* isolate, PyObject* obj);
    };

    class PyVM : public node::ObjectWrap {

     public:
      PyObject* mPyObject;
      int val;
      static void Init(Local<Object> exports);

     private:
      explicit PyVM(PyObject *pyObj);
      explicit PyVM(PyObject *pyObj, int val);
      ~PyVM();

      static void New(const FunctionCallbackInfo<Value>& args);
      static void Import(const FunctionCallbackInfo<Value>& args);
      static void MyFunction(const FunctionCallbackInfo<Value>& args);
      static void List(const FunctionCallbackInfo<Value>& args);
      static Persistent<Function> constructor;
    };

}  // namespace vm

#endif
