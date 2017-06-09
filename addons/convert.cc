#include "convert.h"

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

// v8 number to python number
PyObject* Convert::V8ToPyNumber(Isolate *isolate, Local<Value> obj) {
    return PyFloat_FromDouble(obj->NumberValue());
}

// v8 number to python number
PyObject* Convert::V8ToPySeq(Isolate *isolate, Local<Value> obj) {
    if (obj->IsArray()) {
        Local<Array> array = Local<Array>::Cast(obj);
        int len = array->Length();
        PyObject* pyList = PyList_New(len);
        for (int i = 0; i < len; i++) {
            Local<Value> jsVal = array->Get(i);
            PyList_SET_ITEM(pyList, i, Convert::V8ToPy(isolate, jsVal));
        }
        return pyList;
    } else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

// v8 object to python dict
PyObject* Convert::V8ToPyDict(Isolate *isolate, Local<Value> obj) {
    Local<Object> jsObj = obj->ToObject();
    Local<Array> property_names = jsObj->GetPropertyNames();
    int len = property_names->Length();
    PyObject* py_dict = PyDict_New();
    for(int i = 0; i < len; ++i) {
        Local<String> str = property_names->Get(i)->ToString();
        Local<Value> js_val = jsObj->Get(str);
        PyDict_SetItemString(py_dict, *String::Utf8Value(str), Convert::V8ToPy(isolate, js_val));
    }
    return py_dict;
}

// convert v8 data to python data
PyObject* Convert::V8ToPy(Isolate* isolate, Local<Value> obj) {
    if (obj->IsNumber()) {
        return Convert::V8ToPyNumber(isolate, obj);
    } else if (obj->IsObject()) {
        return Convert::V8ToPyDict(isolate, obj);
    } else if (obj->IsArray()) {
        return Convert::V8ToPySeq(isolate, obj);
    } else {
        return Py_None;
    }
}
