#ifndef CONVERT_H
#define CONVERT_H

#include <Python.h>
#include <node.h>

using namespace v8;
using namespace node;

class Convert {
    public:
        static Local<Value> PytoV8Array(Isolate* isolate, PyObject* obj);
        static Local<Value> PyToV8Number(Isolate* isolate, PyObject* obj);
        static Local<Value> PyToV8Object(Isolate *isolate, PyObject *obj);

        static PyObject* V8ToPySeq(Isolate *isolate, Local<Value> obj);
        static PyObject* V8ToPyNumber(Isolate *isolate, Local<Value> obj);
        static PyObject* V8ToPyDict(Isolate *isolate, Local<Value> obj);
        // convert python data to v8 data
        static Local<Value> PyToV8(Isolate* isolate, PyObject* obj);
        // convert v8 data to python data
        static PyObject* V8ToPy(Isolate *isolate, Local<Value> obj);
};

#endif
