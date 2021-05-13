package com.example.testfreetype;

import com.example.testfreetype.bean.TextLayer;

public class TextEngineJni {


    public static long TEXT_ENGINE_ID = 0L;

    static {
        System.loadLibrary("myTest");
    }

    public static native void test(String sdfPath, String outPath);

    public static native long initTextEngine();

    public static native void testFreeTypeInit(long nativeHandle, String ttfPath, int surfaceWidth, int surfaceHeight);

    public static native void freeDraw(long nativeHandle);

    public static native void insetText(long nativeHandle, String ttfPath, String text);

    public static native void testSdfShaderInit(long nativeHandle, String sdfPath, int sWidth, int sHeight, Object bitmap);

    public static native void testSdfShaderOnDraw(long nativeHandle,
                                                  int stokeColor,
                                                  int shadowColor,
                                                  float distanceMark,
                                                  float outlineMark,
                                                  float shadowDistance,
                                                  int shadowAngle,
                                                  float shadowAlpha);

    public static native void testSDF(String imageFile, String outFile, float radius, float imageAspect);

    public static native void testOpengGlSDF(String imageFile, String outFile, float radius, float imageAspect);

    public static native void destroy(long nativeHandle);

    public static native String printTextInfo(long nativeHandle, String ttfPath, String str);

    public static native void glInitTextShader(long nativeHandle, int surfaceWidth, int sufaceHeight);

    public static native void glRenderText(long nativeHandle, String ttfPath, String text);


    //---------------------------------/
    public static native long textEngineCreate();

    public static native void textEngineSurfaceCreated(long handler, Object surface, int width,
                                                       int height);

    public static native void textEngineSurfaceDestroyed(long handler);

    public static native void textEngineRelease(long handler);

    public static native void textEngineDraw(long handler,String ttfPath, String text,String outPath);
    public static native void textEngineDrawLayer(long handler, TextLayer layer);

    public static native void testLayer(long handler,String json,String fontFolder);
}
