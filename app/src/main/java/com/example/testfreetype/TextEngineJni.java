package com.example.testfreetype;

import android.util.Log;

import com.example.testfreetype.bean.TextInfo;
import com.example.testfreetype.bean.TextLayer;

public class TextEngineJni {

    public long TEXT_ENGINE_ID = 0L;

    static {
        System.loadLibrary("myTest");
    }

    public interface TextAreaCallBack {
        void onChangeArea(int layerId, float left, float top, float right, float bottom);
    }

    public void registerTextAreaCallBack(TextAreaCallBack mTextAreaCallBack) {
        this.mTextAreaCallBack = mTextAreaCallBack;
    }

    private TextAreaCallBack mTextAreaCallBack;


    public TextEngineJni() {
        TEXT_ENGINE_ID = textEngineCreate();
    }

    /*-----------------编辑页面----------------------*/

    public void textEngineSurfaceDestroyed() {
        textEngineSurfaceDestroyed(TEXT_ENGINE_ID);
    }

    public void textEngineSurfaceCreated(Object surface, int width,
                                         int height) {
        textEngineSurfaceCreated(TEXT_ENGINE_ID, surface, width, height);
    }

    public void textEngineRelease() {
        textEngineRelease(TEXT_ENGINE_ID);
    }

    public void textEngineDraw(String ttfPath, String text, String outPath) {
        textEngineDraw(TEXT_ENGINE_ID, ttfPath, text, outPath);
    }

    //更新普通预览层
    public void textEngineDrawPreView(TextInfo textInfo) {
        textEngineDrawPreView(TEXT_ENGINE_ID, textInfo);
    }

    //更新json模板预览层
    public void textEngineDrawPreViewByJson(String json, String fontFolder) {
        textEngineDrawPreViewByJson(TEXT_ENGINE_ID, json, fontFolder);
    }

    public int addTextLayer(TextLayer textLayer) {
        return addTextLayer(TEXT_ENGINE_ID, textLayer);
    }

    public int addThePreviewLayer2Map() {
        return addThePreviewLayer2Map(TEXT_ENGINE_ID);
    }

    public int addThePreviewLayerByJson2Map() {
        return addThePreviewLayerByJson2Map(TEXT_ENGINE_ID);
    }

    public void cleanPreview() {
        cleanPreview(TEXT_ENGINE_ID);
    }



    /*-----------------编辑页面----------------------*/


    /*-----------------SDF纹理测试API----------------------*/
    public void testSdfShaderOnDraw(int stokeColor,
                                    int shadowColor,
                                    float distanceMark,
                                    float outlineMark,
                                    float shadowDistance,
                                    int shadowAngle,
                                    float shadowAlpha) {
        testSdfShaderOnDraw(TEXT_ENGINE_ID, stokeColor, shadowColor, distanceMark, outlineMark, shadowDistance, shadowAngle, shadowAlpha);
    }

    public void testSdfShaderInit(String sdfPath, int sWidth, int sHeight, Object bitmap) {
        testSdfShaderInit(TEXT_ENGINE_ID, sdfPath, sWidth, sHeight, bitmap);
    }

    /*-----------------SDF纹理测试API----------------------*/


    /*-----------------FreeType测试API----------------------*/

    public String printTextInfo(String ttfPath, String str) {
        return printTextInfo(TEXT_ENGINE_ID, ttfPath, str);
    }

    public void glInitTextShader(int surfaceWidth, int sufaceHeight) {
        glInitTextShader(TEXT_ENGINE_ID, surfaceWidth, sufaceHeight);
    }

    public void glRenderText(String ttfPath, String text) {
        glRenderText(TEXT_ENGINE_ID, ttfPath, text);
    }

    public void testFreeTypeInit(String ttfPath, int surfaceWidth, int surfaceHeight) {
        testFreeTypeInit(TEXT_ENGINE_ID, ttfPath, surfaceWidth, surfaceHeight);
    }

    public void freeDraw() {
        freeDraw(TEXT_ENGINE_ID);
    }

    public void insetText(String ttfPath, String text) {
        insetText(TEXT_ENGINE_ID, ttfPath, text);
    }

    /*-----------------FreeType测试API----------------------*/


    private native void test(String sdfPath, String outPath);

    private native long initTextEngine();

    private native void testFreeTypeInit(long nativeHandle, String ttfPath, int surfaceWidth, int surfaceHeight);

    private native void freeDraw(long nativeHandle);

    private native void insetText(long nativeHandle, String ttfPath, String text);

    private native void testSdfShaderInit(long nativeHandle, String sdfPath, int sWidth, int sHeight, Object bitmap);

    private native void testSdfShaderOnDraw(long nativeHandle,
                                            int stokeColor,
                                            int shadowColor,
                                            float distanceMark,
                                            float outlineMark,
                                            float shadowDistance,
                                            int shadowAngle,
                                            float shadowAlpha);

    private native void testSDF(String imageFile, String outFile, float radius, float imageAspect);

    private native void testOpengGlSDF(String imageFile, String outFile, float radius, float imageAspect);

    private native void destroy(long nativeHandle);

    private native String printTextInfo(long nativeHandle, String ttfPath, String str);

    private native void glInitTextShader(long nativeHandle, int surfaceWidth, int sufaceHeight);

    private native void glRenderText(long nativeHandle, String ttfPath, String text);


    //-------------编辑页面--------------------/
    public native long textEngineCreate();

    private native void textEngineSurfaceCreated(long handler, Object surface, int width,
                                                 int height);

    private native void textEngineSurfaceDestroyed(long handler);

    private native void textEngineRelease(long handler);

    private native void textEngineDraw(long handler, String ttfPath, String text, String outPath);

    //更新普通预览层
    private native void textEngineDrawPreView(long handler, TextInfo textInfo);

    //更新json模板预览层
    private native void textEngineDrawPreViewByJson(long handler, String json, String fontFolder);

    private native void testLayer(long handler, String json, String fontFolder);

    private native int addTextLayer(long handle, TextLayer textLayer);

    private native int addThePreviewLayer2Map(long handle);

    private native int addThePreviewLayerByJson2Map(long handle);

    private native void cleanPreview(long handle);

    private void onError(int code) {
        Log.e("11111", "code :  " + code);
    }

    private void onTextAreaChanged(float left, float top, float right, float bottom) {
        //Log.e("11111", "onTextAreaChanged : left " + left + "top: " + top + " right:" + right + "bottom: " + bottom);
        if (mTextAreaCallBack != null) {
            mTextAreaCallBack.onChangeArea(0, left, top, right, bottom);
        }
    }
}

