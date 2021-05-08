package com.example.testfreetype.util

import android.graphics.SurfaceTexture
import android.util.Log
import android.view.Surface
import android.view.TextureView
import android.view.WindowId
import com.example.testfreetype.TextEngineJni
import com.example.testfreetype.bean.TextConfigManager
import com.example.testfreetype.bean.TextLayer

class TextEditSurfaceManager : TextureView.SurfaceTextureListener {
    private var mId: Long = 0
    private var currLayerId: Int = 0

    init {
        mId = TextEngineJni.textEngineCreate()
    }


    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        TextEngineJni.textEngineSurfaceDestroyed(mId)
        clear()
        return true
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        TextEngineJni.textEngineSurfaceCreated(mId, Surface(surface), width, height)
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {

    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {

    }

    fun addDefaultTextLayer(ttfPath: String, text: String) {
        currLayerId = TextConfigManager.addLayer(ttfPath, text)
    }


    fun drawText(ttfPath: String?, text: String?, outPath: String?) {
        TextEngineJni.textEngineDraw(mId, ttfPath, text, outPath)
    }
    fun drawLayer(textLayer: TextLayer) {
        TextEngineJni.textEngineDrawLayer(mId, textLayer)
    }

    fun drawCurrLayer(ttfPath: String?, text: String?) {
        val layer = TextConfigManager.getLayer(currLayerId)
        if (layer != null) {
            val ttf = TextConfigManager.getStringSame(layer.ttfPath, ttfPath)
            val char = TextConfigManager.getStringSame(layer.char, text)

            Log.e("11111", "layer $currLayerId ttf $ttf text $char")
            TextEngineJni.textEngineDraw(mId, ttf, char, null)
        }
    }

    fun clear() {
        TextConfigManager.clear()
    }
}