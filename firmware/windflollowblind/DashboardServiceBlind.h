#ifndef DASHBOARD_SERVICE_BLIND_H
#define DASHBOARD_SERVICE_BLIND_H

#include <Arduino.h>
#include "SmartCamService.h"

class DashboardServiceBlind : public SmartCamService {
public:
    DashboardServiceBlind();
    ~DashboardServiceBlind();
    bool init() override;
    void tick() override;
    bool deinit() override;
    bool isRunning() const override;

    bool servePage(const char* page);
    bool registerPage(const char* route, const char* filePath);
    bool setSystemInfo(const char* key, const char* value);
    bool broadcastEvent(const char* eventType, const char* jsonData);

    const char* name() const override { return "DashboardServiceBlind"; }

    void registerRoutes();
    void handleRoot();
    void handleCss();
    void handleJs();
    void handleSystemInfo();
    void handleNetworkInfo();
    void handleWifiStatus();
    void handleWifiScan();
    void handleWifiConfig();
    void handleConfigGet();
    void handleConfigPost();
    void handleConfigReset();
    void handleMotorMove();
    void handleMotorStop();
    void handleMotorStatus();

private:
    bool m_running;
};

extern DashboardServiceBlind dashboardServiceBlind;

#endif // DASHBOARD_SERVICE_BLIND_H
