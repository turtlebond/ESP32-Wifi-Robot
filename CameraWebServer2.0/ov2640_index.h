const char ov2640_index[] PROGMEM = R"=====(
<!doctype html>
<html>
  <head>
    <meta charset=utf-8>
    <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1.0, maximum-scale=1.0, user-scalable=0" />
    <meta name="mobile-web-app-capable" content="yes">
    <title>ESP32 Touch Control</title>
    <style type="text/css"> 
* {
  -webkit-touch-callout: none; /* prevent callout to copy image, etc when tap to hold */
  -webkit-text-size-adjust: none; /* prevent webkit from resizing text to fit */
  /* make transparent link selection, adjust last value opacity 0 to 1.0 */
  -webkit-tap-highlight-color: rgba(0,0,0,0); 
  -webkit-user-select: none; /* prevent copy paste, to allow, change 'none' to 'text' */
  -webkit-tap-highlight-color: rgba(0,0,0,0); 
  -webkit-appearance: none; }
body {
  background-color: #000000;
  margin: 0px;}
canvas {
  display:block; 
  position:absolute; 
  z-index: 1; }
.container {
  width:auto;
  text-align:center;
  background-color:#ff0000;}
.button{
  cursor: pointer;
  border-radius: 10%;
  border: 2px;
  border-style: solid;
  border-color: white;
  text-align:center;
  margin: 8px;
  padding: 2px;}
.button light{
  transition: background 0.4s;}
.drop{
  cursor: pointer;
  border-radius: 10%;
  border: 2px;
  border-style: solid;
  border-color: white;
  text-align:center;
  margin: 8px;
  padding: 2px;
  background-color: #000;
  color: #fff;}
input[type="checkbox"]:checked + label {
    background: gray;}
.sidenav {
  overflow: hidden;
  color: white;
  //background-color: green;
  position: fixed;
  font-family: 'Roboto', sans-serif;
  width: 100%;
  display: flex;
  justify-content: space-between;  }
canvas#stream{
  //display: block;
  margin: 0px;
  //width: 100%;
  height: auto;
  //position:absolute;
  //top:6vh;right:0;left:0;
  margin:auto;
  background:black}
    </style>
    </head>
    <body scroll="no" style="overflow: hidden; height:100vh;">
                    <!--div id="stream-container" class="image-container "-->
                    <div style="position:relative;z-index:0;">
                    <canvas id="stream" width="640" height="480"> 
                        </canvas>
                    </div>
    <div id="navBottom" class="sidenav" style="bottom:0;z-index: 2;">
		<a id="toggle-stream" class="button" type="button" style="color:white; text-decoration:none;">Start Stream</a>
		<a id="off" class="button off" type="button" onclick="shutdown()" style="">OFF</a>
		<a id="light" class="button light" type="button" onclick="handleHeadlightClick()" style="transition: background 0.4s;">LIGHT</a>
		<select id="myDropdown" class="drop" onchange="updateColor()" >
			<option value="OFF" style="background:black;">OFF</option>
			<option value="White" style="background:white;">White</option>
			<option value="Orange" style="background:Orange;">Orange</option>
			<option value="Red" style="background:Red;">Red</option>
			<option value="Yellow" style="background:Yellow;">Yellow</option>
			<option value="Pink" style="background:Pink;">Pink</option>
			<option value="Violet" style="background:Violet;">Violet</option>
			<option value="Blue" style="background:Blue;">Blue</option>
			<option value="Green" style="background:Green;">Green</option>
			<option value="Brown" style="background:Brown;">Brown</option>	
			<option value="Grey" style="background:Grey;">Grey</option>
			<option value="Lavender" style="background:Lavender;">Lavender</option>	
			<option value="Lime" style="background:Lime;">Lime</option>
			<option value="Turquoise" style="background:Turquoise;">Turquoise</option>
			<option value="Purple" style="background:Purple;">Purple</option>	
			<option value="Tomato" style="background:Tomato;">Tomato</option>
			<option value="Magenta" style="background:Magenta;">Magenta</option>	
			<option value="Maroon" style="background:Maroon;">Maroon</option>
			<option value="Gold" style="background:Gold;">Gold</option>	
		</select>
	</div>
    <script>
document.addEventListener('DOMContentLoaded', function (event) {
  var baseHost = document.location.origin
  var streamUrl = baseHost + ':81'
  var scanvas = document.getElementById('stream');
  var ctx = scanvas.getContext('2d');
  var view = new Image();
//  const view = document.getElementById('stream')
  var streamButton = document.getElementById('toggle-stream')
  const stopStream = () => {
    window.stop();
    streamButton.innerHTML = 'Start Stream'
  }
  const startStream = () => {
    view.src = `${streamUrl}/stream`
 //   view.src = "pexels.jpeg"
    streamButton.innerHTML = 'Stop Stream'
    view.onload = function loop(){
    //Draw the image onto the canvas.
    ctx.drawImage(view, 0,0, 640, 480);
    setTimeout(loop, 1000 / 30); // drawing at 30fps
		}
  }
  streamButton.onclick = () => {
    const streamEnabled = streamButton.innerHTML === 'Stop Stream'
    if (streamEnabled) {
      stopStream()
    } else {
      startStream()
    }
  }
})
function shutdown(){
  if(confirm("This will shutdown the ESP32.\nAre you sure?")){
    websock.send(JSON.stringify({ cmd : "power", val : 1 }));
  }
}
host = window.location.hostname;
var ipaddr= host + ":82"; // websock port of ESP32, normally xx.xx.xx.xx:81
var websock = new WebSocket('ws://' + ipaddr);
websock.binaryType = 'arraybuffer';
websock.onopen = function(evt) {
  console.log('websock open');
};
websock.onclose = function(evt) {
 console.log('websock close');
};
websock.onerror = function(evt) { console.log(evt);  };
var Vector2 = function (x,y) {
  this.x = x || 0;
  this.y = y || 0;
};
Vector2.prototype = {
  reset: function ( x, y ) {
    this.x = x;
    this.y = y;
    return this;
  },
  copyFrom : function (v) {
    this.x = v.x;
    this.y = v.y;
  },
  plusEq : function (v) {
    this.x+=v.x;
    this.y+=v.y;
    return this;
  },
  minusEq : function (v) {
    this.x-=v.x;
    this.y-=v.y;
    return this; 
  },
  equals : function (v) {
    return((this.x==v.x)&&(this.y==v.x));
  }
};
var canvas,scanvas, c,container,halfWidth,halfHeight,leftTouchID = -1, 
  leftTouchPos = new Vector2(0,0),
  leftTouchStartPos = new Vector2(0,0),
  leftVector = new Vector2(0,0);
  var sendFlag = false;
  setupCanvas();
  var mouseX, mouseY, mouseDown = false,touches = []; 
function setupCanvas() {
  canvas = document.createElement( 'canvas' );
  c = canvas.getContext( '2d' );
  container = document.createElement( 'div' );
  container.className = "container";
  document.body.appendChild( container );
  container.appendChild(canvas);
  resetCanvas();
  c.strokeStyle = "#ffffff";
  c.lineWidth =2;
}
function resetCanvas (e) {
  // resize the canvas - but remember - this clears the canvas too. 
  canvas.width = window.innerWidth; 
  canvas.height = window.innerHeight;
	var aspectRatio;
  //halfWidth = canvas.width/2; 
  halfWidth = canvas.width;
  halfHeight = canvas.height/2;
	switch(window.orientation){
		case 90 : aspectRatio = 480/960;break;
		case -90 : aspectRatio = 480/640;break;
		default : aspectRatio = 640/480;break;
	}
   maxHeight = window.innerHeight - document.getElementById('navBottom').offsetHeight;
   if(window.innerWidth*aspectRatio <= maxHeight){
     document.getElementById('stream').style.height = window.innerWidth*aspectRatio + "px";
     document.getElementById('stream').style.width = window.innerWidth + "px";
   }
   else {
     document.getElementById('stream').style.height = maxHeight + "px";
     document.getElementById('stream').style.width = maxHeight/aspectRatio + "px";
   }
  //make sure we scroll to the top left. 
  window.scrollTo(0,0); 
}
function onTouchStart(e) {
  for(var i = 0; i<e.changedTouches.length; i++){
    var touch =e.changedTouches[i];
    if((leftTouchID<0) && (touch.clientX<halfWidth)){
      leftTouchID = touch.identifier;
      leftTouchStartPos.reset(touch.clientX, touch.clientY);
      leftTouchPos.copyFrom(leftTouchStartPos);
      leftVector.reset(0,0);
      continue;
    } else{
      makeBullet();
    }
  }
  touches = e.touches;
}
function onTouchMove(e) {
  // Prevent the browser from doing its default thing (scroll, zoom)
  e.preventDefault();
  for(var i = 0; i<e.changedTouches.length; i++){
    var touch =e.changedTouches[i];
    if(leftTouchID == touch.identifier){
      leftTouchPos.reset(touch.clientX, touch.clientY);
      leftVector.copyFrom(leftTouchPos);
      leftVector.minusEq(leftTouchStartPos);
      sendFlag = true;
      break;
    }
  }
  touches = e.touches;
}
function onTouchEnd(e){
  touches = e.touches;
  for(var i = 0; i<e.changedTouches.length; i++){
    var touch =e.changedTouches[i];
    if(leftTouchID == touch.identifier){
      leftTouchID = -1;
      leftVector.reset(0,0);
      leftMot = rightMot = 0;
      speedMot = dirMot = 0;
      sendFlag = true;
      break;
    }
  }
}
function onMouseUp(event) { 
  leftVector.reset(0,0);
  leftMot = rightMot = 0;
  speedMot = dirMot = 0;
  mouseDown = false;
  sendFlag = true;
}
function onMouseMove(event){
  mouseX = event.offsetX;
  mouseY = event.offsetY;
  if(mouseDown){
    leftTouchPos.reset(event.offsetX, event.offsetY);
    leftVector.copyFrom(leftTouchPos);
    leftVector.minusEq(leftTouchStartPos);
    sendFlag = true;
  }
}
function onMouseDown(event) {
  leftTouchStartPos.reset(event.offsetX, event.offsetY);
  leftTouchPos.copyFrom(leftTouchStartPos);
  leftVector.reset(0,0);
  mouseDown = true;
}
function draw() {
  c.clearRect(0,0,canvas.width, canvas.height); 
  //if touch
  for(var i=0; i<touches.length; i++) {
    var touch = touches[i]; 
    if(touch.identifier == leftTouchID){
      c.beginPath(); 
      c.strokeStyle = "white"; 
      c.lineWidth = 6; 
      c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 40,0,Math.PI*2,true); 
      c.stroke();
      c.beginPath(); 
      c.strokeStyle = "white"; 
      c.lineWidth = 2; 
      c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 60,0,Math.PI*2,true); 
      c.stroke();
      c.beginPath(); 
      c.strokeStyle = "white"; 
      c.arc(leftTouchPos.x, leftTouchPos.y, 40, 0,Math.PI*2, true); 
      c.stroke(); 
    } else {
      c.beginPath(); 
      c.fillStyle = "white";
      //c.fillText("touch id : "+touch.identifier+" x:"+touch.clientX+" y:"+touch.clientY, touch.clientX+30, touch.clientY-30); 
      c.beginPath(); 
      c.strokeStyle = "red";
      c.lineWidth = "6";
      c.arc(touch.clientX, touch.clientY, 40, 0, Math.PI*2, true); 
      c.stroke();
    }
  }
  //if not touch   
  if(mouseDown){
    c.beginPath(); 
    c.strokeStyle = "white"; 
    c.lineWidth = 6; 
    c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 40,0,Math.PI*2,true); 
    c.stroke();
    c.beginPath(); 
    c.strokeStyle = "white"; 
    c.lineWidth = 2; 
    c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 60,0,Math.PI*2,true); 
    c.stroke();
    c.beginPath(); 
    c.strokeStyle = "white"; 
    c.arc(leftTouchPos.x, leftTouchPos.y, 40, 0,Math.PI*2, true); 
    c.stroke(); 
    c.fillStyle  = "white"; 
    //c.fillText("mouse : "+mouseX+", "+mouseY, mouseX, mouseY); 
    c.beginPath(); 
    c.strokeStyle = "white";
    c.lineWidth = "6";
    c.arc(mouseX, mouseY, 40, 0, Math.PI*2, true); 
    c.stroke();
  }
}
var rawLeft, rawRight, MaxJoy = 255, MinJoy = -255, MaxValue = 255,MinValue = -255;
var leftMot = 0, rightMot = 0;
function Remap(value, from1, to1, from2, to2){
  return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}
//source: http://www.dyadica.co.uk/basic-differential-aka-tank-drive/
function tankDrive(x, y){
  var z = Math.sqrt(x * x + y * y);
  var rad = Math.acos(Math.abs(x) / z);
  if (isNaN(rad)) rad = 0;
  var angle = rad * 180 / Math.PI;
  var tcoeff = -1 + (angle / 90) * 2;
  var turn = tcoeff * Math.abs(Math.abs(y) - Math.abs(x));
  turn = Math.round(turn * 100) / 100;
  var move = Math.max(Math.abs(y), Math.abs(x));
  if ((x >= 0 && y >= 0) || (x < 0 && y < 0)){
    rawLeft = move;
    rawRight = turn;
  }else{
    rawRight = move;
    rawLeft = turn;
  }
  if (y < 0){
    rawLeft = 0 - rawLeft;
    rawRight = 0 - rawRight;
  }
  leftMot = Remap(rawLeft, MinJoy, MaxJoy, MinValue, MaxValue);
  rightMot = Remap(rawRight, MinJoy, MaxJoy, MinValue, MaxValue);
}
//https://www.instructables.com/Hacking-a-RC-Car-With-Arduino-and-Android/
var direction,speed;
function tankDrive2(x, y){
  speedMot = Remap(y, MinJoy, MaxJoy, MinValue, MaxValue);
  dirMot = Remap(x, MinJoy, MaxJoy, MinValue, MaxValue);
  //console.log("x:" + x + " y:" +y);
  //console.log("speed:" + speedMot + " dir:" +dirMot);
}
function sendControls(){
  var en=0;
  if(sendFlag == true){
    leftVector.x = Math.min(Math.max(parseInt(leftVector.x), -255), 255);
    leftVector.y = Math.min(Math.max(parseInt(leftVector.y), -255), 255);
    if(en) {
        //using 2 motor with pwm for speed and direction    
        tankDrive(leftVector.x, -leftVector.y);
        if(leftMot > 0) leftMot += 70;
        if(leftMot < 0) leftMot -= 70;
        if(rightMot > 0) rightMot += 70;
        if(rightMot < 0) rightMot -= 70;
        leftMot = Math.min(Math.max(parseInt(leftMot), -255), 255);
        rightMot = Math.min(Math.max(parseInt(rightMot), -255), 255);
        websock.send(JSON.stringify({ cmd : "pos", val : leftMot, val2 : rightMot }));
        sendFlag = false;
    }
    else {
        tankDrive2(leftVector.x, -leftVector.y);
        if(speedMot > 0 && speedMot <30 ) speedMot = 0;
        if(speedMot < 0 && speedMot >-30 ) speedMot = 0;
        if(dirMot > 0 && dirMot <30 ) dirMot = 0;
        if(dirMot < 0 && dirMot >-30 ) dirMot = 0;
        websock.send(JSON.stringify({ cmd : "pos", val : dirMot, val2 : speedMot }));
        sendFlag = false;
    }
  }
}
var headlight = 0;
function handleHeadlightClick(){
  if (headlight == 0){
    headlight = 1;
	document.getElementById("light").style.backgroundColor = "gray";
  } else{
    headlight = 0;
	document.getElementById("light").style.backgroundColor = "black";
  }
  websock.send(JSON.stringify({ cmd : "light", val : headlight }));
}
function updateColor(){
	var color=document.getElementById("myDropdown").value;
	var value;
	var text_color="White";
	switch(color){
		case "OFF": value=0;color="Black";break;
		case "White": value=0xFFFFFF;text_color="Black";break;					
		case "Yellow": value=0xFFFF00;text_color="Black";break;
		case "Orange": value=0xFFA500;break;
		case "Red": value=0xFF0000;	break;
		case "Pink": value=0xFFC0CB;break;
		case "Violet": value=0xEE82EE;break;
		case "Blue": value=0x0000FF;break;
		case "Green": value=0x008000;break;
		case "Brown": value=0xA52A2A;break;										
		case "Grey": value=0x808080;break;					
		case "Lavender": value=0xE6E6FA;text_color="Black";break;	
		case "Lime": value=0x00FF00;text_color="Black";break;	
		case "Turquoise": value=0x40E0D0;break;	
		case "Purple": value=0x800080;break;		
		case "Tomato": value=0xFF6347;break;		
		case "Magenta": value=0xFF00FF;break;		
		case "Maroon": value=0x800000;break;	
		case "Gold": value=0xFFD700;text_color="Black";break;
		default: value=0;break;
		}
	document.getElementById("myDropdown").style.background=color;
	document.getElementById("myDropdown").style.color=text_color;
	websock.send(JSON.stringify({ cmd : "color", val : value }))
}
 function resizeStream() {
   aspectRatio = 640/480;
   maxHeight = window.innerHeight - document.getElementById('navBottom').offsetHeight;
   if(window.innerWidth*aspectRatio <= maxHeight){
     document.getElementById('stream').style.height = window.innerWidth*aspectRatio + "px";
     document.getElementById('stream').style.width = window.innerWidth + "px";
   }
   else {
     document.getElementById('stream').style.height = maxHeight + "px";
     document.getElementById('stream').style.width = maxHeight/aspectRatio + "px";
   }
 }
window.addEventListener('resize', resizeStream, false);
window.addEventListener('onorientationchange',resetCanvas);
window.addEventListener('resize',resetCanvas);
canvas.addEventListener( 'touchstart', onTouchStart, false );
canvas.addEventListener( 'touchmove', onTouchMove, false );
canvas.addEventListener( 'touchend', onTouchEnd, false );
canvas.addEventListener( 'mousemove', onMouseMove, false );
canvas.addEventListener( 'mousedown', onMouseDown, false );
canvas.addEventListener( 'mouseup', onMouseUp, false );
setInterval(draw, 1000/30); // draw app at 30fps
setInterval(sendControls, 1000/20); // send control input at 20fps
resizeStream();
        </script>
    </body>
</html>
)=====";
