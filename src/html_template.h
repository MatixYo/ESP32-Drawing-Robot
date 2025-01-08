const char htmlTemplate[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    body {
      font-family: 'Trebuchet MS', Arial, sans-serif;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      background-color: #f9f9f9;
      color: #333;
    }

    h1 {
      margin-top: 20px;
      color: #005f99;
    }

    h2 {
      color: #007acc;
    }

    .container {
      max-width: 600px;
      width: 90%%;
      background: #fff;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
      border-radius: 8px;
      padding: 20px;
      margin: 20px 0;
    }

    .control-group {
      margin-bottom: 20px;
    }

    .control-group label {
      display: block;
      margin-bottom: 5px;
      font-weight: bold;
    }

    .slider-container {
      display: flex;
      align-items: center;
    }

    .slider {
      flex: 1;
      margin: 0 10px;
    }

    .value {
      font-weight: bold;
      width: 40px;
      text-align: center;
    }

    .button-group {
      display: flex;
      justify-content: center;
      gap: 10px;
    }

    button {
      background-color: #007acc;
      color: white;
      border: none;
      padding: 10px 20px;
      border-radius: 5px;
      font-size: 16px;
      cursor: pointer;
      transition: background-color 0.2s ease-in-out;
    }

    button:hover {
      background-color: #005f99;
    }

    button:active {
      background-color: #00497a;
    }
  </style>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
</head>
<body>
  <h1>ESP32 Drawing Robot</h1>

  <div class="container">
    <h2>Set XY Position</h2>
    <div class="control-group">
      <label for="xSlider">Pen X Position: <span class="value" id="xPos">%XPOS%</span></label>
      <div class="slider-container">
        <input type="range" min="%XMIN%" max="%XMAX%" class="slider" id="xSlider" value="%XPOS%" onchange="updateX(this.value)">
      </div>
    </div>
    <div class="control-group">
      <label for="ySlider">Pen Y Position: <span class="value" id="yPos">%YPOS%</span></label>
      <div class="slider-container">
        <input type="range" min="%YMIN%" max="%YMAX%" class="slider" id="ySlider" value="%YPOS%" onchange="updateY(this.value)">
      </div>
    </div>
  </div>

  <div class="container">
    <h2>Draw Circle</h2>
    <div class="control-group">
      <label for="radiusSlider">Radius: <span class="value" id="radiusVal">30</span></label>
      <div class="slider-container">
        <input type="range" min="10" max="100" class="slider" id="radiusSlider" value="30">
      </div>
    </div>
    <div class="button-group">
      <button onclick="drawCircle(xSlider.value, ySlider.value, radiusSlider.value)">Draw Circle</button>
    </div>
  </div>

  <div class="container">
    <h2>Set Servo Angles</h2>
    <div class="control-group">
      <label for="angle1">Servo Left Angle: <span class="value" id="a1Pos">0</span></label>
      <div class="slider-container">
        <input type="range" min="0" max="180" class="slider" id="angle1" value="0" onchange="updateA1(this.value)">
      </div>
    </div>
    <div class="control-group">
      <label for="angle2">Servo Right Angle: <span class="value" id="a2Pos">180</span></label>
      <div class="slider-container">
        <input type="range" min="0" max="180" class="slider" id="angle2" value="180" onchange="updateA2(this.value)">
      </div>
    </div>
  </div>

  <div class="container button-group">
    <button onclick="lift('up')">Lift Up</button>
    <button onclick="lift('down')">Lift Down</button>
    <button onclick="reset()">Reset</button>
  </div>

  <script>
      $.ajaxSetup({
      timeout: 10000,
      error: function(xhr) {
        alert(xhr.responseText);
      }
    });

    const xSlider = document.getElementById('xSlider');
    const xPos = document.getElementById('xPos');
    xPos.textContent = xSlider.value;
    xSlider.oninput = function () { xPos.textContent = this.value; };

    const ySlider = document.getElementById('ySlider');
    const yPos = document.getElementById('yPos');
    yPos.textContent = ySlider.value;
    ySlider.oninput = function () { yPos.textContent = this.value; };

    const angle1 = document.getElementById('angle1');
    const angle2 = document.getElementById('angle2');
    const a1Pos = document.getElementById('a1Pos');
    const a2Pos = document.getElementById('a2Pos');
    a1Pos.textContent = angle1.value;
    a2Pos.textContent = angle2.value;
    angle1.oninput = function () { a1Pos.textContent = this.value; };
    angle2.oninput = function () { a2Pos.textContent = this.value; };

    const radiusSlider = document.getElementById('radiusSlider');
    const radiusVal = document.getElementById('radiusVal');
    radiusVal.textContent = radiusSlider.value;
    radiusSlider.oninput = function () { radiusVal.textContent = this.value; };

    function updateX(x) { sendXY(x, ySlider.value); }
    function updateY(y) { sendXY(xSlider.value, y); }
    function sendXY(x, y) { $.get('/controlXY?x=' + x + '&y=' + y); }

    function updateA1(a1) { sendAngle(a1, angle2.value); }
    function updateA2(a2) { sendAngle(angle1.value, a2); }
    function sendAngle(a1, a2) { $.get('/controlAngles?a1=' + a1 + '&a2=' + a2); }

    function drawCircle(x, y, r) { $.get('/circle?x=' + x + '&y=' + y + '&r=' + r); }
    function lift(direction) { $.get('/lift?direction=' + direction); }
    function reset() { $.get('/reset'); }
  </script>
</body>
</html>
)rawliteral";
