#include "vmbtgatt.h"
#include "LGATTSABC.h"
#include "stdlib.h"

#ifdef APP_LOG
#undef APP_LOG
#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();
#endif

static LGATTServiceInfo g_abc_decl[] =
{
    {TYPE_SERVICE, "6e400001-b5a3-f393-e0a9-e50e24dcca9e", TRUE, 0, 0, 0},
    {TYPE_CHARACTERISTIC, "6e400002-b5a3-f393-e0a9-e50e24dcca9e", FALSE, VM_GATT_CHAR_PROP_NOTIFY |VM_GATT_CHAR_PROP_READ, VM_GATT_PERM_READ, 0},
    {TYPE_CHARACTERISTIC, "6e400003-b5a3-f393-e0a9-e50e24dcca9e", FALSE, VM_GATT_CHAR_PROP_NOTIFY |VM_GATT_CHAR_PROP_WRITE | VM_GATT_CHAR_PROP_READ, VM_GATT_PERM_WRITE, 0},
 
    {TYPE_END, 0, 0, 0, 0, 0}
};


LGATTServiceInfo *LGATTUT::onLoadService(int32_t index)
{
    return g_abc_decl;
}

uint16_t LGATTUT::getHandle(int32_t type)
{
    if (0 == type)
    {
        return _handle_notify;
    }
    else if (1 == type)
    {
        return _handle_write;
    }

    return 0;
}

// characteristic added
boolean LGATTUT::onCharacteristicAdded(LGATTAttributeData &data)
{
  
    /*   struct LGATTAttributeData
    uint16_t handle;  This is the handle of the attribute and it is required in LGATTServer.sendIndication(). 
    LGATTUUID uuid;   This is the attribute UUID. 
    boolean failed;   This represents if the transaction failed or not.
    */
    const VM_BT_UUID *uuid = &(data.uuid);
    APP_LOG("LGATTSABC::onCharacteristicAdded data failed=[%d] uuid[12] = [0x%x] len=[%d]", data.failed, uuid->uuid[12], uuid->len);
    if (!data.failed)
    {
        if (0x02 == uuid->uuid[12]) //use the uuid[12] to distinguish the Characteristic type
        {
            _handle_notify = data.handle;
            
        }
        else if (0x03 == uuid->uuid[12])
        {
            _handle_write = data.handle;
        }
        APP_LOG("LGATTSABC::onCharacteristicAdded uuid[12] = [0x%x]", uuid->uuid[12]);     
    }
    else
    {
      APP_LOG("LGATTSABC:: data failed");
    }
    return true;
}
// connected or disconnected
/***********
an incoming connection:      data.connected set to true. 
central device disconnects:  data.connected set to false.
***********/
boolean LGATTUT::onConnection(const LGATTAddress &addr, boolean connected)
{
    _connected = connected;
    APP_LOG("LGATTSABC::onConnection connected [%d], [%x:%x:%x:%x:%x:%x]", _connected, 
        addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0]);

    return true;
}
// read action comming from master
boolean LGATTUT::onRead(LGATTReadRequest &data)
{
    APP_LOG("LGATTSABC::onRead _connected [%d]", _connected);

    if (_connected)
    {
       LGATTAttributeValue value = {0};
        char *str ="";
        char i=0;
        char x_len=0,y_len=0,z_len=0;
        char x_lenn=0,y_lenn=0,z_lenn=0;
        
        x_len=String(acc_x).length();
        y_len=String(acc_y).length();
        z_len=String(acc_z).length();
        x_lenn=String(gyo_x).length();
        y_lenn=String(gyo_y).length();
        z_lenn=String(gyo_z).length();
    
      String(acc_x).toCharArray(str,x_len);
      str[x_len-1] = ',';
      APP_LOG("LGATTSABC::acc_x:[%s], acc_x len:[%d]",str, x_len);
      
       String(acc_y).toCharArray(&str[x_len],y_len);
       str[x_len+y_len-1] = ',';
      APP_LOG("LGATTSABC::acc_y:[%s], acc_y len:[%d]",&str[x_len],y_len);
      
       String(acc_z).toCharArray(&str[x_len+y_len], z_len);
       str[x_len+y_len+z_len-1] = ',';
      APP_LOG("LGATTSABC::acc_z:[%s], acc_z len:[%d]",&str[x_len+y_len], z_len);
      /*
      String(gyo_x).toCharArray(&str[x_len+y_len-2], z_len);
      str[x_len+y_len+z_len+x_lenn-1] = ',';
      APP_LOG("LGATTSABC::gyo_x:[%s], gyo_x len:[%d]",&str[x_len+y_len+z_len], x_lenn);
      
       String(gyo_y).toCharArray(&str[x_len+y_len+z_len], x_lenn);
       str[x_len+y_len+z_len+x_lenn+y_lenn-1] = ',';
      APP_LOG("LGATTSABC::gyo_y:[%s], gyo_y len:[%d]",&str[x_len+y_len+z_len+x_lenn],y_lenn);
      
       String(gyo_z).toCharArray(&str[x_len+y_len+z_len+x_lenn+y_lenn], z_lenn);      
      APP_LOG("LGATTSABC::gyo_z:[%s], gyo_z len:[%d]",&str[x_len+y_len+z_len+x_lenn+y_lenn], z_lenn);
      */
     // value.len = x_len+y_len+z_len+x_lenn+y_lenn+z_lenn-1;
     value.len = x_len+y_len+z_len-1;
      memcpy(value.value, str,value.len);
        
        /*
        
        for(i=0;i<7;i++)
        {
          value.value[i]=char(String(acc_x)[i]);
        }
        value.value[7]=' ';
        for(i=0;i<7;i++)
        {
          value.value[i+8]=char(String(acc_y)[i]);
        }
        value.value[15]=' ';
        for(i=0;i<7;i++)
        {
          value.value[i+16]=char(String(acc_z)[i]);
        }
        value.value[23] = '\0';
        //memcpy(value.value, str, strlen(str));
        value.len = 24;
      */
        
        APP_LOG("LGATTSABC::onRead length:[%d]  onRead value:[%s]", value.len, value.value);
        
        data.ackOK(value); //send an ack to client
       
    }
    return true;
}
// write action comming from master
boolean LGATTUT::onWrite(LGATTWriteRequest &data)
{
    APP_LOG("LGATTSABC::onWrite _connected [%d]", _connected);
    // todo read UART data.
    if (_connected)
    {
        // if need to rsp to central.
        if (data.need_rsp)
        {
            LGATTAttributeValue value;
            value.len = 0;
            data.ackOK();
        }
    
        APP_LOG("central data on peripheral rx[%s][%d]", data.value.value, data.value.len);
    }
    return true;
}

