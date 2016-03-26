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
  IMAGE_PATH: 'img/',
  imagePaths: {
    background: 'background.png',
    bigNumbers: 'big_number_3x5.png',
    numbers: 'number_3x5.png',
    letters: 'cap_letters.png',
    btMark: 'bluetooth_mark.png',
    phoneBattery: 'phone_battery.png',
    phoneBatteryCharging: 'phone_battery_charging.png',
    battery: 'watch_battery.png',
    yesNo: 'yes_no.png'
  },

  init: function(doneAction) {
    'use strict';
    // load images
    var key,
        img,
        count = 0,
        self, 
        iload = function () {
          count++;
          if (count === Object.keys(self.imagePaths).length) {
            doneAction();
          }
        };

    if (Object.keys(this).length <= 3) {
      for (key in this.imagePaths) {
        img = new Image();
        img.src = this.IMAGE_PATH + this.imagePaths[key];
        this[key] = img;
        self = this;
        img.onload = iload;
      }
    }
  } 
};

var numbers = {
  bigSize: {w: 42, h: 60},
  tinySize: {w: 3, h: 5},

  __drawNumber: function (ctx, n, x, y, src, size) {
    'use strict';
    ctx.drawImage(src, n * size.w, 0, size.w, size.h, x, y, size.w, size.h);
  },

  drawBig: function (ctx, n, x, y) {
    'use strict';
    this.__drawNumber(ctx, Math.floor(n % 10), x, y, images.bigNumbers, this.bigSize);
  },

  drawTiny: function (ctx, n, x, y) {
    'use strict';
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

  FRAME_W: 70,
  FRAME_H: 82,


  setCanvas: function(canvas) {
    'use strict';
    this.canvas = canvas;
    if (canvas.getContext) {
      this.ctx = canvas.getContext('2d');
      var vendors = ['', 'o', 'ms', 'moz', 'webkit'],
          i, vendor;
      for (i = 0; i < vendors.length; i++) {
        vendor = vendors[i];
        this.ctx[vendor + (vendor.length === 0 ? 'i' : 'I') + 'mageSmoothingEnabled'] = false;
      }
      this.ctx.strokeStyle = '#fff';
      this.ctx.fillStyle = '#fff';
    }
  },

  beginDraw: function() {
    'use strict';
    if (this.ctx) {
      var self = this;
      window.requestAnimationFrame(function(time) {
        self.draw(time);
      });
    }
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
      try {
        imgData = this.ctx.getImageData(0, 0, this.canvas.width, this.canvas.height);
        this.imageBuff.buff = imgData;
        this.imageBuff.time = minTime;
      } catch (e) {
      }
    } else {
      this.ctx.putImageData(this.imageBuff.buff, 0, 0);
    }

    if (config['sec-layer']) {
      this.drawSeconds(time);
    }

    if (config['battery-layer']) {
      this.drawBattery(7);
    }

    if (config['bt-phone-layer']) {
      this.drawBtPhone();
    }

    if (config['frame-layer']) {
      this.drawFrame(time);
    }

    this.beginDraw();
  },

  drawBattery: function (level) {
    'use strict';
    var MARGIN = 2,
        pImg = {
          x: SX - 3 - MARGIN - numbers.tinySize.w - images.battery.width,
          y: SY + CH * WN - 1
        },
        p = {
          x: pImg.x + images.battery.width + MARGIN,
          y: pImg.y + Math.floor((images.battery.height - numbers.tinySize.h) / 2) + 1
        };
    this.ctx.drawImage(images.battery, pImg.x, pImg.y);
    numbers.drawTiny(this.ctx, level, p.x, p.y);
  },

  drawBtPhone: function () {
    'use strict';
    var battery = navigator.battery || navigator.webkitBattery || navigator.mozBattery,
        self = this,
        handleBattery = function (drawer, bat) {
          if (bat) {
            drawer.drawPhone(bat);
          } else {
            drawer.drawBt();
          }
        };
    if (navigator.getBattery) {
      navigator.getBattery().then(function (b) {
        handleBattery(self, b);
      });
    } else {
      handleBattery(self, battery);
    }
  },

  drawBt: function () {
    'use strict';
    var MARGIN = 1,
        pImg = {
          x: SX + CW * DW + 1,
          y: SY + CH * WN + 0
        },
        p = {
          x: pImg.x + images.btMark.width + MARGIN,
          y: pImg.y + Math.floor((images.btMark.height - images.yesNo.height) / 2)
        };
    this.ctx.drawImage(images.btMark, pImg.x, pImg.y);
    this.ctx.drawImage(images.yesNo, 0, 0, images.yesNo.width / 2, images.yesNo.height, p.x, p.y, images.yesNo.width / 2, images.yesNo.height);
  },

  drawPhone: function (battery) {
    'use strict';
    var MARGIN = 1,
        pImg = {
          x: SX + CW * DW + 1 + (MARGIN + numbers.tinySize.w) * 2,
          y: SY + CH * WN - 1
        },
        p = {
          x: pImg.x - (MARGIN + numbers.tinySize.w) * 2,
          y: pImg.y + Math.floor((images.phoneBattery.height - numbers.tinySize.h) / 2) + 1
        },
        level = Math.round(battery.level * 100);
    this.ctx.drawImage(battery.charing ? images.phoneBatteryCharging : images.phoneBattery, pImg.x, pImg.y);
    if (level < 100) {
      numbers.drawTiny(this.ctx, level / 10, p.x, p.y);
      p.x += numbers.tinySize.w + MARGIN;
      numbers.drawTiny(this.ctx, level, p.x, p.y);
    } else {
      letters.drawString(this.ctx, 'FL', p.x, p.y, MARGIN);
    }
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
    this.ctx.drawImage(images.background, 0, 0);
  },

  drawCalendar: function(time) {
    'use strict';
    this.drawHourMinute(time);
    this.drawDates(time);
  },

  drawHourMinute: function(time) {
    'use strict';
    this.ctx.save();
    this.ctx.globalCompositeOperation = 'lighter';
    var x1 = SX, x2 = SX + CW * 4, y1 = SY, y2 = SY + CH * 6,
        hours = time.getHours(), 
        minutes = time.getMinutes();
    numbers.drawBig(this.ctx, hours / 10, x1, y1);
    numbers.drawBig(this.ctx, hours, x2, y1);
    numbers.drawBig(this.ctx, minutes / 10, x1, y2);
    numbers.drawBig(this.ctx, minutes, x2, y2);
    this.ctx.restore();
  },

  drawDates: function(time) {
    'use strict';
    var st = this.get1stDay(time),
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
    var unit = 2,
        delta = isLeftSide ? -unit : unit,
        p = {
          x: (isLeftSide ? SX - DX - unit : SX + DW * CW + DX) + 0.5, 
          y: SY + CH * week + CH / 2 - 1 + 0.5
        };
    this.ctx.save();
    this.ctx.beginPath();
    this.ctx.moveTo(p.x, p.y);
    this.ctx.lineTo(p.x + delta, p.y - unit);
    this.ctx.lineTo(p.x + delta, p.y + unit);
    this.ctx.closePath();
    this.ctx.stroke();
    this.ctx.fill();
    this.ctx.restore();
  },

  drawDate: function(day, week, date, isToday) {
    'use strict';
    var startPoint = {
      x: SX + DX + CW * day, 
      y: SY + DY + CH * week
    };
    this.ctx.save();
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
    this.ctx.restore();
  },

  get1stDay: function(time) {
    'use strict';
    var d = new Date(time.getTime() - 604800000); // 604800000 = 3600 * 24 * 7 * 1000 means 1 week ago.
    d.setDate(1);
    d.setDate(d.getDate() - d.getDay());
    return d;
  },

  drawFrame: function (time) {
    'use strict';
    this.drawFrameScale();
    this.drawHourLine(time);
    this.drawMinuteLine(time);
  },

  drawFrameScale: function () {
    'use strict';
    var scalePoints = [
      [11, 0, 1, 1],
      [23, 0, 1, 1],
      [24, 0, 1, 1],
      [34, 0, 1, 1],
      [44, 0, 1, 1],
      [53, 0, 1, 1],
      [62, 0, 1, 1],
      [71, 0, 1, 1],
      [72, 0, 1, 1],
      [81, 0, 1, 1],
      [90, 0, 1, 1],
      [99, 0, 1, 1],
      [109, 0, 1, 1],
      [119, 0, 1, 1],
      [120, 0, 1, 1],
      [132, 0, 1, 1],
      [24, 1, 1, 1],
      [71, 1, 1, 1],
      [72, 1, 1, 1],
      [119, 1, 1, 1],
      [24, 2, 1, 1],
      [71, 2, 1, 1],
      [72, 2, 1, 1],
      [119, 2, 1, 1],
      [0, 4, 1, 1],
      [143, 4, 1, 1],
      [0, 19, 1, 1],
      [143, 19, 1, 1],
      [0, 31, 1, 1],
      [143, 31, 1, 1],
      [0, 42, 1, 1],
      [143, 42, 1, 1],
      [0, 43, 1, 1],
      [1, 43, 1, 1],
      [2, 43, 1, 1],
      [141, 43, 1, 1],
      [142, 43, 1, 1],
      [143, 43, 1, 1],
      [0, 51, 1, 1],
      [143, 51, 1, 1],
      [0, 60, 1, 1],
      [143, 60, 1, 1],
      [0, 68, 1, 1],
      [143, 68, 1, 1],
      [0, 76, 1, 1],
      [143, 76, 1, 1],
      [0, 83, 1, 1],
      [1, 83, 1, 1],
      [2, 83, 1, 1],
      [141, 83, 1, 1],
      [142, 83, 1, 1],
      [143, 83, 1, 1],
      [0, 84, 1, 1],
      [1, 84, 1, 1],
      [2, 84, 1, 1],
      [141, 84, 1, 1],
      [142, 84, 1, 1],
      [143, 84, 1, 1],
      [0, 91, 1, 1],
      [143, 91, 1, 1],
      [0, 99, 1, 1],
      [143, 99, 1, 1],
      [0, 107, 1, 1],
      [143, 107, 1, 1],
      [0, 116, 1, 1],
      [143, 116, 1, 1],
      [0, 124, 1, 1],
      [1, 124, 1, 1],
      [2, 124, 1, 1],
      [141, 124, 1, 1],
      [142, 124, 1, 1],
      [143, 124, 1, 1],
      [0, 125, 1, 1],
      [143, 125, 1, 1],
      [0, 136, 1, 1],
      [143, 136, 1, 1],
      [0, 148, 1, 1],
      [143, 148, 1, 1],
      [0, 163, 1, 1],
      [143, 163, 1, 1],
      [24, 165, 1, 1],
      [71, 165, 1, 1],
      [72, 165, 1, 1],
      [119, 165, 1, 1],
      [24, 166, 1, 1],
      [71, 166, 1, 1],
      [72, 166, 1, 1],
      [119, 166, 1, 1],
      [11, 167, 1, 1],
      [23, 167, 1, 1],
      [24, 167, 1, 1],
      [34, 167, 1, 1],
      [44, 167, 1, 1],
      [53, 167, 1, 1],
      [62, 167, 1, 1],
      [71, 167, 1, 1],
      [72, 167, 1, 1],
      [81, 167, 1, 1],
      [90, 167, 1, 1],
      [99, 167, 1, 1],
      [109, 167, 1, 1],
      [119, 167, 1, 1],
      [120, 167, 1, 1],
      [132, 167, 1, 1]
    ],
    i, p;

    for (i = 0; i < scalePoints.length; i++) {
      p = scalePoints[i];
      this.ctx.fillRect(p[0], p[1], p[2], p[3]);
    }
  },

  drawHourLine: function (time) {
    'use strict';
    var min = (time.getHours() % 12) * 60 + time.getMinutes(),
        angle = min * Math.PI * 2 / 720;
    this.drawFrameLine(time, angle, this.FRAME_W, this.FRAME_H, true);
  },

  drawMinuteLine: function (time) {
    'use strict';
    var sec = time.getMinutes() * 60 + time.getSeconds(),
        angle = sec * Math.PI * 2 / 3600;
    this.drawFrameLine(time, angle, this.FRAME_W, this.FRAME_H, false);
  },

  drawFrameLine: function (time, angle, w, h, isLine) {
    'use strict';
    var center = {
      x: this.canvas.width / 2 - 0.5,
      y: this.canvas.height / 2 - 0.5
    };  

    this.ctx.save();
    if (isLine) {
      this.ctx.beginPath();
      this.ctx.moveTo(center.x, center.y);
      this.ctx.arc(center.x, center.y, this.canvas.width + this.canvas.height, -Math.PI / 2, angle - Math.PI / 2, false);
      this.ctx.clip();

      this.ctx.strokeRect(center.x - w, center.y - h, w * 2 + 1, h * 2 + 1);
    } else if (time.getMilliseconds() < 650) {
      var end = {
        x: 0, 
        y: 0
      };
      if (angle === 0) {
        end.y = -h;
      } else if (angle == Math.PI) {
        end.y = h;
      } else {
        angle -= Math.PI / 2;
        var rate = 1 / Math.tan(angle);
        end.y = (angle < 0 || angle > Math.PI) ? -h : h;
        end.x = Math.round(end.y * rate);
        if (Math.abs(end.x) > w) {
          end.x = Math.sign(end.x) * w;
          end.y = Math.round(end.x / rate);
        }
      }
      var p = {
        x: center.x + end.x + Math.sign(end.x) * 0.5, 
        y: center.y + end.y + Math.sign(end.y) * 0.5
      }, 
          PATH_SZ = 2,
          isCorner = w - Math.abs(end.x) < PATH_SZ && h - Math.abs(end.y) < PATH_SZ,
          rotateAngle, 
          unit = 2,
          offset = 2;

      if (Math.abs(end.y) === h) {
        rotateAngle = Math.sign(end.y) * Math.PI / 2;
        if (isCorner) {
          rotateAngle -= Math.sign(end.x) * Math.sign(end.y) * Math.PI / 4;
        }
      } else {
        rotateAngle = end.x > 0 ? 0 : Math.PI;
        if (isCorner) {
          rotateAngle += Math.sign(end.x) * Math.sign(end.y) * Math.PI / 4;
        }
      }

      this.ctx.translate(p.x + 0.5, p.y + 0.5);
      this.ctx.rotate(rotateAngle);
      this.ctx.beginPath();
      this.ctx.moveTo(-offset, 0);
      this.ctx.lineTo(-offset - unit, -unit);
      this.ctx.lineTo(-offset - unit, unit);
      this.ctx.closePath();
      this.ctx.stroke();
      this.ctx.fill();
    }
    this.ctx.restore();
  }
};

