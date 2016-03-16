'use strict';

var SX = 23,    // start point x
    SY = 18,    // start point y
    DX = 3,     // margin x
    DY = 3,     // margin y
    CW = 14,    // cell width
    CH = 12,    // cell height
    WN = 11,    // number of weeks
    DW = 7;     // days in a week

var SEC_SX = SX,
    SEC_SY = (SY + CH * WN + DY),
    SEC_W = (CW * DW - 1),
    SEC_H = 5;


var images = {
  IMAGE_PATH: '../../resources/images/',
  imagePaths: {
    background: 'background.png',
    bigNumbers: 'big_number_3x5.png',
    numbers: 'number_3x5.png',
    letters: 'cap_letters.png',
    btMark: 'bluetooth_mark.png',
    phoneBattery: 'phone_battery.png',
    phoneBatteryCharging: 'phone_battery_charging.png',
    battery: 'watch_battery.png',
    batteryCharging: 'watch_battery_charging.png',
    yesNo: 'yes_no.png'
  },

  init: function() {
    'use strict';
    // load images
    var key,
        img;

    if (Object.keys(this).length <= 3) {
      for (key in this.imagePaths) {
        img = new Image();
        img.src = this.IMAGE_PATH + this.imagePaths[key];
        this[key] = img;
      }
    }
  }
};

var numbers = {
  bigSize: [42, 60],
  tinySize: [3, 5],

  __drawNumber: function (ctx, n, x, y, src, size) {
    ctx.drawImage(src, n * size[0], 0, size[0], size[1], x, y, size[0], size[1]);
  },

  drawBig: function (ctx, n, x, y) {
    this.__drawNumber(ctx, n, x, y, images.bigNumbers, this.bigSize);
  },

  drawTiny: function (ctx, n, x, y) {
    this.__drawNumber(ctx, n, x, y, images.numbers, this.tinySize);
  }
}

var watch = {
  canvas: null,
  ctx: null,
  getConfig: null,

  MON_NAMES: [
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
  ],


  setCanvas: function(canvas) {
    'use strict';
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    var vendors = ['', 'o', 'ms', 'moz', 'webkit'],
        i, vendor;
    for (i = 0; i < vendors.length; i++) {
      vendor = vendors[i];
      this.ctx[vendor + (vendor.length === 0 ? 'i' : 'I') + 'mageSmoothingEnabled'] = false;
    }
  },

  beginDraw: function() {
    'use strict';
    var self = this;
    window.requestAnimationFrame(function(time) {
      self.draw(time);
    });
  },

  draw: function() {
    'use strict';
    var time = new Date();
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
    this.drawBackground();
    this.drawCalendar(time);

    this.beginDraw();
  },

  drawBackground: function () {
    'use strict';
    this.ctx.drawImage(images.background, 0.5, 0.5);
  },

  drawCalendar: function(time) {
    'use strict';
    this.drawHourMinute(time);
    this.drawDates(time);
  },

  drawHourMinute: function(time) {
    'use strict';
    var comp = this.ctx.globalCompositeOperation;
    this.ctx.globalCompositeOperation = 'lighter';
    var x1 = SX, x2 = SX + CW * 4, y1 = SY, y2 = SY + CH * 6;
    var hours = time.getHours(), 
        minutes = time.getMinutes();
    numbers.drawBig(this.ctx, Math.floor(hours / 10), x1, y1);
    numbers.drawBig(this.ctx, hours % 10, x2, y1);
    numbers.drawBig(this.ctx, Math.floor(minutes / 10), x1, y2);
    numbers.drawBig(this.ctx, minutes % 10, x2, y2);
    this.ctx.globalCompositeOperation = comp;
  },

  drawDates: function(time) {
    'use strict';
    var comp = this.ctx.globalCompositeOperation,
        st = this.get1stDay(time),
        week, day,
        includeToday,
        needDisplayMonth,
        needDisplayYear,
        isToday;
    this.ctx.strokeStyle = '#fff';

    for (week = 0; week < WN; week++) {
      includeToday = false;
      for (day = 0; day < DW; day++) {
        isToday = st.getDate() === time.getDate() && st.getMonth() === time.getMonth() && st.getYear() === time.getYear();
        this.drawDate(day, week, st.getDate(), isToday);
        st.setDate(st.getDate() + 1);
        includeToday = includeToday || isToday;
      }

      needDisplayMonth = st.getDate() > 1 && st.getDate() <= DW + 1;
      needDisplayYear = week === 0 || (st.getMonth() === 0 && needDisplayMonth);
    }

    this.ctx.globalCompositeOperation = comp;
  },

  drawDate: function(day, week, date, isToday) {
    'use strict';
    var startPoint = {
      x: SX + DX + CW * day, 
      y: SY + DY + CH * week
    };
    this.ctx.globalCompositeOperation = 'difference';
    if (date > 9) {
      numbers.drawTiny(this.ctx, Math.floor(date / 10), startPoint.x, startPoint.y);
    }
    numbers.drawTiny(this.ctx, date % 10, startPoint.x + 4, startPoint.y);

    if (isToday) {
      this.ctx.globalCompositeOperation = 'lighter';
      // mark today
      this.ctx.strokeRect(startPoint.x - DX + 1.5, startPoint.y - DY + 0.5, CW - 4, CH - 2);
      // mark weekday
      this.ctx.strokeRect(startPoint.x - DX + 0.5, SY - CH + 3.5, CW - 2, CH - 4);
    }
  },

  get1stDay: function(time) {
    'use strict';
    var d = new Date(time.getTime() - 604800000); // 604800000 = 3600 * 24 * 7 * 1000 means 1 week ago.
    d.setDate(1);
    d.setDate(d.getDate() - d.getDay());
    return d;
  },
}
