package com.example.testfreetype.util

import android.graphics.SurfaceTexture
import android.view.Surface
import android.view.TextureView
import com.example.testfreetype.bean.LayerManager
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.bean.TextLayer

class TextEditSurfaceManager : TextureView.SurfaceTextureListener {
    private var currLayerId: Int = 0


    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        TextEngineHelper.getTextEngine().textEngineSurfaceDestroyed()
        return true
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        TextEngineHelper.getTextEngine().textEngineSurfaceCreated(Surface(surface), width, height)
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }

    fun addTextLayer(textLayer: TextLayer) {
        val addTextLayer = TextEngineHelper.getTextEngine().addTextLayer(textLayer)
        if (addTextLayer != -1) {
            textLayer.layerId = addTextLayer
            LayerManager.addLayer(addTextLayer, textLayer)
        }
    }


    fun drawText(ttfPath: String?, text: String?, outPath: String?) {
        TextEngineHelper.getTextEngine().textEngineDraw(ttfPath, text, outPath)
    }

    fun drawPreViewLayer(textInfo: TextInfo) {
        TextEngineHelper.getTextEngine().textEngineDrawPreView(textInfo)
    }

    fun addThePreviewLayer2Map() {
        val layerId = TextEngineHelper.getTextEngine().addThePreviewLayer2Map()
    }

    fun addThePreviewLayerByJson2Map() {
        val layerId = TextEngineHelper.getTextEngine().addThePreviewLayerByJson2Map()
    }

    fun drawPreViewLayerByJson(json: String, fontFolder: String) {
        TextEngineHelper.getTextEngine().textEngineDrawPreViewByJson(json, fontFolder)
    }


}