# AIS_Maggellan_DHT22

if using Mega2560 and serial2 please edit file AIS_NB_BC95.cpp 

```
void AIS_NB_BC95:: setupDevice(String serverPort)
{
	Serial2.begin(9600);
    _Serial = &Serial2;
    
```    
