/*
 * MIT License
 *
 * Copyright (c) 2024 cr4zyengineer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(__APPLE__)

#define GL_SILENCE_DEPRECATION 1

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>
#import <OpenGL/OpenGL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

#include <lautils/bitwalker.h>
#include <la64/device/display.h>
#include <la64/core.h>

static void run_on_main(void (^block)(void))
{
    if ([NSThread isMainThread])
    {
        block();
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), block);
    }
}

static void die(const char* msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    abort();
}

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok)
    {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        GLsizei log_size = (len > 1) ? len : 2;
        char *log = (char *)malloc((size_t)log_size);
        glGetShaderInfoLog(s, log_size, NULL, log);
        fprintf(stderr, "Shader compile failed:\n%s\n", log);
        free(log);
        abort();
    }
    return s;
}

static GLuint linkProgram(GLuint vs, GLuint fs)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);

    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok)
    {
        GLint len = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        GLsizei log_size = (len > 1) ? len : 2;
        char *log = (char *)malloc((size_t)log_size);
        glGetProgramInfoLog(p, log_size, NULL, log);
        fprintf(stderr, "Program link failed:\n%s\n", log);
        free(log);
        abort();
    }
    return p;
}

la64_display_t *la64_display_alloc(void)
{
    la64_display_t *display = calloc(1, sizeof(la64_display_t));
    if(display == NULL) return NULL;

    display->palette = calloc(3, 256);
    if(display->palette == NULL)
    {
        free(display);
        return NULL;
    }

    for (int i = 0; i < 256; i++)
    {
        uint8_t g = (uint8_t)i;
        display->palette[i*3 + 0] = g;
        display->palette[i*3 + 1] = g;
        display->palette[i*3 + 2] = g;
    }

    display->fb = calloc(1, LA64_FB_SIZE);
    if(display->fb == NULL)
    {
        free(display->palette);
        free(display);
        return NULL;
    }

    display->enabled = 0;
    return display;
}

void la64_display_dealloc(la64_display_t *display)
{
    if(display == NULL) return;

    if(display->enabled)
    {
        pthread_cancel(display->pthread);
        pthread_join(display->pthread, NULL);
    }

    if(display->palette) free(display->palette);
    if(display->fb) free(display->fb);
    free(display);
}

static NSOpenGLContext* makeGLContext(NSView *view)
{
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if(!pf) die("NSOpenGLPixelFormat failed");

    NSOpenGLContext *ctx = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    if(!ctx) die("NSOpenGLContext failed");

    [ctx setView:view];
    [ctx makeCurrentContext];

    GLint swapInt = 1;
    [ctx setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    return ctx;
}

void *display_start(void *arg)
{
    @autoreleasepool
    {
        run_on_main(^{
            la64_display_t *display = (la64_display_t*)arg;
            if(display == NULL) return;

            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
            [NSApp activateIgnoringOtherApps:YES];

            NSRect r = NSMakeRect(100, 100, 500, 500);
            NSWindow *win = [[NSWindow alloc] initWithContentRect:r styleMask:(NSWindowStyleMaskTitled |  NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable) backing:NSBackingStoreBuffered defer:NO];
            [win setTitle:@"LA64LCD @ 64Hz"];
            [win makeKeyAndOrderFront:nil];

            NSView *view = [win contentView];
            [view setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
            [view setWantsBestResolutionOpenGLSurface:YES];

            NSOpenGLContext *ctx = makeGLContext(view);

            const char* vsSrc =
                "#version 150 core\n"
                "in vec2 aPos;\n"
                "in vec2 aUV;\n"
                "out vec2 vUV;\n"
                "void main(){\n"
                "  vUV = aUV;\n"
                "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                "}\n";

            const char* fsSrc =
                "#version 150 core\n"
                "in vec2 vUV;\n"
                "out vec4 FragColor;\n"
                "uniform sampler2D uIndexTex;\n"
                "uniform sampler2D uPalette; // 2D 256x1 instead of 1D (more compatible)\n"
                "void main(){\n"
                "  float idxN = texture(uIndexTex, vUV).r;\n"
                "  float idx  = floor(idxN * 255.0 + 0.5);\n"
                "  float u    = (idx + 0.5) / 256.0;\n"
                "  vec3 rgb   = texture(uPalette, vec2(u, 0.5)).rgb;\n"
                "  FragColor  = vec4(rgb, 1.0);\n"
                "}\n";

            GLuint prog = linkProgram(
                compileShader(GL_VERTEX_SHADER, vsSrc),
                compileShader(GL_FRAGMENT_SHADER, fsSrc)
            );

            float verts[] = {
                -1.f,-1.f,  0.f,0.f,
                1.f,-1.f,  1.f,0.f,
                1.f, 1.f,  1.f,1.f,
                -1.f, 1.f,  0.f,1.f
            };
            uint16_t idxs[] = { 0,1,2, 2,3,0 };

            GLuint vao,vbo,ebo;
            glGenVertexArrays(1,&vao);
            glGenBuffers(1,&vbo);
            glGenBuffers(1,&ebo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idxs),idxs,GL_STATIC_DRAW);

            GLint locPos = glGetAttribLocation(prog, "aPos");
            GLint locUV  = glGetAttribLocation(prog, "aUV");
            glEnableVertexAttribArray((GLuint)locPos);
            glVertexAttribPointer((GLuint)locPos, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
            glEnableVertexAttribArray((GLuint)locUV);
            glVertexAttribPointer((GLuint)locUV, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
            glBindVertexArray(0);

            GLuint texIndex, texPal;
            glGenTextures(1, &texIndex);
            glBindTexture(GL_TEXTURE_2D, texIndex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, LA64_FB_WIDTH, LA64_FB_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glGenTextures(1, &texPal);
            glBindTexture(GL_TEXTURE_2D, texPal);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, display->palette);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glUseProgram(prog);
            glUniform1i(glGetUniformLocation(prog, "uIndexTex"), 0);
            glUniform1i(glGetUniformLocation(prog, "uPalette"), 1);

            double prev = now_sec();
            double acc  = 0.0;

            while(1)
            {
                NSEvent *event = nil;
                do
                {
                    event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate dateWithTimeIntervalSinceNow:0.0] inMode:NSDefaultRunLoopMode dequeue:YES];
                    if(event) [NSApp sendEvent:event];
                } while(event);

                if(![win isVisible]) break;

                double now = now_sec();
                acc += (now - prev);
                prev = now;

                if(acc < (1.0/64.0))
                {
                    struct timespec ts = {0, 1000000};
                    nanosleep(&ts, NULL);
                    continue;
                }
                acc = 0.0;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texIndex);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, LA64_FB_WIDTH, LA64_FB_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, display->fb);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texPal);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_RGB, GL_UNSIGNED_BYTE, display->palette);

                NSRect bounds = [view bounds];
                GLint ww = (GLint)bounds.size.width;
                GLint wh = (GLint)bounds.size.height;
                [ctx update];

                NSRect boundsPts = [view bounds];
                NSRect boundsPx  = [view convertRectToBacking:boundsPts];

                GLint winW = (GLint)boundsPx.size.width;
                GLint winH = (GLint)boundsPx.size.height;

                const float fbAspect  = (float)LA64_FB_WIDTH / (float)LA64_FB_HEIGHT;
                const float winAspect = (float)winW / (float)winH;

                GLint vpX = 0, vpY = 0, vpW = winW, vpH = winH;

                if(winAspect > fbAspect)
                {
                    vpH = winH;
                    vpW = (GLint)lroundf((float)vpH * fbAspect);
                    vpX = (winW - vpW) / 2;
                    vpY = 0;
                }
                else
                {
                    vpW = winW;
                    vpH = (GLint)lroundf((float)vpW / fbAspect);
                    vpX = 0;
                    vpY = (winH - vpH) / 2;
                }

                glViewport(vpX, vpY, vpW, vpH);
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(prog);
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

                [ctx flushBuffer];
            }

            [NSOpenGLContext clearCurrentContext];
            return;
        });
    }

    return NULL;
}

#endif /* __APPLE__ */

