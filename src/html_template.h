const char htmlTemplate[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <title>ESP32 Drawing Robot</title>
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
      <label for="radiusSlider">Radius: <span class="value" id="radiusVal">20</span></label>
      <div class="slider-container">
        <input type="range" min="10" max="100" class="slider" id="radiusSlider" value="20">
      </div>
    </div>
    <div class="button-group">
      <button onclick="drawCircle(radiusSlider.value)">Draw Circle</button>
    </div>
  </div>

  <div class="container button-group">
    <button onclick="lift('up')">Lift Up</button>
    <button onclick="lift('down')">Lift Down</button>
    <button onclick="homeXY()">Home XY</button>
    <button onclick="reset()">Reset ESP</button>
  </div>

  <script>
      $.ajaxSetup({
      timeout: 10000,
      // error: function(xhr) {
      //   alert(xhr.responseText);
      // }
    });

    const xSlider = document.getElementById('xSlider');
    const xPos = document.getElementById('xPos');
    xPos.textContent = xSlider.value;
    xSlider.oninput = function () { xPos.textContent = this.value; };

    const ySlider = document.getElementById('ySlider');
    const yPos = document.getElementById('yPos');
    yPos.textContent = ySlider.value;
    ySlider.oninput = function () { yPos.textContent = this.value; };

    const radiusSlider = document.getElementById('radiusSlider');
    const radiusVal = document.getElementById('radiusVal');
    radiusVal.textContent = radiusSlider.value;
    radiusSlider.oninput = function () { radiusVal.textContent = this.value; };

    function updateX(x) { sendXY(x, ySlider.value); }
    function updateY(y) { sendXY(xSlider.value, y); }
    function sendXY(x, y) { $.get('/controlXY?x=' + x + '&y=' + y); }

    function drawCircle(r) { $.get('/circle?&r=' + r); }
    function lift(direction) { $.get('/lift?direction=' + direction); }
    function homeXY() { $.get('/homeXY'); }
    function reset() { $.get('/reset'); }
  </script>
</body>
</html>
)rawliteral";
// TODO: homing should set position to home position
