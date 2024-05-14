## JUST A GUIDE TO UDP

The implementation of UDP is far more simpler than TCP. If you inspect the code, the UDP will not need ``listen()`` as it does not maintain any connection hence there will be no queue of pending connections.

To start the demo:
``cc listener.c -o listener``
``cc talker.c -o talker``

