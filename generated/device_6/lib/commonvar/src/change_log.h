#ifndef CHANGELOG_H_
#define CHANGELOG_H_

#define CO_INVENTOR_CORE "2.6.1"
/*
2.2.0 >> update fiture scheduling
2.2.1 >> fix bug scheduling
2.2.2 >> fix bug BLE restart advertisment data
2.3.0 >> Now BLE Advertisment state dynamically updated. 
        When reset or powered on the device will advertise regardless of internet connection or BLE setup. until standby interval (second) passed.
        After that it will advertise according to the internet connection regardless of BLE setup.
        If the device is connected to the internet, it will stop advertising.
        If the device is not connected to the internet, it will advertise.
        If you want to stop this features of the BLE advertising state, you can modify mqttTask.cpp.

2.4.0 >> Add support for dynamic voltage sense (solar sense, batsense, charge enable, control 3.3V, control battery)
         moving version related to change_log.h

2.4.1 >> init all voltage related control and sense, todo: bat voltage for esps3 still wrong for ADC_CHANNEL_ENUM

2.4.2 >> fix dynamic advertising ble

2.4.3 >> fix watchdog voltage reading for ESP32 S3

2.4.4 >> fix watchdog for charge detection even when no sensor available using logic (when charge the battery voltage will jump)

2.5.0 >> add get radio status
      >> fixing get radio acknowledge status
      >> major update to radio task
      >> add error buffer to watchDog, add get_error_status, insert_errorcode, update error code
      >> add get_bluetooth status
      >> add get mqtt status
      >> add get radio status
      >> add hostname to network
      >> add new fuction send radio external simple (only headerradio struct param required)
      >> fix unused variable in radio task
      >> add commondef.h in commonvar
      >> refactor all dynamic allocation in handler to use spi ram if available
      >> add esp_now capability

2.6.0
      >> fixing esp_now node role
      >> refactor otadownload to commonvar.cpp
      >> add default ota check in main.cpp
      >> device info use key in heatbeat (secure or not)
      >> fix typo esp now
      >> feat add get_rtc_temp function
      >> fix validate data type for json to struct command
      >> major feat: support recorded_at in json command and struct (when in json it will be datetime timestamp while in struct it will be epoch)
      >> major feat: add check data, upload data, get data
      >> fix upload data, get data function
      >> fix id command halnder validate format is <int> it should be uint32_t
      >> feat: add clear schedule to reset all schedule

2.6.1 >> add free space and definition for file system
      >> fix struct management cannot handle string properly   
      */



#define HARDWARE_VER "1.0.0"
/*
1.0.0 >> Initial release
*/
#define SOFTWARE_VER "1.0.0"
/*
1.0.0 >> Initial release
*/
#endif