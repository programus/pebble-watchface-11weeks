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

  init: function(doneAction) {
    'use strict';
    // load images
    var key,
        img,
        count = 0;

    if (Object.keys(this).length <= 3) {
      for (key in this.imagePaths) {
        img = new Image();
        img.src = this.IMAGE_PATH + this.imagePaths[key];
        this[key] = img;
        self = this;
        img.onload = function () {
          count++;
          if (count === Object.keys(self.imagePaths).length) {
            doneAction();
          }
        }
      }
    }
  } 
};

var numbers = {
  bigSize: {w: 42, h: 60},
  tinySize: {w: 3, h: 5},

  __drawNumber: function (ctx, n, x, y, src, size) {
    ctx.drawImage(src, n * size.w, 0, size.w, size.h, x, y, size.w, size.h);
  },

  drawBig: function (ctx, n, x, y) {
    this.__drawNumber(ctx, Math.floor(n % 10), x, y, images.bigNumbers, this.bigSize);
  },

  drawTiny: function (ctx, n, x, y) {
    this.__drawNumber(ctx, Math.floor(n % 10), x, y, images.numbers, this.tinySize);
  }
};

var letters = {
  size: {w: 3, h: 5},
  a: 'a'.charCodeAt(0),
  A: 'A'.charCodeAt(0),
  count: 26,

  drawLetter: function (ctx, letter, x, y) {
    'use strict';
    var code = letter.charCodeAt(0),
        index = code >= this.a ? code - this.a : code - this.A;
    if (index < this.count) {
      ctx.drawImage(images.letters, index * this.size.w, 0, this.size.w, this.size.h, x, y, this.size.w, this.size.h);
    }
  },

  drawString: function (ctx, str, x, y, space) {
    'use strict';
    var i;
    for (i = 0; i < str.length; i++) {
      this.drawLetter(ctx, str.charAt(i), x, y);
      x += this.size.w + space;
    }
  }
};

var watch = {
  canvas: null,
  ctx: null,
  getConfig: null,
  imageBuff: {
    buff: null,
    minTime: 0,
  },

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
    this.ctx.strokeStyle = '#fff';
    this.ctx.fillStyle = '#fff';
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
    var time = new Date(),
        minTime = Math.floor(time.getTime() / 60000),
        config = this.getConfig(),
        imgData;
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
    if (!this.imageBuff.buff || minTime != this.imageBuff.time) {
      this.drawBackground();
      this.drawCalendar(time);
      imgData = this.ctx.getImageData(0, 0, this.canvas.width, this.canvas.height);
      this.imageBuff.buff = imgData;
      this.imageBuff.time = minTime;
    } else {
      this.ctx.putImageData(this.imageBuff.buff, 0, 0);
    }

    if (config['sec-layer']) {
      this.drawSeconds(time);
    }

    this.beginDraw();
  },

  drawSeconds: function (time) {
    'use strict';
    // draw number
    var p = {
      x: this.canvas.width / 2 - numbers.tinySize.w,
      y: SEC_SY
    };
    numbers.drawTiny(this.ctx, time.getSeconds() / 10, p.x, p.y);
    numbers.drawTiny(this.ctx, time.getSeconds(), p.x + numbers.tinySize.w + 1, p.y);

    // draw marks
    var MARK_W = 10,
        MARK_H = 3,
        rect = {
          x: SEC_SX + (CW - MARK_W) / 2 - 1,
          y: SEC_SY + (SEC_H - MARK_H) / 2,
          w: MARK_W,
          h: MARK_H
        }, 
        i, index, cent;

    for (i = 0; i < DW; i++) {
      index = (i > 3) ? i - 1 : i;
      cent = Math.floor(time.getSeconds() / 10);
      if (index <= cent && i != 3) {
        if (index === cent) {
          rect.w = time.getSeconds() % 10;
        }
        this.ctx.fillRect(rect.x, rect.y, rect.w, rect.h);
      }
      rect.w = MARK_W;
      rect.x += CW;
    }
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
    var x1 = SX, x2 = SX + CW * 4, y1 = SY, y2 = SY + CH * 6,
        hours = time.getHours(), 
        minutes = time.getMinutes();
    numbers.drawBig(this.ctx, hours / 10, x1, y1);
    numbers.drawBig(this.ctx, hours, x2, y1);
    numbers.drawBig(this.ctx, minutes / 10, x1, y2);
    numbers.drawBig(this.ctx, minutes, x2, y2);
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

      if (needDisplayYear) {
        this.drawYear(st.getFullYear(), week);
      } else if (includeToday) {
        this.drawCurrWeekIndicator(week, true);
      }

      if (needDisplayMonth) {
        this.drawMonth(st.getMonth(), week);
      } else if (includeToday) {
        this.drawCurrWeekIndicator(week, false);
      }
    }

    this.ctx.globalCompositeOperation = comp;
  },

  drawYear: function(year, week) {
    'use strict';
    var p = {
      x: SX - DX - numbers.tinySize.w * 2 - 3, 
      y: SY + CH * week
    };
    numbers.drawTiny(this.ctx, year / 1000, p.x, p.y);
    p.x += numbers.tinySize.w + 2;
    numbers.drawTiny(this.ctx, year / 100, p.x, p.y);
    p.y += numbers.tinySize.h + 1;
    numbers.drawTiny(this.ctx, year, p.x, p.y);
    p.x -= numbers.tinySize.w + 2;
    numbers.drawTiny(this.ctx, year / 10, p.x, p.y);
  },

  drawMonth: function(month, week) {
    'use strict';
    var p = {
      x: SX + DW * CW + DX,
      y: SY + DY + CH * week
    };
    letters.drawString(this.ctx, this.MON_NAMES[month], p.x, p.y, 1);
  },

  drawCurrWeekIndicator: function(week, isLeftSide) {
    'use strict';
    var unit = 3,
        delta = isLeftSide ? -unit : unit,
        p = {
          x: isLeftSide ? SX - DX - unit + 2 : SX + DW * CW + DX, 
          y: SY + CH * week + CH / 2 - 1
        };
    this.ctx.beginPath();
    this.ctx.moveTo(p.x, p.y);
    this.ctx.lineTo(p.x + delta, p.y - unit);
    this.ctx.lineTo(p.x + delta, p.y + unit);
    this.ctx.closePath();
    this.ctx.fill();
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
      // mark today
      this.ctx.strokeRect(startPoint.x - DX + 1.5, startPoint.y - DY + 0.5, CW - 4, CH - 2);
      // mark weekday
      this.ctx.globalCompositeOperation = 'lighter';
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
};

