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
  <p>Pen X Position: <span id="xPos"></span></p>
  <input type="range" min="-50" max="50" class="slider" id="xSlider" value="%XPOS%" onchange="updateX(this.value)"/>
  <p>Pen Y Position: <span id="yPos"></span></p>
  <input type="range" min="0" max="100" class="slider" id="ySlider" value="%YPOS%" onchange="updateY(this.value)"/>
  <button onclick="lift('up')">Lift Up</button>
  <button onclick="lift('down')">Lift Down</button>
  <script>
    var xSlider = document.getElementById('xSlider');
    var xPos = document.getElementById('xPos');
    xPos.innerHTML = xSlider.value;
    xSlider.oninput = function() { xPos.innerHTML = this.value; };

    var ySlider = document.getElementById('ySlider');
    var yPos = document.getElementById('yPos');
    yPos.innerHTML = ySlider.value;
    ySlider.oninput = function() { yPos.innerHTML = this.value; };

    function updateX(x) { sendXY(x, ySlider.value); }
    function updateY(y) { sendXY(xSlider.value, y); }
    function sendXY(x, y) {
      $.get('/controlXY?x=' + x + '&y=' + y);
    }
    function lift(direction) { $.get('/lift?direction=' + direction); }
  </script>
</body>
</html>
)rawliteral";