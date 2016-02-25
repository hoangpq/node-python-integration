#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <Python.h>
#include <node_object_wrap.h>

namespace demo {

class MyObject : public node::ObjectWrap {

 PyObject* mPyObject;
 public:
  static void Init(v8::Local<v8::Object> exports);
  void MyFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit MyObject(double value = 0);
  explicit MyObject(PyObject *pyObj);
  ~MyObject();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Import(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void PlusOne(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void MinusOne(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  double value_;
};

}  // namespace demo

#endif