const L1 = 25.8; // Distance between the two servos (mm)
const L2 = 60.0; // Length of the first arm connected to servo (mm)
const L3 = 70.0; // Length of the second arm connected to the pen (mm)

function calculateXY(rightAngle, leftAngle) {
    // Convert angles to radians
    rightAngle = rightAngle * Math.PI / 180;
    leftAngle = leftAngle * Math.PI / 180 - 180;

    // Calculate the position of the pen
    const Cx = sin(rightAngle) * L2 + L1 / 2;
    const Cy = cos(rightAngle) * L2;

    const Ex = sin(leftAngle) * L2 - L1 / 2;
    const Ey = cos(leftAngle) * L2;

    const EC = Math.sqrt((Ex - Cx) ** 2 + (Ey - Cy) ** 2);
    const di

    return { x, y };
}