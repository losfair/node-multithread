"use strict";

let console = {
    "log": (msg) => {
        _API_consoleLog(msg.toString());
    },
    "error": (msg) => {
        _API_consoleLog("[ERROR] " + msg.toString());
    },
    "info": (msg) => {
        _API_consoleLog("[INFO] " + msg.toString());
    }
}

let sharedVariables = {
    "set": (k, v) => {
        _API_setSharedVariableValue(k.toString(), v.toString());
    },
    "get": (k) => {
        return _API_getSharedVariableValue(k.toString());
    }
}
