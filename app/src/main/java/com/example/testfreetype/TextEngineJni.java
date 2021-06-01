package com.example.testfreetype;

import android.util.Log;

import com.example.testfreetype.bean.TextInfo;
import com.example.testfreetype.bean.TextLayer;

public class TextEngineJni {

    public long TEXT_ENGINE_ID = 0L;

    static {
        System.loadLibrary("myTest");
    }

    /*public void textMatrix(MatrixInfo matrixInfo) {
        textLayerTransform(TEXT_ENGINE_ID, , matrixInfo.getTx(), matrixInfo.getTy(), matrixInfo.getScale(), matrixInfo.getRangle());
    }*/

    public void textLayerTransform(int layerId, float tx, float ty,float centerX, float centerY, float s, float r) {
        textLayerTransform(TEXT_ENGINE_ID, layerId, tx, ty,centerX,centerY, s, r);
    }

    public void setBasicTextAttributes(int layerId, int subId, String ttf, String text, int fontSize, int fontColor) {
        setBasicTextAttributes(TEXT_ENGINE_ID, layerId, subId, ttf, text, fontSize, fontColor);
    }

    public void updateTextLayerFrameIndex(int layerId, int frameIndex) {
        updateTextLayerFrameIndex(TEXT_ENGINE_ID,layerId,frameIndex);
    }


    public interface TextEngineStatus {
        void onTextLayerAreaChange(int layerId, float left, float top, float right, float bottom);

        void onPreviewInit(int layerId);

        void onTextLevelChange(boolean isAdd, int layerId, int subTextId);
    }

    public void registerTextEngineStatus(TextEngineStatus mTextEngineStatus) {
        this.mTextEngineStatus = mTextEngineStatus;
    }

    private TextEngineStatus mTextEngineStatus;


    public TextEngineJni() {
        TEXT_ENGINE_ID = textEngineCreate();
    }

    /*-----------------编辑页面----------------------*/

    public void textEngineSurfaceDestroyed() {
        textEngineSurfaceDestroyed(TEXT_ENGINE_ID);
        TEXT_ENGINE_ID = 0;
    }

    public void init() {
        if (TEXT_ENGINE_ID == 0)
            TEXT_ENGINE_ID = textEngineCreate();
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

    public void setStrokeAttributes(int layerId, int subId, float lineDistance, float outLineDistance, int outLineColor) {
        setStrokeAttributes(TEXT_ENGINE_ID, layerId, subId, lineDistance, outLineDistance, outLineColor);
    }

    public int addTextLayer(TextLayer textLayer) {
        return addTextLayer(TEXT_ENGINE_ID, textLayer);
    }

    public void removeTextLayer(int layerId) {
        removeTextLayer(TEXT_ENGINE_ID, layerId);
    }

    public void addSimpleSubText(int layerId, int subTextId, String ttfPath, String text, int fontSize, int fonColor) {
        addSimpleSubText(TEXT_ENGINE_ID, layerId, subTextId, ttfPath, text, fontSize, fonColor);
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


    public native void printAll(long nativeHandle);


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

    //添加一个普通文字层
    private native void addSimpleSubText(long handle, int layerId, int subTextId, String ttfPath, String text, int fontSize, int fonColor);


    private native int addThePreviewLayer2Map(long handle);

    private native int addThePreviewLayerByJson2Map(long handle);

    private native void cleanPreview(long handle);

    private native void textLayerTransform(long handler, int layerId, float tx, float ty, float centerX, float centerY,float sc, float r);

    private native void setBasicTextAttributes(long handler, int layerId, int subId, String ttf, String text, int fontSize, int fontColor);

    private native void setStrokeAttributes(long handler, int layerId, int subId, float lineDistance, float outLineDistance, int outLineColor);

    private native void removeTextLayer(long handle, int layerId);

    private native void updateTextLayerFrameIndex(long handle,int layerId, int frameIndex);

    private void onError(int code) {
        Log.e("11111", "code :  " + code);
    }

    private void onTextLayerAreaChange(int layerId, float left, float top, float right, float bottom) {
        if (mTextEngineStatus != null) {
            mTextEngineStatus.onTextLayerAreaChange(layerId, left, top, right, bottom);
        }
    }

    private void onPreviewInit(int layerId) {
        if (mTextEngineStatus != null) {
            mTextEngineStatus.onPreviewInit(layerId);
        }
    }

    private void onTextLevelChange(boolean isAdd, int layerId, int subTextId) {
        Log.e("11111", "isAdd " + isAdd + " layerId  " + layerId + " subTextId  " + subTextId + "");
        if (mTextEngineStatus != null) {
            mTextEngineStatus.onTextLevelChange(isAdd, layerId, subTextId);
        }
    }
}

