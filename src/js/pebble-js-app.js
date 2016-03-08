var battery;

var CHARGING_MASK =           0x80;
var LEVEL_MASK =              0x7f;
var BATTERY_API_UNSUPPORTED = 0x70;

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
  Pebble.sendAppMessage(dict);
  console.log('battery state sent');
};

var handleBattery = function () {
  'use strict';
  if (battery) {
    battery.addEventListener('chargingchange', sendBatteryState);
    battery.addEventListener('levelchange', sendBatteryState);
  }
  sendBatteryState();
};

Pebble.addEventListener('ready', function (e) {
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
});

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL('file:///Users/programus/git-repos.localized/pebble-watchface-11weeks/src/html/config.html');
})