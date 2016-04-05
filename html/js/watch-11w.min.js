var SX=23,SY=18,DX=3,DY=3,CW=14,CH=12,WN=11,DW=7,SEC_SX=SX,SEC_SY=SY+CH*WN+DY,SEC_W=CW*DW-1,SEC_H=5,images={IMAGE_PATH:"../resources/images/",imagePaths:{background:"background.png",bigNumbers:"big_number_3x5.png",numbers:"number_3x5.png",letters:"cap_letters.png",btMark:"bluetooth_mark.png",phoneBattery:"phone_battery.png",phoneBatteryCharging:"phone_battery_charging.png",battery:"watch_battery.png",yesNo:"yes_no.png"},init:function(t){"use strict"
var e,i,a,s=0,r=function(){s++,s===Object.keys(a.imagePaths).length&&t()}
if(Object.keys(this).length<=3)for(e in this.imagePaths)i=new Image,i.src=this.IMAGE_PATH+this.imagePaths[e],this[e]=i,a=this,i.onload=r}},numbers={bigSize:{w:42,h:60},tinySize:{w:3,h:5},__drawNumber:function(t,e,i,a,s,r){"use strict"
t.drawImage(s,e*r.w,0,r.w,r.h,i,a,r.w,r.h)},drawBig:function(t,e,i,a){"use strict"
this.__drawNumber(t,Math.floor(e%10),i,a,images.bigNumbers,this.bigSize)},drawTiny:function(t,e,i,a){"use strict"
this.__drawNumber(t,Math.floor(e%10),i,a,images.numbers,this.tinySize)}},letters={size:{w:3,h:5},a:"a".charCodeAt(0),A:"A".charCodeAt(0),count:26,drawLetter:function(t,e,i,a){"use strict"
var s=e.charCodeAt(0),r=s>=this.a?s-this.a:s-this.A
r<this.count&&t.drawImage(images.letters,r*this.size.w,0,this.size.w,this.size.h,i,a,this.size.w,this.size.h)},drawString:function(t,e,i,a,s){"use strict"
var r
for(r=0;r<e.length;r++)this.drawLetter(t,e.charAt(r),i,a),i+=this.size.w+s}},watch={canvas:null,ctx:null,getConfig:null,imageBuff:{buff:null,minTime:0},MON_NAMES:["JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"],FRAME_W:70,FRAME_H:82,setCanvas:function(t){"use strict"
if(this.canvas=t,t.getContext){this.ctx=t.getContext("2d")
var e,i,a=["","o","ms","moz","webkit"]
for(e=0;e<a.length;e++)i=a[e],this.ctx[i+(0===i.length?"i":"I")+"mageSmoothingEnabled"]=!1
this.ctx.strokeStyle="#fff",this.ctx.fillStyle="#fff"}},beginDraw:function(){"use strict"
if(this.ctx){var t=this
window.requestAnimationFrame(function(e){t.draw(e),t.beginDraw()})}},draw:function(){"use strict"
var t,e=new Date,i=Math.floor(e.getTime()/6e4),a=this.getConfig()
if(this.ctx.clearRect(0,0,this.canvas.width,this.canvas.height),this.imageBuff.buff&&i==this.imageBuff.time)this.ctx.putImageData(this.imageBuff.buff,0,0)
else{this.drawBackground(),this.drawCalendar(e)
try{t=this.ctx.getImageData(0,0,this.canvas.width,this.canvas.height),this.imageBuff.buff=t,this.imageBuff.time=i}catch(s){}}a["sec-layer"]&&this.drawSeconds(e),a["battery-layer"]&&this.drawBattery(7),a["bt-phone-layer"]&&this.drawBtPhone(),a["frame-layer"]&&this.drawFrame(e)},drawBattery:function(t){"use strict"
var e=2,i={x:SX-3-e-numbers.tinySize.w-images.battery.width,y:SY+CH*WN-1},a={x:i.x+images.battery.width+e,y:i.y+Math.floor((images.battery.height-numbers.tinySize.h)/2)+1}
this.ctx.drawImage(images.battery,i.x,i.y),numbers.drawTiny(this.ctx,t,a.x,a.y)},drawBtPhone:function(){"use strict"
var t=navigator.battery||navigator.webkitBattery||navigator.mozBattery,e=this,i=function(t,e){e?t.drawPhone(e):t.drawBt()}
navigator.getBattery?navigator.getBattery().then(function(t){i(e,t)}):i(e,t)},drawBt:function(){"use strict"
var t=1,e={x:SX+CW*DW+1,y:SY+CH*WN+0},i={x:e.x+images.btMark.width+t,y:e.y+Math.floor((images.btMark.height-images.yesNo.height)/2)}
this.ctx.drawImage(images.btMark,e.x,e.y),this.ctx.drawImage(images.yesNo,0,0,images.yesNo.width/2,images.yesNo.height,i.x,i.y,images.yesNo.width/2,images.yesNo.height)},drawPhone:function(t){"use strict"
var e=1,i={x:SX+CW*DW+1+2*(e+numbers.tinySize.w),y:SY+CH*WN-1},a={x:i.x-2*(e+numbers.tinySize.w),y:i.y+Math.floor((images.phoneBattery.height-numbers.tinySize.h)/2)+1},s=Math.round(100*t.level)
this.ctx.drawImage(t.charing?images.phoneBatteryCharging:images.phoneBattery,i.x,i.y),100>s?(numbers.drawTiny(this.ctx,s/10,a.x,a.y),a.x+=numbers.tinySize.w+e,numbers.drawTiny(this.ctx,s,a.x,a.y)):letters.drawString(this.ctx,"FL",a.x,a.y,e)},drawSeconds:function(t){"use strict"
var e={x:this.canvas.width/2-numbers.tinySize.w,y:SEC_SY}
numbers.drawTiny(this.ctx,t.getSeconds()/10,e.x,e.y),numbers.drawTiny(this.ctx,t.getSeconds(),e.x+numbers.tinySize.w+1,e.y)
var i,a,s,r=10,n=3,h={x:SEC_SX+(CW-r)/2-1,y:SEC_SY+(SEC_H-n)/2,w:r,h:n}
for(i=0;DW>i;i++)a=i>3?i-1:i,s=Math.floor(t.getSeconds()/10),s>=a&&3!=i&&(a===s&&(h.w=t.getSeconds()%10),this.ctx.fillRect(h.x,h.y,h.w,h.h)),h.w=r,h.x+=CW},drawBackground:function(){"use strict"
this.ctx.drawImage(images.background,0,0)},drawCalendar:function(t){"use strict"
this.drawHourMinute(t),this.drawDates(t)},drawHourMinute:function(t){"use strict"
this.ctx.save(),this.ctx.globalCompositeOperation="lighter"
var e=SX,i=SX+4*CW,a=SY,s=SY+6*CH,r=t.getHours(),n=t.getMinutes()
numbers.drawBig(this.ctx,r/10,e,a),numbers.drawBig(this.ctx,r,i,a),numbers.drawBig(this.ctx,n/10,e,s),numbers.drawBig(this.ctx,n,i,s),this.ctx.restore()},drawDates:function(t){"use strict"
var e,i,a,s,r,n,h=this.get1stDay(t)
for(e=0;WN>e;e++){for(a=!1,i=0;DW>i;i++)n=h.getDate()===t.getDate()&&h.getMonth()===t.getMonth()&&h.getYear()===t.getYear(),this.drawDate(i,e,h.getDate(),n),h.setDate(h.getDate()+1),a=a||n
s=h.getDate()>1&&h.getDate()<=DW+1,r=0===e||0===h.getMonth()&&s,r?this.drawYear(h.getFullYear(),e):a&&this.drawCurrWeekIndicator(e,!0),s?this.drawMonth(h.getMonth(),e):a&&this.drawCurrWeekIndicator(e,!1)}},drawYear:function(t,e){"use strict"
var i={x:SX-DX-2*numbers.tinySize.w-3,y:SY+CH*e}
numbers.drawTiny(this.ctx,t/1e3,i.x,i.y),i.x+=numbers.tinySize.w+2,numbers.drawTiny(this.ctx,t/100,i.x,i.y),i.y+=numbers.tinySize.h+1,numbers.drawTiny(this.ctx,t,i.x,i.y),i.x-=numbers.tinySize.w+2,numbers.drawTiny(this.ctx,t/10,i.x,i.y)},drawMonth:function(t,e){"use strict"
var i={x:SX+DW*CW+DX,y:SY+DY+CH*e}
letters.drawString(this.ctx,this.MON_NAMES[t],i.x,i.y,1)},drawCurrWeekIndicator:function(t,e){"use strict"
var i=2,a=e?-i:i,s={x:(e?SX-DX-i:SX+DW*CW+DX)+.5,y:SY+CH*t+CH/2-1+.5}
this.ctx.save(),this.ctx.beginPath(),this.ctx.moveTo(s.x,s.y),this.ctx.lineTo(s.x+a,s.y-i),this.ctx.lineTo(s.x+a,s.y+i),this.ctx.closePath(),this.ctx.stroke(),this.ctx.fill(),this.ctx.restore()},drawDate:function(t,e,i,a){"use strict"
var s={x:SX+DX+CW*t,y:SY+DY+CH*e}
this.ctx.save(),this.ctx.globalCompositeOperation="difference",i>9&&numbers.drawTiny(this.ctx,Math.floor(i/10),s.x,s.y),numbers.drawTiny(this.ctx,i%10,s.x+4,s.y),a&&(this.ctx.strokeRect(s.x-DX+1.5,s.y-DY+.5,CW-4,CH-2),this.ctx.globalCompositeOperation="lighter",this.ctx.strokeRect(s.x-DX+.5,SY-CH+3.5,CW-2,CH-4)),this.ctx.restore()},get1stDay:function(t){"use strict"
var e=new Date(t.getTime()-6048e5)
return e.setDate(1),e.setDate(e.getDate()-e.getDay()),e},drawFrame:function(t){"use strict"
this.drawFrameScale(),this.drawHourLine(t),this.drawMinuteLine(t)},drawFrameScale:function(){"use strict"
var t,e,i=[[11,0,1,1],[23,0,1,1],[24,0,1,1],[34,0,1,1],[44,0,1,1],[53,0,1,1],[62,0,1,1],[71,0,1,1],[72,0,1,1],[81,0,1,1],[90,0,1,1],[99,0,1,1],[109,0,1,1],[119,0,1,1],[120,0,1,1],[132,0,1,1],[24,1,1,1],[71,1,1,1],[72,1,1,1],[119,1,1,1],[24,2,1,1],[71,2,1,1],[72,2,1,1],[119,2,1,1],[0,4,1,1],[143,4,1,1],[0,19,1,1],[143,19,1,1],[0,31,1,1],[143,31,1,1],[0,42,1,1],[143,42,1,1],[0,43,1,1],[1,43,1,1],[2,43,1,1],[141,43,1,1],[142,43,1,1],[143,43,1,1],[0,51,1,1],[143,51,1,1],[0,60,1,1],[143,60,1,1],[0,68,1,1],[143,68,1,1],[0,76,1,1],[143,76,1,1],[0,83,1,1],[1,83,1,1],[2,83,1,1],[141,83,1,1],[142,83,1,1],[143,83,1,1],[0,84,1,1],[1,84,1,1],[2,84,1,1],[141,84,1,1],[142,84,1,1],[143,84,1,1],[0,91,1,1],[143,91,1,1],[0,99,1,1],[143,99,1,1],[0,107,1,1],[143,107,1,1],[0,116,1,1],[143,116,1,1],[0,124,1,1],[1,124,1,1],[2,124,1,1],[141,124,1,1],[142,124,1,1],[143,124,1,1],[0,125,1,1],[143,125,1,1],[0,136,1,1],[143,136,1,1],[0,148,1,1],[143,148,1,1],[0,163,1,1],[143,163,1,1],[24,165,1,1],[71,165,1,1],[72,165,1,1],[119,165,1,1],[24,166,1,1],[71,166,1,1],[72,166,1,1],[119,166,1,1],[11,167,1,1],[23,167,1,1],[24,167,1,1],[34,167,1,1],[44,167,1,1],[53,167,1,1],[62,167,1,1],[71,167,1,1],[72,167,1,1],[81,167,1,1],[90,167,1,1],[99,167,1,1],[109,167,1,1],[119,167,1,1],[120,167,1,1],[132,167,1,1]]
for(t=0;t<i.length;t++)e=i[t],this.ctx.fillRect(e[0],e[1],e[2],e[3])},drawHourLine:function(t){"use strict"
var e=t.getHours()%12*60+t.getMinutes(),i=e*Math.PI*2/720
this.drawFrameLine(t,i,this.FRAME_W,this.FRAME_H,!0)},drawMinuteLine:function(t){"use strict"
var e=60*t.getMinutes()+t.getSeconds(),i=e*Math.PI*2/3600
this.drawFrameLine(t,i,this.FRAME_W,this.FRAME_H,!1)},drawFrameLine:function(t,e,i,a,s){"use strict"
var r={x:this.canvas.width/2-.5,y:this.canvas.height/2-.5}
if(this.ctx.save(),s)this.ctx.beginPath(),this.ctx.moveTo(r.x,r.y),this.ctx.arc(r.x,r.y,this.canvas.width+this.canvas.height,-Math.PI/2,e-Math.PI/2,!1),this.ctx.clip(),this.ctx.strokeRect(r.x-i,r.y-a,2*i+1,2*a+1)
else if(t.getMilliseconds()<650){var n={x:0,y:0}
if(0===e)n.y=-a
else if(e==Math.PI)n.y=a
else{e-=Math.PI/2
var h=1/Math.tan(e)
n.y=0>e||e>Math.PI?-a:a,n.x=Math.round(n.y*h),Math.abs(n.x)>i&&(n.x=Math.sign(n.x)*i,n.y=Math.round(n.x/h))}var c,o={x:r.x+n.x+.5*Math.sign(n.x),y:r.y+n.y+.5*Math.sign(n.y)},g=2,u=i-Math.abs(n.x)<g&&a-Math.abs(n.y)<g,x=2,y=2
Math.abs(n.y)===a?(c=Math.sign(n.y)*Math.PI/2,u&&(c-=Math.sign(n.x)*Math.sign(n.y)*Math.PI/4)):(c=n.x>0?0:Math.PI,u&&(c+=Math.sign(n.x)*Math.sign(n.y)*Math.PI/4)),this.ctx.translate(o.x+.5,o.y+.5),this.ctx.rotate(c),this.ctx.beginPath(),this.ctx.moveTo(-y,0),this.ctx.lineTo(-y-x,-x),this.ctx.lineTo(-y-x,x),this.ctx.closePath(),this.ctx.stroke(),this.ctx.fill()}this.ctx.restore()}}
