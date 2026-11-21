(function () {
  'use strict';

  const params = new URLSearchParams(window.location.search);
  const scenario = {
    alerts: params.has('alerts'),
    fire: params.has('fire'),
    stuck: params.has('stuck')
  };

  const mock = {
    '/api/sensors': {
      temperature: scenario.fire ? 52.7 : 28.4,
      humidity: scenario.alerts ? 34 : 62,
      pressure: scenario.alerts ? 999.4 : 1008.2,
      distanceMm: 420,
      aht: true, bmp: true, tof: true,
      auto: false,
      autoState: 'OFF',
      autoSpeed: 140,
      avoidCount: 0,
      stuck: scenario.stuck,
      stuckSeq: scenario.stuck ? 1 : 0,
      alerts: {
        fire: scenario.fire,
        rainSensor: false,
        rain: scenario.alerts,
        hot: scenario.alerts,
        cold: false,
        dry: scenario.alerts,
        trendReady: true,
        trend30m: scenario.alerts ? -1.8 : -0.4,
        trend3h: scenario.alerts ? -3.2 : -1.1
      }
    },
    '/api/motor/status': {
      direction: 0,
      leftSpeed: 0,
      rightSpeed: 0,
      moving: false
    },
    '/api/config': {
      pwmMin: 60,
      pwmMax: 255,
      pwmDefault: 179,
      autoSpeed: 140,
      motorsEnabled: true
    },
    '/api/auto': {
      enabled: false,
      state: 'off',
      stuck: scenario.stuck,
      stuckSeq: scenario.stuck ? 1 : 0,
      avoidCount: 0,
      autoSpeed: 140
    },
    '/api/system/info': {
      preview: true,
      chip: 'ESP32',
      mac: 'aa:bb:cc:dd:ee:ff',
      flashBytes: 4194304,
      freeHeap: 112344,
      uptimeSec: 3600
    },
    '/api/wifi': { ok: true, mode: 'ap' }
  };

  function respond(body, status) {
    return {
      ok: status >= 200 && status < 300,
      status: status || 200,
      headers: { get: (k) => (String(k).toLowerCase() === 'content-type' ? 'application/json' : null) },
      json: async () => body,
      text: async () => JSON.stringify(body)
    };
  }

  function syncAuto() {
    const a = mock['/api/auto'];
    const s = mock['/api/sensors'];
    s.auto = a.enabled;
    s.autoState = a.enabled ? (a.state && a.state !== 'off' ? a.state : 'frente') : 'off';
    s.autoSpeed = a.autoSpeed;
  }

  const originalFetch = window.fetch.bind(window);
  window.fetch = function (input, opts) {
    const url = String(input);
    const path = url.split('?')[0];
    if (Object.prototype.hasOwnProperty.call(mock, path)) {
      if (opts && opts.method && opts.method.toUpperCase() === 'POST') {
        let b = {};
        try { b = opts.body ? JSON.parse(opts.body) : {}; } catch (e) {}
        if (path === '/api/config') {
          Object.assign(mock['/api/config'], b);
          if (b.autoSpeed !== undefined) { mock['/api/auto'].autoSpeed = b.autoSpeed; mock['/api/sensors'].autoSpeed = b.autoSpeed; }
        } else if (path === '/api/wifi') {
          Object.assign(mock['/api/wifi'], b);
        } else if (path === '/api/motor/move') {
          const moving = !((b.direction === 'stop') || (b.leftSpeed === 0 && b.rightSpeed === 0 && b.direction === undefined));
          mock['/api/motor/status'] = {
            direction: b.direction ? ({frente:1,tras:2,esquerda:3,direita:4,stop:0}[b.direction]||0) : 0,
            leftSpeed: b.leftSpeed !== undefined ? b.leftSpeed : (b.speed || 0),
            rightSpeed: b.rightSpeed !== undefined ? b.rightSpeed : (b.speed || 0),
            moving: moving
          };
        } else if (path === '/api/motor/stop') {
          mock['/api/motor/status'] = { direction: 0, leftSpeed: 0, rightSpeed: 0, moving: false };
        } else if (path === '/api/auto') {
          if (b.enabled !== undefined) mock['/api/auto'].enabled = !!b.enabled;
          if (b.autoSpeed !== undefined) {
            mock['/api/auto'].autoSpeed = b.autoSpeed;
            mock['/api/config'].autoSpeed = b.autoSpeed;
            mock['/api/sensors'].autoSpeed = b.autoSpeed;
          }
          syncAuto();
          return respond(Object.assign({ ok: true }, mock['/api/auto']), 200);
        } else if (path === '/api/system/restart') {
          return respond({ ok: true }, 200);
        }
        return respond({ ok: true }, 200);
      }
      syncAuto();
      return respond(mock[path], 200);
    }
    return originalFetch(input, opts);
  };

  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.getRegistrations().then(regs => regs.forEach(r => r.unregister())).catch(() => {});
  }
})();