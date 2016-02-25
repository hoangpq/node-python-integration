// connvert.h

using namespace v8;
using namespace node;

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