startDriver TuyaMCU
tuyaMcu_setBaudRate 115200
setChannelType 0 OpenClosed
SetChannelLabel 0 MainButton
setChannelType 1 OpenClosed
SetChannelLabel 0 PlusButton
setChannelType 2 OpenClosed
SetChannelLabel 0 MinusButton
setChannelType 3 ReadOnly
SetChannelLabel 3 Temperature
setChannelType 10 Toggle
SetChannelLabel 10 LED0
setChannelType 11 Toggle
SetChannelLabel 11 LED1
setChannelType 12 Toggle
SetChannelLabel 12 LED2
setChannelType 13 Toggle
SetChannelLabel 13 LED3
setChannelType 14 Toggle
SetChannelLabel 14 LED4
setChannelType 15 Toggle
SetChannelLabel 15 LED5
setChannelType 16 Toggle
SetChannelLabel 16 LED6
setChannelType 17 Toggle
SetChannelLabel 17 MainWhite
setChannelType 18 Toggle
SetChannelLabel 18 MainRed
setChannelType 20 TextField
SetChannelLabel 20 LEDFlags
setChannelType 21 TextField
SetChannelLabel 21 TriacDelay
setChannelType 22 TextField
SetChannelLabel 22 TriacOnTime
setChannelType 23 Toggle
SetChannelLabel 23 TriacForceOn
setChannelType 32 Toggle
SetChannelLabel 32 Relay
linkTuyaMCUOutputToChannel 0 1 0
linkTuyaMCUOutputToChannel 1 1 1
linkTuyaMCUOutputToChannel 2 1 2
linkTuyaMCUOutputToChannel 50 2 3
linkTuyaMCUOutputToChannel 10 1 10
linkTuyaMCUOutputToChannel 11 1 11
linkTuyaMCUOutputToChannel 12 1 12
linkTuyaMCUOutputToChannel 13 1 13
linkTuyaMCUOutputToChannel 14 1 14
linkTuyaMCUOutputToChannel 15 1 15
linkTuyaMCUOutputToChannel 16 1 16
linkTuyaMCUOutputToChannel 17 1 17
linkTuyaMCUOutputToChannel 18 1 18
linkTuyaMCUOutputToChannel 20 2 20
linkTuyaMCUOutputToChannel 32 1 32
linkTuyaMCUOutputToChannel 40 2 21
linkTuyaMCUOutputToChannel 41 2 22
linkTuyaMCUOutputToChannel 42 1 23