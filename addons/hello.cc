// hello.cc
#include <node.h>
#include <Python.h>

namespace demo {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void Method(const FunctionCallbackInfo<Value>& args) {
    PyObject *x_obj, *y_obj, *yerr_obj;
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

void Test(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(jargs);
}

void init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "hello", Method);
    NODE_SET_METHOD(exports, "test", Test);
}

NODE_MODULE(addon, init)

}  // namespace demo