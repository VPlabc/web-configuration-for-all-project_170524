#ifndef UpdateFW_
#define UpdateFW_
#include <Arduino.h>

#define ServerUpdateFW
class UpdateFW
{
public:
const String FirmwareVer={FW_VERSION};
 void repeatedCall();
 void ShowMess(String txt);
 void FirmwareUpdate();
 byte FirmwareVersionCheck(void);
 
};
extern UpdateFW UPDATEFW;
#endif//UpdateFW_