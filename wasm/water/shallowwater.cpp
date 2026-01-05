#pragma once
#include "quote.h"

#define sizeX 128
#define sizeY 128

struct shallowwaterScene
{
    float h[sizeX*sizeY]; // height
    float qx[sizeX*sizeY]; // momentum X
    float qy[sizeX*sizeY]; // momentum Y

    float h1[sizeX*sizeY]; // height
    float qx1[sizeX*sizeY]; // momentum X
    float qy1[sizeX*sizeY]; // momentum Y

    float d[sizeX*sizeY]; // bed height

    float perspective;
    float angle;
    float waterRes;
    float seabedRes;
    bool playing;

    char outputText[Megabytes(4)];
};

float2 getPos(float angle, float perspective, float3 in)
{
    float2 up = float2(0, 1)*0.5;
    float2 right = float2(-1*0.5, 1*perspective)*2;
    float2 left = float2(1*0.5, 1*perspective)*2;
    in.x -= 0.5;
    in.y -= 0.5;
    in = RotateAroundAxis(in, float3(0,0,1), angle*3.14152128);
    in.x += 0.5;
    in.y += 0.5;

    return ((right * in.x + left * in.y + up * in.z) + float2(0, -perspective * 2));
}

void shallowwater(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{
    shallowwaterScene* scene = (shallowwaterScene*)(((uint8*)(engineState + 1)));

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = memory->platformSetForcefeedbackEffect;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)memory->transientStorage + sizeof(EngineState) + sizeof(shallowwaterScene) + Kilobytes(100));
        
        ArrayClear(engineState->images);
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);
        

        scene->perspective = 0.25;
        scene->waterRes = 64;
        scene->seabedRes = 16;
        scene->playing = true;
    }

    float2 mousePos = float2(input->MouseX, input->MouseY);

    DrawClear(videoBuffer, float3(0.5, 0, 0));

    DrawCircle(videoBuffer, mousePos, 5.0f);

    if (!memory->initialized)
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;

                scene->d[i] = (sin((float)x / sizeX*16) + sin((float)y / sizeX*16))*0.1+0.1;

                scene->h[i] = 0.05;
                if(x > sizeX/2)
                    scene->h[i] = 0.5;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    float total_mass = 0;
    float total_momentumX = 0;
    float total_momentumY = 0;
    //memory->platformWriteFile("lines.txt",
#if 1
    // gravity
    float g = 1.0f;
    float dt = 0.5f;
    float dx = 1.0f;
    float dy = 1.0f;
    float drag = 0.0f;

    dt = 0.25;

    // simulation step
    if(scene->playing)
    {
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int iLeft = x-1;
                int iRight = x+1;
                int iUp = y-1;
                int iDown = y+1;
        
                // wrap around
                //iLeft = mod(iLeft, sizeX);
                //iRight = mod(iRight, sizeX);
                //iUp = mod(iUp, sizeY);
                //iDown = mod(iDown, sizeY);

                bool reflectLeft = iLeft < 0 || iLeft > sizeX-1;
                bool reflectRight = iRight < 0 || iRight > sizeX-1;
                bool reflectUp = iUp < 0 || iUp > sizeY-1;
                bool reflectDown = iDown < 0 || iDown > sizeY-1;
                if(iLeft < 0) iLeft = 0;
                if(iLeft > sizeX-1) iLeft = sizeX-1;
                if(iRight < 0) iRight = 0;
                if(iRight > sizeX-1) iRight = sizeX-1;
                if(iUp < 0) iUp = 0;
                if(iUp > sizeY-1) iUp = sizeY-1;
                if(iDown < 0) iDown = 0;
                if(iDown > sizeY-1) iDown = sizeY-1;
        
                // reflect
                //y*sizeX+x
            
                float h_C = scene->h[y*sizeX+x];
                float q_Cx = scene->qx[y*sizeX+x];
                float q_Cy = scene->qy[y*sizeX+x];

                float q_L = scene->qx[y*sizeX+iLeft];
                float h_L = scene->h[y*sizeX+iLeft];

                float q_R = scene->qx[y*sizeX+iRight];
                float h_R = scene->h[y*sizeX+iRight];

                float q_U = scene->qy[iUp*sizeX+x];
                float h_U = scene->h[iUp*sizeX+x];

                float q_D = scene->qy[iDown*sizeX+x];
                float h_D = scene->h[iDown*sizeX+x];
            
                float qy_L = scene->qy[y*sizeX+iLeft];
                float qy_R = scene->qy[y*sizeX+iRight];
                float qx_U = scene->qx[iUp*sizeX+x];
                float qx_D = scene->qx[iDown*sizeX+x];

                // flip momentum at the boundry causes waves to bounce
                if(reflectLeft) q_L *= -1;
                if(reflectRight) q_R *= -1;
                if(reflectUp) q_U *= -1;
                if(reflectDown) q_D *= -1;
                
                //if (reflectLeft)  { q_L  = -q_Cx; h_L = h_C; qy_L = q_Cy; }
                //if (reflectRight) { q_R  = -q_Cx; h_R = h_C; qy_R = q_Cy; }
                //if (reflectUp)    { q_U  = -q_Cy; h_U = h_C; qx_U = q_Cx; }
                //if (reflectDown)  { q_D  = -q_Cy; h_D = h_C; qx_D = q_Cx; }

                float hmin = 1e-6f;
                h_L = max(h_L, hmin);
                h_R = max(h_R, hmin);
                h_U = max(h_U, hmin);
                h_D = max(h_D, hmin);
                h_C = max(h_C, hmin);
            
                float Fh_Cx = q_Cx; // height flux center x
                float Fq_Cx = (q_Cx*q_Cx) / h_C + 0.5 * g * (h_C*h_C); // momentum flux center x

                float Fh_Cy = q_Cy; // height flux center y
                float Fq_Cy = (q_Cy*q_Cy) / h_C + 0.5 * g * (h_C*h_C); // momentum flux center y

                float Fh_L = q_L; // height flux left
                float Fq_L = (q_L*q_L) / h_L + 0.5 * g * (h_L*h_L); // momentum flux left
        
                float Fh_R = q_R; // height flux right
                float Fq_R = (q_R*q_R) / h_R + 0.5 * g * (h_R*h_R); // momentum flux right
            
                float Fh_U = q_U; // height flux up
                float Fq_U = (q_U*q_U) / h_U + 0.5 * g * (h_U*h_U); // momentum flux up
        
                float Fh_D = q_D; // height flux down
                float Fq_D = (q_D*q_D) / h_D + 0.5 * g * (h_D*h_D); // momentum flux down
            
                float Fqy_Cx = (q_Cx*q_Cy) / h_C;
                float Fqy_L = (q_L*qy_L) / h_L;
                float Fqy_R = (q_R*qy_R) / h_R;

                float Fqx_Cy = (q_Cx*q_Cy) / h_C;
                float Fqx_U = (qx_U*q_U) / h_U;
                float Fqx_D = (qx_D*q_D) / h_D;

                // momentum correction that makes waves not travel "faster than light"
                float aR = max(abs(q_R/h_R) + sqrt(g*h_R), abs(q_Cx/h_C) + sqrt(g*h_C));
                float aL = max(abs(q_L/h_L) + sqrt(g*h_L), abs(q_Cx/h_C) + sqrt(g*h_C));
                float aU = max(abs(q_U/h_U) + sqrt(g*h_U), abs(q_Cy/h_C) + sqrt(g*h_C));
                float aD = max(abs(q_D/h_D) + sqrt(g*h_D), abs(q_Cy/h_C) + sqrt(g*h_C));

                
                //aR *= 1.0f + 2.0f * min(abs(h_R - h_L) / max(min(h_R, h_L), 1e-6f), 1.0f);
                //aL *= 1.0f + 2.0f * min(abs(h_L - h_R) / max(min(h_L, h_R), 1e-6f), 1.0f);
                //aU *= 1.0f + 2.0f * min(abs(h_U - h_D) / max(min(h_U, h_D), 1e-6f), 1.0f);
                //aD *= 1.0f + 2.0f * min(abs(h_D - h_U) / max(min(h_D, h_U), 1e-6f), 1.0f);
                
                // Find average flux "going in vs going out", with momentum correction added.
                float Fh_wall_L = 0.5*(Fh_L + Fh_Cx) - 0.5*aL*(h_C - h_L);
                float Fq_wall_L = 0.5*(Fq_L + Fq_Cx) - 0.5*aL*(q_Cx - q_L);

                float Fh_wall_R = 0.5*(Fh_Cx + Fh_R) - 0.5*aR*(h_R - h_C);
                float Fq_wall_R = 0.5*(Fq_Cx + Fq_R) - 0.5*aR*(q_R - q_Cx);

                float Fh_wall_U = 0.5*(Fh_U + Fh_Cy) - 0.5*aU*(h_C - h_U);
                float Fq_wall_U = 0.5*(Fq_U + Fq_Cy) - 0.5*aU*(q_Cy - q_U);

                float Fh_wall_D = 0.5*(Fh_Cy + Fh_D) - 0.5*aD*(h_D - h_C);
                float Fq_wall_D = 0.5*(Fq_Cy + Fq_D) - 0.5*aD*(q_D - q_Cy);
        
                float Fqy_wall_L = 0.5*(Fqy_L + Fqy_Cx) - 0.5*aL*(q_Cy - qy_L);
                float Fqy_wall_R = 0.5*(Fqy_Cx + Fqy_R) - 0.5*aR*(qy_R - q_Cy);

                float Fqx_wall_U = 0.5*(Fqx_U + Fqx_Cy) - 0.5*aU*(q_Cx - qx_U);
                float Fqx_wall_D = 0.5*(Fqx_Cy + Fqx_D) - 0.5*aD*(qx_D - q_Cx);

                float bL = scene->d[y*sizeX+iLeft];
                float bR = scene->d[y*sizeX+iRight];
                float bU = scene->d[iUp*sizeX+x];
                float bD = scene->d[iDown*sizeX+x];
                float bx = (bR - bL) / (2.0f*dx);
                float by = (bD - bU) / (2.0f*dy);
                float sourceX = -g * h_C * bx - drag * q_Cx;
                float sourceY = -g * h_C * by - drag * q_Cy;

                scene->qx1[y*sizeX+x] = q_Cx - dt * ((Fq_wall_R - Fq_wall_L)/dx + (Fqx_wall_D - Fqx_wall_U)/dy) + dt * sourceX;
                scene->qy1[y*sizeX+x] = q_Cy - dt * ((Fqy_wall_R - Fqy_wall_L)/dx + (Fq_wall_D - Fq_wall_U)/dy) + dt * sourceY;
                scene->h1[y*sizeX+x] = h_C - dt * ((Fh_wall_R - Fh_wall_L)/dx + (Fh_wall_D - Fh_wall_U)/dy);
                
                //float2 s = float2(scene->qx1[y*sizeX+x], scene->qy1[y*sizeX+x]);
                //float l = length(s);
                //if(l > 0)
                //{
                //    float alpha = 0.5;
                //    s /= l;
                //    l = min(l, dt*alpha);
                //    s *= l;
                //}
                //scene->qx1[y*sizeX+x] = s.x;
                //scene->qy1[y*sizeX+x] = s.y;
                
                if (scene->h1[y*sizeX+x] < hmin) {
                    scene->h1[y*sizeX+x] = hmin;
                    scene->qx1[y*sizeX+x] = 0.0f;
                    scene->qy1[y*sizeX+x] = 0.0f;
                }
            }
        }

        for (int i = 0; i < sizeX*sizeY; i++)
        {
            scene->h[i] = scene->h1[i];
            scene->qx[i] = scene->qx1[i];
            scene->qy[i] = scene->qy1[i];
        }
    }
    
    for (int i = 0; i < sizeX*sizeY; i++)
    {
        total_mass += scene->h[i];
        total_momentumX += scene->qx[i];
        total_momentumY += scene->qy[i];
    }

#endif
    
    float2 pos = float2(25, 25);
    float scale = videoBuffer->Height*0.5;
    int outputHalfResX = 130/2;
    int outputHalfResY = 100/2;
    int outputHalfResMin = min(outputHalfResX, outputHalfResY);
    
    char text[512] = {};
    Clear(text, 512);
    Append(text, "Perspective: ");
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, "torvid.net/wasm/water");
    pos += float2(0,25);

    if(Button(engineState, input, videoBuffer, pos, scene->playing ? "Pause" : "Play"))
    {
        scene->playing = ! scene->playing;
    }
    pos += float2(0,25);
    
    pos += float2(0,20);
    

    Clear(text, 512);
    Append(text, "Perspective: ");
    Append(text, scene->perspective);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,8);
    scene->perspective = Slider(engineState, input, videoBuffer, pos, scene->perspective,0, 0.5, 256);
    pos += float2(0,25);
    Clear(text, 512);
    Append(text, "Angle: ");
    Append(text, scene->angle);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,8);
    scene->angle = Slider(engineState, input, videoBuffer, pos, scene->angle,-0.5, 0.5, 256);
    pos += float2(0,25);
    Clear(text, 512);
    Append(text, "Water Res: ");
    Append(text, scene->waterRes);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,8);
    scene->waterRes = (int)Slider(engineState, input, videoBuffer, pos, scene->waterRes, 1, 128, 256);
    pos += float2(0,25);
    Clear(text, 512);
    Append(text, "Seabed Res: ");
    Append(text, scene->seabedRes);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,8);
    scene->seabedRes = (int)Slider(engineState, input, videoBuffer, pos, scene->seabedRes, 1, 128, 256);
    pos += float2(0,25);
    
    pos += float2(0,20);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, "Starting Conditions:");
    pos += float2(0,15);
    if(Button(engineState, input, videoBuffer, pos, "Dam Break 1"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;

                scene->d[i] = (sin((float)x / sizeX*16) + sin((float)y / sizeX*16))*0.1+0.1;

                scene->h[i] = 0.05;
                if(x < sizeX/2)
                    scene->h[i] = 0.5;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    pos += float2(0,20);
    if(Button(engineState, input, videoBuffer, pos, "Dam Break 2"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;

                scene->d[i] = 0;

                scene->h[i] = 0.05;
                if(y < sizeX/2)
                    scene->h[i] = 0.5;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    pos += float2(0,20);
    
    if(Button(engineState, input, videoBuffer, pos, "Empty"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;
                float tx = -(x / (float)sizeX)*2-1;
                float ty = (y / (float)sizeY)*2-1;
                
                scene->d[i] = (0.4+(sin(tx*5)+sin(ty*5)+tx*2+sin(tx*ty*7)+ty*2)*0.1)*0.1;

                scene->h[i] = 0.00;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    pos += float2(0,20);

    if(Button(engineState, input, videoBuffer, pos, "Pooling"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;
                
                scene->d[i] = (sin((float)x / sizeX*16) + sin((float)y / sizeX*16))*0.1+0.1;

                scene->h[i] = 0.02;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    pos += float2(0,20);
    if(Button(engineState, input, videoBuffer, pos, "Slope"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                int i = y*sizeX+x;
                
                scene->d[i] = max(0, (x-sizeX/2)*0.1);

                scene->h[i] = 0.02;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }
    pos += float2(0,20);
    if(Button(engineState, input, videoBuffer, pos, "Stream"))
    {
        // set water height
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                float tx = -(x / (float)sizeX)*2-1;
                float ty = (y / (float)sizeY)*2-1;

                int i = y*sizeX+x;
                
                scene->d[i] = 0.4+(sin(tx*5)+sin(ty*5)+tx*2+sin(tx*ty*7)+ty*2)*0.1;

                scene->h[i] = 0.02;
                scene->qx[i] = 0.0;
                scene->qy[i] = 0.0;
            }
        }
    }

    pos += float2(0,50);
    bool exportClicked = false;//Button(engineState, input, videoBuffer, pos, "Export to \"lines.txt\"");
    

    
    int counter = 0;
    if(exportClicked)
    {
        Clear(scene->outputText, Megabytes(4));
        
        counter += 1;
        Append(scene->outputText, counter, Megabytes(4));
            Append(scene->outputText, " REM ", Megabytes(4));
            Append(scene->outputText, "\n", Megabytes(4));
                    
        counter += 1;
        Append(scene->outputText, counter, Megabytes(4));
            Append(scene->outputText, " INIT ", Megabytes(4));
            Append(scene->outputText, "\n", Megabytes(4));
    }

    pos += float2(0,25);
    
    float2 smallest = float2(99999, 99999);
    float2 largest = float2(-99999, -99999);

    float2 gridpos = float2(videoBuffer->Width/2+100, videoBuffer->Height/2+50);
    int q = scene->seabedRes;
    for (int X = 1; X < q; X++)
    {
        for (int Y = 1; Y < q; Y++)
        {
            float x0 = (X-1) / ((float)q-1);
            float y0 = (Y-1) / ((float)q-1);
            float x = (X) / ((float)q-1);
            float y = (Y) / ((float)q-1);
            int rx0 = x0 * (sizeX-1);
            int ry0 = y0 * (sizeX-1);
            int rx = x * (sizeX-1);
            int ry = y * (sizeX-1);
            int i0 = (ry)*sizeX+((rx));
            int i1 = (ry0)*sizeX+((rx));
            int i2 = (ry)*sizeX+((rx0));
            int i3 = (ry0)*sizeX+((rx0));
            
            float2 pos0 = getPos(scene->angle, scene->perspective, float3(x, y, scene->d[i0]));
            float2 pos1 = getPos(scene->angle, scene->perspective, float3(x, (y0), scene->d[i1]));
            //float2 pos2 = getPos(scene->angle, float3((x0), y, scene->d[i2]));
            float2 pos3 = getPos(scene->angle, scene->perspective, float3((x0), (y0), scene->d[i3]));
            smallest = min(min(min(smallest, pos0), pos1), pos3);
            largest = max(max(max(largest, pos0), pos1), pos3);


            DrawLine(videoBuffer, gridpos + scale*pos0 * float2(1,-1), gridpos + scale*pos1 * float2(1,-1), float3(0.5, 0.5, 0.5));
            DrawLine(videoBuffer, gridpos + scale*pos1 * float2(1,-1), gridpos + scale*pos3 * float2(1,-1), float3(0.5, 0.5, 0.5));
            //DrawLine(videoBuffer, pos3, pos2, float3(0.5, 0.5, 0.5));
            //DrawLine(videoBuffer, pos2, pos0, float3(0.5, 0.5, 0.5));
            
            float2 Pos0 = float2(pos0.x * outputHalfResMin + outputHalfResX, pos0.y * outputHalfResMin + outputHalfResY);
            float2 Pos1 = float2(pos1.x * outputHalfResMin + outputHalfResX, pos1.y * outputHalfResMin + outputHalfResY);
            float2 Pos3 = float2(pos3.x * outputHalfResMin + outputHalfResX, pos3.y * outputHalfResMin + outputHalfResY);

            if(exportClicked)
            {
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                Append(scene->outputText, " MOV ", Megabytes(4));
                Append(scene->outputText, (int)Pos0.x, Megabytes(4));
                Append(scene->outputText, ", ", Megabytes(4));
                Append(scene->outputText, (int)Pos0.y, Megabytes(4));
                Append(scene->outputText, "\n", Megabytes(4));
            
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                Append(scene->outputText, " DRA ", Megabytes(4));
                Append(scene->outputText, (int)Pos1.x, Megabytes(4));
                Append(scene->outputText, ", ", Megabytes(4));
                Append(scene->outputText, (int)Pos1.y, Megabytes(4));
                Append(scene->outputText, "\n", Megabytes(4));
            
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                Append(scene->outputText, " DRA ", Megabytes(4));
                Append(scene->outputText, (int)Pos3.x, Megabytes(4));
                Append(scene->outputText, ", ", Megabytes(4));
                Append(scene->outputText, (int)Pos3.y, Megabytes(4));
                Append(scene->outputText, "\n", Megabytes(4));
            }
        }
    }
    
    q = scene->waterRes;

    for (int X = 1; X < q; X++)
    {
        for (int Y = 1; Y < q; Y++)
        {
            float x0 = (X-1) / ((float)q-1);
            float y0 = (Y-1) / ((float)q-1);
            float x =  (X) / ((float)q-1);
            float y =  (Y) / ((float)q-1);
            int rx0 = x0 * (sizeX-1);
            int ry0 = y0 * (sizeX-1);
            int rx  = x  * (sizeX-1);
            int ry  = y  * (sizeX-1);
            int i0 = ((ry)  )*sizeX+((rx));
            int i1 = ((ry0))*sizeX+((rx));
            int i2 = ((ry)  )*sizeX+((rx0));
            int i3 = ((ry0))*sizeX+((rx0));
            
            float2 pos0 = getPos(scene->angle, scene->perspective, float3(x, y, scene->d[i0]+scene->h[i0]));
            float2 pos1 = getPos(scene->angle, scene->perspective, float3(x, (y0), scene->d[i1]+scene->h[i1]));
            //float2 pos2 = getPos(scene->angle, scale*float3((x0), y, scene->d[i2]+scene->h[i2]));
            float2 pos3 = getPos(scene->angle, scene->perspective, float3((x0), (y0), scene->d[i3]+scene->h[i3]));
            smallest = min(min(min(smallest, pos0), pos1), pos3);
            largest = max(max(max(largest, pos0), pos1), pos3);
            
            float filter = 1e-2f;
            bool drawLine0 = (scene->h[i0] > filter || scene->h[i1] > filter);
            bool drawLine1 = (scene->h[i1] > filter || scene->h[i2] > filter);

            if(drawLine0)
                DrawLine(videoBuffer, gridpos + scale*pos0 * float2(1,-1), gridpos + scale*pos1 * float2(1,-1), float3(0.7,0.7,1));
            if(drawLine1)
                DrawLine(videoBuffer, gridpos + scale*pos1 * float2(1,-1), gridpos + scale*pos3 * float2(1,-1), float3(0.7,0.7,1));
            //if(scene->h[i2] > filter || scene->h[i3] > filter)
            //    DrawLine(videoBuffer, pos3, pos2, float3(0.7,0.7,1));
            //if(scene->h[i3] > filter || scene->h[i0] > filter)
            //    DrawLine(videoBuffer, pos2, pos0, float3(0.7,0.7,1));
            
            float2 Pos0 = float2(pos0.x * outputHalfResY + outputHalfResX, pos0.y * outputHalfResY + outputHalfResY);
            float2 Pos1 = float2(pos1.x * outputHalfResY + outputHalfResX, pos1.y * outputHalfResY + outputHalfResY);
            float2 Pos3 = float2(pos3.x * outputHalfResY + outputHalfResX, pos3.y * outputHalfResY + outputHalfResY);

            if(exportClicked)
            {
                if(drawLine0 && drawLine1)
                {
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " MOV ", Megabytes(4));
                    Append(scene->outputText, (int)Pos0.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos0.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
                    
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " DRA ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
            
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " DRA ", Megabytes(4));
                    Append(scene->outputText, (int)Pos3.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos3.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
                }
                else if(drawLine0)
                {
                    Append(scene->outputText, " MOV ", Megabytes(4));
                    Append(scene->outputText, (int)Pos0.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos0.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
                    
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " DRA ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
                }
                else if(drawLine1)
                {
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " MOV ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos1.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
            
                counter += 1;
                Append(scene->outputText, counter, Megabytes(4));
                    Append(scene->outputText, " DRA ", Megabytes(4));
                    Append(scene->outputText, (int)Pos3.x, Megabytes(4));
                    Append(scene->outputText, ", ", Megabytes(4));
                    Append(scene->outputText, (int)Pos3.y, Megabytes(4));
                    Append(scene->outputText, "\n", Megabytes(4));
                }
            }
        }
    }
    
    float2 mouse = float2(input->MouseX, input->MouseY) / float2(videoBuffer->Width, videoBuffer->Height);
    mouse = (mouse -0.5)*3+0.5;
    float succ = 0;
    if(input->MouseLeft) succ += 1;
    if(input->MouseRight) succ -= 1;

    if(succ != 0 && input->MouseX > 350)
    {
        for (int x = 0; x < sizeX; x++)
        {
            for (int y = 0; y < sizeY; y++)
            {
                float2 poss = float2(x, y) / float2(sizeX, sizeY);
                if(distance(poss, mouse) < 0.1)
                {
                    scene->h[y*sizeX+x] += 0.01*succ;
                    scene->h1[y*sizeX+x] += 0.01*succ;
                    scene->qx[y*sizeX+x] = 0; // momentum X
                    scene->qy[y*sizeX+x] = 0; // momentum Y

                    scene->qx1[y*sizeX+x] = 0; // momentum X
                    scene->qy1[y*sizeX+x] = 0; // momentum Y

                    float hmin = 1e-6f;
                    scene->h[y*sizeX+x] = max(scene->h[y*sizeX+x], hmin);
                    scene->h1[y*sizeX+x] = max(scene->h1[y*sizeX+x], hmin);
                }
            }
        }
    }

    Clear(text, 512);
    Append(text, "min: ");
    Append(text, smallest);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,15);
    
    Clear(text, 512);
    Append(text, "max: ");
    Append(text, largest);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,15);
    
    Clear(text, 512);
    Append(text, "total_mass: ");
    Append(text, total_mass, 100, 1);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,15);
    
    Clear(text, 512);
    Append(text, "total_momentum: ");
    Append(text, total_momentumX, 100, 1);
    Append(text, ", ");
    Append(text, total_momentumY, 100, 1);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,15);
    
    Clear(text, 512);
    Append(text, "fps: ");
    Append(text, 1.0f / input->deltaTime);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y, text);
    pos += float2(0,15);

    
    if(exportClicked)
    {
        memory->platformWriteFile("lines.txt", scene->outputText, StringLength(scene->outputText));
    }


    












#if 0
    if (!memory->initialized)
    {
        // set water height
        for (int i = 0; i < sizeX; i++)
        {
            scene->d[i] = sin((float)i * 0.1)*0.1+0.1;

            scene->h[i] = 0.05;
            if(i > 256)
                scene->h[i] = 0.4;
            scene->q[i] = 0.0;
        }
    }

    // gravity
    float g = 1.0f;
    float dt = 0.5f;
    float dx = 1.0f;
    float drag = 0.001f;

    // simulation step
    for (int i = 0; i < sizeX; i++)
    {
        int iLeft = i-1;
        int iRight = i+1;
        
        // wrap around
        iLeft = mod(iLeft, sizeX);
        iRight = mod(iRight, sizeX);

        bool reflectLeft = iLeft < 0 || iLeft > sizeX-1;
        bool reflectRight = iRight < 0 || iRight > sizeX-1;
        if(iLeft < 0) iLeft = 0;
        if(iLeft > sizeX-1) iLeft = sizeX-1;
        if(iRight < 0) iRight = 0;
        if(iRight > sizeX-1) iRight = sizeX-1;
        
        // reflect
        float q_L = scene->q[iLeft];
        float h_L = scene->h[iLeft];

        float q_C = scene->q[i];
        float h_C = scene->h[i];

        float q_R = scene->q[iRight];
        float h_R = scene->h[iRight];

        // flip momentum at the boundry causes waves to bounce
        if(reflectLeft)
            q_L *= -1;
        if(reflectRight)
            q_R *= -1;

        float hmin = 1e-6f;
        h_L = max(h_L, hmin);
        h_C = max(h_C, hmin);
        h_R = max(h_R, hmin);

        float Fh_L = q_L; // height flux left
        float Fq_L = (q_L*q_L) / h_L + 0.5 * g * (h_L*h_L); // momentum flux left
        
        float Fh_C = q_C; // height flux left
        float Fq_C = (q_C*q_C) / h_C + 0.5 * g * (h_C*h_C); // momentum flux left

        float Fh_R = q_R; // height flux left
        float Fq_R = (q_R*q_R) / h_R + 0.5 * g * (h_R*h_R); // momentum flux left

        float aR = max(abs(q_R/h_R) + sqrt(g*h_R), abs(q_C/h_C) + sqrt(g*h_C));
        float aL = max(abs(q_L/h_L) + sqrt(g*h_L), abs(q_C/h_C) + sqrt(g*h_C));

        float Fh_wall_L = 0.5*(Fh_L + Fh_C) - 0.5*aL*(h_C - h_L);
        float Fq_wall_L = 0.5*(Fq_L + Fq_C) - 0.5*aL*(q_C - q_L);

        float Fh_wall_R = 0.5*(Fh_C + Fh_R) - 0.5*aR*(h_R - h_C);
        float Fq_wall_R = 0.5*(Fq_C + Fq_R) - 0.5*aR*(q_R - q_C);
        
        float bed = scene->d[i];

        float bL = scene->d[iLeft];
        float bR = scene->d[iRight];
        float bx = (bR - bL) / (2.0f*dx);
        float source = -g * h_C * bx - drag * q_C;

        // viscosity
        //float nu_h = 0.0f;
        //float nu_q = 0.0f;
        //
        //float lap_h = (h_L - 2.0f*h_C + h_R);
        //float lap_q = (q_L - 2.0f*q_C + q_R);
        //
        //float diff_h = nu_h * dt/(dx*dx) * lap_h;
        //float diff_q = nu_q * dt/(dx*dx) * lap_q;

        scene->q1[i] = q_C - (dt/dx) * (Fq_wall_R - Fq_wall_L) + dt * source;
        scene->h1[i] = h_C - (dt/dx) * (Fh_wall_R - Fh_wall_L);
    }
    
    float total_mass = 0;
    float total_momentum = 0;
    for (int i = 0; i < sizeX; i++)
    {
        total_mass += scene->h[i];
        total_momentum += scene->q[i];
    }

#if 0

    float alpha = 0.5f;
    for (int i = 0; i < sizeX; i++)
    {
        int iL = mod(i-1, sizeX);
        int iR = mod(i+1, sizeX);
    
        float h = scene->h1[i];
        float q = scene->q1[i];
    
        float h_blur = h + alpha * (scene->h1[iL] - 2.0f*h + scene->h1[iR]);
        float q_blur = q + alpha * (scene->q1[iL] - 2.0f*q + scene->q1[iR]);
    
        scene->h[i] = h_blur;
        scene->q[i] = q_blur;
    }

#else
    
    for (int i = 0; i < sizeX; i++)
    {
        scene->h[i] = scene->h1[i];
        scene->q[i] = scene->q1[i];
    }

#endif

    float2 pos = float2(200, 400);
    float scale = 200.0f;

    char text[512] = {};
    Append(text, "total_mass: ");
    Append(text, total_mass);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y+5, text);

    Clear(text, 512);
    Append(text, "total_momentum: ");
    Append(text, total_momentum);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y+12+5, text);

    Clear(text, 512);
    Append(text, "fps: ");
    Append(text, 1.0f / input->deltaTime);
    DrawFont(engineState->fontSpritesheet, videoBuffer, pos.x, pos.y+24+5, text);

    // draw
    for (int i = 0; i < sizeX; i++)
    {
        DrawLine(videoBuffer, 
            pos+float2(i,   -(scene->h[i]   + scene->d[i]) * scale), 
            pos+float2(i-1, 0), float3(0,0,1));

        DrawLine(videoBuffer, 
            pos+float2(i,   -(scene->d[i]) * scale), 
            pos+float2(i-1, 0), float3(1.0,0.5,0.5));

    }
    for (int i = 1; i < sizeX; i++)
    {
        DrawLine(videoBuffer, 
            pos+float2(i,   -(scene->h[i]   + scene->d[i]) * scale), 
            pos+float2(i-1, -(scene->h[i-1] + scene->d[i-1]) * scale));
        //DrawLine(videoBuffer, 
        //    pos+float2(i,   -(scene->d[i]) * scale), 
        //    pos+float2(i-1, -(scene->d[i-1]) * scale));
    }


    DrawLine(videoBuffer, pos, pos+float2(sizeX, 0));

#endif
        memory->initialized = true;
}