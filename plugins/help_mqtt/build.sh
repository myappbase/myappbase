#!/usr/bin/env bash
/usr/bin/c++ -std=c++11  async_subscribe.cpp  -o async_subscribe -L /usr/local/lib/ -lpaho-mqttpp3 -lpaho-mqtt3as -lssl -lcrypto

/root/myapp/my/opt/clang8/bin/clang++ -std=c++11  async_subscribe.cpp  -o async_subscribe -L /usr/local/lib/ -lpaho-mqttpp3 -lpaho-mqtt3as -lssl -lcrypto

/root/myapp/my/opt/clang8/bin/clang++ -std=c++11  async_publish.cpp  -o async_publish -L /usr/local/lib/ -lpaho-mqttpp3 -lpaho-mqtt3as -lssl -lcrypto
