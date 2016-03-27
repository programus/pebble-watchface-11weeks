var battery;
var config;

var CHARGING_MASK =           0x80;
var LEVEL_MASK =              0x7f;
var BATTERY_API_UNSUPPORTED = 0x70;

var PKEY_CONFIG = "pebble-watch-11weeks-config";
var DEFAULT_CONFIG = 0;

var getBatteryStateInt = function () {
  'use strict';
  var state = 0;
  console.log("battery is: " + battery);
  if (battery) {
    state = Math.round(battery.level * 100);
    if (battery.charging) {
      state |= CHARGING_MASK;
    }
    console.log('got battery state: ' + state);
  } else {
    state = BATTERY_API_UNSUPPORTED;
    console.log('battery API unsupported.');
  }
  return state;
};

var sendBatteryState = function () {
  'use strict';
  var dict = {
    "KEY_PHONE_BATTERY": getBatteryStateInt()
  };
  Pebble.sendAppMessage(dict, function() {
    console.log('Message sent successfully: ' + JSON.stringify(dict));
  }, function(e) {
    console.log('Message failed: ' + JSON.stringify(e));
  });
};

var handleBattery = function () {
  'use strict';
  if (battery) {
    battery.addEventListener('chargingchange', sendBatteryState);
    battery.addEventListener('levelchange', sendBatteryState);
  }
  sendBatteryState();
};

var initBattery = function () {
  'use strict';
  battery = navigator.battery || navigator.webkitBattery || navigator.mozBattery;
  console.log("set battery: " + battery);
  if (navigator.getBattery) {
    console.log("battery API exists.");
    navigator.getBattery().then(function (b) {
      battery = b;
      console.log("set battery in then(): " + b);
      handleBattery();
    });
  } else {
    handleBattery();
  }
};

var sendConfig = function () {
  'use strict';
  var dict = {
    'KEY_CONFIG_VALUE': config
  };
  Pebble.sendAppMessage(dict, function() {
    console.log('Message sent successfully: ' + JSON.stringify(dict));
  }, function(e) {
    console.log('Message failed: ' + JSON.stringify(e));
  });
};

Pebble.addEventListener('ready', function (e) {
  'use strict';
  initBattery();
});

Pebble.addEventListener('showConfiguration', function(e) {
  'use strict';
  Pebble.openURL('http://programus.coding.me/pebble-watchface-11weeks/html/config.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  config = configData.config;
  sendConfig();
});