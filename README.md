Qt Cloud Services WebSockets Chat Example
=========================================

> Note: this example is currently experimental and requires advanced knowledge of Qt!

This is a small chat application using the managed WebSockets provided by Qt Cloud Services.
You can launch this example multiple times and on different computers and will instantly have a chat room where everyone that is connected can write messages.

This example requires the Qt WebSockets module which is part of Qt 5.3. It can be run with Qt 5.3.0-beta1 and onwards.

When building the example you need to have a Managed WebSocket instance running and copy the MWS_GATEWAY_ID and MWS_INSTANCE_ADDRESS into mainwindow.cpp. You need to disable Access Control Management by setting it to **None** in your websocket instance.

For detailed instructions follow the [getting started](https://developer.qtc.io/mws/getting-started) instructions.


----------------------------
What to do if the example does not work:
----------------------------

Make sure that you have the qtwebsockets module.

***

If you get errors like this:

```
error: 'YOUR_MWS_GATEWAY_ID_HERE' was not declared in this scope
error: 'YOUR_MWS_INSTANCE_ADDRESS_HERE' was not declared in this scope
```
then you forgot to copy your data from your [Qt Cloud Services Account](https://console.qtcloudservices.com) into `mainwindow.cpp`.

***

The example runs and shows this message:
```
Error: 204 - Host requires authentication: {"error":{"reason":"AccessDenied","message":"Access denied"}}
```
That means you did not disable the access control for this simple example.
In the management console choose your websocket instance and set the **Access Control Management** to **None**.

***

For example with Ubuntu you might come across following error message during build:
```
/usr/bin/ld: cannot find -lGL
```
In this case you are missing OpenGL related packages which can be installed with following command:
```
 sudo apt-get install libgl1-mesa-dev
```
