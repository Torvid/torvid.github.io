
// 2d physics

#if 0

if (Input->FDown)
engineState->Bouncy = !engineState->Bouncy;

float2 MousePos = float2(Input->MouseX, Input->MouseY);

engineState->Bodies2D[0] = RigidBody2DBox(float2(500, 500), float2(750, 50), normalize(float2(0.2, 2.5)), 1, true);
engineState->Bodies2D[1] = RigidBody2DBox(float2(650, 100), float2(400, 5), normalize(float2(0, 0.5)), 1, true);
engineState->Bodies2D[2] = RigidBody2DBox(float2(150, 400), float2(550, 60), normalize(float2(-0.6, 0.5)), 1, true);
engineState->Bodies2D[3] = RigidBody2DBox(float2(1000, 400), float2(50, 200), normalize(float2(-0.0, 0.5)), 1, true);

engineState->Bodies2D[4] = RigidBody2DBox(float2(500, 490), float2(900, 100), normalize(float2(-0.0, 0.5)), 1, true);

if (Input->MouseLeft)
{
    //for (int i = 0; i < 50-4; i++)
    //{
    //    engineState->Bodies2D[4+i] = RigidBody2DSphere(float2(Input->MouseX+i*10, Input->MouseY), 20, false);
    //}
    engineState->Bodies2D[8] = RigidBody2DSphere(float2(Input->MouseX, Input->MouseY), 50, 2, false);
    engineState->Bodies2D[9] = RigidBody2DSphere(float2(Input->MouseX + 300, Input->MouseY), 50, 1, false);
    engineState->Bodies2D[10] = RigidBody2DSphere(float2(Input->MouseX + 400, Input->MouseY), 50, 1, false);
    engineState->Bodies2D[11] = RigidBody2DSphere(float2(Input->MouseX + 500, Input->MouseY), 50, 1, false);
    engineState->Bodies2D[12] = RigidBody2DSphere(float2(Input->MouseX + 600, Input->MouseY), 50, 1, false);
    engineState->Bodies2D[13] = RigidBody2DSphere(float2(Input->MouseX + 700, Input->MouseY), 50, 1, false);
}
engineState->Bodies2D[8].angularVelocity += Y * 0.01;

for (int i = 0; i < ArrayCount(engineState->Bodies2D); i++)
{
    RigidBody2D* Body = &engineState->Bodies2D[i];
    if (!Body->initialized)
        continue;

    // Draw
    if (Body->type == EntityType_Sphere)
        DrawCircle(VideoBuffer, Body->position, Body->radius);
    else if (Body->type == EntityType_Box)
        DrawBox(VideoBuffer, Body->position, Body->size, Body->up, Body->right);

    DrawLine(VideoBuffer, Body->position, Body->position + Body->up * 50);
    DrawLine(VideoBuffer, Body->position, Body->position + Body->right * 50);

    // De-intersect
    // Currently only spheres can be dynamic
    if (Body->type != EntityType_Sphere)
        continue;

    Body->velocity += float2(0, 1);
    Body->position += Body->velocity;

    float2 DeIntersectPosition = Body->position;
    float2 LastDeIntersectPosition = Body->position;

    RigidBody2D* HitDynamicBody = 0;

    float2 DeintersectVector = float2(0, 0);
    for (int j = 0; j < 10; j++)
    {
        for (int k = 0; k < ArrayCount(engineState->Bodies2D); k++)
        {
            RigidBody2D* OtherBody = &engineState->Bodies2D[k];
            if (!OtherBody->initialized)
                continue;
            if (OtherBody == Body)
                continue;

            float2 DeIntersect = float2(0, 0);
            if (OtherBody->type == EntityType_Box)
                DeIntersect = CircleBoxDeIntersection(OtherBody->position, OtherBody->size, OtherBody->up, OtherBody->right, Body->position, Body->radius);
            else if (OtherBody->type == EntityType_Sphere)
                DeIntersect = CircleCircleDeIntersection(OtherBody->position, OtherBody->radius, Body->position, Body->radius);

            DeintersectVector += DeIntersect;
            DeIntersectPosition += DeIntersect;
            DrawLine(VideoBuffer, LastDeIntersectPosition, DeIntersectPosition);
            LastDeIntersectPosition = DeIntersectPosition;
            if (length(DeIntersect) > 0)
            {
                //float MoveDistance = distance(engineState->LastSpherePos, engineState->SpherePos);
                // TODO: If the other object is also dynamic, do something else here.
                Body->position += DeIntersect;
                if (!OtherBody->kinematic)
                    HitDynamicBody = OtherBody;
            }
        }
    }

    // Add force
    float bouncyness = engineState->Bouncy ? 1 : 0;
    if (length(DeintersectVector) > 0)
    {
        // When something hits something, angular velocity and velocity try to balance out. 
        // If it's spinning a lot, it starts moves
        // If it's moving a lot, it starts spinning

        float2 normal = normalize(DeintersectVector);
        float2 tangent = float2(-normal.y, normal.x);

        float2 StartVel = Body->velocity;
        float StartAngularVel = Body->angularVelocity;

        float2 NewVel = Body->velocity;
        float NewAngularVel = Body->angularVelocity;

        float2 BounceVel = reflect(NewVel, normal);
        float2 StopVel = tangent * dot(tangent, NewVel);
        NewVel = lerp(StopVel, BounceVel, bouncyness);

        NewVel *= 0.5f;
        NewAngularVel *= 0.5f;
        NewAngularVel += dot(tangent, NewVel) / 100;
        NewVel += tangent * NewAngularVel * 50;

        float2 DeltaVel = (NewVel - StartVel);
        float DeltaAngularVel = (NewAngularVel - StartAngularVel);

        if (HitDynamicBody)
        {
            float MassRatio1 = Body->mass / HitDynamicBody->mass;
            float MassRatio2 = HitDynamicBody->mass / Body->mass;

            Body->velocity += DeltaVel * MassRatio2;
            Body->angularVelocity += DeltaAngularVel * MassRatio2;

            HitDynamicBody->velocity -= DeltaVel * 0.75 * MassRatio1;
            HitDynamicBody->angularVelocity -= DeltaAngularVel * 0.75 * MassRatio1;
        }
        else
        {
            Body->velocity += DeltaVel;
            Body->angularVelocity += DeltaAngularVel;
        }
    }
    Body->up = rotate(Body->up, Body->angularVelocity);
    Body->right = rotate(Body->right, Body->angularVelocity);
}
#endif



bool SphereBoxIntersect(transform* Sphere, transform* Box)
{
    float3 LocalPos = Sphere->position - Box->position;
    LocalPos = float3(dot(LocalPos, Box->right * Box->scale.x), dot(LocalPos, Box->forward * Box->scale.y), dot(LocalPos, Box->up * Box->scale.z));

    if (LocalPos.x < 0.5 && LocalPos.x > -0.5 &&
        LocalPos.y < 0.5 && LocalPos.y > -0.5 &&
        LocalPos.z < 0.5 && LocalPos.z > -0.5)
    {
        return true;
    }
    return false;
}

float2 CircleBoxDeIntersection(float2 BoxPos, float2 BoxSize, float2 BoxUp, float2 BoxRight, float2 SpherePos, float SphereRadius)
{
    SpherePos = BoxPos + float2(dot(SpherePos - BoxPos, BoxRight), dot(SpherePos - BoxPos, BoxUp));

    float2 InverseBoxUp = float2(BoxRight.y, BoxUp.y);
    float2 InverseBoxRight = float2(BoxRight.x, BoxUp.x);

    float2 LargeBoxSize = BoxSize + SphereRadius * 2;
    float p1 = (BoxPos.x - LargeBoxSize.x * 0.5f);
    float p2 = (BoxPos.x + LargeBoxSize.x * 0.5f);
    float p3 = (BoxPos.y - LargeBoxSize.y * 0.5f);
    float p4 = (BoxPos.y + LargeBoxSize.y * 0.5f);

    bool InBox = SpherePos.x > p1 &&
        SpherePos.x < p2&&
        SpherePos.y > p3 &&
        SpherePos.y < p4;

    float2 deIntersectPos1 = BoxPos + sign(SpherePos - BoxPos) * LargeBoxSize * 0.5;
    if (abs((SpherePos - BoxPos) / BoxSize).x < abs((SpherePos - BoxPos) / BoxSize).y)
        deIntersectPos1.x = SpherePos.x;
    else
        deIntersectPos1.y = SpherePos.y;

    float2 a = abs(BoxPos - SpherePos) - BoxSize * 0.5;
    float q = min(a.x, a.y);
    bool incross = q < 0;

    float2 corner1 = BoxPos + BoxSize * float2(0.5, 0.5);
    float2 corner2 = BoxPos + BoxSize * float2(-0.5, 0.5);
    float2 corner3 = BoxPos + BoxSize * float2(0.5, -0.5);
    float2 corner4 = BoxPos + BoxSize * float2(-0.5, -0.5);

    bool insphere = distance(SpherePos, corner1) < SphereRadius ||
        distance(SpherePos, corner2) < SphereRadius ||
        distance(SpherePos, corner3) < SphereRadius ||
        distance(SpherePos, corner4) < SphereRadius;

    float2 deIntersectPos2 = float2(0, 0);
    if (distance(SpherePos, corner1) < SphereRadius)
        deIntersectPos2 = corner1 + normalize(SpherePos - corner1) * SphereRadius;
    else if (distance(SpherePos, corner2) < SphereRadius)
        deIntersectPos2 = corner2 + normalize(SpherePos - corner2) * SphereRadius;
    else if (distance(SpherePos, corner3) < SphereRadius)
        deIntersectPos2 = corner3 + normalize(SpherePos - corner3) * SphereRadius;
    else if (distance(SpherePos, corner4) < SphereRadius)
        deIntersectPos2 = corner4 + normalize(SpherePos - corner4) * SphereRadius;

    float2 deIntersectPos = SpherePos;
    if (incross && InBox)
        deIntersectPos = deIntersectPos1;
    else if (insphere)
        deIntersectPos = deIntersectPos2;

    float2 r = deIntersectPos - SpherePos;
    r = float2(dot(r, InverseBoxRight), dot(r, InverseBoxUp));
    return r;
}


float2 CircleCircleDeIntersection(float2 Sphere1Pos, float Sphere1Radius, float2 Sphere2Pos, float Sphere2Radius)
{
    return normalize(Sphere1Pos - Sphere2Pos) * min(length(Sphere1Pos - Sphere2Pos) - Sphere1Radius - Sphere2Radius, 0);
}

float3 SphereSphereDeIntersection(float3 Sphere1Pos, float Sphere1Radius, float3 Sphere2Pos, float Sphere2Radius)
{
    return normalize(Sphere1Pos - Sphere2Pos) * min(length(Sphere1Pos - Sphere2Pos) - Sphere1Radius - Sphere2Radius, 0);
}
float3 SphereBoxDeIntersection(transform Box, transform Sphere)
{
    float SphereRadius = max(Sphere.scale);
    transform BigBox = Box;
    BigBox.scale = Box.scale + SphereRadius * 2;

    float3 SpherePos = WorldToLocalNoScale(Box, Sphere.position);// -Box.position; // local sphere position
    // TODO: rewrite this in terms of a distance function. It'd be way faster!
    bool InsideBigBox =
        SpherePos.x < BigBox.scale.x * 0.5 &&
        SpherePos.y < BigBox.scale.y * 0.5 &&
        SpherePos.z < BigBox.scale.z * 0.5 &&
        SpherePos.x > -BigBox.scale.x * 0.5 &&
        SpherePos.y > -BigBox.scale.y * 0.5 &&
        SpherePos.z > -BigBox.scale.z * 0.5;

    float3 boxScaledGradient = abs(SpherePos) - Box.scale * 0.5;
    float InsideCombinedShape = min(boxScaledGradient);

    float InsideCombinedShape2 = max(max(
        min(boxScaledGradient.x, boxScaledGradient.y),
        min(boxScaledGradient.x, boxScaledGradient.z)),
        min(boxScaledGradient.y, boxScaledGradient.z));

    float3 DeIntersectVectorLocal = float3(0, 0, 0);
    if (InsideCombinedShape > 0) // Inside a corner
    {
        //float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        if (SpherePos.x < 0 && SpherePos.y < 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y < 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y > 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y > 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y < 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y < 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y > 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y > 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, 1), 0, SpherePos, SphereRadius);
    }
    else if (InsideCombinedShape2 > 0) // Inside an edge
    {
        float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        if (SpherePosLocal.z < 1 && SpherePosLocal.z > -1)
        {
            if (SpherePosLocal.x > 1 && SpherePosLocal.y > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.y > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x > 1 && SpherePosLocal.y < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.y < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
        }
        else if (SpherePosLocal.y < 1 && SpherePosLocal.y > -1)
        {
            if (SpherePosLocal.x > 1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 0, 1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 0, 1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x > 1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 0, -1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 0, -1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
        }
        else if (SpherePosLocal.x < 1 && SpherePosLocal.x > -1)
        {
            if (SpherePosLocal.y > 1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, 1, 1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y < -1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, -1, 1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y > 1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, 1, -1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y < -1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, -1, -1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
        }
    }
    else if (InsideBigBox) // Inside a face
    {
        float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        float largest = max(SpherePosLocal);
        float smallest = min(SpherePosLocal);
        DeIntersectVectorLocal = SpherePos;
        if (abs(smallest) < largest)
        {
            if (SpherePosLocal.x == largest)
                DeIntersectVectorLocal.x = BigBox.scale.x * 0.5;
            else if (SpherePosLocal.y == largest)
                DeIntersectVectorLocal.y = BigBox.scale.y * 0.5;
            else if (SpherePosLocal.z == largest)
                DeIntersectVectorLocal.z = BigBox.scale.z * 0.5;
        }
        else
        {
            if (SpherePosLocal.x == smallest)
                DeIntersectVectorLocal.x = -BigBox.scale.x * 0.5;
            else if (SpherePosLocal.y == smallest)
                DeIntersectVectorLocal.y = -BigBox.scale.y * 0.5;
            else if (SpherePosLocal.z == smallest)
                DeIntersectVectorLocal.z = -BigBox.scale.z * 0.5;
        }
        DeIntersectVectorLocal -= SpherePos;
    }
    else // Outisde of the box.
    {
    }

    return LocalToWorldVectorNoScale(Box, DeIntersectVectorLocal);
}
//my preferred datastructures are C array and void pointer



void UpdatePhysics(game_memory* Memory, EngineState* engineState, EngineSaveState* engineSaveState, game_offscreen_buffer* VideoBuffer)
{
    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* Body = &engineState->Entities[i];
        if (!ValidPhysics(Body))
            continue;

        Body->grounded = false;
        Body->TouchingEntity = 0;
        // De-intersect
        // Currently only spheres can be dynamic
        if (Body->type != EntityType_Sphere)
            continue;
        if (Body->kinematic)
            continue;
        Body->velocity += float3(0, 0, -0.005);
        Body->t.position += Body->velocity;

        //if (!isfinite(Body->t.position))
        //    Body->t.position = float3(0, 0, 0);

        Entity* HitBody = 0;
        Entity* HitDynamicBody = 0;

        float3 DeintersectVector = float3(0, 0, 0);
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < ArrayCount(engineState->Entities); k++)
            {
                Entity* OtherBody = &engineState->Entities[k];
                if (!ValidPhysics(OtherBody))
                    continue;
                if (OtherBody == Body)
                    continue;

                float3 DeIntersect = float3(0, 0, 0);
                if (OtherBody->type == EntityType_Box)
                    DeIntersect = SphereBoxDeIntersection(OtherBody->t, Body->t);
                else if (OtherBody->type == EntityType_Sphere)
                    DeIntersect = SphereSphereDeIntersection(OtherBody->t.position, OtherBody->t.scale.x, Body->t.position, Body->t.scale.y);

                DeintersectVector += DeIntersect;

                if (length(DeIntersect) > 0)
                {
                    HitBody = OtherBody;
                    //float MoveDistance = distance(engineState->LastSpherePos, engineState->SpherePos);
                    // TODO: If the other object is also dynamic, do something else here.
                    Body->t.position += DeIntersect;
                    if (!OtherBody->kinematic)
                        HitDynamicBody = OtherBody;
                }
            }
        }

        // Add force
        Body->deIntersectVector = DeintersectVector;
        float bouncyness = 0.0;// engineState->Bouncy ? 1 : 0;
        if (length(DeintersectVector) > 0)
        {
            Body->grounded = true;
            Body->TouchingEntity = HitBody;

            float3 StartVel = Body->velocity;
            float StartAngularVel = Body->angularVelocity;
            float3 NewVel = Body->velocity;
            float NewAngularVel = Body->angularVelocity;

            // When something hits something, angular velocity and velocity try to balance out. 
            // If it's spinning a lot, it starts moves
            // If it's moving a lot, it starts spinning
            float3 normalizedVelocity = normalize(Body->velocity);
            float3 normal = normalize(DeintersectVector);
            
            bool safe = true;

            if(dot(normal, normal) == 0)
                safe = false;

            if (normalizedVelocity == normal)
                safe = false;

            if (safe)
            {
                float3 tangentN = cross(normalizedVelocity, normal);
                if (dot(tangentN, tangentN) == 0)
                    safe = false;

                if (safe)
                {
                    float3 tangent = normalize(tangentN);
                    float3 binormal = normalize(cross(normal, tangent));
                    if(dot(binormal, binormal) == 0)
                        safe = false;

                    if (safe)
                    {
                        float3 BounceVel = reflect(NewVel, normal);
                        float3 StopVel = binormal * dot(binormal, NewVel);
                        NewVel = lerp(StopVel, BounceVel, bouncyness);

                        // Tangent is up, velocity is down, they cancen out to 0.
                        float3 thing = lerp(Body->angularVelocityAxis, tangent, 0.5f);

                        if (dot(thing, thing) == 0)
                            safe = false;

                        if (safe)
                        {
                            Body->angularVelocityAxis = normalize(thing); // move angular velocity a little bit to the hit angle
                            NewAngularVel += abs(dot(binormal, NewVel) / 100);
                            NewVel += binormal * NewAngularVel * 50;
                        }
                    }
                }
            }

            NewVel *= 0.5f;
            NewAngularVel *= 0.5f;
            //Body->velocity = NewVel;
            //Body->angularVelocity = NewAngularVel;


            float3 DeltaVel = (NewVel - StartVel);
            float DeltaAngularVel = (NewAngularVel - StartAngularVel);
            
            if (HitDynamicBody)
            {
                float MassRatio1 = Body->mass / HitDynamicBody->mass;
                float MassRatio2 = HitDynamicBody->mass / Body->mass;
            
                Body->velocity += DeltaVel * MassRatio2 * 0.5f;
                Body->angularVelocity += DeltaAngularVel * MassRatio2 * 0.5f;
            
                //HitDynamicBody->velocity -= DeltaVel * 0.75 * MassRatio1;
                //HitDynamicBody->angularVelocity -= DeltaAngularVel * 0.75 * MassRatio1;
                HitDynamicBody->velocity -= DeltaVel * MassRatio1 * 0.5f;
                HitDynamicBody->angularVelocity -= DeltaAngularVel * MassRatio1 * 0.5f;
            }
            else
            {
                Body->velocity += DeltaVel;
                Body->angularVelocity += DeltaAngularVel;
            }
        }
        //float drag = 0.015;
        //float friction = 0.015;
        float drag = 0.015;
        float friction = 0.015;
        if(Body->grounded)
            Body->velocity *= 1.0 - friction;
        //Body->velocity *= 1.0 - Body->friction;

        Body->velocity *= 1.0 - drag;
        //Body->velocity *= 1.0 - Body->drag;

        Body->t = rotate(Body->t, Body->angularVelocityAxis, -Body->angularVelocity * 200);

        DrawLine3D(VideoBuffer, engineState->Camera, Body->t.position, Body->t.position + Body->velocity);
    }
}