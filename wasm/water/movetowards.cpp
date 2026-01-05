#pragma once
#include "quote.h"

struct springPoint
{
    float2 position;
    float2 velocity;
};

struct Scene4
{
    float speed;
    float fps;

    float f;
    float z;
    float r;

    springPoint mouseFollower;
};

void DrawLineSegment(VideoBuffer* videoBuffer, float t0, float t1, float p0, float p1, float2 pos, float3 color, float alpha)
{
    float scale = 8;
    DrawLine(videoBuffer, float2(t0 * scale, p0 * scale) + pos, float2(t1 * scale, p1 * scale) + pos, color, alpha);
}
float TargetFunction(float t)
{
    float target = 0.0f;
    if (t > 0.2f)
        target = 100.0f;
    if (t > 1.2f)
        target = sin(t*3) * 30.0f+50;
    if (t > 4.0f)
        target = frac(t * 2) * 50 + 50;
    return target;
}

float lerpFunction_bad(float value, float target, float dt, float speed)
{
    return lerp(value, target, speed * dt);
}
float lerpFunction_good(float value, float target, float dt, float speed)
{
    float K = 1.0f - pow(speed, dt);
    value = lerp(value, target, K);
    return value;
}
//float lerpFunction_better(float value, float target, float dt, float speed)
//{
//    return lerp(target, value, exp2(-speed * dt));
//}
// Gradually changes a value towards a desired goal over time.
float smoothdamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
{
    // Based on Game Programming Gems 4 Chapter 1.10
    smoothTime = max(0.0001F, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48F * x * x + 0.235F * x * x * x);
    float change = current - target;
    float originalTo = target;

    // Clamp maximum speed
    float maxChange = maxSpeed * smoothTime;
    change = clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (*currentVelocity + omega * change) * deltaTime;
    *currentVelocity = (*currentVelocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // Prevent overshooting
    if (originalTo - current > 0.0F == output > originalTo)
    {
        output = originalTo;
        (*currentVelocity) = (output - originalTo) / deltaTime;
    }

    return output;
}



float accelerate(float y, float x, float* xp, float* xd, float* yd, float T, float f, float z, float r)
{
    float k1 = z / pi * f;
    float k2 = 1.0f / ((2 * pi * f) * (2 * pi * f));
    float k3 = r * z / (2 * pi * f);

    *xd = (x - *xp) / T;
    *xp = x;
    float k2_stable = max(k2, 1.1f * (T * T / 4 + T * k1 / 2));
    y = y + T * *yd;
    *yd = *yd + T * (x + k3 * *xd - y - k1 * *yd) / k2_stable;
    return y;
}

//float accelerate(float current, float target, float* velocity, float deltaTime, float damping)
//{
//    //deltaTime = deltaTime * (1.0 - pow(deltaTime, 2));
//    float acceleration = (target - current) * 50.1;
//    current += *velocity * deltaTime + 0.5 * acceleration * deltaTime * deltaTime;
//    *velocity += acceleration * deltaTime;
//
//    float newVelocity = *velocity * pow(damping*0.01, deltaTime);
//    current += (newVelocity - *velocity) / log(damping * 0.01);
//    *velocity = newVelocity;
//
//
//    //current += *velocity;
//    //
//    //*velocity += (target - current) * deltaTime;
//    //
//    //*velocity = lerpFunction_better(*velocity, 0, deltaTime, damping);
//    return current;
//}

float bounce(float current, float target, float* velocity, float deltaTime, float damping)
{
    float last = current;
    current += *velocity;



    //float d1 = dot(uv, vec2(1, 0));
    //float d2 = abs(dot(uv, vec2(0, 1)));
    //
    //float col = d1 + d2;


    // passed target
    if ((last < target && current > target) || (last > target && current < target))
    {
        *velocity *= -1;
        current = target;// *velocity;
    }

    *velocity += (target - current) * deltaTime;
    
    *velocity *= damping;

    return current;
}

springPoint bounce(springPoint current, float2 target, float deltaTime, float damping)
{
    springPoint last = current;
    current.position += current.velocity;

    // passed target
    if (dot(current.position - target, current.velocity) > 0 && dot(last.position - target, current.velocity) < 0)
    {
        current.position -= current.velocity;
        current.velocity *= -1;
        //current = target;// *velocity;
    }

    current.velocity += (target - current.position) * deltaTime;

    current.velocity *= damping;

    return current;
    //current.position.x = bounce(current.position.x, target.x, &current.velocity.x, deltaTime, damping);
    //current.position.y = bounce(current.position.y, target.y, &current.velocity.y, deltaTime, damping);
    //return current;
}


void SimulateLine(VideoBuffer* videoBuffer, Scene4* scene, int frameCount, float2 pos)
{
    float deltaTime = 1.0f / scene->fps;

    float value = 0;
    float lastValue = 0;

    float velocity = 0;
    float lastVelocity = 0;

    float time = 0;
    float lastTime = 0;

    float target = 0;
    float lastTarget = 0;

    float xp = 0;
    float xd = 0;
    float yd = 0;
    for (int i = 0; i < frameCount; i++)
    {
        lastTime = time;
        time += deltaTime;
        lastTarget = target;
        target = TargetFunction(time);

        lastValue = value;

        // bad
        //value = lerp(value, target, speed* dt);
        
        // good
        //float K = 1.0f - pow(speed, dt);
        //value = lerp(value, target, K);
        
        // better
        //value = lerpFunction_better(value, target, dt, speed);
        
        //value = smoothdamp(value, target, &velocity, speed, 1000000, dt);
        
        //value = MoveTowards(value, target, dt * speed*10.0);

        //value = accelerate(value, target, &xp, &xd, &yd, deltaTime, scene->f, scene->z, scene->r);

        //value = accelerate(value, target, &velocity, deltaTime, scene->speed);
        
        value = bounce(value, target, &velocity, deltaTime, scene->speed);

        DrawLineSegment(videoBuffer, lastTime * 30.0f, time * 30.0f, lastValue, value, pos, float3(1.00, 0.50, 0.50), 1.0f);
    }
}
void DrawTarget(VideoBuffer* videoBuffer, Scene4* scene, int frameCount, float2 pos)
{
    float dt = 1.0f / scene->fps;
    float time = 0;
    float lastTime = 0;
    float target = 0;
    float lastTarget = 0;
    for (int i = 0; i < frameCount; i++)
    {
        lastTime = time;
        time += dt;
        lastTarget = target;
        target = TargetFunction(time);

        DrawLineSegment(videoBuffer, lastTime * 30.0f, time * 30.0f, lastTarget, target, pos, float3(0.75, 0.50, 0.75), 0.5f);
    }
}

void movetowards(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{
    Scene4* scene = (Scene4*)(((uint8*)(engineState + 1)));

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = memory->platformSetForcefeedbackEffect;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)memory->transientStorage + sizeof(EngineState) + sizeof(Scene3) + Kilobytes(100));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);

        memory->initialized = true;
    }

    scene->mouseFollower = bounce(scene->mouseFollower, float2(input->MouseX, input->MouseY), input->deltaTime, 0.95f);

    DrawClear(videoBuffer, float3(0.5, 0, 0));

    DrawBox(videoBuffer, scene->mouseFollower.position, float2(5, 5));


    //const int frameCount = 240*2;
    float2 pos = float2(100, 0);

    scene->speed = Slider(engineState, input, videoBuffer, &pos, scene->speed, 0.0f, 16.0f, {}, 400);
    Text(engineState, videoBuffer, &pos, "Speed: ", scene->speed);
    pos.y += 10;

    scene->fps = Slider(engineState, input, videoBuffer, &pos, scene->fps, 1.0f, 240.0f, {}, 400);
    Text(engineState, videoBuffer, &pos, "FPS: ", scene->fps);
    pos.y += 10;

    // natural frequency in osclations per second.
    scene->f = Slider(engineState, input, videoBuffer, &pos, scene->f, 0.0f, 8.0f, {}, 400);
    Text(engineState, videoBuffer, &pos, "frequency: ", scene->f);
    pos.y += 10;
    // 
    scene->z = Slider(engineState, input, videoBuffer, &pos, scene->z, 0.0f, 4.0f, {}, 400);
    Text(engineState, videoBuffer, &pos, "damping: ", scene->z);
    pos.y += 10;
    scene->r = Slider(engineState, input, videoBuffer, &pos, scene->r, -5.0f, 5.0f, {}, 400);
    Text(engineState, videoBuffer, &pos, "acceleration: ", scene->r);
    pos.y += 10;

    int frameCount = scene->fps * 6.0f;

    SimulateLine(videoBuffer, scene, frameCount, pos);

    DrawTarget(videoBuffer, scene, frameCount, pos);
}