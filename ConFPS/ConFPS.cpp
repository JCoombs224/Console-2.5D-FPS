#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
using namespace std;

#include <Windows.h>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 16.0f;
float fPlayerY = 16.0f;
float fPlayerA = 0.0f;

int nMapHeight = 32;
int nMapWidth = 32;

float fFOV = 3.14159 / 4.0; // 45 deg
float fMaxDepth = 16.0f; // Max camera render distance

int main()
{
    // Create Screen Buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight]; // Use wchar because we are going to use UNICODE
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0; // Windows thing, isn't that useful but we need to have it anyway

    wstring map; // wstring for UNICODE

    map += L"################################";
    map += L"#..............................#";
    map += L"#...#######..........######....#";
    map += L"#..............................#";
    map += L"#.....#..................#.....#";
    map += L"#..............................#";
    map += L"#.....#..................#.....#";
    map += L"#..............................#";
    map += L"#.....#........................#";
    map += L"#..............................#";
    map += L"#############.........##########";
    map += L"#...........#.........#........#";
    map += L"#..............................#";
    map += L"#..............................#";
    map += L"#...........#..................#";
    map += L"#..............................#";
    map += L"#...#######..........######....#";
    map += L"#..............................#";
    map += L"#.....#..................#.....#";
    map += L"#..............................#";
    map += L"#.....#..................#.....#";
    map += L"#..............................#";
    map += L"#.....#........................#";
    map += L"#..............................#";
    map += L"#############.........##########";
    map += L"#...........#.........#........#";
    map += L"#..............................#";
    map += L"#..............................#";
    map += L"#...........#..................#";
    map += L"#...........#..................#";
    map += L"#...........#..................#";
    map += L"################################";

    
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    // Game Loop
    while (true)
    {
        // Get Delta Time
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fDeltaTime = elapsedTime.count();

        bool bShowMap = false;

        // Controls
        // Handle CCW Rotation
        if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
            fPlayerA -= (0.8f) * fDeltaTime;

        if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
            fPlayerA += (0.8f) * fDeltaTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fDeltaTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fDeltaTime;

            // Collision Detection
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fDeltaTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fDeltaTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            fPlayerX -= cosf(fPlayerA) * 5.0f * fDeltaTime;
            fPlayerY += sinf(fPlayerA) * 5.0f * fDeltaTime;

            // Collision Detection
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += cosf(fPlayerA) * 5.0f * fDeltaTime;
                fPlayerY -= sinf(fPlayerA) * 5.0f * fDeltaTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            fPlayerX += cosf(fPlayerA) * 5.0f * fDeltaTime;
            fPlayerY -= sinf(fPlayerA) * 5.0f * fDeltaTime;

            // Collision Detection
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= cosf(fPlayerA) * 5.0f * fDeltaTime;
                fPlayerY += sinf(fPlayerA) * 5.0f * fDeltaTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fDeltaTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fDeltaTime;

            // Collision Detection
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * 5.0f * fDeltaTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fDeltaTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'M') & 0x8000)
        {
            bShowMap = true;
        }

        // loop through each column on the screen itself
        for (int x = 0; x < nScreenWidth; x++)
        {
            // Porter Algorithm? (check this)
            // For each column, calculate the projected ray angle into world space
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
            float fEyeY = cosf(fRayAngle);

            // Raycast
            while (!bHitWall && fDistanceToWall < fMaxDepth)
            {
                fDistanceToWall += 0.1f;

                // From the unit vector, create a line of a given distance. Since walls will be on integer boundaries (anything from x to x+1 will still be a wall) we can just use an int
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fMaxDepth;
                }
                else
                {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                        vector<pair<float, float>> p; // distance, dot
                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy); // calculate vector magnitude
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }

                            // Sort Pairs from closest to farthest
                            sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });

                            float fBound = 0.006; // radians
                            if (acos(p.at(0).second) < fBound) bBoundary = true;
                            if (acos(p.at(1).second) < fBound) bBoundary = true;
                            //if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            // Calculate distance to ceiling and floor
            int nCeiling = (float)(nScreenHeight / 2.0f) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            // Use unicode shade characters to represent the shading
            short nShade = ' ';

            if (fDistanceToWall <= fMaxDepth / 4.0f)       nShade = 0x2588;    // Very close
            else if (fDistanceToWall <= fMaxDepth / 3.0f)  nShade = 0x2593;
            else if (fDistanceToWall <= fMaxDepth / 2.0f)  nShade = 0x2592;
            else if (fDistanceToWall <= fMaxDepth)         nShade = 0x2591;
            else                                        nShade = ' ';       // Too far away - blank space

            if (bBoundary)   nShade = ' ';

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling)
                    screen[y * nScreenWidth + x] = ' '; // shade ceiling
                else if(y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade; // shade wall
                else
                {
                    short nFloorShade = ' ';
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)       nFloorShade = '#';
                    else if (b < 0.5)   nFloorShade = 'x';
                    else if (b < 0.75)  nFloorShade = '.';
                    else if (b < 0.9)   nFloorShade = '-';
                    else                nFloorShade = ' ';
                    screen[y * nScreenWidth + x] = nFloorShade;
                }
            }
        }

        // Display Stats
        swprintf_s(screen, 44, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fDeltaTime);

        // Display Map
        if (bShowMap)
        {
            for (int nx = 0; nx < nMapWidth; nx++)
                for (int ny = 0; ny < nMapWidth; ny++)
                {
                    screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
                }

            float fViewX = fPlayerX + sinf(fPlayerA);
            float fViewY = fPlayerY + cosf(fPlayerA);

            short nAngleSymbol = ' ';

            // Determine which triangle to represent for player view angle on map
            /*if (fViewY == fPlayerY && fViewX < fPlayerX)        nAngleSymbol = 0x23F4;
            else if (fViewY == fPlayerY && fViewX > fPlayerX)   nAngleSymbol = 0x23F5;
            else if (fViewY < fPlayerY && fViewX == fPlayerX)   nAngleSymbol = 0x23F6;
            else if (fViewY > fPlayerY && fViewX == fPlayerX)   nAngleSymbol = 0x23F7;*/

            screen[((int)fViewY + 1) * nScreenWidth + (int)fViewX] = '@';
            screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';
        }

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    return 0;
}


