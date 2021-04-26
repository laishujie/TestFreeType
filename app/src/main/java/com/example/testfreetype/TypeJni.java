package com.example.testfreetype;

public class TypeJni {
    static{
        System.loadLibrary("myTest");
    }

    public static native void test();


    public static native long testSdfShaderInit(String sdfPath,int sWidth,int sHeight,Object bitmap);
    public static native long testSdfShaderOnDraw(long nativeHandle,
                                                  int stokeColor,
                                                  int shadowColor,
                                                  float distanceMark,
                                                  float outlineMark,
                                                  float shadowDistance,
                                                  int shadowAngle,
                                                  float shadowAlpha);
    public static native void testSDF(String imageFile,String outFile,float radius,float imageAspect);
    public static native void testOpengGlSDF(String imageFile,String outFile,float radius,float imageAspect);
}
