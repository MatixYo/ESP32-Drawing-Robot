const char htmlTemplate[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    body { text-align: center; font-family: 'Trebuchet MS', Arial; margin: auto; }
    .slider { width: 300px; }
  </style>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
</head>
<body>
  <h1>ESP32 Drawing Robot</h1>
  <h2>Set XY Position</h2>
  <p>Pen X Position: <span id="xPos"></span></p>
  <input type="range" min="-50" max="50" class="slider" id="xSlider" value="%XPOS%" onchange="updateX(this.value)"/>
  <p>Pen Y Position: <span id="yPos"></span></p>
  <input type="range" min="30" max="125" class="slider" id="ySlider" value="%YPOS%" onchange="updateY(this.value)"/>

  <h2>Set Servo Angles</h2>
  <p>Servo Left Angle: <span id="a1Pos"></span></p>
  <input type="range" min="0" max="180" class="slider" id="angle1" value="0" onchange="updateA1(this.value)"/>
  <p>Servo Right Angle: <span id="a2Pos"></span></p>
  <input type="range" min="0" max="180" class="slider" id="angle2" value="180" onchange="updateA2(this.value)"/>

  <div>
    <button onclick="lift('up')">Lift Up</button>
    <button onclick="lift('down')">Lift Down</button>
    <button onclick="lift('reset')">Reset</button>
  </div>
  <script>
    $.ajaxSetup({
      timeout: 1000,
      error: function(xhr) {
        alert(xhr.responseText);
      }
    });

    var xSlider = document.getElementById('xSlider');
    var xPos = document.getElementById('xPos');
    xPos.innerHTML = xSlider.value;
    xSlider.oninput = function() { xPos.innerHTML = this.value; };

    var ySlider = document.getElementById('ySlider');
    var yPos = document.getElementById('yPos');
    yPos.innerHTML = ySlider.value;
    ySlider.oninput = function() { yPos.innerHTML = this.value; };

    var angle1 = document.getElementById('angle1');
    var angle2 = document.getElementById('angle2');
    var a1Pos = document.getElementById('a1Pos');
    var a2Pos = document.getElementById('a2Pos');
    a1Pos.innerHTML = angle1.value;
    a2Pos.innerHTML = angle2.value;
    angle1.oninput = function() { a1Pos.innerHTML = this.value; };
    angle2.oninput = function() { a2Pos.innerHTML = this.value; };

    function updateX(x) { sendXY(x, ySlider.value); }
    function updateY(y) { sendXY(xSlider.value, y); }
    function sendXY(x, y) {
      $.get('/controlXY?x=' + x + '&y=' + y);
    }

    function updateA1(a1) { sendAngle(a1, angle2.value); }
    function updateA2(a2) { sendAngle(angle1.value, a2); }
    function sendAngle(a1, a2) {
    $.get('/controlAngles?a1=' + a1 + '&a2=' + a2);
    }
    function lift(direction) { $.get('/lift?direction=' + direction); }
  </script>
</body>
</html>
)rawliteral";