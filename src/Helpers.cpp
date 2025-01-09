#include <Helpers.h>

struct Arc
{
    Position center;
    float radius;
    float endAngleRad;
    bool clockwise;
};

Servo servoLift;
Servo servoLeft;
Servo servoRight;

// Initial position of the pen
Position currentPosition = HOMING_POSITION;

// Targets
Position *linearTargetPosition = nullptr;
Arc *arcTarget = nullptr;

// Last update time
int lastUpdate;

// Static functions
float angleBetweenPoints(const Position &p1, const Position &p2)
{
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

float distanceBetweenPoints(const Position &p1, const Position &p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function to calculate servo angles
bool calculateServoAngles(const Position &position, Angles &angles)
{
    if (position.x < MIN_X || position.x > MAX_X || position.y < MIN_Y || position.y > MAX_Y)
    {
        return false;
    }
    // Calculate the horizontal distances from each servo
    float x1 = position.x + L1 / 2; // Offset for the left servo
    float x2 = position.x - L1 / 2; // Offset for the right servo

    // Calculate the distance from each servo base to the target point
    float D1 = sqrt(pow(x1, 2) + pow(position.y, 2));
    float D2 = sqrt(pow(x2, 2) + pow(position.y, 2));

    // Check if the point is reachable
    if (D1 > (L2 + L3) || D2 > (L2 + L3) || D1 < fabs(L2 - L3) || D2 < fabs(L2 - L3))
    {
        return false; // Target is unreachable
    }

    // Calculate the angles for the left servo (θ1)
    float gamma1 = atan2(x1, position.y); // Angle from the servo to the point
    float theta1 = acos((pow(L2, 2) + pow(D1, 2) - pow(L3, 2)) / (2 * L2 * D1));
    angles.left = degrees(theta1 - gamma1); // Counterclockwise rotation for left servo

    // Calculate the angles for the right servo (θ2)
    float gamma2 = atan2(x2, position.y); // Angle from the servo to the point
    float theta2 = acos((pow(L2, 2) + pow(D2, 2) - pow(L3, 2)) / (2 * L2 * D2));
    angles.right = 180 - degrees(gamma2 + theta2); // Counterclockwise rotation for right servo

    return true; // Successful calculation
}

bool arePositionsEqual(const Position &pos1, const Position &pos2)
{
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    return abs(dx) <= MAX_DELTA && abs(dy) <= MAX_DELTA;
}

// Actuation functions
bool setServoAngles(Angles &angles)
{
    int left = map(angles.left * 100, 0, 18000, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    int right = map(angles.right * 100, 0, 18000, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);

    servoLeft.write(left);
    servoRight.write(right);

    // TODO: add checks
    return true;
}

bool setPenPosition(const Position &position)
{
    Angles angles;

    if (calculateServoAngles(position, angles))
    {
        setServoAngles(angles);
        currentPosition = position;
        return true;
    }

    return false;
}

void updateLinearMove(float delta)
{
    if (!linearTargetPosition)
        return;

    float travelDistance = SPEED * delta;

    // Calculate the distance to the target point
    float dx = currentPosition.x - linearTargetPosition->x;
    float dy = currentPosition.y - linearTargetPosition->y;
    float angle = atan2(dy, dx);
    float maxDistance = sqrt(pow(dx, 2) + pow(dy, 2));

    float distance = min(travelDistance, maxDistance);

    // Move the pen to the new position
    Position newPosition;
    newPosition.x = currentPosition.x - cos(angle) * distance;
    newPosition.y = currentPosition.y - sin(angle) * distance;
    setPenPosition(newPosition);

    if (arePositionsEqual(currentPosition, *linearTargetPosition))
    {
        linearTargetPosition = nullptr;
    }
}

void updateArcMove(float delta)
{
    if (!arcTarget)
        return;

    Arc &arc = *arcTarget;

    float angularDeltaRad = SPEED * delta / arc.radius;
    float currentAngleRad = angleBetweenPoints(arc.center, currentPosition);
    currentAngleRad += arc.clockwise * angularDeltaRad;

    // TODO: finish this function
}

void updateToolPosition()
{
    int currentTime = millis();
    float delta = (currentTime - lastUpdate) / 1000.0;
    lastUpdate = currentTime;

    if (linearTargetPosition)
    {
        updateLinearMove(delta);
    }
    else if (arcTarget)
    {
        updateArcMove(delta);
    }
}

// Public methods
Position getCurrentPosition()
{
    return currentPosition;
}

void homeXY()
{
    liftTool(true);
    setPenPosition(HOMING_POSITION);
}

void linearMove(Position &position)
{
    Serial.printf("Moving to X: %.2f, Y: %.2f\n", position.x, position.y);
    linearTargetPosition = &position;
}

void arcMove(Position &center, bool clockwise, Position *end)
{
    Serial.printf("Moving in an arc.");

    Arc arc;
    arc.center = center;
    arc.radius = distanceBetweenPoints(center, currentPosition);
    arc.endAngleRad = angleBetweenPoints(center, end ? *end : currentPosition);
    arc.clockwise = clockwise;

    Serial.printf("Center: X: %.2f, Y: %.2f, Radius: %.2f, End Angle: %.2f, Clockwise: %d\n", center.x, center.y, arc.radius, arc.endAngleRad, clockwise);

    arcTarget = &arc;
}

void liftTool(bool up)
{
    servoLift.write(up ? LIFT_UP_ANGLE : LIFT_DOWN_ANGLE);
    delay(500);
}
