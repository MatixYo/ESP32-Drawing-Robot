#include <helpers.h>

struct Arc
{
    Position center;
    float radius;
    int dir;
    float angleDelta;
};

Servo servoLift;
Servo servoLeft;
Servo servoRight;

// Initial position of the pen
Position currentPosition = HOMING_POSITION;

// Targets
Position *linearTargetPosition = nullptr;
Arc *arcTarget = nullptr;
int delayUntil;

// Last update time
int lastUpdate;

// Speed
float speed = DEFAULT_SPEED;

// Static functions
/**
 * @brief Returns the angle between two points in radians (-
 */
float angleBetweenPoints(const Position &p1, const Position &p2)
{
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

/**
 * @brief Returns the distance between two points
 */
float distanceBetweenPoints(const Position &p1, const Position &p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function to calculate servo angles
bool calculateServoAngles(const Position &position, Angles &angles)
{
    // Invert x axis
    int x = -position.x;
    int y = position.y;

    if (x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
    {
        return false;
    }
    // Calculate the horizontal distances from each servo
    float x1 = x + L1 / 2; // Offset for the left servo
    float x2 = x - L1 / 2; // Offset for the right servo

    // Calculate the distance from each servo base to the target point
    float D1 = sqrt(pow(x1, 2) + pow(y, 2));
    float D2 = sqrt(pow(x2, 2) + pow(y, 2));

    // Check if the point is reachable
    if (D1 > (L2 + L3) || D2 > (L2 + L3) || D1 < fabs(L2 - L3) || D2 < fabs(L2 - L3))
    {
        return false; // Target is unreachable
    }

    // Calculate the angles for the left servo (θ1)
    float gamma1 = atan2(x1, y); // Angle from the servo to the point
    float theta1 = acos((pow(L2, 2) + pow(D1, 2) - pow(L3, 2)) / (2 * L2 * D1));
    angles.left = degrees(theta1 - gamma1); // Counterclockwise rotation for left servo

    // Calculate the angles for the right servo (θ2)
    float gamma2 = atan2(x2, y); // Angle from the servo to the point
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

    if(left < MIN_PULSE_WIDTH || left > MAX_PULSE_WIDTH || right < MIN_PULSE_WIDTH || right > MAX_PULSE_WIDTH) return false;
    
    servoLeft.write(left);
    servoRight.write(right);

    return true;
}

void setPenPosition(const Position &position)
{
    Angles angles;

    bool canMove = calculateServoAngles(position, angles);
    bool canSet = setServoAngles(angles);

    // In case of unreachable position, restart the ESP
    if(!canMove || !canSet) {
        ESP.restart();
    }
    
    currentPosition = position;
}

void updateLinearMove(float delta)
{
    if (!linearTargetPosition)
        return;

    float travelDistance = speed * delta;

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

    float angularDeltaRad = speed * delta / arc.radius;

    float currentAngleRad = angleBetweenPoints(arc.center, currentPosition);
    float angleIncrement = arc.dir * angularDeltaRad;
    arc.angleDelta += angleIncrement;
    currentAngleRad += angleIncrement;

    if (arc.angleDelta >= 2 * PI)
    {
        arcTarget = nullptr;
        return;
    }

    Position newPosition;
    newPosition.x = arc.center.x + arc.radius * cos(currentAngleRad);
    newPosition.y = arc.center.y + arc.radius * sin(currentAngleRad);

    setPenPosition(newPosition);
}

void updateToolPosition()
{
    int currentTime = millis();
    float delta = (currentTime - lastUpdate) / 1000.0;
    lastUpdate = currentTime;

    if (currentTime < delayUntil)
        return;

    if (linearTargetPosition)
    {
        updateLinearMove(delta);
    }
    else if (arcTarget)
    {
        updateArcMove(delta);
    }
}

void resetTargets()
{
    linearTargetPosition = nullptr;
    arcTarget = nullptr;
}

// Public methods
Position getCurrentPosition()
{
    return currentPosition;
}

void waitFor(int delayMs)
{
    delayUntil = millis() + delayMs;
}

void homeXY()
{
    resetTargets();
    enableTool(false);
    setPenPosition(HOMING_POSITION);
}

void linearMove(Position &position)
{
    resetTargets();
    linearTargetPosition = new Position();
    linearTargetPosition->x = position.x;
    linearTargetPosition->y = position.y;
}

void arcMove(Position center, bool clockwise, Position *end)
{
    resetTargets();

    static Arc arc;
    arc.center = center;
    arc.radius = distanceBetweenPoints(center, currentPosition);
    arc.dir = clockwise ? 1 : -1;
    arc.angleDelta = 0;

    arcTarget = &arc;
}

void internalEnableTool(bool enable)
{
    servoLift.write(enable ? LIFT_DOWN_ANGLE : LIFT_UP_ANGLE);
}

void enableTool(bool enable)
{
    resetTargets();

    internalEnableTool(enable);
    waitFor(LIFT_WAIT);
}

void assemblyPosition()
{
    internalEnableTool(true);
    Angles angles = {0, 180};
    setServoAngles(angles);
}

bool isBusy()
{
    return linearTargetPosition || arcTarget || millis() < delayUntil;
}

float getSpeed()
{
    return speed;
}

void setSpeed(float newSpeed)
{
    speed = constrain(newSpeed, MIN_SPEED, MAX_SPEED);
}
