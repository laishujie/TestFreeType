package com.example.testfreetype.util

import android.graphics.SurfaceTexture
import android.view.Surface
import android.view.TextureView
import com.example.testfreetype.TextEngineJni

class TextEditSurfaceManager : TextureView.SurfaceTextureListener {
    private var mId: Long = 0

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

    fun drawText(ttfPath: String, text: String) {
        TextEngineJni.textEngineDraw(mId, ttfPath, text)
    }
}