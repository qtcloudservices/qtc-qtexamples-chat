Qt Cloud Services WebSockets Chat Example
=========================================

> Note: this example is currently experimental and requires advanced knowledge of Qt!

This is a small chat application using the managed WebSockets provided by Qt Cloud Services.
You can launch this example multiple times and on different computers and will instantly have a chat room where everyone that is connected can write messages.

This example requires the Qt WebSockets module which is part of Qt 5.3. It can be run with Qt 5.3.0-beta1 and onwards.

When building the example you need to have a Managed WebSocket instance running and copy the MWS_GATEWAY_ID and MWS_INSTANCE_ADDRESS into mainwindow.cpp.

For detailed instructions follow the [getting started](https://developer.qtc.io/mws/getting-started) instructions.


----------------------------
What to do if the example does not compile:
----------------------------

Make sure that you have the qtwebsockets module.

If you get errors like this:

```
error: 'YOUR_MWS_GATEWAY_ID_HERE' was not declared in this scope
error: 'YOUR_MWS_INSTANCE_ADDRESS_HERE' was not declared in this scope
```
then you forgot to copy your data from your [Qt Cloud Services Account](https://console.qtcloudservices.com) into `mainwindow.cpp`.
