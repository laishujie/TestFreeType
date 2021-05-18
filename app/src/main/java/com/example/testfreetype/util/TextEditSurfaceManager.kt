package com.example.testfreetype.util

import android.graphics.SurfaceTexture
import android.view.Surface
import android.view.TextureView
import com.example.testfreetype.TextEngineJni
import com.example.testfreetype.bean.LayerManager
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.bean.TextLayer

class TextEditSurfaceManager : TextureView.SurfaceTextureListener {
    private var mId: Long = 0
    private var currLayerId: Int = 0

    init {
        mId = TextEngineJni.textEngineCreate()
    }


    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        TextEngineJni.textEngineSurfaceDestroyed(mId)
        return true
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        TextEngineJni.textEngineSurfaceCreated(mId, Surface(surface), width, height)
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }

    fun addTextLayer(textLayer: TextLayer) {
        val addTextLayer = TextEngineJni.addTextLayer(mId, textLayer)
        if (addTextLayer != -1) {
            textLayer.layerId = addTextLayer
            LayerManager.addLayer(addTextLayer, textLayer)
        }
    }


    fun drawText(ttfPath: String?, text: String?, outPath: String?) {
        TextEngineJni.textEngineDraw(mId, ttfPath, text, outPath)
    }

    fun drawPreViewLayer(textLayer: TextInfo) {
        TextEngineJni.textEngineDrawPreView(mId, textLayer)
    }

    fun addThePreviewLayer2Map() {
        val layerId = TextEngineJni.AddThePreviewLayer2Map(mId)
    }

    fun addThePreviewLayerByJson2Map() {
        val layerId = TextEngineJni.AddThePreviewLayerByJson2Map(mId)
    }

    fun drawPreViewLayerByJson(json: String, fontFolder: String) {
        TextEngineJni.textEngineDrawPreViewByJson(mId, json, fontFolder)
    }

}