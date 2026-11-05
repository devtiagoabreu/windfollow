#ifndef DASHBOARD_SERVICE_H
#define DASHBOARD_SERVICE_H

#include <Arduino.h>
#include "SmartCamService.h"

class DashboardService : public SmartCamService {
public:
    DashboardService();
    ~DashboardService();
    bool init() override;
    void tick() override;
    bool deinit() override;
    bool isRunning() const override;

    bool servePage(const char* page);
    bool registerPage(const char* route, const char* filePath);
    bool setSystemInfo(const char* key, const char* value);
    bool broadcastEvent(const char* eventType, const char* jsonData);

    const char* name() const override { return "DashboardService"; }

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
    void handleMotionMove();
    void handleMotionStop();
    void handleTrackingStart();
    void handleTrackingStop();
    void handleTrackingStatus();

private:
    bool m_running;
};

extern DashboardService dashboardService;

#endif
