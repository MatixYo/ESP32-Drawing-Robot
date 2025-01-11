const char htmlTemplate[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <link rel="icon" href="data:," />
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

        .button-group {
            display: flex;
            justify-content: center;
            gap: 10px;
        }

        button {
            background-color: #007acc;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 8px;
            font-size: 14px;
            cursor: pointer;
            transition: background-color 0.2s ease-in-out;
        }

        button:hover {
            background-color: #005f99;
        }

        button:active {
            background-color: #00497a;
        }

        .bed {
            position: relative;
            margin: 15px 0;
            background-color: #dcd9d9;
            border: 2px solid #007acc;
            border-radius: 2px;
            background-image: linear-gradient(rgba(31, 130, 206, 0.3) .1em, transparent .1em), linear-gradient(90deg, rgba(28, 117, 180, 0.3) .1em, transparent .1em);
            background-size: 40px 40px;
            cursor: pointer;
        }

        #canvas {
            width: 100%%;
        }

        .marker {
            position: absolute;
            display: flex;
            justify-content: center;
            align-items: center;
            width: 2px;
            height: 20px;
            transform: translate(-50%%, -50%%);
            transition: all 0.2s ease-in-out;
        }

        .marker::before,
        .marker::after {
            content: "";
            position: absolute;
            width: 2px;
            height: 100%%;
            border-radius: 9999px;
            background-color: #333;
        }

        .marker::before {
            transform: rotate(45deg);
        }

        .marker::after {
            transform: rotate(-45deg);
        }

        #bed-container.move #button-move,
        #bed-container.line #button-line {
            background-color: #00497a;
        }

        .position {
            display: flex;
            justify-content: space-between;
            margin: 10px 0;
            color: #555;
            font-weight: bold;
            font-size: 14px;
        }
    </style>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
</head>

<body>
    <h1>ESP32 Drawing Robot</h1>

    <div class="container move" id="bed-container">
        <h2>Set XY Position</h2>
        <div class="bed" onclick="handleBedClick(event)">
            <canvas id="canvas"></canvas>
            <div class="marker"></div>
        </div>
        <div class="position">
            <div id="targetPosition"></div>
            <div id="mousePosition"></div>
        </div>
        <div class="button-group">
            <button id="button-move" onclick="setMode('move')">Move XY</button>
            <button id="button-line" onclick="setMode('line')">Draw Line</button>
            <button id="button-clear" onclick="clearGcode()">Clear Gcode</button>
            <button id="button-print" onclick="sendPrint()">Print</button>
        </div>
    </div>

    <div class="container button-group">
        <button onclick="raise()">Raise Pen</button>
        <button onclick="raise(false)">Lower Pen</button>
        <button onclick="home()">Home XY</button>
        <button onclick="restart()">Restart ESP</button>
        <button onclick="assembly()">Assembly</button>
    </div>

    <div class="container">
        <h2>Gcode</h2>
        <pre id="gcode"></pre>
    </div>

    <script>
        $.ajaxSetup({ timeout: 10000 });

        const X_MIN = %X_MIN%;
        const X_MAX = %X_MAX%;
        const Y_MIN = %Y_MIN%;
        const Y_MAX = %Y_MAX%;
        const X_SIZE = X_MAX - X_MIN;
        const Y_SIZE = Y_MAX - Y_MIN;

        $('#canvas').attr({ width: X_SIZE * 10, height: Y_SIZE * 10 });

        let mode = 'move', gcode = '';

        function updateGcode(g) {
            gcode = g;
            $('#gcode').text(gcode);

            // draw gcode on canvas
            const canvas = document.getElementById('canvas');
            const ctx = canvas.getContext('2d');
            ctx.reset();
            ctx.lineWidth = 5;

            if (!gcode) return;

            for (const line of gcode.split('\n')) {
                const [cmd] = line.split(' ');
                if (cmd === 'G1') {
                    const [, x, y] = line.match(/X(-?\d+\.\d+) Y(-?\d+\.\d+)/);
                    const x0 = nmap(parseFloat(x), X_MIN, X_MAX, 0, canvas.width);
                    const y0 = nmap(parseFloat(y), Y_MIN, Y_MAX, 0, canvas.height);
                    ctx.lineTo(x0, y0);
                } else if (cmd === 'M3') {
                    ctx.strokeStyle = "#000"; // Change color to indicate spindle/laser ON
                    ctx.beginPath(); // Start a new path
                } else if (cmd === 'M5') {
                    ctx.stroke(); // Draw the current path
                    ctx.strokeStyle = "black"; // Change back to default
                    ctx.beginPath(); // Start a new path after spindle/laser OFF
                }
            }

            ctx.stroke();
        }

        function clearGcode() {
            updateGcode('');
        }

        function addGcode(g) {
            updateGcode(gcode ? `${gcode}\n${g}` : g);
        }

        // Checks if the last command in the gcode is a drawing command
        function drawingMode() {
            const cmd = gcode.split('\n').map(l => l.split(' ')[0]).reverse().find(c => ['M3', 'M5'].includes(c));

            if (cmd === 'M3') return true;
            return false;
        }

        function startDrawing() {
            const isDrawing = drawingMode();
            if (isDrawing) return false;
            addGcode('M3');
            return true;
        }

        function endDrawing() {
            const isDrawing = drawingMode();
            if (!isDrawing) return;
            addGcode('M5');
        }

        function f(value) {
            return value.toFixed(1);
        }

        function nmap(x, in_min, in_max, out_min, out_max) {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        const modes = ['move', 'line'];

        function setMode(newMode) {
            if (!modes.includes(newMode)) return;
            mode = newMode;
            endDrawing();
            $('#bed-container').removeClass(modes.join(" ")).addClass(mode);
        }

        function updateRenderedPosition(x, y) {
            $("#targetPosition").text(`X: ${f(x)}, Y: ${f(y)}`);

            const x0 = nmap(x, X_MIN, X_MAX, 0, X_SIZE);
            const y0 = nmap(y, Y_MIN, Y_MAX, 0, Y_SIZE);

            $(".marker").css({ left: x0 + "%%", top: y0 + "%%" });
        }

        function drawLinePoint(x, y) {
            addGcode(`G1 X${f(x)} Y${f(y)}`);
            if(startDrawing()) {
                addGcode(`G1 X${f(x)} Y${f(y)}`);
            }
        }

        function handleBedClick(e) {
            const rect = e.currentTarget.getBoundingClientRect();
            const x = nmap(e.clientX - rect.left, 0, rect.width, X_MIN, X_MAX);
            const y = nmap(e.clientY - rect.top, 0, rect.height, Y_MIN, Y_MAX);

            if (mode === 'move') {
                move(x, y);
            } else if (mode === 'line') {
                drawLinePoint(x, y);
            }
        }

        function sendPrint() {
            endDrawing();
            sendGcode(`${gcode}\nG28`);
        }

        updateRenderedPosition(%X_POS%, %Y_POS%);

        function move(x, y) {
            sendGcode(`G1 X${f(x)} Y${f(y)}`);
            updateRenderedPosition(x, y);
        }

        async function home() {
            sendGcode('G28');
            await new Promise(r => setTimeout(r, 500));
            const state = await getState();
            updatePosition(state.x, state.y);
        }

        function raise(up = true) {
            sendGcode(up ? 'M5' : 'M3');
        }

        function sendGcode(g) {
            $.post('/gcode', { gcode: g });
        }

        function getState() {
            return $.get('/state');
        }
        function restart() { $.post('/restart'); }
        function assembly() { $.post('/assembly'); }
    </script>
</body>

</html>
)rawliteral";
// TODO: show mouse position on the bed
// TODO: sth else I forgot
