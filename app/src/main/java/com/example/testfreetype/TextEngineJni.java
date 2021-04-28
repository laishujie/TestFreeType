package com.example.testfreetype;

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


    public static native void testSdfShaderInit(long nativeHandle,String sdfPath, int sWidth, int sHeight, Object bitmap);

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
}
