//
// Created by USER on 05/12/2020.
//

#ifndef ANDROID_PROJECT_DATA_STRUCTURES_H
#define ANDROID_PROJECT_DATA_STRUCTURES_H

#include <istream>
#include <iosfwd>
#include <strstream>
#include <fstream>
#include <string>

const uint32_t WINDOW_WIDTH = 2560;
const uint32_t WINDOW_HEIGHT = 1550;

struct vec3d {float x, y, z;};
struct triangle {
    vec3d p[3];

    float color;

};
struct mesh {
    std::vector <triangle> tris;

private:
    std::string file_read(const char* filename) {
        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        if (rw == NULL) return NULL;

        Sint64 res_size = SDL_RWsize(rw);
        char* res = (char*)malloc(res_size + 1);

        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
            nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
            nb_read_total += nb_read;
            buf += nb_read;
        }
        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
            free(res);
            return NULL;
        }

        std::string str(res);
        return res;

    }

public:
    void load_obj( const char* filename ){
        file_readLine( filename );
    }

private:
    void file_readLine(const char* filename) {

        std::string stuff = file_read(filename);

        std::string line = "";

        std::vector<vec3d> verts;

        for( int I = 0 ; I < stuff.size() ; I++ ){

            if( stuff[I] == '\n' ){
                ///eol
                if (line.find("v") != std::string::npos) {
                    //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING: %s\n", line.c_str());
                    Do_Something_with_Line(line, verts);
                }
                if (line.find("f") != std::string::npos) {
                    //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING: %s\n", line.c_str());
                    Do_Something_with_Line(line, verts);
                }

                line = "";
            }else{
                line += stuff[I];
            }

        }
        if (line.find("v") != std::string::npos) {
            //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING: %s\n", line.c_str());
            Do_Something_with_Line(line, verts);
        }
        if (line.find("f") != std::string::npos) {
            //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING: %s\n", line.c_str());
            Do_Something_with_Line(line, verts);
        }

    }

    void Do_Something_with_Line( std::string sline , std::vector<vec3d> &verts ){

        replaceAll( sline , "\r" , "" );
        replaceAll( sline , "\\r" , "" );

        char *line = &sline[0];

        std::strstream s;
        s << line;

        char junk;

        if (line[0] == 'v')
        {
            //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING ALERT: %s\n", "GETTING VERICES");
            vec3d v;
            s >> junk >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }

        if (line[0] == 'f')
        {
            //__android_log_print(ANDROID_LOG_DEBUG, "LINE", "STRING ALERT: %s\n", "GETTING F");
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });

        }

    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

};
struct mat4x4 {float m[4][4] = {0};};

struct mesh meshCube;
struct mat4x4 matProj;
vec3d vCamera;

void Create_Projection_matrix(){
    /*meshCube.tris = {

            // SOUTH
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

            // EAST
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

            // NORTH
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

            // WEST
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

            // TOP
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

    };
     */

    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

}


#endif //ANDROID_PROJECT_DATA_STRUCTURES_H
