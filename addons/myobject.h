#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <Python.h>
#include <node_object_wrap.h>

namespace demo {

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

class MyObject : public node::ObjectWrap {

 public:
  PyObject* mPyObject;
  int val;
  static void Init(Local<Object> exports);

 private:
  explicit MyObject(PyObject *pyObj);
  explicit MyObject(PyObject *pyObj, int val);
  ~MyObject();

  static void New(const FunctionCallbackInfo<Value>& args);
  static void Import(const FunctionCallbackInfo<Value>& args);
  static void MyFunction(const FunctionCallbackInfo<Value>& args);
  static void List(const FunctionCallbackInfo<Value>& args);
  static Persistent<Function> constructor;
};

}  // namespace demo

#endif