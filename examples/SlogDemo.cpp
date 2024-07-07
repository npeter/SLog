#include "SLog.h"
#include "Credentials.h"

#ifndef CREDENTIALS
#define WIFI_SSID "myssid"         // your network SSID
#define WIFI_PWD "mypwd"           // your network password
#define SYSLOG_HOST "192.42.42.42" // your SysLog-Host
#endif

void setup()
{
  bool wifi_connected = false;

  // initialize SLog serial interface
  g_Slog.Begin(Serial, 115200);

#ifdef ARDUINO_USB_CDC_ON_BOOT
  delay(5000); // wait for USD device ready
#endif

  SLOG_NOTICE_PRINTF("SLogDemo started\n");

  SLOG_INFO_PRINTF("Try to connect with WFi %s\n", WIFI_SSID);

  {
    uint32_t start_time_ms = millis();
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    do
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        wifi_connected = true;
        break;
      }
    } while (millis() - start_time_ms < 30000);
  }

  if (wifi_connected)
  {
    // initialize SLog host
    g_Slog.Begin(String(SYSLOG_HOST), 514);

    IPAddress ip = WiFi.localIP();
    SLOG_DEBUG_PRINTF("Connected with %s ip=%d.%d.%d.%d\n", WiFi.SSID(), ip[0], ip[1], ip[2], ip[3]);
  }
  else
  {
    SLOG_ERROR_PRINTF("WiFi not connected\n");
  }
}

/**
 * Test SLOG levels
 */
void slog_test_print()
{
  for (uint8_t slog_lvl_msk = SLOG_WARNING; slog_lvl_msk <= SLOG_DEBUG; slog_lvl_msk++)
  {
    g_Slog.SetLvlMsk(slog_lvl_msk);

    Serial.printf("SlogDemo: from %s to %s with serial_enable=%s syslog_enable=%s lvl_msk=%s\n",
                  g_Slog.Lvl2Str(SLOG_EMERGENCY).c_str(),
                  g_Slog.Lvl2Str(slog_lvl_msk),
                  g_Slog.GetEnableSerial() ? "true" : "false",
                  g_Slog.GetEnableSyslog() ? "true" : "false",
                  g_Slog.GetLvlMskStr().c_str());

    for (uint8_t slog_lvl = SLOG_EMERGENCY; slog_lvl <= SLOG_DEBUG; slog_lvl++)
    {
      SLOG_PRINTF(slog_lvl, "SLog level=%s\n%s", g_Slog.Lvl2Str(slog_lvl).c_str(), slog_lvl == SLOG_DEBUG ? "\n" : "");
    }
    delay(1000);
  }
  g_Slog.SetLvlMsk(SLOG_DEBUG);
  delay(5000);
}


void loop()
{

  g_Slog.SetEnableSerial(true);
  g_Slog.SetEnableSyslog(true);
  slog_test_print();

  g_Slog.SetEnableSerial(false);
  slog_test_print();

  g_Slog.SetEnableSerial(true);
  g_Slog.SetEnableSyslog(false);
  slog_test_print();
}
