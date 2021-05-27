package com.example.testfreetype.util

import android.graphics.SurfaceTexture
import android.view.Surface
import android.view.TextureView
import com.example.testfreetype.bean.TextInfo
import com.example.testfreetype.bean.TextLayer

class TextEditSurfaceManager : TextureView.SurfaceTextureListener {

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

    fun setBasicTextAttributes(textLayer: TextLayer) {
        val layerId = textLayer.layerId
        textLayer.getFirst()?.apply {
            TextEngineHelper.getTextEngine()
                .setBasicTextAttributes(layerId, id, ttfPath, char, size, fontColor)
        }
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